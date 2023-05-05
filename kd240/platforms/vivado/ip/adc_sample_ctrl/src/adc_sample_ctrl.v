// Copyright (C) 2023, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

// Convert PWM sync signal to update on ADC Interface by keeping
// a consistent ADC sample rate with a consistent number of samples
// per sync signal, and line up as close as possible with falling edge
// of csn (on SCLK clock domain).  Uses PLL-type mechanisms.

`timescale 1 ns / 1 ps


module adc_sample_ctrl #(

  parameter ADC_SCLKS_PER_SAMPLE = 16,
  parameter MAX_CHANGE_CLKS = 3,
  parameter real CLK_PERIOD_NS = 10.0,
  parameter real SCLK_PERIOD_NS = 21.0
)
(

  // Interface to System (on clk domain)
  input wire pwm_sync,
  output wire sample_adc,
  output wire [1:0] status,

  // Interface to ADC (on clk domain)
  input wire csn,

  // System signals
  input wire clk,
  input wire resetn,
  input wire sresetn

);

  // Local parameters
  localparam integer STARTUP_SAMPLES = 8;
  localparam integer STARTUP_BITS = $clog2(STARTUP_SAMPLES) + 1;
  localparam real CLK_RATIO_REAL = SCLK_PERIOD_NS / CLK_PERIOD_NS;
  localparam integer CLK_RATIO_INT = SCLK_PERIOD_NS / CLK_PERIOD_NS;
  localparam integer CLK_RATIO = (CLK_RATIO_REAL > (0.5 + CLK_RATIO_INT)) ?
    CLK_RATIO_INT + 1 :
    CLK_RATIO_INT;
  localparam integer SAMPLE_MAX_BITS = $clog2(ADC_SCLKS_PER_SAMPLE * (CLK_RATIO + 1)) + 1;
  // Use DEBUG = 1 to see if remainder is working - indicated by sync_edge_sample_cnt being constant
  localparam DEBUG = 0;

  localparam [4:0]  fsm_c_IDLE = 'h0,
                    fsm_c_TEST_ADC = 'h1,
                    fsm_c_COMPUTE = 'h2,
                    fsm_c_WAIT_FIRST_SYNC = 'h3,
                    fsm_c_WAIT_SYNC = 'h4,
                    fsm_c_GOT_SYNC = 'h5;
  localparam [1:0]  fsm_stat_STARTUP = 'h0,
                    fsm_stat_DIFF_ZERO = 'h1,
                    fsm_stat_REAQUIRE = 'h3,
                    fsm_stat_LOCK = 'h2;
  localparam        fsm_a_IDLE = 2'b0,
                    fsm_a_BUSY = 2'b1;
  localparam [1:0]  fsm_t_IDLE = 2'b00,
                    fsm_t_UPDATE_ADC = 2'b01,
                    fsm_t_ADC_START = 2'b10,
                    fsm_t_ADC_WAIT_FINISH = 2'b11;
  localparam [3:0]  fsm_s_IDLE = 5'h0,
                    fsm_s_WAIT_FOR_SYNC = 5'h1,
                    fsm_s_CHECK_SAMPLE = 5'h2,
                    fsm_s_ADJUST = 5'h3;
  localparam [1:0]  fsm_u_IDLE = 2'h0,
                    fsm_u_UPDATE = 2'h1,
                    fsm_u_REMAINDER = 2'h2,
                    fsm_u_ADJUST = 2'h3;

  // Signal declarations

  // Finite State Machines
  reg [1:0] fsm_update = fsm_u_IDLE;
  reg [4:0] fsm_control = fsm_c_IDLE;
  reg [3:0] last_fsm_control = fsm_c_IDLE;
  reg [2:0] fsm_status = fsm_stat_STARTUP;
  reg [1:0] fsm_test = fsm_t_IDLE;
  reg [3:0] fsm_sample = fsm_s_IDLE;
  reg       fsm_adc = fsm_a_IDLE;
  reg       load_new_count = 0;
  reg       sample_adc_reg = 0;
  wire      sample_adc_rising;

  // Misc
  reg [STARTUP_BITS-1:0] test_adc_cnt = 0;
  reg [SAMPLE_MAX_BITS-1:0] sample_cnt = 0;
  reg pwm_sync_reg = 0;
  wire pwm_sync_rising;
  reg [SAMPLE_MAX_BITS-1:0] expected_sample_cnt = 0;
  reg csn_reg;
  wire csn_rising;
  wire csn_falling;
  reg [SAMPLE_MAX_BITS-1:0] sync_edge_sample_cnt = 0;
  reg [SAMPLE_MAX_BITS-1:0] csn_falling_sample_cnt = 0;
  reg [SAMPLE_MAX_BITS-1:0] desired_sample_trigger_count = 0;
  reg signed [SAMPLE_MAX_BITS-1:0] difference_sample_and_trigger = 'h0;
  reg [SAMPLE_MAX_BITS-1:0] samples_per_sync_cycle_cnt = 0;
  reg [SAMPLE_MAX_BITS-1:0] last_samples_per_sync_cycle_cnt = 0;
  reg [15:0] pwm_sync_cycle_cnt = 0;
  reg [15:0] last_pwm_sync_cycle_cnt = 0;
  reg [15:0] pwm_sync_cycle_clocks = 0;
  reg [3:0] adj_cntdown = 'h0;

  reg signed [SAMPLE_MAX_BITS-1:0] remainder = 0;
  reg set_remainder = 1'b1;
  reg [SAMPLE_MAX_BITS-1:0] remainder_clock_countdown = 0;
  reg add_delay = 1'b0;
  reg [1:0] lock_cntdown = 'b11;

  // Adder to accumulate values to average (over 8 samples)
  reg [SAMPLE_MAX_BITS-1:0] csn_fall_add_cnts;

  // Sample ADC (drive UPDATE pin) always when in TEST_ADC state (startup after reset)
  // when the ADC is not busy doing another sample.  Should sample at maximum rate.
  // When outside of TEST_ADC, sample the ADC when the fsm_update machine is in UPDATE state.
  assign sample_adc = (fsm_control == fsm_c_TEST_ADC) ?
          (fsm_adc == fsm_a_IDLE) : ((fsm_update == fsm_u_UPDATE) ? 1'b1 : 1'b0);
  assign status = fsm_status;

  // Capture the current sample count when the pwm_sync rising edge happens
  // This is used to keep track of how the relationship between ADC sampling
  // and pwm_sync edges change as adjustments are made to align the edges.
  // When using DEBUG mode, only remainder adjustments are made.  Check that
  // sync_edge_sample_cnt is unchanging, which indicates the remainder part is
  // working.
  always @(posedge clk)
  begin
    if (resetn == 1'b0  || sresetn == 1'b0)
      begin
        sync_edge_sample_cnt <= 'h0;
      end
    else if (pwm_sync_rising)
      begin
        sync_edge_sample_cnt <= sample_cnt;
      end
  end

  always @(posedge clk)
    csn_reg <= csn;

  always @(posedge clk)
    sample_adc_reg <= sample_adc;

  assign csn_rising = !csn_reg & csn;
  assign csn_falling = csn_reg & !csn;
  assign sample_adc_rising = !sample_adc_reg & sample_adc;
  assign pwm_sync_rising = pwm_sync & ~pwm_sync_reg;

  // Misc logic
  always @(posedge clk)
  begin
    if (resetn == 1'b0 || sresetn == 1'b0)
      begin
        pwm_sync_reg <= 'h0;
        sample_cnt <= 0;
        test_adc_cnt <= 'h0;
        pwm_sync_cycle_cnt <= 'h0;
        last_pwm_sync_cycle_cnt <= 'h0;
        pwm_sync_cycle_clocks <= 'h0;
        desired_sample_trigger_count <= 'h0;
        difference_sample_and_trigger <= 'h0;
        samples_per_sync_cycle_cnt <= 0;
        last_samples_per_sync_cycle_cnt <= 0;
        sync_edge_sample_cnt <= 0;
        set_remainder <= 1'b1;
        remainder <= 0;
        last_fsm_control <= fsm_c_IDLE;
        remainder_clock_countdown <= 0;
        add_delay <= 1'b0;
        csn_falling_sample_cnt <= 0;
        csn_fall_add_cnts <= 0;
        lock_cntdown <= 'b11;
      end
    else
      begin
        pwm_sync_reg <= pwm_sync;
        last_fsm_control <= fsm_control;

        if (pwm_sync_rising)
          begin
            // Store the sample count on the rising edge of pwm_sync.  If this value moves from
            // pwm_sync cycle to pwm_sync cycle, then remainder adjustments need to be made.
            sync_edge_sample_cnt <= sample_cnt;
            // Take last time's value (before it is updated) and subtract sample_cnt (new value) to
            // find the remainder.  This difference indicates that a remainder delay has to be
            // implemented.  The remainder can be negative.
            // Do this calculation once at the end of the first pwm cycle and store it for use later.
            if (set_remainder && fsm_control == fsm_c_WAIT_SYNC)
              begin
                remainder <= sample_cnt - sync_edge_sample_cnt;
                set_remainder <= 1'b0;
              end
          end

        // Accumulate the sample count every time csn falling happens and then
        // find the average count.  This is used to decide what the desired count
        // should be for pwm_sync alignment.  In simulations this value is the same
        // every time.  So, averaging may not be necessary.  The averaging happens
        // during the TEST_ADC phase.
        if (csn_falling && fsm_control == fsm_c_TEST_ADC)
          csn_fall_add_cnts <= csn_fall_add_cnts + sample_cnt;
        else if (fsm_control == fsm_c_COMPUTE)
          csn_falling_sample_cnt <= (csn_fall_add_cnts >> 3);

        // If no averaging for csn_falling_sample_cnt, use this logic instead
        //  if (fsm_control == fsm_c_COMPUTE)
        //    csn_falling_sample_cnt <= sample_cnt;


        // Count how long to stay in fsm_c_TEST_ADC state, parameter controlled
        if (fsm_control == fsm_c_TEST_ADC && csn_rising)
          test_adc_cnt <= test_adc_cnt + 1;

        // Count to keep track of clks in the sample, starting with sample_adc rising edge.
        // Used everywhere to align pwm_sync with csn falling.  Counts 1 to N clocks in the sample.
        if (sample_adc_rising)
          sample_cnt <= 1;
       else
          sample_cnt <= sample_cnt + 1;
      end

      // Capture the sample count at the end of each sample during the test period.  Used to
      // know how many clocks to expect for a full sample.  Used in calculations elsewhere.
      if ((fsm_control == fsm_c_TEST_ADC) && csn_rising)
        expected_sample_cnt <= sample_cnt;

      // Calculate the trigger count where we want the pwm_sync to happen for best csn_falling
      // matching.  Set to value that understands latency due to cross clock domains.
      if (fsm_control > fsm_c_COMPUTE)
        if (CLK_RATIO == 1)
          desired_sample_trigger_count <= csn_falling_sample_cnt;
        else
          desired_sample_trigger_count <= csn_falling_sample_cnt - CLK_RATIO;

      // Find out if last_pwm_sync_cycle_cnt can be divided by expected sample_cnt evenly.
      // If there are leftovers, try to adjust samples so that only full cycles are contained
      // in pwm_sync cycles. (SIM DEBUG ONLY)
      if (pwm_sync_rising)
        begin
          last_samples_per_sync_cycle_cnt <= samples_per_sync_cycle_cnt;
          samples_per_sync_cycle_cnt <= 0;
        end
      // Count each time csn_rising goes high while in WAIT_SYNC.  Ensures only full samples
      // get counted.
      else if (fsm_control == fsm_c_WAIT_SYNC && csn_rising)
        samples_per_sync_cycle_cnt <= samples_per_sync_cycle_cnt + 1'b1;

      // Do the math to figure out how close the pwm_sync came to the desired_sample_trigger_count.
      // Use this difference to decide on how many clocks to adjust per PWM cycle.  If
      // sync_edge_sample_cnt changes with each pwm_sync rising edge, then the remainder logic
      // is not working properly.
      if (fsm_control == fsm_c_GOT_SYNC)
        difference_sample_and_trigger <= desired_sample_trigger_count - sync_edge_sample_cnt;

      // Keep track of clocks in the pwm_cycle (SIM DEBUG only - not used in control logic)
      if (fsm_control == fsm_c_WAIT_FIRST_SYNC)
        pwm_sync_cycle_cnt <= 'h0;
      else if (pwm_sync_rising)
        begin
          pwm_sync_cycle_cnt <= 'h0;
          last_pwm_sync_cycle_cnt <= pwm_sync_cycle_cnt;
          pwm_sync_cycle_clocks <= pwm_sync_cycle_cnt + 1'b1;
        end
      else if (fsm_control > fsm_c_WAIT_FIRST_SYNC)
        pwm_sync_cycle_cnt <= pwm_sync_cycle_cnt + 1'b1;

      // Calculate the difference in sample_clks from one cycle to the other to find out
      // how many clocks to 'eat' to make sure we get an integer number of samples per pwm_cycle.
      // This counter is loaded every time after the first sync cycle, using the remainder found
      // during the TEST_ADC phase.  remainder_clock_countdown goes from remainder down to 1.
      if (fsm_control == fsm_c_GOT_SYNC && last_fsm_control != fsm_c_WAIT_FIRST_SYNC)
        begin
          // If remainder is negative, add the remainder to the total number of clocks in the sample
          // (since remainder is negative, the result is smaller than the total number of clocks in the sample)
          if (remainder < 0)
            remainder_clock_countdown <= (expected_sample_cnt + 1'b1) + remainder;

          // If remainder is positive, use remainder for countdown
          else if (remainder > 0)
            remainder_clock_countdown <= remainder;

          // If remainder is zero, no delays are needed (never tested in simulation)
          else if (remainder == 0)
            remainder_clock_countdown <= 0;

        end
      // Decide to add remainder delay if the countdown is greater than zero.  Once all the
      // required delays have been added, stop adding those delays.  There should now be an integer
      // number of samples in the pwm sync period.
      else if (fsm_control > fsm_c_WAIT_FIRST_SYNC)
        begin
          if (csn_rising && remainder_clock_countdown > 0)
              add_delay <= 1'b1;
          else if (remainder_clock_countdown == 0)
            add_delay <= 1'b0;

          // Once the update state machine goes to the REMAINDER state, reduce the countdown by 1.
          if (fsm_update == fsm_u_REMAINDER)
            remainder_clock_countdown <= remainder_clock_countdown - 1'b1;
        end

      // Check that LOCK state is stable before reporting it
      if (fsm_control == fsm_c_WAIT_SYNC && difference_sample_and_trigger == 0)
        begin
          if (lock_cntdown > 0 && pwm_sync_rising)
            lock_cntdown <= lock_cntdown - 1'b1;
          else if (pwm_sync_rising)
            lock_cntdown <= 0;
        end
      else if (fsm_control == fsm_c_WAIT_SYNC && difference_sample_and_trigger != 0)
        lock_cntdown <= 'b11;

  end

  // FSM to control the sampling adjustments between the pwm_sync and the sample_cnt
  // fsm_s_IDLE = 5'h0,
  // fsm_s_WAIT_FOR_SYNC = 5'h1,
  // fsm_s_CHECK_SAMPLE = 5'h2,
  // fsm_s_ADJUST = 5'h3;
  always @(posedge clk)
  begin
    if (resetn == 1'b0 || sresetn == 1'b0)
      begin
        adj_cntdown <= 'h0;
        fsm_sample <= fsm_s_IDLE;
        load_new_count <= 1'b0;
      end
    else
      case (fsm_sample)
        // Start state, never goes back here unless a reset happens
        fsm_s_IDLE:
          // Let fsm_control finish figuring out remainder before starting adjustments
          if (fsm_control == fsm_c_GOT_SYNC && last_fsm_control == fsm_c_WAIT_SYNC)
            fsm_sample <= fsm_s_WAIT_FOR_SYNC;
        // Wait for pwm_sync to happen
        fsm_s_WAIT_FOR_SYNC:
        begin
          if (pwm_sync_rising)
            fsm_sample <= fsm_s_CHECK_SAMPLE;
        end
        // This state compares the sample count when pwm_sync rising edge happened to
        // a desired sample count for LOCK condition, and decides if adjustment is
        // necessary.
        fsm_s_CHECK_SAMPLE:
          if (DEBUG)
            fsm_sample <= fsm_s_WAIT_FOR_SYNC;
          else
            begin
              if (sync_edge_sample_cnt != desired_sample_trigger_count)
                fsm_sample <= fsm_s_ADJUST;
              else
                fsm_sample <= fsm_s_WAIT_FOR_SYNC;
            end
        // If adjustment is necessary, decide how many clocks to adjust.  Set the countdown
        // timer for adjustment, with maximum clocks set by parameter MAX_CHANGE_CLKS.
        // Countdown goes from MAX_CHANGE_CLKS down to 1.
        fsm_s_ADJUST:
          begin
            // If the difference between the edge and the desired edge sample counts
            // is negative, either set to MAX_CHANGE_CLKS or the positive value of the
            // difference (if smaller than MAX_CHANGE_CLKS).
            if (difference_sample_and_trigger < 0)
            begin
              // Just looking at inverse instead of doing the full change in sign to save
              // some logic.
              if ((~difference_sample_and_trigger) >= MAX_CHANGE_CLKS)
                adj_cntdown <= MAX_CHANGE_CLKS;
              else
                // Change to positive
                adj_cntdown <= (-1)*difference_sample_and_trigger;

              load_new_count <= 1'b1;
            end
            // If the difference between the edge and the desired edge sample counts
            // is greater than MAX_CHANGE_CLKS, then adjust by MAX_CHANGE_CLKS.
            else if (difference_sample_and_trigger > MAX_CHANGE_CLKS)
            begin
              adj_cntdown <= MAX_CHANGE_CLKS;
              load_new_count <= 1'b1;
            end
            // If the difference between the edge and the desired edge sample counts
            // is between 0 and MAX_CHANGE_CLKS inclusive, then adjust by the difference.
            else
            begin
              adj_cntdown <= difference_sample_and_trigger;
              load_new_count <= 1'b1;
            end

            // Handshaking with the fsm_update state machine which is actually in charge
            // of sampling the ADC.  Once that machine goes to fsm_u_ADJUST, start the
            // countdown timer to control how long that machine stays in fsm_u_ADJUST state.
            if (fsm_update == fsm_u_ADJUST)
              begin
                load_new_count <= 1'b0;
                if (adj_cntdown == 1)
                  fsm_sample <= fsm_s_WAIT_FOR_SYNC;
                else if (adj_cntdown > 1)
                  adj_cntdown <= adj_cntdown - 1'b1;
              end
          end
        default:
          fsm_sample <= fsm_s_IDLE;
      endcase
  end

  // FSM to coordinate different stages of the sample control logic
  // fsm_c_IDLE = 'h0,
  // fsm_c_TEST_ADC = 'h1,
  // fsm_c_COMPUTE = 'h2,
  // fsm_c_WAIT_FIRST_SYNC = 'h3,
  // fsm_c_WAIT_SYNC = 'h4,
  // fsm_c_GOT_SYNC = 'h5;
  always @(posedge clk)
  begin
    if (resetn == 1'b0 || sresetn == 1'b0)
      fsm_control <= fsm_c_IDLE;
    else
      case (fsm_control)
        // Starts in IDLE, and never goes back there until there is a reset
        fsm_c_IDLE:
          fsm_control <= fsm_c_TEST_ADC;
        // First time out of reset collect some data to use for determining
        // things like:
        // 1. How many clocks from adc_sample going high to CSn going low (sampled in clk domain)
        // 2. How many clocks in one sample cycle (from sample_adc to next sample_adc)
        // Sets the expected_sample_cnt to be used in calculations later.
        fsm_c_TEST_ADC:
          if (test_adc_cnt == STARTUP_SAMPLES && csn_rising)  // test ChrisA 04/14
            fsm_control <= fsm_c_COMPUTE;
        // Do calculations in COMPUTE state to set the desired trigger point in the sample cycle
        // to align pwm_sync with csn falling
        fsm_c_COMPUTE:
           fsm_control <= fsm_c_WAIT_FIRST_SYNC;
        // Wait for the first time pwm_sync happens.  Used to capture some data to compare with
        // next pwm_sync cycles
        fsm_c_WAIT_FIRST_SYNC:
          if (pwm_sync_rising)
            fsm_control <= fsm_c_GOT_SYNC;
        // Wait for all other pwm_sync cycles
        fsm_c_WAIT_SYNC:
          if (pwm_sync_rising)
            fsm_control <= fsm_c_GOT_SYNC;
        // Just indicates that a pwm_sync cycle was seen
        fsm_c_GOT_SYNC:
          // could do a watchdog timer or something, for now just go back to fsm_c_WAIT_SYNC
          fsm_control <= fsm_c_WAIT_SYNC;
        default:
          fsm_control <= fsm_c_IDLE;
      endcase
  end

  // FSM to indicate status to the outside world
  // fsm_stat_STARTUP = 'h0,
  // fsm_stat_DIFF_ZERO = 'h1,
  // fsm_stat_REAQUIRE = 'h3,
  // fsm_stat_LOCK = 'h2;
  always @(posedge clk)
  begin
    if (resetn == 1'b0 || sresetn == 1'b0)
      fsm_status <= fsm_stat_STARTUP;
    else
      case (fsm_status)
        // Starts here.  Wait for difference_sample_and_trigger to have real data, and
        // only then start looking for the lock condition where difference_sample_and_trigger
        // is zero (aligned)
        fsm_stat_STARTUP:
          if (fsm_control == fsm_c_WAIT_SYNC && difference_sample_and_trigger == 0)
            fsm_status <= fsm_stat_DIFF_ZERO;
        fsm_stat_DIFF_ZERO:
          if (lock_cntdown == 0)
            fsm_status <= fsm_stat_LOCK;
        // If pwm_sync and csn falling are aligned, stay in LOCK state.  But if the
        // difference is != 0, go to REAQUIRE state.
        fsm_stat_LOCK:
          if (difference_sample_and_trigger != 0)
            fsm_status <= fsm_stat_REAQUIRE;
        // Wait for LOCK to happen again.  Ideally, this state should never be reached.
        fsm_stat_REAQUIRE:
          if (lock_cntdown == 0)
            fsm_status <= fsm_stat_LOCK;
        default:
          fsm_status <= fsm_stat_STARTUP;
      endcase
  end

  // FSM to keep track of when the ADC is doing a sample and when it is idle.
  // fsm_a_IDLE = 2'b0,
  // fsm_a_BUSY = 2'b1;
  always @(posedge clk)
  begin
    if (resetn == 1'b0 || sresetn == 1'b0)
      fsm_adc <= fsm_a_IDLE;
    else
      case (fsm_adc)
        fsm_a_IDLE:
          if (csn_falling)
            fsm_adc <= fsm_a_BUSY;
        fsm_a_BUSY:
          if (csn_rising)
            fsm_adc <= fsm_a_IDLE;
        default:
          fsm_adc <= fsm_a_IDLE;
      endcase
  end

  // FSM to control test state information gathering when no adjustments are being made.
  // fsm_t_IDLE = 2'b00,
  // fsm_t_UPDATE_ADC = 2'b01,
  // fsm_t_ADC_START = 2'b10,
  // fsm_t_ADC_WAIT_FINISH = 2'b11;
  always @(posedge clk)
  begin
    if (resetn == 1'b0 || sresetn == 1'b0)
      fsm_test <= fsm_t_IDLE;
    else
      case (fsm_test)
        // Start here.  Only executes when the fsm_control state machine is in TEST_ADC state.
        // When sample_adc goes high, starts the machine.
        fsm_t_IDLE:
          if ((fsm_control == fsm_c_TEST_ADC) && sample_adc)
            fsm_test <= fsm_t_UPDATE_ADC;
        // Waits in this state until the csn falling edge is seen in the clk domain.
        // The actual CSn signal is in the SCLK domain, which is assumed to be slower than clk.
        fsm_t_UPDATE_ADC:
          if (csn_falling)
            fsm_test <= fsm_t_ADC_START;
        // This state is not actually used for anything - could be used in the future.
        fsm_t_ADC_START:
          fsm_test <= fsm_t_ADC_WAIT_FINISH;
        // This state waits for the csn rising edge in the clk domain which indicates the end of
        // the sample.
        fsm_t_ADC_WAIT_FINISH:
          if (csn_rising)
            fsm_test <= fsm_t_IDLE;
        default:
          fsm_test <= fsm_t_IDLE;
      endcase
  end

  // FSM to control the sample_adc signal to the ADC.  It is responsible for adjusting
  // the wait time before asserting sample_adc based on calculations done in some of the
  // other state machines.  There are two kinds of delays - remainder and adjustments.
  // The remainder delay is to make sure that we finish whole samples during the pwm_cycle,
  // instead of having partial samples at the end or beginning of the cycle.  If no adjustment
  // delays are made, using only remainder delays should result in a constant sync_edge_sample_cnt.
  // The remainder is calculated during the first pwm_sync cycle and stored.
  // Adjustment delays are used to align the pwm_sync rising edge to the csn falling edge.
  // Both will probably need to be done during the same cycle in the beginning.  After LOCK is
  // achieved, then only the remainder delays will continue.
  // fsm_u_IDLE = 2'h0,
  // fsm_u_UPDATE = 2'h1,
  // fsm_u_REMAINDER = 2'h2,
  // fsm_u_ADJUST = 2'h3;
  always @(posedge clk)
  begin
    if (resetn == 1'b0 || sresetn == 1'b0)
      fsm_update <= fsm_u_IDLE;
    else
      case (fsm_update)
        // Starts here and returns here after every sample_adc is generated.
        // Check for add_delay (remainder delay) and if it is required, go to REMAINDER state.
        // Otherwise, check for adjustment delay (from fsm_sample state machine), and if adjustment
        // is required, go to ADJUST state.  Otherwise, go straight to UPDATE state.
        // In each case, transition only when at the end of the sample cycle based on
        // expected_sample_cnt.
        fsm_u_IDLE:
          if (add_delay && (sample_cnt == (expected_sample_cnt)))
            fsm_update <= fsm_u_REMAINDER;
          else if ((DEBUG == 0) && (fsm_sample == fsm_s_ADJUST) && (sample_cnt == (expected_sample_cnt)))
            fsm_update <= fsm_u_ADJUST;
          else if (sample_cnt == (expected_sample_cnt))
            fsm_update <= fsm_u_UPDATE;
        // In UPDATE state, the sample_adc will be driven high.  Go back to IDLE when csn has
        // fallen to start the sampling process.
        fsm_u_UPDATE:
          if (csn_falling)
            fsm_update <= fsm_u_IDLE;
        // Enter this state if a remainder delay needs to be used.  Remainder delays are only one
        // clock each sample until the correct number of delays have been used.  Then they are not used
        // again until the next pwm_sync cycle.  If an adjustment also needs to be made, transition
        // directly to the ADJUST state from REMAINEDER.
        fsm_u_REMAINDER:
          if (fsm_sample == fsm_s_ADJUST && sample_cnt == expected_sample_cnt)
            fsm_update <= fsm_u_ADJUST;
          else
            fsm_update <= fsm_u_UPDATE;
        // Enter this state only if an adjustment is necessary.  The amount of time spent in this state
        // depends on the adj_cntdown value (maximum is MAX_CHANGE_CLKS) set and controlled by the
        // fsm_sample state machine. Exit when the countdown timer reaches one.
        // Go directly to UPDATE at the end.
        fsm_u_ADJUST:
          if (adj_cntdown == 1)
            fsm_update <= fsm_u_UPDATE;
        default:
          fsm_update <= fsm_u_IDLE;
      endcase
  end

endmodule

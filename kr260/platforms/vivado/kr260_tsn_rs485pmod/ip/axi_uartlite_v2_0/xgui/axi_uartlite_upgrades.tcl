namespace eval axi_uartlite_v2_0_utils {

#        proc warning_for_upgrade_from_axi_uartlite_v1_02_a {} {

#            return "Upgrading from AXI UARTLite core version 1.02.a to version 2.0 will provide this warning message. User need to make sure about the updates in the parameters while upgrading to the new version.\n"
#        }

        proc upgrade_from_axi_uartlite_v1_02_a {xciValues} {

          namespace import ::xcoUpgradeLib::*
          upvar $xciValues valueArray

          removeParameter c_instance valueArray

	  return
        }

}


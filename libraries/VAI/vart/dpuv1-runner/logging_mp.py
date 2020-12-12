# Copyright 2019 Xilinx Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""
This module provides a MP-safe file logger class for logging and provides a helper
function setup_logger().

To use, import this module in your application code and call setup_logger()
A prefix for the log file names can also be specified here (see the docstring
for setup_logger). See $VAI_ALVEO_ROOT/neptune/server.py for an example of how to setup
the logging in your own application, as well as logging.ini for an example
configuration file.

Then, in code, you can use create a logger e.g.
import logging
logger = logging.getLogger(__name__)

And use logger.<debug, info, status, warning, error, critical> to log the
message to the log.

MP Logging taken from: https://gist.github.com/JesseBuesking/10674086
"""

from datetime import datetime
import logging, logging.config
import os
import multiprocessing
import sys
import threading
import traceback

# create logging directory if it doesn't exist
logging_directory = './logs/'
if not os.path.exists(logging_directory):
    print("making %s for logs" % logging_directory)
    os.makedirs(logging_directory)

STATUS_LEVEL = 25

def status(self, message, *args, **kws):
    if self.isEnabledFor(STATUS_LEVEL):
        # Yes, logger takes its '*args' as 'args'.
        self._log(STATUS_LEVEL, message, args, **kws)

def setup_logger(filepath, prefix=None):
    """
    Sets up logging for this application based on the configuration file provided

    Args:
        filepath (str): Path to the logging .ini file
        prefix (str): Name to append to the log name
    """

    # add custom logging level to filter out application INFO-type messages
    # from INFO messages from imported libraries
    logging.addLevelName(STATUS_LEVEL, "STATUS")
    logging.Logger.status = status

    # log names are taken as the current date
    log_name = '{:%Y-%m-%d}.log'.format(datetime.now())
    if prefix is not None:
        log_name = prefix + '_' + log_name

    # configure the logger based on the .ini file and return to user
    logging.config.fileConfig(
        filepath,
        disable_existing_loggers=False,
        defaults={'logfilename': logging_directory + log_name}
    )

class MultiProcessingLog(logging.Handler):
    def __init__(self, name):
        logging.Handler.__init__(self)

        self._handler = logging.FileHandler(name)
        self.queue = multiprocessing.Queue(-1)

        self.t = threading.Thread(target=self.receive)
        self.t.daemon = True
        self.t.start()

    def setFormatter(self, fmt):
        logging.Handler.setFormatter(self, fmt)
        self._handler.setFormatter(fmt)

    def receive(self):
        while True:
            try:
                record = self.queue.get()
                if not record:
                    break
                self._handler.emit(record)
                # print('received on pid {}'.format(os.getpid()))
            except (KeyboardInterrupt, SystemExit):
                raise
            except EOFError:
                break
            except:
                traceback.print_exc(file=sys.stderr)

    def send(self, s):
        self.queue.put_nowait(s)

    def _format_record(self, record):
        # ensure that exc_info and args have been stringified. Removes any
        # chance of unpickleable things inside and possibly reduces message size
        # sent over the pipe
        if record.args:
            record.msg = record.msg % record.args
            record.args = None
        if record.exc_info:
            dummy = self.format(record)
            record.exc_info = None

        return record

    def emit(self, record):
        try:
            s = self._format_record(record)
            self.send(s)
        except (KeyboardInterrupt, SystemExit):
            raise
        except:
            self.handleError(record)

    def close(self):
        self.queue.put_nowait(None)
        self.t.join()
        self._handler.close()
        logging.Handler.close(self)

class TraceSupressionFormatter(logging.Formatter):
    def formatException(self, exc_info):
        """
        Overrides the default formatException method in logging.Formatter to
        disable printing the stack trace

        Args:
            exc_info (tuple): exc_info from sys

        Returns:
            [str]: The formatted string for the exception
        """
        # result = super(TraceSupressionFormatter, self).formatException(exc_info)
        return ''

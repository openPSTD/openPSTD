########################################################################
#                                                                      #
# This file is part of openPSTD.                                       #
#                                                                      #
# openPSTD is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or    #
# (at your option) any later version.                                  #
#                                                                      #
# openPSTD is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of       #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
# GNU General Public License for more details.                         #
#                                                                      #
# You should have received a copy of the GNU General Public License    #
# along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    #
#                                                                      #
########################################################################

__author__ = 'michiel'

import abc
import helper
import threading

class Reciever:
    def __init__(self, model, ui):
        self.model = model
        self.ui = ui


class operation:
    __metaclass__ = abc.ABCMeta

    def run(self, r):
        pass

STATUS_NONE = 1
STATUS_RUNNING = 2
STATUS_FINISHED = 3
STATUS_ERROR = 4

class LongOperation:
    __metaclass__ = abc.ABCMeta
    def __init__(self):
        self.statusTextChanged = []
        self.statusText = 'None'
        self.statusChanged = []
        self.status = STATUS_NONE
        self._thread = threading.Thread(target=self.process)
        self._r = None

    def start(self):
        self._thread.start()

    def _process(self):
        self._change_status(STATUS_RUNNING)
        success = self.process(self._r)
        if success is None or success is False:
            self._change_status(STATUS_ERROR)
        else:
            self._change_status(STATUS_FINISHED)


    def process(self, r):
        return False

    def change_status_text(self, new_status_text):
        self.statusText = new_status_text
        helper.CallObservers(self.statusTextChanged)

    def _change_status(self, newStatus):
        self.status = newStatus
        helper.CallObservers(self.statusChanged)
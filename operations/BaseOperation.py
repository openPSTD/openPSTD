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

from abc import ABCMeta, abstractmethod
import threading
import traceback

import helper


class OperationRunner:
    __metaclass__ = ABCMeta

    @abstractmethod
    def run_operation(self, op):
        """
        Run an operation. This is an empty method and should be overridden in a subclass

        :rtype : operation
        :param op: The operation that has to be executed.
        """
        pass

class Receiver(OperationRunner):
    def __init__(self, model, ui):
        self.model = model
        self.ui = ui

    def run_operation(self, op):
        """
        Run an operation on the same receiver.

        :type op: operation
        :param op: The operation that has to be executed.
        """
        op.run(self)


class operation(metaclass=ABCMeta):

    @abstractmethod
    def run(self, r: Receiver):
        """
        Makes the operation run. This is an empty method in the base class and should be overridden in the subclass.

        :type r: Receiver
        :param r: The receiver that the operation should use.
        """
        pass

STATUS_NONE = 1
STATUS_RUNNING = 2
STATUS_FINISHED = 3
STATUS_ERROR = 4

class LongOperation(metaclass=ABCMeta):

    def __init__(self):
        self.statusTextChanged = []
        self.statusText = 'None'
        self.statusChanged = []
        self.status = STATUS_NONE
        self._thread = threading.Thread(target=self._process)
        self._r = None
        """:type: Receiver"""


    def start(self, r: Receiver):
        """
        starts the operation

        :type r: Receiver
        :param r: the receiver that the operation must run on.
        """
        self._r = r
        self._thread.start()

    def _process(self):
        self._change_status(STATUS_RUNNING)
        try:
            success = self.process(self._r)
            if success is None or success is False:
                self._change_status(STATUS_ERROR)
            else:
                self._change_status(STATUS_FINISHED)
        except Exception as inst:
            print(type(inst))
            print(inst.args)
            print(inst)
            print(traceback.format_exc())
            self._change_status(STATUS_ERROR)

    @abstractmethod
    def process(self, r: Receiver):
        """
        The process that is called when this operation is run. This operation is run in a seperate thread, so that other
        threads still can continue for UX.

        :type r: Receiver
        :param r: the receiver that the operation can run on.
        """
        return False

    def change_status_text(self, new_status_text: str):
        """
        Change the status text

        :type new_status_text: str
        """
        self.statusText = new_status_text
        helper.CallObserversQT(self.statusTextChanged)

    def _change_status(self, new_status: int):
        """
        Change the status of the operation. This can be the following values: STATUS_NONE, STATUS_RUNNING,
        STATUS_FINISHED, STATUS_ERROR.

        :param new_status: The new status.
        """
        self.status = new_status
        helper.CallObserversQT(self.statusChanged)
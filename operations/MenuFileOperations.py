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

import operations.BaseOperation
import operations.ViewOperations
import helper
import json

class Open(operations.BaseOperation.operation):
    def __init__(self, filename):
        self.filename = filename

    def run(self, receiver):
        f = open(self.filename, 'r')
        receiver.model.SceneDesc = json.load(f)
        f.close()
        helper.CallObservers(receiver.model.SceneDescChanged)
        receiver.run_another_operation(operations.ViewOperations.ViewWholeScene())


class Save(operations.BaseOperation.operation):
    def __init__(self, filename):
        self.filename = filename

    def run(self, r):
        f = open(self.filename, 'w')
        json.dump(r.model.SceneDesc, f, sort_keys=True, indent=4, separators=(',', ': '))
        f.close()

class New(Open):
    emptyDocumentFilename = "emptyFile.jps"
    #emptyDocumentFilename = "testFile.jps"

    def __init__(self):
        super(New, self).__init__(New.emptyDocumentFilename)
        print("WARNING: loading test file in place of the standard empty file")

class Close(operations.BaseOperation.operation):
    def run(self, r):
        r.ui.close()
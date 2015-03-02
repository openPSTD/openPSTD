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

import helper
import json

class model:
    #emptyDocumentFilename = "emptyFile.jps"
    emptyDocumentFilename = "testFile.jps"

    def __init__(self):
        self.SceneDesc = None
        self.Simulation = None
        self.AppConfig = None

        self.SceneDescChanged = []
        self.SimulationChanged = []

    def loadEmptyDocument(self):
        print("WARNING: loading test file in place of the standard empty file")
        self.load(model.emptyDocumentFilename)

    def load(self, filename):
        f = open(filename, 'r')
        self.SceneDesc = json.load(f)
        f.close()
        helper.CallObservers(self.SceneDescChanged)


    def save(self, filename):
        f = open(filename, 'w')
        json.dump(self.SceneDesc, f)
        f.close()

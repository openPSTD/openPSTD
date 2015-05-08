########################################################################
# #
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

import json
import argparse
import tempfile

import SimulationReader
import helper
import InteractiveView


__author__ = 'michiel'


class Model:
    def __init__(self):
        self.debug_data = {
            'viewer':
                {
                    'positions': [[0, 0] for _ in range(12)],
                    'Colors': [(1, 0, 0, 1)for _ in range(12)]
                }
        }

        self.SceneDesc = None
        self.Simulation = SimulationReader.reader()
        self.AppConfig = None
        self.Args = None
        self.visible_frame = 1
        self.interactivity = InteractiveView.InteractiveView()

        self.Simulation.frame_info_updated.append(self._Simulation_frame_info_updated)

        self.SceneDescChanged = []
        self.SimulationChanged = []

        # parse arguments of the command line
        parser = argparse.ArgumentParser(prog="openPSTD", description="Stand-alone application openPSTD")
        parser.add_argument('-c', '--config', action="store", help="The configuration file",
                            default="configuration.json")
        self.Args = parser.parse_args()

        # load configuration
        self._load_config()

    def _load_config(self):
        f = open(self.Args.config, 'r')
        self.AppConfig = json.load(f)
        f.close()

    def get_calculation_directory(self):
        if self.AppConfig['working_dir'] is None:
            return tempfile.gettempdir()
        else:
            return self.AppConfig['working_dir']

    def _Simulation_frame_info_updated(self):
        helper.CallObservers(self.SimulationChanged)
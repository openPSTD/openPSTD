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
import os
import copy
from kernel.pstd import PSTD

class Simulate(operations.BaseOperation.LongOperation):

    class PstdException(Exception):
        def __init__(self, error_status):
            self.error_status = error_status
        def __str__(self):
            return repr(self.error_status['message'])

    def __init__(self):
        super(Simulate, self).__init__()
        self.pstd_meshdata = None
        self.create_array = True
        self.create_plots = False

    def process(self, r):
        scene_desc = copy.deepcopy(r.model.SceneDesc)

        scene_desc['plotdir'] = os.path.join(r.model.get_calculation_directory(), scene_desc['plotdir'])

        print("Output directory: " + scene_desc['plotdir'])

        r.model.Simulation.simulation_scene_desc = scene_desc
        r.model.Simulation.simulation_result_path = scene_desc['plotdir']

        result = False
        def status_messages(status):
            nonlocal result
            nonlocal self

            self.change_status_text(status['message'])
            if status['status'] == 'error':
                result = False
                raise Simulate.PstdException(status)

            elif status['status'] == 'starting':
                self.pstd_meshdata = status['pstd_desc']
                r.model.Simulation.simulation_result_pstd_mesh = status['pstd_desc']

            elif status['status'] == 'running':
                r.model.Simulation.update_frame_info()

            elif status['status'] == 'success':
                result = True

        try:
            pstd = PSTD(False, self.create_plots, self.create_array, scene_desc, status_messages)
            pstd.run()
        except Simulate.PstdException as e:
            pass

        return result

class CreateMovie(operations.BaseOperation.operation):
    def __init__(self):
        pass

    def run(self, r):
        pass
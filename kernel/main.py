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

import argparse
from core.classes import *
from pstd import PSTD

# Parse arguments from command line
parser = argparse.ArgumentParser(prog="openPSTD",
                                 description="Stand-alone application openPSTD")
parser.add_argument('scene_file',help="JSON file containing scene description")
parser.add_argument('-m','--multi-threaded', action="store_true",help="Run openPSTD multi-threaded")
parser.add_argument('-p','--write-plot', action="store_true",help="Write plot to image (only when matplotlib is installed)")
parser.add_argument('-a','--write-array',action="store_true",help="Write array to file")

interpreter = os.path.basename(sys.executable)
# Blender is invoked differently: blender -b -P script.py [args]
if interpreter.startswith("blender"):
    parser.add_argument('-b',action="store_true",help="Flag from blender interpreter. Not for stand-alone use")
    parser.add_argument('-P',action="store_true",help="Flag from blender interpreter. Not for stand-alone use")
else:
    stand_alone=True

args = parser.parse_args()

pstd = PSTD(args.multi_threaded,args.write_plot,args.write_array,args.scene_file)
pstd.run()
# Exit to prevent the Blender interpreter from processing
    # the subsequent command line arguments.
exit()
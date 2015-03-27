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
import json

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

interpreter = os.path.basename(sys.executable)
stand_alone = not(interpreter.startswith("blender"))

def ConsoleOutput(arg):
    if not stand_alone:
        pickle.dump(arg,sys.stdout,0)
        if arg['status']=='error':
            exit(0)
    else:
        if arg['status'] == 'error':
            print('\n')
            print('Error encountered while running openPSTD:')
            print(arg['message'])
            exit(1)
        elif arg['status'] == 'starting':
            print("\n%s\n"%("-"*20))
            print(arg['scene'])
            print("\n%s\n"%("-"*20))
        elif arg['status'] == 'running':
            sys.stdout.write("\r%d"%(arg['frame']+1))
            sys.stdout.flush()
        elif arg['status'] == 'success':
            print(arg['message'])


# Use binary mode IO in Python 3+
if not stand_alone:
    if hasattr(sys.stdin, 'detach'): sys.stdin = sys.stdin.detach()
    if hasattr(sys.stdout, 'detach'): sys.stdout = sys.stdout.detach()

if not has_matplotlib and stand_alone and args.write_plot:
    print('Warning: matplotlib not installed, plotting to image files not supported')

if stand_alone:
    if args.scene_file:
        f = open(args.scene_file, 'r')
        scene_desc = json.load(f)
        f.close()
        os.chdir(os.path.dirname(os.path.abspath(args.scene_file)))
    else:
        output_error("Specify scene file", ConsoleOutput)
        exit(1)
else:
    scene_desc = pickle.load(sys.stdin)

pstd = PSTD(args.multi_threaded, args.write_plot, args.write_array, scene_desc, ConsoleOutput)
pstd.run()
# Exit to prevent the Blender interpreter from processing
# the subsequent command line arguments.

exit()
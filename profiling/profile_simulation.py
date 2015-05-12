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

import os, sys
import argparse
import pickle
import json,time
from datetime import datetime
import yappi
sys.path.append(os.path.abspath('..'))
from kernel.pstd import PSTD
from kernel.core.functions import output_error


if __name__ == "__main__":
    # Parse arguments from command line
    parser = argparse.ArgumentParser(prog="openPSTD",
                                     description="Stand-alone application openPSTD")
    parser.add_argument('scene_file',help="JSON file containing scene description")
    parser.add_argument('-m','--multi-threaded', action="store_true",help="Run openPSTD multi-threaded")
    parser.add_argument('-p','--write-plot', action="store_true",help="Write plot to image (only when matplotlib is installed)")
    parser.add_argument('-a','--write-array',action="store_true",help="Write array to file")
    parser.add_argument('-f','--file',default="",type=str,help="Write profiling output to specified file")
    interpreter = os.path.basename(sys.executable)
    # Blender is invoked differently: blender -b -P script.py [args]
    stand_alone=True

    args = parser.parse_args()

    interpreter = os.path.basename(sys.executable)

    def output_function(arg):
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

    if stand_alone:
        if args.scene_file:
            f = open(args.scene_file, 'r')
            scene_desc = json.load(f)
            f.close()
        else:
            output_error("Specify scene file", output_function)
            exit(1)
    else:
        scene_desc = pickle.load(sys.stdin)
    pstd = PSTD(args.multi_threaded, args.write_plot, args.write_array, scene_desc, output_function)
    out = sys.__stdout__
    if args.file:
        print("Writing to %s"%args.file)
        out = open(args.file,'w')
        out.write("openPSTD --- %s\n"%datetime.now().strftime('%d-%m-%y %H:%M'))
        out.write("Profile output\n"
                  "Scene file %s\n"
                  "Parallel: %s\n"
                  "Plotting: %s\n"
                  "Writing arrays: %s\n\n"%(args.scene_file,args.multi_threaded,args.write_plot,args.write_array))
    time_start = time.time()
    yappi.start()
    pstd.run()
    yappi.stop()
    time_passed = time.time() - time_start
    out.write("Time passed: %.3f\n"%time_passed)
    funcstats = yappi.get_func_stats()
    funcstats.sort(sort_type='tsub', sort_order="desc")
    funcstats.print_all(out=out,columns={0:("name",36), 1:("ncall", 8), 2:("tsub", 8), 3:("ttot", 8), 4:("tavg",8)})
    yappi.get_thread_stats().print_all(out=out)
    if args.file:
        out.close()
    exit()
# yappi.start()
# foo()

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

import sys
import wave
import array
import struct
import argparse
import numpy as np

try: from scipy.interpolate import interp1d
except: pass

try: range = xrange
except: pass

parser = argparse.ArgumentParser()
parser.add_argument("--resample", type=int, help="destination file sample rate")
parser.add_argument("--normalize", action='store_true', help="whether to normalize the output file")
parser.add_argument("--rk", type=float, help="CFL number RK-scheme")
parser.add_argument("--dx", type=float, help="grid spacing")
parser.add_argument("--c1", type=float, help="sound speed of air")
parser.add_argument("-b", action='store_true', help="bogus argument for Blender command line")
parser.add_argument("-P", type=str, help="bogus argument for Blender command line")
parser.add_argument("source")
parser.add_argument("destination")
args = parser.parse_args()

f = open(args.source,"rb")
f.seek(0, 2)
fs = f.tell()
f.seek(0, 0)

num_samples = fs // struct.calcsize('f')
samples = array.array('f')
samples.fromfile(f, num_samples)

f.close()

def resample(old_xs, ys, new_xs):
    def resample_part(old_xs, ys, new_xs, s, n, w=20):
        spline = interp1d(old_xs[max(s-w,0):s+n+w], ys[max(s-w,0):s+n+w], kind='cubic')
        xmin, xmax = old_xs[s:s+n].min(), old_xs[s:s+n].max()
        return spline(new_xs[(new_xs >= xmin) & (new_xs <= xmax)])
    return np.concatenate([resample_part(old_xs,ys,new_xs,a,100) for a in range(0,len(old_xs),100)])
    
def write_wav(arr, fn, sr):
    maxy = 2**15-1
    a = array.array('h',(int(y*maxy) for y in arr))
    w = wave.open(fn,'wb')
    w.setparams((1,2,sr,0,'NONE','not compressed'))
    w.writeframes(a.tostring())
    
def write_bin(arr, fn, sr):
    f = open(fn, 'wb')
    arr.tofile(f)
    f.close()
    
write_fn = write_wav if args.destination.lower().endswith('.wav') else write_bin
sample_rate = 1. / (args.rk * args.dx / args.c1)

if args.resample is not None: 
    if interp1d:
        dtRK = 1. / float(sample_rate)
        new_sample_rate = args.resample
            
        old_xs = np.linspace(0, num_samples * dtRK, num_samples, endpoint=False)
        new_xs = np.linspace(0, old_xs.max(), old_xs.max() * new_sample_rate, endpoint=False)
        samples = resample(old_xs, samples, new_xs)
        sample_rate = new_sample_rate
    else:
        print ("Resampling depends on the scipy module being installed")
        exit(1)
else:
    samples = np.array(samples)

if args.normalize:
    abs_max = np.max(samples.max(),np.abs(samples.min()))
    if abs_max > 1e-12:
        samples /= abs_max

write_fn(samples, args.destination, int(sample_rate))

sys.exit()

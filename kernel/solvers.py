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

import os
import sys
import time
import math
import struct
import multiprocessing as mp
import shutil
import pickle
import threading
#from pstd import exit_with_error,subsample,write_array_to_file,write_plot_to_file


from core.classes import *

try:
    import ConfigParser as configparser
except:
    import configparser

# Abbreviate different type of calculation directions and measures
h, v = BoundaryType.HORIZONTAL, BoundaryType.VERTICAL
P, V = CalculationType.PRESSURE, CalculationType.VELOCITY


class SingleThreaded:
    def __init__(self, cfg, scene, data_writer, receiver_files, output_fn):
        # Loop over time steps
        for frame in range(int(cfg.TRK)):
            output_fn({'status': 'running', 'message': "Calculation frame:%d" % (frame + 1), 'frame': frame + 1})

            # Keep a reference to current matrix contents
            for d in scene.domains: d.push_values()

            # Loop over subframes
            for subframe in range(6):
                # Loop over calculation directions and measures
                for boundary_type, calculation_type in [(h, P), (v, P), (h, V), (v, V)]:
                    # Loop over domains
                    for d in scene.domains:
                        if not d.is_rigid():
                            # Calculate sound propagations for non-rigid domains
                            if d.should_update(boundary_type):
                                def calc(domain, bt, ct):
                                    domain.calc(bt, ct)

                                calc(d, boundary_type, calculation_type)

                for domain in scene.domains:
                    if not domain.is_rigid():
                        def calc(d):
                            d.u0 = d.u0_old + (-cfg.dtRK * cfg.alfa[subframe] * ( 1 / d.rho * d.Lpx)).real
                            d.w0 = d.w0_old + (-cfg.dtRK * cfg.alfa[subframe] * ( 1 / d.rho * d.Lpz)).real
                            d.px0 = d.px0_old + (
                            -cfg.dtRK * cfg.alfa[subframe] * (d.rho * pow(cfg.c1, 2.) * d.Lvx)).real
                            d.pz0 = d.pz0_old + (
                            -cfg.dtRK * cfg.alfa[subframe] * (d.rho * pow(cfg.c1, 2.) * d.Lvz)).real

                        calc(domain)


                # Sum the pressure components
                for d in scene.domains:
                    d.p0 = d.px0 + d.pz0

            # Apply pml matrices to boundary domains
            scene.apply_pml_matrices()

            for rf, r in zip(receiver_files, scene.receivers):
                rf.write(struct.pack('f', r.calc()))
                rf.flush()

            if frame % cfg.save_nth_frame == 0:
                data_writer.write_to_file(frame)


class MultiThreaded:
    def __init__(self, cfg, scene, data_writer, receiver_files, output_fn):
        # Loop over time steps
        pool = mp.Pool(processes=8)
        for frame in range(int(cfg.TRK)):
            output_fn({'status': 'running', 'message': "Calculation frame:%d" % (frame + 1), 'frame': frame + 1})

            # Keep a reference to current matrix contents
            for domain in scene.domains: domain.push_values()

            # Loop over sub-frames
            for sub_frame in range(6):
                job_list = []
                # Loop over calculation directions and measures
                for boundary_type, calculation_type in [(h, P), (v, P), (h, V), (v, V)]:
                    # Loop over domains
                    for domain in scene.domains:
                        if not domain.is_rigid():
                            # Calculate sound propagations for non-rigid domains
                            if domain.should_update(boundary_type):
                                def calc(domain, bt, ct):
                                    domain.calc(bt, ct)
                                try:
                                    job_list.append(pool.apply_async(calc,[domain,boundary_type,calculation_type]))

                                except Exception as e:
                                    output_error(e, output_fn)
                [job.wait() for job in job_list]
                job_list = []
                for domain in scene.domains:
                    if not domain.is_rigid():
                        def calc(d):
                            d.u0 = d.u0_old + (-cfg.dtRK * cfg.alfa[sub_frame] * ( 1 / d.rho * d.Lpx)).real
                            d.w0 = d.w0_old + (-cfg.dtRK * cfg.alfa[sub_frame] * ( 1 / d.rho * d.Lpz)).real
                            d.px0 = d.px0_old + (
                            -cfg.dtRK * cfg.alfa[sub_frame] * (d.rho * pow(cfg.c1, 2.) * d.Lvx)).real
                            d.pz0 = d.pz0_old + (
                            -cfg.dtRK * cfg.alfa[sub_frame] * (d.rho * pow(cfg.c1, 2.) * d.Lvz)).real

                        try:
                            job_list.append(pool.apply_async(calc,[domain]))
                        except Exception as e:
                            output_error(e, output_fn)
                [job.wait() for job in job_list]


                # Sum the pressure components
                for domain in scene.domains:
                    domain.p0 = domain.px0 + domain.pz0

            # Apply pml matrices to boundary domains
            scene.apply_pml_matrices()

            for rf, r in zip(receiver_files, scene.receivers):
                rf.write(struct.pack('f', r.calc()))
                rf.flush()

            if frame % cfg.save_nth_frame == 0:
                data_writer.write_to_file(frame)
        pool.close()


class GpuAccelerated:
    def __init__(self, cfg, scene, data_writer, receiver_files, output_fn):
            import pycuda.driver as cuda
            from pycuda.tools import make_default_context
            from pycuda.compiler import SourceModule
            from pycuda.driver import Function

            cuda.init()
            context = make_default_context()
            stream = cuda.Stream()

            plan_set = {} #will be filled as needed in spatderp3_gpu(~)
            g_bufr = cuda.mem_alloc(int(8*500*128)) #temporary buffers. TODO: dynamically grow them
            g_bufi = cuda.mem_alloc(int(8*500*128))            

            kernelcode = SourceModule(""" 
              __global__ void derifact_multiplication(double *matr, double *mati, double *vecr, double *veci, int fftlen, int fftnum)
              {
                int index_x = blockIdx.x*blockDim.x + threadIdx.x; 
                int index_y = blockIdx.y*blockDim.y + threadIdx.y;

                int matindex = index_y*fftlen+index_x; //mat should be a contiguous array
                
                // if N1%16>0, we're starting too many threads.
                // There is probably a better way to do this, but just eating the surplus should work.
                if (matindex < fftlen*fftnum) {
                    double matreal = matr[matindex];
                    double matimag = mati[matindex];
                    double vecreal = vecr[index_x];
                    double vecimag = veci[index_x];

                    matr[matindex] = matreal*vecreal - matimag*vecimag;
                    mati[matindex] = matreal*vecimag + matimag*vecreal;
                }
              }
              """)
            mulfunc = kernelcode.get_function("derifact_multiplication")

            # Loop over time steps
            for frame in range(int(cfg.TRK)):
                output_fn({'status': 'running', 'message': "Calculation frame:%d" % (frame + 1), 'frame': frame + 1})

                # Keep a reference to current matrix contents
                for d in scene.domains: d.push_values()

                # Loop over subframes
                for subframe in range(6):
                    # Loop over calculation directions and measures
                    for boundary_type, calculation_type in [(h, P), (v, P), (h, V), (v, V)]:
                        # Loop over domains
                        for d in scene.domains:
                            if not d.is_rigid():
                                # Calculate sound propagations for non-rigid domains
                                if d.should_update(boundary_type):
                                    def calc_gpu(domain, bt, ct, context, stream, plan_set,g_bufr,g_bufi,mulfunc):
                                        domain.calc_gpu(bt, ct, context, stream, plan_set,g_bufr,g_bufi,mulfunc)

                                    calc_gpu(d, boundary_type, calculation_type, context, stream, plan_set,g_bufr,g_bufi,mulfunc)

                    for domain in scene.domains:
                        if not domain.is_rigid():
                            def calc(d):
                                d.u0 = d.u0_old + (-cfg.dtRK * cfg.alfa[subframe] * ( 1 / d.rho * d.Lpx)).real
                                d.w0 = d.w0_old + (-cfg.dtRK * cfg.alfa[subframe] * ( 1 / d.rho * d.Lpz)).real
                                d.px0 = d.px0_old + (
                                -cfg.dtRK * cfg.alfa[subframe] * (d.rho * pow(cfg.c1, 2.) * d.Lvx)).real
                                d.pz0 = d.pz0_old + (
                                -cfg.dtRK * cfg.alfa[subframe] * (d.rho * pow(cfg.c1, 2.) * d.Lvz)).real

                            calc(domain)


                    # Sum the pressure components
                    for d in scene.domains:
                        d.p0 = d.px0 + d.pz0

                # Apply pml matrices to boundary domains
                scene.apply_pml_matrices()

                for rf, r in zip(receiver_files, scene.receivers):
                    rf.write(struct.pack('f', r.calc()))
                    rf.flush()

                if frame % cfg.save_nth_frame == 0:
                    data_writer.write_to_file(frame)

            context.pop()

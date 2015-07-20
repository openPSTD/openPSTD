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
import numpy, scipy.io
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
    #@profile
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

            #debug: check if output of solvers is same
            if frame == int(cfg.TRK) - 1:
                cnt=0
                for d in scene.domains:
                    if not d.is_rigid():
                        scipy.io.savemat('finalresCPU'+str(cnt), mdict={'resCPU'+str(cnt): d.p0})
                        cnt = cnt+1


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
    #@profile
    def __init__(self, cfg, scene, data_writer, receiver_files, output_fn):
        use_cuda = False
        use_opencl = False
        print "importing gpu libs"
        try:
            Error("PREFER OCL") #TODO remove
            import pycuda.driver as cuda
            from pycuda.tools import make_default_context
            from pycuda.compiler import SourceModule
            from pycuda.driver import Function
            from pyfft.cuda import Plan
            use_cuda = True
            print "USING CUDA"
        except:
            try:
                import pyopencl as cl
                from pyfft.cl import Plan
                use_opencl = True
                print "USING OpenCL"
            except:
                print "NEITHER CUDA NOR OPENCL AVAILABLE. ABORTING."
                exit()

        if use_cuda:        
            cuda.init()
            context = make_default_context()
            stream = cuda.Stream()

            plan_set = {}
            len_max = area_max = 0
            for domain in scene.domains:
                dim = domain.bottomright - domain.topleft
                if dim[0] > len_max: len_max = int(dim[0])
                if dim[1] > len_max: len_max = int(dim[1])
                if dim[0]*nearest_2power(dim[1]+cfg.Wlength*2) > area_max: area_max = int(dim[0]*nearest_2power(dim[1]+cfg.Wlength*2))
                if dim[1]*nearest_2power(dim[0]+cfg.Wlength*2) > area_max: area_max = int(dim[1]*nearest_2power(dim[0]+cfg.Wlength*2))
                if str(int(nearest_2power(dim[0]+cfg.Wlength*2))) not in plan_set.keys():
                    plan_set[str(int(nearest_2power(dim[0]+cfg.Wlength*2)))] = Plan(int(nearest_2power(dim[0]+cfg.Wlength*2)), dtype=np.float64, stream=stream, context=context, fast_math=False)
                if str(int(nearest_2power(dim[0]+cfg.Wlength*2))) not in plan_set.keys():
                    plan_set[str(int(nearest_2power(dim[1]+cfg.Wlength*2)))] = Plan(int(nearest_2power(dim[1]+cfg.Wlength*2)), dtype=np.float64, stream=stream, context=context, fast_math=False)

            g_bufl = {} #m/d(r/i) -> windowed matrix/derfact real/imag buffers. m(1/2/3)->p(#) buffers. spatderp3 will expand them if needed
            g_bufl["mr"] = cuda.mem_alloc(8*area_max)
            g_bufl["mi"] = cuda.mem_alloc(8*area_max)
            g_bufl["m1"] = cuda.mem_alloc(8*area_max)
            g_bufl["m2"] = cuda.mem_alloc(8*area_max)
            g_bufl["m3"] = cuda.mem_alloc(8*area_max)
            g_bufl["m_size"] = 8*area_max
            g_bufl["dr"] = cuda.mem_alloc(8*len_max) #dr also used by A (window matrix)
            g_bufl["di"] = cuda.mem_alloc(8*len_max)
            g_bufl["d_size"] = 8*len_max

            script_dir = os.path.dirname(__file__)
            rel_path = "cuda_kernels.cu"
            abs_file_path = os.path.join(script_dir, rel_path)                                    
            src_file = open(abs_file_path, 'r')
                                                
            kernelcode = SourceModule(src_file.read())
            mulfunc = {}
            mulfunc["pres_window"] = kernelcode.get_function("pressure_window_multiplication")
            mulfunc["velo_window"] = kernelcode.get_function("velocity_window_multiplication")
            mulfunc["derifact"] = kernelcode.get_function("derifact_multiplication")

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
                                    def calc_cuda(domain, bt, ct, context, stream, plan_set,g_bufl,mulfunc):
                                        domain.calc_cuda(bt, ct, context, stream, plan_set,g_bufl,mulfunc)
                                        #domain.calc(bt, ct)

                                    calc_cuda(d, boundary_type, calculation_type, context, stream, plan_set,g_bufl,mulfunc)

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

                #debug: check if output of solvers is same
                if frame == int(cfg.TRK) - 1:
                    cnt=0
                    for d in scene.domains:
                        if not d.is_rigid():
                            scipy.io.savemat('finalresGPU'+str(cnt), mdict={'resGPU'+str(cnt): d.p0})
                            cnt = cnt+1

            context.pop()
        
        elif use_opencl:
            context = cl.create_some_context(interactive=False)
            queue = cl.CommandQueue(context)

            plan_set = {}
            len_max = area_max = 0
            for domain in scene.domains:
                dim = domain.bottomright - domain.topleft
                if dim[0] > len_max: len_max = int(dim[0])
                if dim[1] > len_max: len_max = int(dim[1])
                if dim[0]*nearest_2power(dim[1]+cfg.Wlength*2) > area_max: area_max = int(dim[0]*nearest_2power(dim[1]+cfg.Wlength*2))
                if dim[1]*nearest_2power(dim[0]+cfg.Wlength*2) > area_max: area_max = int(dim[1]*nearest_2power(dim[0]+cfg.Wlength*2))
                if str(int(nearest_2power(dim[0]+cfg.Wlength*2))) not in plan_set.keys():
                    plan_set[str(int(nearest_2power(dim[0]+cfg.Wlength*2)))] = Plan(int(nearest_2power(dim[0]+cfg.Wlength*2)), dtype=np.float64, queue=queue, fast_math=False)
                if str(int(nearest_2power(dim[0]+cfg.Wlength*2))) not in plan_set.keys():
                    plan_set[str(int(nearest_2power(dim[1]+cfg.Wlength*2)))] = Plan(int(nearest_2power(dim[1]+cfg.Wlength*2)), dtype=np.float64, queue=queue, fast_math=False)
                        
            mf = cl.mem_flags            
            
            g_bufl = {} #m/d(r/i) -> windowed matrix/derfact real/imag buffers. m(1/2/3)->p(#) buffers. spatderp3 will expand them if needed
            g_bufl["mr"] = cl.Buffer(context, mf.READ_WRITE, size=8*area_max)
            g_bufl["mi"] = cl.Buffer(context, mf.READ_WRITE, size=8*area_max)
            g_bufl["m1"] = cl.Buffer(context, mf.READ_WRITE, size=8*area_max)
            g_bufl["m2"] = cl.Buffer(context, mf.READ_WRITE, size=8*area_max)
            g_bufl["m3"] = cl.Buffer(context, mf.READ_WRITE, size=8*area_max)
            g_bufl["m_size"] = 8*area_max
            g_bufl["dr"] = cl.Buffer(context, mf.READ_WRITE, size=8*len_max) #dr also used by A (window matrix)
            g_bufl["di"] = cl.Buffer(context, mf.READ_WRITE, size=8*len_max)
            g_bufl["d_size"] = 8*len_max

            script_dir = os.path.dirname(__file__)
            rel_path = "ocl_kernels.cl"
            abs_file_path = os.path.join(script_dir, rel_path)
            
            src_file = open(abs_file_path, 'r')
            kernelcode = cl.Program(context, src_file.read()).build()
            
            mulfunc = {}
            mulfunc["pres_window"] = cl.Kernel(kernelcode, "pressure_window_multiplication")
            mulfunc["velo_window"] = cl.Kernel(kernelcode, "velocity_window_multiplication")
            mulfunc["derifact"] = cl.Kernel(kernelcode, "derifact_multiplication")
            

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
                                    def calc_ocl(domain, bt, ct, context, queue, plan_set,g_bufl,mulfunc):
                                        domain.calc_ocl(bt, ct, context, queue, plan_set,g_bufl,mulfunc)

                                    calc_ocl(d, boundary_type, calculation_type, context, queue, plan_set,g_bufl,mulfunc)

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

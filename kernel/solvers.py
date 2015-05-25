########################################################################
# #
# This file is part of openPSTD.                                       #
# #
# openPSTD is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or    #
# (at your option) any later version.                                  #
# #
# openPSTD is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of       #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
# GNU General Public License for more details.                         #
# #
# You should have received a copy of the GNU General Public License    #
# along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    #
#                                                                      #
########################################################################

import struct
import multiprocessing as mp
from kernel.core.classes import *

try:
    import ConfigParser as configparser
except:
    import configparser

# Abbreviate different type of calculation directions and measures
h, v = BoundaryType.HORIZONTAL, BoundaryType.VERTICAL
P, V = CalculationType.PRESSURE, CalculationType.VELOCITY


def update_domain_values(domain, cfg, sub_frame):
    domain.update_values(cfg, sub_frame)  # Note that cfg can be generalized to the solver.


def calc_domain(domain):
    for boundary_type, calculation_type in [(h, P), (v, P), (h, V), (v, V)]:
        if not domain.is_rigid():
            # Calculate sound propagations for non-rigid domains
            if domain.should_update(boundary_type):
                domain.calc(boundary_type, calculation_type)

def calc_domain_gpu(domain, context, stream, plan_set, g_bufl, mulfunc):
    for boundary_type, calculation_type in [(h, P), (v, P), (h, V), (v, V)]:
        if not domain.is_rigid():
            # Calculate sound propagations for non-rigid domains
            if domain.should_update(boundary_type):
                domain.calc_gpu(boundary_type, calculation_type, context, stream, plan_set, g_bufl, mulfunc)

def update_domain(domain, sub_frame):
    domain.rk_update(sub_frame)


class SingleThreaded:
    def __init__(self, cfg, scene, data_writer, receiver_files, output_fn):
        # Loop over time steps
        for frame in range(int(cfg.TRK)):
            output_fn({'status': 'running', 'message': "Calculation frame:%d" % (frame + 1), 'frame': frame + 1})

            # Keep a reference to current matrix contents
            for domain in scene.domains: domain.push_values()

            # Loop over subframes
            for sub_frame in range(6):
                # Loop over calculation directions and measures
                for domain in scene.domains:
                    calc_domain(domain)
                # Update acoustic values
                for domain in scene.domains:
                    if not domain.is_rigid():
                        update_domain(domain, sub_frame)

                # Sum the pressure components
                for domain in scene.domains:
                    domain.field_dict['p0'] = domain.field_dict['px0'] + domain.field_dict['pz0']

            # Apply pml matrices to boundary domains
            scene.apply_pml_matrices()

            for rf, r in zip(receiver_files, scene.receivers):
                rf.write(struct.pack('f', r.calc()))
                rf.flush()

            if frame % cfg.save_nth_frame == 0:
                data_writer.write_to_file(frame)


class MultiThreaded:
    def __init__(self, cfg, scene, data_writer, receiver_files, output_fn):
        self.cfg = cfg
        self.scene = scene
        self.data_writer = data_writer
        self.receiver_files = receiver_files
        self.output_fn = output_fn
        self.domain_worker_list = []
        self._distribute_domains(4)
        self.run()
        self.dom_obj_dict = {}

    def _distribute_domains(self, num_workers):
        for i in num_workers:
            self.domain_worker_list[i] = DomainWorker()
        worker = 0
        for domain in self.scene.domains:
            # if not domain.is_rigid()
            self.domain_worker_list[worker].add_domain(domain)
            worker += 1
            worker %= num_workers

    def run(self):
        # Loop over time steps

        for frame in range(int(self.cfg.TRK)):
            self.output_fn({'status': 'running', 'message': "Calculation frame:%d" % (frame + 1), 'frame': frame + 1})

            # Keep a reference to current matrix contents
            for domain in self.scene.domains:
                domain.push_values()

            # Loop over sub-frames
            for sub_frame in range(6):
                # Loop over workers
                for worker in self.domain_worker_list:
                    worker.calc_domains()
                    self.dom_obj_dict.update(worker.get_numerical_data_dict)
                for worker in self.domain_worker_list:
                    worker.update_domains(sub_frame)





                for domain in self.scene.domains:
                    job_list.append(executor.submit(calc_domain, domain))
                [job.result() for job in job_list]
                job_list = []
                for domain in self.scene.doains:
                    if not domain.is_rigid():
                        job_list.append(executor.submit(update_domain, domain, sub_frame))
                [job.result() for job in job_list]

                # Sum the pressure components
                for domain in self.scene.domains:
                    domain.field_dict['p0'] = domain.field_dict['px0'] + domain.field_dict['pz0']

            # Apply pml matrices to boundary domains
            self.scene.apply_pml_matrices()

            for rf, r in zip(self.receiver_files, self.scene.receivers):
                rf.write(struct.pack('f', r.calc()))
                rf.flush()

            if frame % self.cfg.save_nth_frame == 0:
                self.data_writer.write_to_file(frame)




class DomainWorker(mp.Process):
    def __init__(self):
        super()
        self.domain_list = []

    def add_domain(self, domain):
        """
        Add a domain to the domain worker
        :param domain: domain to be added. Should not already be in the list
        """
        assert domain not in self.domain_list
        self.domain_list.append(domain)

    def get_dependencies(self):
        """
        Obtain id's of the domains this domain must know to update its values
        :return: list with id's
        """
        dep = []
        for domain in self.domain_list:
            for adj in domain.neighbour_dict:
                for neighbour in domain.neighbour_dict[adj]:
                    dep.append(neighbour.id)
        return dep

    def calc_domains(self):
        """
        Perform one computing step for all domains owned by the worker.
        Stores the result in the domains corresponding numerical object
        """
        for domain in self.domain_list:
            for boundary_type, calculation_type in [(h, P), (v, P), (h, V), (v, V)]:
                if not domain.is_rigid():  # should be checked earlier
                    if domain.should_update(boundary_type):
                        source = "L%s%s" % calculation_type, boundary_type
                        domain.dom_obj[source] = domain.calc(boundary_type, calculation_type)

    def update_domains(self, sub_frame):
        """
        Updates the domains value for the Runge-Kutta time step
        :param sub_frame:
        """
        for domain in self.domain_list:
            domain.rk_update(sub_frame)

    def get_numerical_data_dict(self):
        """
        Obtain the numerical data objects for each domain owned by the worker
        :return:
        """
        return {domain.id: domain.dom_obj for domain in self.domain_list}

    def set_numerical_data_dict(self, data):
        """
        Set the numerical date object for each domain the worker needs to calculate.
        data parameter must cater to the needs of the dependencies exactly.
        :param data: dictionary with domain ids as keys and numerical objects as values
        :return:
        """
        assert data.keys() == set(self.get_dependencies())  # move to unit testing
        for domain in self.domain_list:
            for adj in domain.neighbour_dict:
                for neighbour in domain.neighbour_dict[adj]:
                    domain.dom_obj = data[neighbour.id]

class GpuAccelerated:
    def __init__(self, cfg, scene, data_writer, receiver_files, output_fn):
        import pycuda.driver as cuda
        from pycuda.tools import make_default_context
        from pycuda.compiler import SourceModule
        from pycuda.driver import Function
        from pyfft.cuda import Plan

        cuda.init()
        context = make_default_context()
        stream = cuda.Stream()

        plan_set = {} #will be filled as needed in spatderp3_gpu(~), prefill with 128, 256 and 512
        plan_set[str(128)] = Plan(128, dtype=np.float64, context=context, stream=stream, fast_math=False)
        plan_set[str(256)] = Plan(256, dtype=np.float64, context=context, stream=stream, fast_math=False)
        plan_set[str(512)] = Plan(512, dtype=np.float64, context=context, stream=stream, fast_math=False)
            
        g_bufl = {} #m/d(r/i) -> windowed matrix/derfact real/imag buffers. m(1/2/3)->p(#) buffers. spatderp3 will expand them if needed
        g_bufl["mr"] = cuda.mem_alloc(8*128*128)
        g_bufl["mi"] = cuda.mem_alloc(8*128*128)
        g_bufl["m1"] = cuda.mem_alloc(8*128*128)
        g_bufl["m2"] = cuda.mem_alloc(8*128*128)
        g_bufl["m3"] = cuda.mem_alloc(8*128*128)
        g_bufl["m_size"] = 8*128*128
        g_bufl["dr"] = cuda.mem_alloc(8*128) #dr also used by A (window matrix)
        g_bufl["di"] = cuda.mem_alloc(8*128)
        g_bufl["d_size"] = 8*128

        kernelcode = SourceModule(""" 
              #include <stdio.h>
              __global__ void derifact_multiplication(double *matr, double *mati, double *vecr, double *veci, int fftlen, int fftnum)
              {
                int index_x = blockIdx.x*blockDim.x + threadIdx.x; 
                int index_y = blockIdx.y*blockDim.y + threadIdx.y;

                int matindex = index_y*fftlen+index_x; //mat should be a contiguous array
                //printf("Block(x,y): (%d,%d). Thread(x,y): (%d,%d)\\n",blockIdx.x,blockIdx.y,threadIdx.x,threadIdx.y);
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

              __global__ void pressure_window_multiplication(double *mr, double *mi, double *A, double *p1, double *p2, double *p3, int winlen, int Ns1, int Ns2, int Ns3, int fftlen, int fftnum, double R21, double R00, double R31, double R10) //passing a few by value seems to be more efficient than building an array first in pycuda
              {
                int index_x = blockIdx.x*blockDim.x + threadIdx.x; 
                int index_y = blockIdx.y*blockDim.y + threadIdx.y;

                int matindex = index_y*fftlen+index_x;

                double G = 1;
                if (index_x < winlen) {
                    G = A[index_x];
                } else if (index_x > winlen+Ns2-1 && index_x < winlen*2+Ns2) {
                    G = A[index_x-Ns2];
                }
                if (index_y < fftnum) { //eat the surplus
                    mi[matindex] = 0;
                    if (index_x < winlen) {
                        mr[matindex] = G*(R21*p1[Ns1*index_y+index_x-winlen+Ns1] + R00*p2[Ns2*index_y+winlen-1-index_x]);
                    } else if (index_x < winlen + Ns2) {
                        mr[matindex] = p2[Ns2*index_y+index_x-winlen];
                    } else if (index_x < winlen*2+Ns2) {
                        mr[matindex] = G*(R31*p3[Ns3*index_y+index_x-winlen-Ns2] + R10*p2[Ns2*index_y+2*Ns2+winlen-1-index_x]);
                    } else {
                        mr[matindex] = 0; //zero padding
                    }
                    //if(mr[matindex]==0 && matindex < 50) printf("zero at:%d\\n",matindex%fftlen);
                }
              }

              __global__ void velocity_window_multiplication(double *mr, double *mi, double *A, double *p1, double *p2, double *p3, int winlen, int Ns1, int Ns2, int Ns3, int fftlen, int fftnum, double R21, double R00, double R31, double R10) //passing a few by value seems to be more efficient than building an array first in pycuda
              {
                int index_x = blockIdx.x*blockDim.x + threadIdx.x; 
                int index_y = blockIdx.y*blockDim.y + threadIdx.y;

                int matindex = index_y*fftlen+index_x;

                double G = 1;
                if (index_x < winlen) {
                    G = A[index_x];
                } else if (index_x > winlen+Ns2-1 && index_x < winlen*2+Ns2) {
                    G = A[index_x-Ns2];
                }
                if (index_y < fftnum) { //eat the surplus
                    mi[matindex] = 0;
                    if (index_x < winlen) {
                        mr[matindex] = G*(R21*p1[Ns1*index_y+index_x-winlen+Ns1-1] + R00*p2[Ns2*index_y+winlen-index_x]);
                    } else if (index_x < winlen + Ns2) {
                        mr[matindex] = p2[Ns2*index_y+index_x-winlen];
                    } else if (index_x < winlen*2+Ns2) {
                        mr[matindex] = G*(R31*p3[Ns3*index_y+index_x-winlen-Ns2+1] + R10*p2[Ns2*index_y+2*Ns2+winlen-2-index_x]);
                    } else {
                        mr[matindex] = 0; //zero padding
                    }
                    //if(mr[matindex]==0 && matindex < 50) printf("zero at:%d\\n",matindex%fftlen);
                }
              }
              """)
        mulfunc = {}
        mulfunc["pres_window"] = kernelcode.get_function("pressure_window_multiplication")
        mulfunc["velo_window"] = kernelcode.get_function("velocity_window_multiplication")
        mulfunc["derifact"] = kernelcode.get_function("derifact_multiplication")

        # Loop over time steps
        for frame in range(int(cfg.TRK)):
            output_fn({'status': 'running', 'message': "Calculation frame:%d" % (frame + 1), 'frame': frame + 1})

            # Keep a reference to current matrix contents
            for domain in scene.domains: domain.push_values()

            # Loop over subframes
            for sub_frame in range(6):
                # Loop over calculation directions and measures
                for domain in scene.domains:
                    calc_domain_gpu(domain, context, stream, plan_set, g_bufl, mulfunc)
                # Update acoustic values
                for domain in scene.domains:
                    if not domain.is_rigid():
                        update_domain(domain, sub_frame)

                # Sum the pressure components
                for domain in scene.domains:
                    domain.field_dict['p0'] = domain.field_dict['px0'] + domain.field_dict['pz0']

            # Apply pml matrices to boundary domains
            scene.apply_pml_matrices()

            for rf, r in zip(receiver_files, scene.receivers):
                rf.write(struct.pack('f', r.calc()))
                rf.flush()

            if frame % cfg.save_nth_frame == 0:
                data_writer.write_to_file(frame)

        context.pop()

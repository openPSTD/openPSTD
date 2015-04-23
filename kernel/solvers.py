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
import concurrent.futures
#from pstd import exit_with_error,subsample,write_array_to_file,write_plot_to_file


from kernel.core.classes import *

try:
    import ConfigParser as configparser
except:
    import configparser

# Abbreviate different type of calculation directions and measures
h, v = BoundaryType.HORIZONTAL, BoundaryType.VERTICAL
P, V = CalculationType.PRESSURE, CalculationType.VELOCITY

def update_domain_values(domain,cfg,sub_frame):
    domain.update_values(cfg,sub_frame) # Note that cfg can be generalized to the solver.


def calc_domain(domain,bt,ct):
    domain.calc(bt,ct)

def update_domain(domain,sub_frame):
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
                for boundary_type, calculation_type in [(h, P), (v, P), (h, V), (v, V)]:
                    # Loop over domains
                    for domain in scene.domains:
                        if not domain.is_rigid():
                            # Calculate sound propagations for non-rigid domains
                            if domain.should_update(boundary_type):
                                calc_domain(domain, boundary_type, calculation_type)

                for domain in scene.domains:
                    if not domain.is_rigid():
                        u0 = domain.u0
                        update_domain(domain,sub_frame)
                        print(u0-domain.u0)


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


class MultiThreaded:
    def __init__(self, cfg, scene, data_writer, receiver_files, output_fn):
        # Loop over time steps
        executor = concurrent.futures.ThreadPoolExecutor(8)
        for frame in range(int(cfg.TRK)):
            output_fn({'status': 'running', 'message': "Calculation frame:%d" % (frame + 1), 'frame': frame + 1})

            # Keep a reference to current matrix contents
            for domain in scene.domains: domain.push_values()

            # Loop over sub-frames
            for sub_frame in range(2):
                job_list = []
                # Loop over calculation directions and measures
                for boundary_type, calculation_type in [(h, P), (v, P), (h, V), (v, V)]:
                    # Loop over domains
                    for domain in scene.domains:
                        if not domain.is_rigid():
                            # Calculate sound propagations for non-rigid domains
                            if domain.should_update(boundary_type):
                                job_list.append(executor.submit(calc_domain, domain, boundary_type, calculation_type))
                [job.result() for job in job_list]
                job_list = []
                for domain in scene.domains:
                    if not domain.is_rigid():
                        job_list.append(executor.submit(update_domain, domain,sub_frame))
                [job.result() for job in job_list]

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
        executor.shutdown()

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

import os
import sys
import time
import math
import struct
import shutil
import pickle
import threading
#from pstd import exit_with_error,subsample,write_array_to_file,write_plot_to_file


from core.classes import *

try: import ConfigParser as configparser
except: import configparser

# Abbreviate different type of calculation directions and measures
h,v = BoundaryType.HORIZONTAL,  BoundaryType.VERTICAL
P,V = CalculationType.PRESSURE, CalculationType.VELOCITY


class PSTDSolver:
    def __init__(self,cfg,scene,stand_alone,data_writer,receiver_files):
        # Loop over time steps
        for frame in range(int(cfg.TRK)):
            if stand_alone:
                sys.stdout.write("\r%d"%(frame+1))
                sys.stdout.flush()

            # Keep a reference to current matrix contents
            for d in scene.domains: d.push_values()

            # Loop over RK substeps
            for subframe in range(6):
                # Loop over calculation directions and measures
                for boundary_type, calculation_type in [(h,P),(v,P),(h,V),(v,V)]:
                    # Loop over domains
                    for d in scene.domains:
                        if not d.is_rigid():
                            # Calculate sound propagations for non-rigid domains
                            if d.should_update(boundary_type):
                                def calc(domain, bt, ct):
                                    domain.calc(bt, ct)
                                try: calc(d, boundary_type, calculation_type)
                                except Exception as e: exit_with_error(e)

                for domain in scene.domains:
                    if not domain.is_rigid():
                        def calc(d):
                            d.u0  = d.u0_old  + (-cfg.dtRK * cfg.alfa[subframe] * ( 1 / d.rho       * d.Lpx)).real
                            d.w0  = d.w0_old  + (-cfg.dtRK * cfg.alfa[subframe] * ( 1 / d.rho       * d.Lpz)).real
                            d.px0 = d.px0_old + (-cfg.dtRK * cfg.alfa[subframe] * (d.rho*pow(cfg.c1,2.) * d.Lvx)).real
                            d.pz0 = d.pz0_old + (-cfg.dtRK * cfg.alfa[subframe] * (d.rho*pow(cfg.c1,2.) * d.Lvz)).real
                        calc(domain)


                # Sum the pressure components
                for d in scene.domains:
                    d.p0 = d.px0 + d.pz0

            # Apply pml matrices to boundary domains
            scene.apply_pml_matrices()

            for rf,r in zip(receiver_files, scene.receivers):
                rf.write(struct.pack('f',r.calc()))
                rf.flush()

            if frame % cfg.save_nth_frame == 0:
                data_writer.write_to_file(frame)

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

"""
Utility classes that interpret output data from the openPSTD simulation process

Author: Thomas Krijnen
"""

import io
import os
import json
import array
import struct
import pickle
import pickletools

from array import array

class PickleStream:
    def __init__(self, f):
        self.w = io.BytesIO()
        self.r = f
    def read(self,n):
        a = self.r.read(n)
        self.w.write(a)
        if n == 1: return a[0]
        if n == 2: return struct.unpack("<H",a)[0]
        if n == 4: return struct.unpack("<i",a)[0]
        else: return None
    def readline(self):
        self.w.write(self.r.readline())
    def get(self):
        coda = chr(self.read(1))

        #horrible hack to catch the fact that pyopencl outputs a meaningless Error I can't suppress
        if coda is 'E':
            for i in range(124):
                coda = chr(self.read(1))                    
                print(coda)

        while True:
            print(coda)
            if not coda:
                raise EOFError
            
            opcode = pickletools.code2op[coda]
                
            if opcode.arg is not None:
                n = opcode.arg.n
                if n > 0:
                    self.read(n)
                elif n == pickletools.UP_TO_NEWLINE:
                    self.readline()
                elif n == pickletools.TAKEN_FROM_ARGUMENT1:
                    self.read(self.read(1))
                elif n == pickletools.TAKEN_FROM_ARGUMENT4:
                    self.read(self.read(4))
            if coda == '.':
                break
            coda = chr(self.read(1))
        self.w.seek(0)
        return pickle.load(self.w)


class Listener:
    ob_to_listener = {}
    def __init__(self, receiver):
        self.array = array('f')
        self.fn = os.path.join(receiver.pstd_output_directory, 'rec-%s.bin'%receiver.pstd_output_id)
        self.itemsize = struct.calcsize('f')
        self.opened = False
        self.receiver_object = receiver
        self.file = None
        self.try_open()
        Listener.ob_to_listener[receiver] = self
    @classmethod
    def getFromObj(cls, arg):
        return cls.ob_to_listener.get(arg, None)
    @classmethod
    def closeAll(cls):
        for ob, listener in cls.ob_to_listener.items():
            if listener.file:
                listener.file.close()
                listener.file = None
    def try_open(self):
        try:
            self.file = open(self.fn, 'rb')
            self.opened = True
        except: pass
        return self.opened
    def read(self):
        if not self.opened:
            if not self.try_open():
                return False
        d = self.file.read()
        has_data = len(d) >= self.itemsize
        if has_data:
            self.array.extend(struct.unpack('%df'%(len(d) // self.itemsize), d))
        return has_data


class DomainInfo:
    class SubDomainInfo:
        def __init__(self, **di):
            if di:
                self.data = {'id': di['id'],
                             'nx': di['nx'],
                             'nz': di['nz'],
                             'cx': di['center'][0],
                             'cz': di['center'][1],
                             'sx': di['size'][0],
                             'sz': di['size'][1]}
        def __getattr__(self, k):
            return self.data[k]
        @staticmethod
        def restore(data):
            d = DomainInfo.SubDomainInfo()
            d.data = data
            return d
    def __init__(self, pstd_data=None, ll=None):
        self.subdomains = []
        if pstd_data and ll:
            dx, dz = pstd_data['dx'], pstd_data['dz'], 
            for domain in pstd_data['domains']:
                self.subdomains.append(self.SubDomainInfo(id=domain['id'],
                                                          nx=domain['Nx'],
                                                          nz=domain['Nz'],
                                                          center=(domain['center'][0] + ll.x, domain['center'][1] + ll.y, 0),
                                                          size=(domain['size'][0] - dx, domain['size'][1] - dz, 0)))
    def __repr__(self):
        return json.dumps([d.data for d in self.subdomains])
    @staticmethod
    def restore(str):
        d = DomainInfo()
        d.subdomains = [DomainInfo.SubDomainInfo.restore(d) for d in json.loads(str)]
        return d

pressure_level_cache = {}
float_size = struct.calcsize('f')
def read_pressure_level_data(dir, id, frame):
    tup = (dir, id, frame)
    data = pressure_level_cache.get(tup, None)
    if data: return data
    
    fn = os.path.join(dir,"%s-%06d.bin"%(id, frame))
    if os.path.exists(fn): 
        f = open(fn, "rb")
        f.seek(0, 2)
        fsize = f.tell()
        n = fsize // float_size
        f.seek(0, 0)
        data = array('f')
        data.fromfile(f, n)
        f.close()
        pressure_level_cache[tup] = data
        
    return data
        

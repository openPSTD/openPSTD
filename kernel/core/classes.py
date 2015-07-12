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
import os
import math
import array
import itertools
import matplotlib.pyplot as plt
import shutil
import numpy as np
import scipy.io

from os.path import commonprefix

try:
    import matplotlib.lines as lines
    import matplotlib.patches as patches
    has_matplotlib = True
except: has_matplotlib = False

# Only for python 2.*
try: range = xrange
except: pass

from .functions import *
from .kcalc import Kcalc

EPSILON = 1e-9

# A wrapper class over a numpy array that represents an n-dimensional tuple in cartesian space
class Coordinate(object):
    def __init__(self, x, *args):
        self.array = np.array([x]+list(args)) if len(args) else np.array(list(x))
        self.type = self.__class__.__name__
    def __len__(self):
        return len(self.array)
    def __iter__(self):
        return iter(self.array)
    def __getitem__(self,i):
        return self.array[i]
    def __add__(self,other):
        return Point(self.array+other.array)
    def __sub__(self,other):
        return Point(self.array-other.array)
    def __lt__(self,other):
        return (self.array < other.array).any()
    def __repr__(self):
        return "%s(%s)"%(self.type + " "*(5-len(self.type)),", ".join("%3d"%f for f in self.array))
        
# A specification of the coordinate class that represents a size with a width, height and depth
class Size(Coordinate):
    width = property(lambda s: s[0])
    height = property(lambda s: s[1])
    depth = property(lambda s: s[2])
    
# A specification of the coordinate class that represents a point with an x, y and z component
class Point(Coordinate):
    x = property(lambda s: s[0])
    y = property(lambda s: s[1])
    z = property(lambda s: s[2])

# A rectangular domain; the building block of the scene geometry
class Domain(object):
    ADJACENCIES = ['left', 'right', 'top', 'bottom']
    OPPOSITES = dict((ADJACENCIES[0:2], ADJACENCIES[2:4], ADJACENCIES[0:2][::-1], ADJACENCIES[2:4][::-1]))
    HORIZONTAL = set(ADJACENCIES[:2])
    VERTICAL = set(ADJACENCIES[2:])
    def __init__(self, cfg, id, alpha, a, b, edges, t='air', pml_for=None):
        self.id = id
        self.alpha = alpha
        self.topleft = a
        self.cfg = cfg
        if isinstance(b, Point):
            self.bottomright = b
            self.size = Size(self.bottomright-self.topleft)
        elif isinstance(b, Size):
            self.size = b
            self.bottomright = Point(self.topleft+self.size)
        
        # Initialize the zero matrices. For velocity matrices
        # an additional cell is needed
        self.u0 = self.Z(0,1)
        self.w0 = self.Z(1,0)
        self.p0 = self.Z()
        self.px0 = self.Z()
        self.pz0 = self.Z()
        self.u0_old = self.w0_old = self.p0_old = self.px0_old = self.pz0_old = None
        
        self.left, self.right, self.top, self.bottom = [], [], [], []
        self.leftB, self.rightB, self.topB, self.bottomB = [], [], [], []
        
        # TK: TODO: This is probably wrong.. especially with two neighbouring PML domains
        self.impedance = -((math.sqrt(1.-self.alpha)+1.)/(math.sqrt(1.-self.alpha)-1))
        self.rho = self.cfg.rho * self.impedance if self.impedance < 1000 else 1e200
        
        self.is_pml = t == 'pml'
        self.pml_for = pml_for or []
        
        pml_for_is_pml = [d.is_pml for d in self.pml_for]
        assert(len(self.pml_for) == 0 or pml_for_is_pml.count(pml_for_is_pml[0]) == len(pml_for_is_pml))
        
        self.is_sec_pml = self.is_pml and pml_for_is_pml[0]
        self.edges = edges
        
        self.rho_matrices = {}
        
        self.update_for = {}
        
        self.clear_matrices()
        
        self.local = False
    def __repr__(self):
        return "%s:\n %s\n o-------------------o\n |  %s  | \n o-------------------o\n      %s\n"%(self.id,self.topleft, self.size, self.bottomright)
    def __lt__(self,other):
        return self.topleft < other.topleft
    def Z(self,a=0,b=0): return np.zeros((self.size.height+a, self.size.width+b))
    def push_values(self):
        self.u0_old = self.u0
        self.w0_old = self.w0
        self.p0_old = self.p0
        self.px0_old = self.px0
        self.pz0_old = self.pz0
    def clear_matrices(self):
        self.Lpx = self.Z(0,1)
        self.Lpz = self.Z(1,0)
        self.Lvx = self.Z()
        self.Lvz = self.Z()
    def calc_rho_matrices(self):
        l = self.left   if len(self.left  ) else [None]
        r = self.right  if len(self.right ) else [None]
        b = self.bottom if len(self.bottom) else [None]
        t = self.top    if len(self.top   ) else [None]
        for pair in ((l,r),(b,t)):
            for p in itertools.product(*pair):
                rhos = [d.rho if d else 1e200 for d in p]
                rp, rv = Rmatrices(rhos[0], self.rho, rhos[1])
                self.rho_matrices[frozenset(d for d in p if d)] = {'p': rp, 'v':rv}
    def contains_point(self, p):
        return p.x > self.topleft.x and p.x < self.bottomright.x and p.y > self.topleft.y and p.y < self.bottomright.y
    def neighbours(self):
        return [self.left, self.right, self.top, self.bottom]
    def is_neighbour_of(self, other):
        return other in sum(self.neighbours(),[])
    def num_neighbours(self,with_pml=True):
        return len([n for n in sum(self.neighbours(),[]) if n is not None and (not n.is_pml or with_pml)])
    def is_rigid(self):
        return self.impedance > 1000
    def calc_pml(self):
        # Only calculate PML matrices for pml domains with a single non-pml neighbour
        # or for secondary PML domains with a single PML neighbour
        num_air_neighbours = self.num_neighbours(False)
        num_neighbours = self.num_neighbours(True)
        assert (num_air_neighbours == 1 and self.is_pml) or (num_neighbours <= 2 and self.is_sec_pml)
        if self.is_sec_pml:
            self.is_2d = num_neighbours == 2
            # If this domain has its neighbour to the left or to the right a PML 
            # matrix is obtained for horizontal attenuation
            self.is_horizontal = len(self.left) > 0 or len(self.right) > 0
            if self.is_2d:
                # Corner PML domains should have a horizontal as well as a vertical component
                # In particular not two neighbours in the same direction.
                assert self.is_horizontal and (len(self.top) > 0 or len(self.bottom) > 0)
                self.is_lower = len(self.left) > 0, len(self.bottom) > 0
            else:
                # If this neighbour is located to the left or bottom the attenuation is reversed
                self.is_lower = len(self.left) > 0 or len(self.bottom) > 0
        else:
            self.is_2d = False
            # If this domain has a neighbour to the left or to the right and this neighbour
            # is not a PML domain, a PML matrix is obtained for horizontal attenuation
            self.is_horizontal = (len(self.left) > 0 and all([d.is_pml == False for d in self.left])) or (len(self.right) > 0 and all([d.is_pml == False for d in self.right]))
            # If this neighbour is located to the left or bottom the attenuation is reversed
            self.is_lower = (len(self.left) > 0 and all([d.is_pml == False for d in self.left])) or (len(self.bottom) > 0 and all([d.is_pml == False for d in self.bottom]))
        def make_attenuation(horitontal, ascending):
            # Depending on the direction of the attenuation the matrices need to be left- or
            # right-multiplied
            left = np.ones((self.size.height,1)) if horitontal else 1
            right = 1 if horitontal else np.ones((1,self.size.width))
            pfact = self.cfg.pfact if ascending else self.cfg.pfact[::-1]
            ufact = self.cfg.ufact if ascending else self.cfg.ufact[::-1]
            if not horitontal: 
                pfact = pfact.reshape(-1,1)
                ufact = ufact.reshape(-1,1)
            return left * pfact * right, left * ufact * right
        if self.is_sec_pml and self.is_2d:
            # PML is the product of horizontal and vertical attenuation
            a = make_attenuation(True, self.is_lower[0])
            b = make_attenuation(False, self.is_lower[1])
            self.pml_p, self.pml_u = (a[0], b[0]), (a[1], b[1])
        else:
            self.pml_p, self.pml_u = make_attenuation(self.is_horizontal, self.is_lower)
    def should_update(self, d):
        # Cache this value, it's not changing anyway and we're in Python after all
        if d in self.update_for: return self.update_for[d]
        def check():
            if self.num_neighbours(False) == 1 and self.is_pml:
                # Always calculate in direction orthogonal to boundary
                if self.is_horizontal and d == BoundaryType.HORIZONTAL: return True
                if not self.is_horizontal and d == BoundaryType.VERTICAL: return True
                
                if self.local: return False
                
                # Find my non-PML neighbour
                for a in self.ADJACENCIES:
                    domains = getattr(self, a)
                    if len(domains) == 1 and not domains[0].is_pml:
                        opposites = getattr(domains[0], self.OPPOSITES[a])
                        assert len(opposites) == 1 and opposites[0] == self
                        return not domains[0].edges[self.OPPOSITES[a][0]]['lr']
            else: return True
        self.update_for[d] = check()
        return self.update_for[d]
    def apply_pml(self):
        assert (self.num_neighbours(False) == 1 and self.is_pml) or (self.num_neighbours(True) <= 2 and self.is_sec_pml)
        # The pressure and velocity matrices are multiplied by the PML values
        if self.is_sec_pml and self.is_2d:
            self.px0 *= self.pml_p[0]
            self.pz0 *= self.pml_p[1]
            self.u0  *= self.pml_u[0]
            self.w0  *= self.pml_u[1]
        elif self.is_horizontal:
            self.px0 *= self.pml_p
            self.u0  *= self.pml_u
        else:
            self.pz0 *= self.pml_p
            self.w0  *= self.pml_u
    def get_range(self, bt):
        if bt == BoundaryType.HORIZONTAL:
            a, b = self.topleft.y, self.bottomright.y
        else:
            a, b = self.topleft.x, self.bottomright.x
        return range(int(a), int(b))
    def get_detached(self, adj):
        bt = BoundaryType.ortho(BoundaryType.from_domain_adjacency(adj))
        own = set(self.get_range(bt))
        for neighbor in getattr(self, adj):
            own -= set(neighbor.get_range(bt))
            
        def decompose(s):
            s = sorted(s)
            r = []
            for i,x in enumerate(s):
                if i == 0 or s[i-1]+1 < x: r.append([x,x])
                if i == len(s) - 1 or s[i+1]-1 > x: r[-1][-1] = x + 1
            return r
            
        return decompose(own)
    def find_neighbor_at(self, adj, d):
        """
        Finds the neighbor of this domain at the offset specified in d relative
        to global scene coordinates in the direction specified by adj
        """
        domains = getattr(self, adj)
        coord = 'y' if adj in Domain.HORIZONTAL else 'x'
        for domain in domains:
            lower, upper = sorted(map(int, [getattr(domain.topleft, coord), getattr(domain.bottomright, coord)]))
            if d >= lower and d <= upper: return domain
        raise LookupError("No domain found at %s for %.2f" % (adj, d))
    #@profile
    def calc(self, bt, ct, dest = None):

        # str = ": Calculating L%s%s for domain '%s'        "%(ct,bt,self.id)
        # sys.stdout.write(str)
        # sys.stdout.flush()

        # Find the input matrices
        domains1 = self.left  if bt == BoundaryType.HORIZONTAL else self.bottom
        domains2 = self.right if bt == BoundaryType.HORIZONTAL else self.top

        if len(domains1) == 0: domains1 = [None]
        if len(domains2) == 0: domains2 = [None]

        own_range = self.get_range(bt)

        if dest:
            if bt == BoundaryType.HORIZONTAL:
                source = self.Z(0,1)
            else:
                source = self.Z(1,0)
        else:
            source = getattr(self,"L%s%s"%(ct,bt))

        for domain1, domain2 in itertools.product(domains1, domains2):
            rho_matrix_key = frozenset(d for d in (domain1, domain2) if d)

            range_intersection = set(own_range)
            # print (self.id, bt, 'own', own_range)

            for other_domain in (domain1, domain2):
                if other_domain is not None:
                    if bt == BoundaryType.HORIZONTAL:
                        other_range = range(int(other_domain.topleft.y), int(other_domain.bottomright.y))
                    else:
                        other_range = range(int(other_domain.topleft.x), int(other_domain.bottomright.x))
                    # print (other_domain.id, bt, 'other', other_range)
                    range_intersection &= set(other_range)

            if not len(range_intersection): continue

            range_start, range_end = min(range_intersection), max(range_intersection)+1

            primary_dimension   = self.size.width if bt == BoundaryType.HORIZONTAL else self.size.height
            secundary_dimension = range_end - range_start #self.size.height if bt == BoundaryType.HORIZONTAL else self.size.width

            Ntot = 2. * self.cfg.Wlength + primary_dimension
            if ct == CalculationType.PRESSURE: Ntot += 1
            else: primary_dimension += 1

            matrix1 = matrix2 = None
            if ct == CalculationType.VELOCITY and domain1 is None and domain2 is None:
                # For a PML layer parallel to its interface direction the matrix is concatenated with zeros
                # TK: TODO: This statement is probably no longer correct, with arbitrary domain configurations
                # a PML domain can also have a neighbour
                #   |             |
                # __|_____________|___
                #   |     PML     |
                #  <--------------->
                matrix1 = self.Z(0 if bt == BoundaryType.HORIZONTAL else 1, 1 if bt == BoundaryType.HORIZONTAL else 0)
                matrix2 = self.Z(0 if bt == BoundaryType.HORIZONTAL else 1, 1 if bt == BoundaryType.HORIZONTAL else 0)
                domain1 = domain2 = self
            else:
                if domain1 is None: domain1 = self
                if domain2 is None: domain2 = self

            matrix0 = self.p0 if ct == CalculationType.PRESSURE else getattr(self,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')
            if matrix1 is None:
                matrix1 = domain1.p0 if ct == CalculationType.PRESSURE else getattr(domain1,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')
            if matrix2 is None:
                matrix2 = domain2.p0 if ct == CalculationType.PRESSURE else getattr(domain2,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')

            a = 0 if ct == CalculationType.PRESSURE else 1
            b = 1 if bt == BoundaryType.HORIZONTAL else 0

            if dest:
                kc = dest['fact']
            else:
                kc = Kcalc.derfactp((Ntot, self.cfg.dx)) if ct == CalculationType.PRESSURE else Kcalc.derfactu((Ntot, self.cfg.dx))

            # Determine which rho matrix instance to use
            rmat = self.rho_matrices[rho_matrix_key][ct]

            if bt == BoundaryType.HORIZONTAL:
                matrix0_offset = self.topleft.y
                matrix0_indexed = matrix0[range_start-matrix0_offset:range_end-matrix0_offset,:]
                matrix1_offset = domain1.topleft.y
                matrix1_indexed = matrix1[range_start-matrix1_offset:range_end-matrix1_offset,:]
                matrix2_offset = domain2.topleft.y
                matrix2_indexed = matrix2[range_start-matrix2_offset:range_end-matrix2_offset,:]
            else:
                matrix0_offset = self.topleft.x
                matrix0_indexed = matrix0[:,range_start-matrix0_offset:range_end-matrix0_offset]
                matrix1_offset = domain1.topleft.x
                matrix1_indexed = matrix1[:,range_start-matrix1_offset:range_end-matrix1_offset]
                matrix2_offset = domain2.topleft.x
                matrix2_indexed = matrix2[:,range_start-matrix2_offset:range_end-matrix2_offset]

            matrix = spatderp3(matrix0_indexed,kc,self.cfg.Wlength,self.cfg.A,primary_dimension,secundary_dimension,nearest_2power(Ntot),rmat,matrix1_indexed,matrix2_indexed,a,b)

            if bt == BoundaryType.HORIZONTAL:
                source[range_start-matrix0_offset:range_end-matrix0_offset,:] = matrix
            else:
                source[:,range_start-matrix0_offset:range_end-matrix0_offset] = matrix

        return source

        
        # sys.stdout.write("\b"*len(str))

    def calc_cuda(self, bt, ct, context, stream, plan_set, g_bufl, mulfunc, dest = None):

        # Find the input matrices
        domains1 = self.left  if bt == BoundaryType.HORIZONTAL else self.bottom
        domains2 = self.right if bt == BoundaryType.HORIZONTAL else self.top

        if len(domains1) == 0: domains1 = [None]
        if len(domains2) == 0: domains2 = [None]

        own_range = self.get_range(bt)

        if dest:
            if bt == BoundaryType.HORIZONTAL:
                source = self.Z(0,1)
            else:
                source = self.Z(1,0)
        else:
            source = getattr(self,"L%s%s"%(ct,bt))

        for domain1, domain2 in itertools.product(domains1, domains2):
            rho_matrix_key = frozenset(d for d in (domain1, domain2) if d)

            range_intersection = set(own_range)
            # print (self.id, bt, 'own', own_range)

            for other_domain in (domain1, domain2):
                if other_domain is not None:
                    if bt == BoundaryType.HORIZONTAL:
                        other_range = range(int(other_domain.topleft.y), int(other_domain.bottomright.y))
                    else:
                        other_range = range(int(other_domain.topleft.x), int(other_domain.bottomright.x))
                    # print (other_domain.id, bt, 'other', other_range)
                    range_intersection &= set(other_range)

            if not len(range_intersection): continue

            range_start, range_end = min(range_intersection), max(range_intersection)+1

            primary_dimension   = self.size.width if bt == BoundaryType.HORIZONTAL else self.size.height
            secundary_dimension = range_end - range_start #self.size.height if bt == BoundaryType.HORIZONTAL else self.size.width

            Ntot = 2. * self.cfg.Wlength + primary_dimension
            if ct == CalculationType.PRESSURE: Ntot += 1
            else: primary_dimension += 1

            matrix1 = matrix2 = None
            if ct == CalculationType.VELOCITY and domain1 is None and domain2 is None:
                # For a PML layer parallel to its interface direction the matrix is concatenated with zeros
                # TK: TODO: This statement is probably no longer correct, with arbitrary domain configurations
                # a PML domain can also have a neighbour
                #   |             |
                # __|_____________|___
                #   |     PML     |
                #  <--------------->
                matrix1 = self.Z(0 if bt == BoundaryType.HORIZONTAL else 1, 1 if bt == BoundaryType.HORIZONTAL else 0)
                matrix2 = self.Z(0 if bt == BoundaryType.HORIZONTAL else 1, 1 if bt == BoundaryType.HORIZONTAL else 0)
                domain1 = domain2 = self
            else:
                if domain1 is None: domain1 = self
                if domain2 is None: domain2 = self

            matrix0 = self.p0 if ct == CalculationType.PRESSURE else getattr(self,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')
            if matrix1 is None:
                matrix1 = domain1.p0 if ct == CalculationType.PRESSURE else getattr(domain1,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')
            if matrix2 is None:
                matrix2 = domain2.p0 if ct == CalculationType.PRESSURE else getattr(domain2,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')

            a = 0 if ct == CalculationType.PRESSURE else 1
            b = 1 if bt == BoundaryType.HORIZONTAL else 0

            if dest:
                kc = dest['fact']
            else:
                kc = Kcalc.derfactp((Ntot, self.cfg.dx)) if ct == CalculationType.PRESSURE else Kcalc.derfactu((Ntot, self.cfg.dx))

            # Determine which rho matrix instance to use
            rmat = self.rho_matrices[rho_matrix_key][ct]

            if bt == BoundaryType.HORIZONTAL:
                matrix0_offset = self.topleft.y
                matrix0_indexed = matrix0[range_start-matrix0_offset:range_end-matrix0_offset,:]
                matrix1_offset = domain1.topleft.y
                matrix1_indexed = matrix1[range_start-matrix1_offset:range_end-matrix1_offset,:]
                matrix2_offset = domain2.topleft.y
                matrix2_indexed = matrix2[range_start-matrix2_offset:range_end-matrix2_offset,:]
            else:
                matrix0_offset = self.topleft.x
                matrix0_indexed = matrix0[:,range_start-matrix0_offset:range_end-matrix0_offset]
                matrix1_offset = domain1.topleft.x
                matrix1_indexed = matrix1[:,range_start-matrix1_offset:range_end-matrix1_offset]
                matrix2_offset = domain2.topleft.x
                matrix2_indexed = matrix2[:,range_start-matrix2_offset:range_end-matrix2_offset]

            matrix = spatderp3_cuda(matrix0_indexed,kc,self.cfg.Wlength,self.cfg.A,primary_dimension,secundary_dimension,nearest_2power(Ntot),rmat,matrix1_indexed,matrix2_indexed,a,b,context,stream,plan_set,g_bufl,mulfunc)

            if bt == BoundaryType.HORIZONTAL:
                source[range_start-matrix0_offset:range_end-matrix0_offset,:] = matrix
            else:
                source[:,range_start-matrix0_offset:range_end-matrix0_offset] = matrix

        return source

    def calc_ocl(self, bt, ct, context, plan_set, g_bufl, mulfunc, dest = None):
        
		# Find the input matrices
        domains1 = self.left  if bt == BoundaryType.HORIZONTAL else self.bottom
        domains2 = self.right if bt == BoundaryType.HORIZONTAL else self.top

        if len(domains1) == 0: domains1 = [None]
        if len(domains2) == 0: domains2 = [None]

        own_range = self.get_range(bt)

        if dest:
            if bt == BoundaryType.HORIZONTAL:
                source = self.Z(0,1)
            else:
                source = self.Z(1,0)
        else:
            source = getattr(self,"L%s%s"%(ct,bt))

        for domain1, domain2 in itertools.product(domains1, domains2):
            rho_matrix_key = frozenset(d for d in (domain1, domain2) if d)

            range_intersection = set(own_range)
            # print (self.id, bt, 'own', own_range)

            for other_domain in (domain1, domain2):
                if other_domain is not None:
                    if bt == BoundaryType.HORIZONTAL:
                        other_range = range(int(other_domain.topleft.y), int(other_domain.bottomright.y))
                    else:
                        other_range = range(int(other_domain.topleft.x), int(other_domain.bottomright.x))
                    # print (other_domain.id, bt, 'other', other_range)
                    range_intersection &= set(other_range)

            if not len(range_intersection): continue

            range_start, range_end = min(range_intersection), max(range_intersection)+1

            primary_dimension   = self.size.width if bt == BoundaryType.HORIZONTAL else self.size.height
            secundary_dimension = range_end - range_start #self.size.height if bt == BoundaryType.HORIZONTAL else self.size.width

            Ntot = 2. * self.cfg.Wlength + primary_dimension
            if ct == CalculationType.PRESSURE: Ntot += 1
            else: primary_dimension += 1

            matrix1 = matrix2 = None
            if ct == CalculationType.VELOCITY and domain1 is None and domain2 is None:
                # For a PML layer parallel to its interface direction the matrix is concatenated with zeros
                # TK: TODO: This statement is probably no longer correct, with arbitrary domain configurations
                # a PML domain can also have a neighbour
                #   |             |
                # __|_____________|___
                #   |     PML     |
                #  <--------------->
                matrix1 = self.Z(0 if bt == BoundaryType.HORIZONTAL else 1, 1 if bt == BoundaryType.HORIZONTAL else 0)
                matrix2 = self.Z(0 if bt == BoundaryType.HORIZONTAL else 1, 1 if bt == BoundaryType.HORIZONTAL else 0)
                domain1 = domain2 = self
            else:
                if domain1 is None: domain1 = self
                if domain2 is None: domain2 = self

            matrix0 = self.p0 if ct == CalculationType.PRESSURE else getattr(self,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')
            if matrix1 is None:
                matrix1 = domain1.p0 if ct == CalculationType.PRESSURE else getattr(domain1,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')
            if matrix2 is None:
                matrix2 = domain2.p0 if ct == CalculationType.PRESSURE else getattr(domain2,'u0' if bt == BoundaryType.HORIZONTAL else 'w0')

            a = 0 if ct == CalculationType.PRESSURE else 1
            b = 1 if bt == BoundaryType.HORIZONTAL else 0

            if dest:
                kc = dest['fact']
            else:
                kc = Kcalc.derfactp((Ntot, self.cfg.dx)) if ct == CalculationType.PRESSURE else Kcalc.derfactu((Ntot, self.cfg.dx))

            # Determine which rho matrix instance to use
            rmat = self.rho_matrices[rho_matrix_key][ct]

            if bt == BoundaryType.HORIZONTAL:
                matrix0_offset = self.topleft.y
                matrix0_indexed = matrix0[range_start-matrix0_offset:range_end-matrix0_offset,:]
                matrix1_offset = domain1.topleft.y
                matrix1_indexed = matrix1[range_start-matrix1_offset:range_end-matrix1_offset,:]
                matrix2_offset = domain2.topleft.y
                matrix2_indexed = matrix2[range_start-matrix2_offset:range_end-matrix2_offset,:]
            else:
                matrix0_offset = self.topleft.x
                matrix0_indexed = matrix0[:,range_start-matrix0_offset:range_end-matrix0_offset]
                matrix1_offset = domain1.topleft.x
                matrix1_indexed = matrix1[:,range_start-matrix1_offset:range_end-matrix1_offset]
                matrix2_offset = domain2.topleft.x
                matrix2_indexed = matrix2[:,range_start-matrix2_offset:range_end-matrix2_offset]

            matrix = spatderp3_ocl(matrix0_indexed,kc,self.cfg.Wlength,self.cfg.A,primary_dimension,secundary_dimension,nearest_2power(Ntot),rmat,matrix1_indexed,matrix2_indexed,a,b,context,plan_set,g_bufl,mulfunc)

            if bt == BoundaryType.HORIZONTAL:
                source[range_start-matrix0_offset:range_end-matrix0_offset,:] = matrix
            else:
                source[:,range_start-matrix0_offset:range_end-matrix0_offset] = matrix

        return source


        # sys.stdout.write("\b"*len(str))

class BoundaryType:
    HORIZONTAL = 'x'
    VERTICAL = 'z'
    @staticmethod
    def from_domain_adjacency(adj):
        # NB: Note that when to domains are adjacent horizontally the
        # boundary between them is indeed oriented vertically.
        return BoundaryType.VERTICAL if adj in Domain.HORIZONTAL else BoundaryType.HORIZONTAL
    @staticmethod
    def ortho(bt):
        return BoundaryType.VERTICAL if bt == BoundaryType.HORIZONTAL else BoundaryType.HORIZONTAL
    
class CalculationType:
    PRESSURE = 'p'
    VELOCITY = 'v'

# Class representing a boundary of a domain. A boundary is shared between two domains.
class Boundary(object):
    def __init__(self, a, b, t):
        order = a.topleft < b.topleft
        self.domain1 = a if order else b
        self.domain2 = b if order else a
        self.boundary_type = t
        if self.boundary_type == BoundaryType.HORIZONTAL:
            self.line = (self.domain1.topleft.x,self.domain1.bottomright.x),(self.domain1.bottomright.y,self.domain1.bottomright.y)
        else:
            self.line = (self.domain1.bottomright.x,self.domain1.bottomright.x),(self.domain1.topleft.y,self.domain1.bottomright.y)
    def create_line(self,fig,tl):
        if not has_matplotlib: return
        # Plots a line on the canvas, 
        # PML|PML is not drawn
        # air|PML in a dark gray
        # air|air in a light gray
        if self.domain1.is_pml and self.domain2.is_pml: return
        c = '#eeeeee' if self.domain1.is_pml == self.domain2.is_pml else '#bbbbbb'
        fig.add_line(lines.Line2D([x-tl.x for x in self.line[0]],[y - tl.y for y in self.line[1]], color=c))
    def __repr__(self):
        attrs = ('x',self.domain1.bottomright.x) if self.boundary_type == BoundaryType.HORIZONTAL else ('y',self.domain1.bottomright.y)
        return "Boundary at %s=%d"%attrs


class Receiver(object):
    def __init__(self, cfg, id, p, c):
        self.id = id
        self.cfg = cfg
        self.nearest_neighbour = getattr(cfg, 'listener_nn', 0) == 1
        self.location = p
        self.grid_location = Point(math.floor(p.x), math.floor(p.y))
        self.grid_offset = self.location - self.grid_location
        self.container = c
        self.array = array.array('d')
    def calc(self):
        if self.nearest_neighbour:
            # Nearest neighbour interpolation
            
            p = self.grid_location - self.container.topleft
            value = self.container.p0[p.y, p.x]
        else:    
            # Spectral interpolation 
            
            top_domain = self.container.find_neighbor_at('top', self.location.x)
            bottom_domain = self.container.find_neighbor_at('bottom', self.location.x)
            
            def calc_fact(size, bt):
                primary_dimension = size.width if bt == BoundaryType.HORIZONTAL else size.height                
                Ntot = 2. * self.cfg.Wlength + primary_dimension + 1 # Pressure matrix, hence +1
                jfact = Kcalc.jfact((Ntot, self.cfg.dx))
                k = Kcalc.k((Ntot, self.cfg.dx))                
                offset = getattr(self.grid_offset, 'x' if bt == BoundaryType.HORIZONTAL else 'y')                
                return np.exp(jfact * k * offset * self.cfg.dx)
                
            def calc(domain, bt):
                return domain.calc(bt, CalculationType.PRESSURE, {
                    'fact': calc_fact(domain.size, bt)
                })
            
            p0dx = calc(self.container, BoundaryType.HORIZONTAL)
            p0dx_top = calc(top_domain, BoundaryType.HORIZONTAL)
            p0dx_bottom = calc(bottom_domain, BoundaryType.HORIZONTAL)
            
            dx = (self.grid_location - self.container.topleft).x
            dx_top = (self.grid_location - top_domain.topleft).x
            dx_bottom = (self.grid_location - bottom_domain.topleft).x
            
            zfact = calc_fact(Size(1, self.container.size.height), BoundaryType.VERTICAL)
            
            bottom_top_key = frozenset([bottom_domain, top_domain])
            rho = self.container.rho_matrices[bottom_top_key]['p']
            Ntot = 2. * self.cfg.Wlength + self.container.size.height + 1
          
            p0shift = spatderp3(p0dx[:,dx:dx+1], zfact, self.cfg.Wlength, self.cfg.A, self.container.size.height, 1, nearest_2power(Ntot), rho, p0dx_bottom[:,dx_bottom:dx_bottom+1], p0dx_top[:,dx_top:dx_top+1], 0, 0)
            
            dy = self.grid_location.y - self.container.topleft.y
            
            value = p0shift[dy, 0]
        
        self.array.append(value)
        
        return value
        
    def create_annotation(self,fig,tl):
        if not has_matplotlib: return
        fig.add_patch(patches.Circle((self.location.x-tl.x,self.location.y-tl.y),radius=2,color='#8899bb',fill=False))

    def __repr__(self):
        return "Receiver at "+repr(self.location)
        
class Scene(object):
    def __init__(self, cfg):
        self.cfg = cfg
        self.domains = []
        self.boundaries = []
        self.receivers = []
        self.source_positions = []
        self.topleft = Point(1e9,1e9)
        self.bottomright = Point(-1e9,-1e9)
        self.size = Size(0,0)
        self.subplot = None
    def __repr__(self):
        return "\n".join(repr(x) for x in self.domains) + "\n" + "\n".join(repr(x) for x in self.boundaries) + "\n" + "\n".join(repr(x) for x in self.receivers) + "\nSources:\n" + "\n".join(repr(x) for x in self.source_positions)
    def __getattr__(self, k):
        matches = [d for d in self.domains if d.id == k]
        assert len(matches) == 1
        return matches[0]
    def add_receiver(self, rx, ry):
        location = Point(rx/self.cfg.dx,ry/self.cfg.dx)
        container = None
        for d in self.domains:
            if not d.is_pml and d.contains_point(location): container = d
        assert container is not None
        receiver = Receiver(self.cfg, "receiver%d"%(len(self.receivers)+1), location, container)
        self.receivers.append(receiver)        
    def add_domain(self, b):
        self.topleft = Point(min(self.topleft.x, b.topleft.x),min(self.topleft.y, b.topleft.y))
        self.bottomright = Point(max(self.bottomright.x, b.bottomright.x),max(self.bottomright.y, b.bottomright.y))
        self.size = Size(self.bottomright-self.topleft)
        
        opposite = {'left':'right','top':'bottom','right':'left','bottom':'top'}
        
        for a in self.domains:
            # TK: Not entirely sure whether this is always correct, for
            # the use cases for openPSTD, in which the PML layers are
            # always added implicitly at the end, it is sufficient:
            # A PML domain can only be a neighbour of another PML domain
            # in case they both connect to a set of two non-PML domains
            # that are neighbours of each other too.
            if b.is_sec_pml and a.is_sec_pml:
                continue
            if b.is_sec_pml and not a.is_pml:
                continue
            if a.is_sec_pml and not b.is_pml:
                continue
            if b.is_pml and a.is_pml:
                if (b.is_sec_pml and a in b.pml_for) or (a.is_sec_pml and b in a.pml_for):
                    # secondary pml layers should have a single pml neighbour
                    pass
                elif not (len(a.pml_for) == 1 and len(b.pml_for) == 1 and a.pml_for[0].is_neighbour_of(b.pml_for[0]) and a.is_sec_pml == b.is_sec_pml):
                    continue
            
            # Test for the following situations:
            #
            #   | a   #   b |   #  _____  #  a | b
            #   |--   #   --|   #  b | a  #  __|__
            #   | b   #   a |   #         # 
            #  (1)    #   (2)   #  (3)    #  (4)
            
            B = None
            orientation = None
            
            if a.topleft.x == b.bottomright.x and len(set(range(int(a.topleft.y),int(a.bottomright.y))) & set(range(int(b.topleft.y),int(b.bottomright.y)))):  # and a.topleft.y == b.topleft.y and a.size.height == b.size.height: # (3)
                B = Boundary(a,b,BoundaryType.VERTICAL)
                orientation = 'left'
            if a.bottomright.x == b.topleft.x and len(set(range(int(a.topleft.y),int(a.bottomright.y))) & set(range(int(b.topleft.y),int(b.bottomright.y)))): # and a.topleft.y == b.topleft.y and a.size.height == b.size.height: # (4)
                B = Boundary(a,b,BoundaryType.VERTICAL)
                orientation = 'right'
            if a.topleft.y == b.bottomright.y and len(set(range(int(a.topleft.x),int(a.bottomright.x))) & set(range(int(b.topleft.x),int(b.bottomright.x)))): # and a.topleft.x == b.topleft.x and a.size.width == b.size.width: # (1)
                B = Boundary(a,b,BoundaryType.HORIZONTAL)
                orientation = 'bottom'
            if a.bottomright.y == b.topleft.y and len(set(range(int(a.topleft.x),int(a.bottomright.x))) & set(range(int(b.topleft.x),int(b.bottomright.x)))): # and a.topleft.x == b.topleft.x and a.size.width == b.size.width: # (2)
                B = Boundary(a,b,BoundaryType.HORIZONTAL)
                orientation = 'top'
            
            # PML layers can only have a single non-PML neighbour
            is_neighbour = bool(orientation)
            a_is_pml_for_other_domain = a.is_pml and not b.is_pml and b not in a.pml_for
            b_is_pml_for_other_domain = b.is_pml and not a.is_pml and a not in b.pml_for
            if orientation and not(a_is_pml_for_other_domain or b_is_pml_for_other_domain):
                self.boundaries.append(B)
                getattr(a,orientation).append(b)
                getattr(b,opposite[orientation]).append(a)
                
        self.domains.append(b)
    def calc_rho_matrices(self): 
        for d in self.domains: d.calc_rho_matrices()
    def add_pml_domains(self, n = None):
        if n is None: n = self.cfg.PMLcells
        D = []
        D2 = []
        for d in self.domains:
            if d.is_pml: continue
            # print d.id + ':'
            # print " ".join([b.id if b is not None else '-' for b in d.neighbours()])
            for adj in Domain.ADJACENCIES:
                rs = d.get_detached(adj)
                # print d.id, adj, rs
                for idx, range in enumerate(rs):
                    pml_id = "%s_%s"%(d.id, adj)
                    if len(rs) > 1: pml_id += '_%d'%idx
                    a = d.edges[adj[0]]['a']
                    pml_a = max(EPSILON, a)
                    if adj == 'left'  : pml_offset = Point(-n                     , range[0] - d.topleft.y)
                    if adj == 'right' : pml_offset = Point( d.size.width          , range[0] - d.topleft.y)
                    if adj == 'bottom': pml_offset = Point( range[0] - d.topleft.x,-n                     )
                    if adj == 'top'   : pml_offset = Point( range[0] - d.topleft.x, d.size.height         )
                    
                    if adj == 'left' or adj == 'right':
                        pml_size = Size(n                  , range[1] - range[0])
                    else:
                        pml_size = Size(range[1] - range[0], n                  )
                        
                    # Secondary PML domains are only added for primary PML domains that
                    # completely overlap with their AIR domain boundary. Furthermore,
                    # otherwise the PML layer is set to be locally reacting.
                    bt = BoundaryType.ortho(BoundaryType.from_domain_adjacency(adj))
                    if bt == BoundaryType.VERTICAL:
                        full_overlap = range[0] == d.topleft.x and range[1] == d.bottomright.x
                    else:
                        full_overlap = range[0] == d.topleft.y and range[1] == d.bottomright.y
                    
                    pml_domain = Domain(self.cfg, pml_id, pml_a, d.topleft + pml_offset, pml_size, {}, 'pml', [d])
                    
                    if not full_overlap: pml_domain.local = True
                    
                    D.append(pml_domain)
                    if a > 0 and full_overlap:
                        sec_adjs = set(Domain.ADJACENCIES) - set([adj, Domain.OPPOSITES[adj]])
                        for sec_adj in sec_adjs:
                            sec_pml_id = "%s_%s_%s"%(d.id, adj, sec_adj)
                            other_pml_a = d.edges[sec_adj[0]]['a']
                            # TK: An attempt to prevent refraction on the secondary
                            # PML corner. This is especially effective in the case
                            # of a fully absorbent connecting to a fully reflective
                            # edge. For arbitrary combinations this is more of an
                            # approximation:
                            sec_pml_a = min(
                                max(EPSILON, other_pml_a),
                                max(EPSILON, pml_a))
                            if sec_adj == 'left'  : sec_pml_offset = Point(-n                    ,  0                     )
                            if sec_adj == 'right' : sec_pml_offset = Point( pml_domain.size.width,  0                     )
                            if sec_adj == 'bottom': sec_pml_offset = Point( 0                    , -n                     )
                            if sec_adj == 'top'   : sec_pml_offset = Point( 0                    ,  pml_domain.size.height)
                            sec_pml_size = Size(n, n)
                            D2.append((pml_domain, sec_adj, Domain(self.cfg, sec_pml_id, sec_pml_a, d.topleft + pml_offset + sec_pml_offset, sec_pml_size, {}, 'pml', [pml_domain])))
        for d in D: self.add_domain(d)
        
        # Sort the domains according to their top-left position and size.
        # The sole purpose is to find duplicates in N log N time.
        def coord_pairs(d1, d2):
            return [
                (d1.topleft.x    , d2.topleft.x    ),
                (d1.topleft.y    , d2.topleft.y    ),
                (d1.bottomright.x, d2.bottomright.x),
                (d1.bottomright.y, d2.bottomright.y)
            ]

        def sort_func(d1t, d2t):
            d1, d2 = d1t[2], d2t[2]
            if d1[0].id != d2[0].id:
                return __cmp__(d1[0].id, d2[0].id)
            for a, b in coord_pairs(d1, d2):
                if a != b: return -1 if a < b else 1
            return 0
            
        def key_func(d):
            return d[0].id, d[2].topleft.x, d[2].topleft.y, d[2].bottomright.x, d[2].bottomright.y
            
        D3 = []
        prev = None
        
        try: sorted_domains = sorted(D2, sort_func)
        except: sorted_domains = sorted(D2, key=key_func)
        
        for D in sorted_domains:
            parent, adj, pml_domain = D
            # Check if the domain for which this domain is a PML layer
            # has been assigned a domain on that edge by now.
            if getattr(parent, adj): continue
            
            # Check whether it completely overlaps with the previous
            # entry in the list and whether the domain has the same ancestor
            if prev is None or \
               prev[0].pml_for != parent.pml_for or \
               any([a != b for a, b in coord_pairs(pml_domain, prev[2])]):
                D3.append(D)
                prev = D
            else:
                # Merge
                prfx = commonprefix((prev[2].id, pml_domain.id))
                prev[2].id = prfx + prev[2].id[len(prfx):] + '_' + pml_domain.id[len(prfx):]
                prev[2].pml_for.append(parent)
                prev = None
        
        for d1, a, d2 in D3:
            self.add_domain(d2)


    def calc_pml_matrices(self):
        for d in self.domains: 
            if d.is_pml: 
                d.calc_pml()
    def apply_pml_matrices(self):
        for d in self.domains: 
            if d.is_pml: 
                d.apply_pml()
    def add_source(self,sx,sy):
        self.source_positions.append(Point(sx, sy))
        for d in [_d for _d in self.domains if not _d.is_pml]:
            Sx, Sy = sx - d.topleft.x*self.cfg.dx, sy - d.topleft.y*self.cfg.dx
            dS = np.fromfunction(lambda y,x: np.sqrt((x*self.cfg.dx-Sx)**2+(y*self.cfg.dx-Sy)**2), (d.size.height, d.size.width))
            p0 = np.exp(-self.cfg.bwidth*np.power(np.abs(dS),2.))
            d.p0 += p0
            d.px0 += np.power(np.cos(np.angle(dS)),2.)*p0
            d.pz0 += np.power(np.sin(np.angle(dS)),2.)*p0
    def draw_boundaries(self, fig):
        self.subplot = self.subplot if self.subplot else fig.add_subplot(111)
        for b in self.boundaries:
            b.create_line(self.subplot, self.topleft)
    def draw_receivers(self, fig):
        self.subplot = self.subplot if self.subplot else fig.add_subplot(111)
        for b in self.receivers:
            b.create_annotation(self.subplot, self.topleft)
    def draw_domain_ids(self, fig):
        self.subplot = self.subplot if self.subplot else fig.add_subplot(111)
        prefix_len = len(commonprefix([d.id for d in self.domains]))
        for d in self.domains:
            self.subplot.text(d.topleft.x-self.topleft.x + 1, d.bottomright.y-self.topleft.y - 1, d.id[prefix_len:], fontsize=10)
    def get(self, field):
        a = np.zeros((self.size.height,self.size.width))
        for b in self.domains:
            tl = b.topleft - self.topleft
            a[tl.y:tl.y+b.size.height,tl.x:tl.x+b.size.width] += getattr(b, field)
        return a
    def set(self, field, t, Lp):
        raise Exception("Do not use")
        A, B = sorted([b for b in self.boundaries if b.boundary_type == t])
        assert A.domain2 == B.domain1
        a, b, c = A.domain1, A.domain2, B.domain2        
        if t == BoundaryType.HORIZONTAL:
            assert a.size.height == b.size.height == c.size.height
            h = a.size.height
            # TODO: Isn't it possible to replace '0:h' simply by ':'
            getattr(a, field)[0:h, 0:a.size.width] = Lp[0:h,                         0:a.size.width                          ]
            getattr(b, field)[0:h, 0:b.size.width] = Lp[0:h,              a.size.width:a.size.width+b.size.width             ]
            getattr(c, field)[0:h, 0:c.size.width] = Lp[0:h, a.size.width+b.size.width:a.size.width+b.size.width+c.size.width]
        else:
            assert a.size.width == b.size.width == c.size.width
            w = a.size.width
            getattr(a, field)[0:a.size.height, 0:w] = Lp[0:w,                           0:a.size.height                            ].transpose()
            getattr(b, field)[0:b.size.height, 0:w] = Lp[0:w,               a.size.height:a.size.height+b.size.height              ].transpose()
            getattr(c, field)[0:c.size.height, 0:w] = Lp[0:w, a.size.height+b.size.height:a.size.height+b.size.height+c.size.height].transpose()

# Class to write data from simulation to file (array to bin or plot to file)
class DataWriter:
    def __init__(self,cfgd,scene,write_plot,write_array):
        self.plotdir = cfgd['plotdir']
        self.scene = scene
        self.write_plot = write_plot
        self.write_array = write_array
        self.visualisation_subsampling=cfgd.get('visualisation_subsampling', 1)
        if write_plot or write_array:
            if not os.path.exists(self.plotdir):
                os.mkdir(self.plotdir)

            if not os.access(self.plotdir, os.W_OK | os.X_OK):
                print("Unable to write to directory '%s'" % self.plotdir)
                exit(2)
        if write_plot:
            fi = plt.figure(1, figsize=(12, 9), dpi=90)
            self.pp = plt.imshow(scene.get('p0'), interpolation='bicubic')
            self.pp.set_cmap('BrBG')
            self.pp.colorbar = plt.colorbar()
            self.scene.draw_boundaries(fi)
            self.scene.draw_domain_ids(fi)
            self.scene.draw_receivers(fi)


    # Handle plotting colour scale and draw
    def _write_plot(self,frame):
        self.pp.set_array(self.scene.get('p0'))
        self.pp.autoscale()
        m = max(abs(self.pp.norm.vmin),abs(self.pp.norm.vmin))
        self.pp.norm.vmin = -m
        self.pp.norm.vmax = m

        temp_fileame = os.path.join(self.plotdir,'temp.png')
        image_filename = os.path.join(self.plotdir,'im-%06d.png'%(frame+1))
        plt.savefig(temp_fileame, bbox_inches=0, pad_inches=0)
        # atomic operation
        shutil.move(temp_fileame, image_filename)

    # Write binary data to file
    def _write_array(self,frame):
        for d in (_d for _d in self.scene.domains if not _d.is_pml):
            array_filename = os.path.join(self.plotdir,'%s-%06d.bin'%(d.id,(frame+1)))
            debug_filename = os.path.join(self.plotdir,'%s-%06d.mat'%(d.id,(frame+1)))
            scipy.io.savemat(debug_filename, mdict={'p0': d.p0})
            array_file = open(array_filename,'wb')
            if self.visualisation_subsampling > 1:
                numpy_array = subsample(d.p0, self.visualisation_subsampling)
            else:
                numpy_array = d.p0
            pa = array.array('f',numpy_array.flatten(order='F'))
            pa.tofile(array_file)
            array_file.close()

    # Public method to write data to file
    def write_to_file(self,frame):
        if self.write_array:
            self._write_array(frame)
        if self.write_plot:
            self._write_plot(frame)

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
import math
import array
import itertools
import shutil
from os.path import commonprefix

import matplotlib.pyplot as plt

try:
    import matplotlib.lines as lines
    import matplotlib.patches as patches

    has_matplotlib = True
except:
    has_matplotlib = False

from .functions import *
from .kcalc import Kcalc

EPSILON = 1e-9

# A wrapper class over a numpy array that represents an n-dimensional vector in cartesian space
class Point(object):
    def __init__(self, x, *args):
        self._array = np.array([x] + list(args)) if len(args) else np.array(list(x))
        def isinteger(x):
            return np.equal(np.mod(x, 1), 0)
        for el in self._array:
            if not isinteger(el):
                print("Warning: %s is not integer valued"%self)
    @property
    def array(self):
        return self._array

    @property
    def x(self):
        return self._array[0]

    @property
    def y(self):
        return self._array[1]

    @property
    def z(self):
        return self._array[2]

    @x.setter
    @y.setter
    @z.setter
    @array.setter
    def set_dimension(self):
        raise AttributeError("Points are immutable")

    def __len__(self):
        return len(self.array)

    def __iter__(self):
        return iter(self.array)

    def __getitem__(self, i):
        return self.array[i]

    def __add__(self, other):
        return Point(self.array + other.array)

    def __sub__(self, other):
        return Point(self.array - other.array)

    def __lt__(self, other):
        return any(self.array < other.array)

    def __repr__(self):
        return "%s(%s)" % ("Point", ", ".join("%.1f" % f for f in self.array))


# A rectangular domain; the building block of the scene geometry
class Domain(object):
    ADJACENCIES = ['left', 'right', 'top', 'bottom']
    OPPOSITES = dict((ADJACENCIES[0:2], ADJACENCIES[2:4], ADJACENCIES[0:2][::-1], ADJACENCIES[2:4][::-1]))
    HORIZONTAL = set(ADJACENCIES[:2])
    VERTICAL = set(ADJACENCIES[2:])

    def __init__(self, cfg, id: str, alpha, top_left: Point, size: Point, edges: dict, t='air', pml_for=None):
        """
        Initialization for the domain object. This represents a rectangular building block of a scene.
        :param cfg: Configuration object of the scene (PSTD_Config_Base instance)
        :param id: String identifier
        :param alpha: Acoustic absorption coefficient pf the domains walls
        :param top_left: The upper left corner of the domain (corner closest to origin)
        :param size: The size of the domain
        :param edges: dictionary with edge information (locally reacting and absorption)
        :param t: material of domain interior. Either air or pml
        :param pml_for: which domain this one is a perfectly matched layer
        :return: Domain instance with specified attributes.
        """
        self.id = id
        self.alpha = alpha
        # 0mar: todo: Is alpha actually acoustic absorption coefficient/same as wiki?
        self.top_left = top_left
        self.cfg = cfg
        self.size = size
        self.bottom_right = self.top_left + self.size
        # Initialize the zero matrices. For velocity matrices
        # an additional cell is needed
        self.u0 = self.domain_size_zeros(0, 1)
        self.w0 = self.domain_size_zeros(1, 0)
        self.p0 = self.domain_size_zeros()
        self.px0 = self.domain_size_zeros()
        self.pz0 = self.domain_size_zeros()
        self.u0_old = self.w0_old = self.p0_old = self.px0_old = self.pz0_old = None
        self.neighbour_dict = {adjacency:[] for adjacency in Domain.ADJACENCIES}
        # self.left, self.right, self.top, self.bottom = [], [], [], []
        # self.leftB, self.rightB, self.topB, self.bottomB = [], [], [], []

        # TK: TODO: This is probably wrong.. especially with two neighbouring PML domains
        self.impedance = -((math.sqrt(1. - self.alpha) + 1.) / (math.sqrt(1. - self.alpha) - 1))
        self.rho = self.cfg.rho * self.impedance if self.impedance < 1000 else 1e200

        self.is_pml = t == 'pml'
        self.pml_for = [] if pml_for is None else pml_for

        pml_for_is_pml = [d.is_pml for d in self.pml_for]
        assert (len(self.pml_for) == 0 or pml_for_is_pml.count(pml_for_is_pml[0]) == len(pml_for_is_pml))

        self.is_sec_pml = self.is_pml and pml_for_is_pml[0]
        self.edges = edges

        self.rho_matrices = {}

        self.update_for = {}
        # Dictionary for speed and pressure matrices.
        self.matrix_dict = {}
        self.clear_matrices()

        self.local = False

    def __repr__(self):
        return "%s:\n %s\n o-------------------o\n |  %s  | \n o-------------------o\n      %s\n" % (
        self.id, self.top_left, self.size, self.bottom_right)

    def __lt__(self, other):
        # OR: Todo: Not sure about the ordering this method implies
        return self.top_left < other.top_left

    def domain_size_zeros(self, a=0, b=0):
        return np.zeros((self.size.y + a, self.size.x + b))

    def push_values(self):
        self.u0_old = self.u0
        self.w0_old = self.w0
        self.p0_old = self.p0
        self.px0_old = self.px0
        self.pz0_old = self.pz0

    def clear_matrices(self):
        self.matrix_dict['Lpx'] = self.domain_size_zeros(0, 1)
        self.matrix_dict['Lpz'] = self.domain_size_zeros(1, 0)
        self.matrix_dict['Lvx'] = self.domain_size_zeros()
        self.matrix_dict['Lvz'] = self.domain_size_zeros()

    def calc_rho_matrices(self):
        l = self.neighbour_dict['left'] if len(self.neighbour_dict['left']) else [None]
        r = self.neighbour_dict['right'] if len(self.neighbour_dict['right']) else [None]
        b = self.neighbour_dict['bottom'] if len(self.neighbour_dict['bottom']) else [None]
        t = self.neighbour_dict['top'] if len(self.neighbour_dict['top']) else [None]
        for pair in ((l, r), (b, t)):
            for p in itertools.product(*pair):
                rhos = [d.rho if d else 1e200 for d in p]
                rp, rv = Rmatrices(rhos[0], self.rho, rhos[1])
                self.rho_matrices[frozenset(d for d in p if d)] = {'p': rp, 'v': rv}

    def contains_point(self, p: Point) -> bool:
        return (self.bottom_right.x >= p.x >= self.top_left.x) and (self.bottom_right.y >= p.y >= self.top_left.y)

    def neighbour_list(self):
        return [self.neighbour_dict[adj] for adj in Domain.ADJACENCIES]
        # return [self.left, self.right, self.top, self.bottom]

    def is_neighbour_of(self, other):
        return other in sum(self.neighbour_list(), [])

    def num_neighbours(self, with_pml=True):
        return len([n for n in sum(self.neighbour_list(), []) if n is not None and (not n.is_pml or with_pml)])

    def is_rigid(self):
        return self.impedance > 1000

    def calc_pml(self):
        # Only calculate PML matrices for pml domains with a single non-pml neighbour
        # or for secondary PML domains with a single PML neighbour
        left = self.neighbour_dict['left']
        right = self.neighbour_dict['right']
        top = self.neighbour_dict['top']
        bottom = self.neighbour_dict['bottom']
        num_air_neighbours = self.num_neighbours(with_pml=False)
        num_neighbours = self.num_neighbours(with_pml=True)
        assert (num_air_neighbours == 1 and self.is_pml) or (num_neighbours <= 2 and self.is_sec_pml)
        if self.is_sec_pml:
            self.is_2d = num_neighbours == 2
            # If this domain has its neighbour to the left or to the right a PML 
            # matrix is obtained for horizontal attenuation
            self.is_horizontal = len(left) > 0 or len(right) > 0
            if self.is_2d:
                # Corner PML domains should have a horizontal as well as a vertical component
                # In particular not two neighbours in the same direction.
                assert self.is_horizontal and (len(top) > 0 or len(bottom) > 0)
                self.is_lower = len(left) > 0, len(bottom) > 0
            else:
                # If this neighbour is located to the left or bottom the attenuation is reversed
                self.is_lower = len(left) > 0 or len(bottom) > 0
        else:
            self.is_2d = False
            # If this domain has a neighbour to the left or to the right and this neighbour
            # is not a PML domain, a PML matrix is obtained for horizontal attenuation
            self.is_horizontal = (len(left) > 0 and all([d.is_pml == False for d in left])) or (
            len(right) > 0 and all([d.is_pml == False for d in right]))
            # If this neighbour is located to the left or bottom the attenuation is reversed
            self.is_lower = (len(left) > 0 and all([d.is_pml == False for d in left])) or (
            len(bottom) > 0 and all([d.is_pml == False for d in bottom]))

        def make_attenuation(horizontal, ascending):
            # Depending on the direction of the attenuation the matrices need to be left- or
            # right-multiplied
            left = np.ones((self.size.y, 1)) if horizontal else 1
            right = 1 if horizontal else np.ones((1, self.size.x))
            pfact = self.cfg.pfact if ascending else self.cfg.pfact[::-1]
            ufact = self.cfg.ufact if ascending else self.cfg.ufact[::-1]
            if not horizontal:
                pfact = pfact.reshape(-1, 1)
                ufact = ufact.reshape(-1, 1)
            return left * pfact * right, left * ufact * right

        if self.is_sec_pml and self.is_2d:
            # PML is the product of horizontal and vertical attenuation
            a = make_attenuation(True, self.is_lower[0])
            b = make_attenuation(False, self.is_lower[1])
            self.pml_p, self.pml_u = (a[0], b[0]), (a[1], b[1])
        else:
            self.pml_p, self.pml_u = make_attenuation(self.is_horizontal, self.is_lower)

    def should_update(self, bt) -> bool:
        # Cache this value, it's not changing anyway and we're in Python after all
        if bt in self.update_for:
            return self.update_for[bt]

        def check():
            if self.num_neighbours(False) == 1 and self.is_pml:
                # Always calculate in direction orthogonal to boundary
                if self.is_horizontal and bt == BoundaryType.HORIZONTAL: return True
                if not self.is_horizontal and bt == BoundaryType.VERTICAL: return True

                if self.local:
                    return False

                # Find my non-PML neighbour
                for adjacency in self.ADJACENCIES:
                    domains = self.neighbour_dict[adjacency]
                    opposite_adjacency = Domain.OPPOSITES[adjacency]
                    if len(domains) == 1 and not domains[0].is_pml:
                        opposite_domains = domains[0].neighbour_dict[opposite_adjacency]
                        assert len(opposite_domains) == 1 and opposite_domains[0] == self
                        return not domains[0].edges[opposite_adjacency[0]]['lr']
            else:
                return True

        self.update_for[bt] = check()
        return self.update_for[bt]

    def apply_pml(self):
        assert (self.num_neighbours(False) == 1 and self.is_pml) or (self.num_neighbours(True) <= 2 and self.is_sec_pml)
        # The pressure and velocity matrices are multiplied by the PML values
        if self.is_sec_pml and self.is_2d:
            self.px0 *= self.pml_p[0]
            self.pz0 *= self.pml_p[1]
            self.u0 *= self.pml_u[0]
            self.w0 *= self.pml_u[1]
        elif self.is_horizontal:
            self.px0 *= self.pml_p
            self.u0 *= self.pml_u
        else:
            self.pz0 *= self.pml_p
            self.w0 *= self.pml_u

    def get_range(self, bt):
        if bt == BoundaryType.HORIZONTAL:
            a, b = self.top_left.y, self.bottom_right.y
        else:
            a, b = self.top_left.x, self.bottom_right.x
        return range(int(a), int(b))

    def get_detached(self, adj):
        bt = BoundaryType.ortho(BoundaryType.from_domain_adjacency(adj))
        own = set(self.get_range(bt))
        for neighbour in self.neighbour_dict[adj]:
            own -= set(neighbour.get_range(bt))

        def decompose(s):
            s = sorted(s)
            r = []
            for i, x in enumerate(s):
                if i == 0 or s[i - 1] + 1 < x: r.append([x, x])
                if i == len(s) - 1 or s[i + 1] - 1 > x: r[-1][-1] = x + 1
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
            lower, upper = sorted(map(int, [getattr(domain.top_left, coord), getattr(domain.bottom_right, coord)]))
            if d >= lower and d <= upper: return domain
        raise LookupError("No domain found at %s for %.2f" % (adj, d))

    def calc(self, bt, ct, dest=None):
        # str = ": Calculating L%s%s for domain '%s'        "%(ct,bt,self.id)
        # sys.stdout.write(str)
        # sys.stdout.flush()
        # Find the input matrices
        domains1 = self.neighbour_dict['left'] if bt == BoundaryType.HORIZONTAL else self.neighbour_dict['bottom']
        domains2 = self.neighbour_dict['right'] if bt == BoundaryType.HORIZONTAL else self.neighbour_dict['top']

        if len(domains1) == 0: domains1 = [None]
        if len(domains2) == 0: domains2 = [None]

        own_range = self.get_range(bt)

        if dest:
            if bt == BoundaryType.HORIZONTAL:
                source = self.domain_size_zeros(0, 1)
            else:
                source = self.domain_size_zeros(1, 0)
        else:
            source = self.matrix_dict["L%s%s" % (ct, bt)]
        for domain1, domain2 in itertools.product(domains1, domains2):
            rho_matrix_key = frozenset(d for d in (domain1, domain2) if d)

            range_intersection = set(own_range)
            # print (self.id, bt, 'own', own_range)

            for other_domain in (domain1, domain2):
                if other_domain is not None:
                    if bt == BoundaryType.HORIZONTAL:
                        other_range = range(int(other_domain.top_left.y), int(other_domain.bottom_right.y))
                    else:
                        other_range = range(int(other_domain.top_left.x), int(other_domain.bottom_right.x))
                    # print (other_domain.id, bt, 'other', other_range)
                    range_intersection &= set(other_range)

            if not len(range_intersection): continue

            range_start, range_end = min(range_intersection), max(range_intersection) + 1

            primary_dimension = self.size.x if bt == BoundaryType.HORIZONTAL else self.size.y
            secondary_dimension = range_end - range_start  #self.size.y if bt == BoundaryType.HORIZONTAL else self.size.x

            Ntot = 2. * self.cfg.Wlength + primary_dimension
            if ct == CalculationType.PRESSURE:
                Ntot += 1
            else:
                primary_dimension += 1
            matrix1 = matrix2 = None
            if ct == CalculationType.VELOCITY and domain1 is None and domain2 is None:
                # For a PML layer parallel to its interface direction the matrix is concatenated with zeros
                # TK: TODO: This statement is probably no longer correct, with arbitrary domain configurations
                # a PML domain can also have a neighbour
                #   |             |
                # __|_____________|___
                #   |     PML     |
                #  <--------------->
                matrix1 = self.domain_size_zeros(0 if bt == BoundaryType.HORIZONTAL else 1,
                                                 1 if bt == BoundaryType.HORIZONTAL else 0)
                matrix2 = self.domain_size_zeros(0 if bt == BoundaryType.HORIZONTAL else 1,
                                                 1 if bt == BoundaryType.HORIZONTAL else 0)
                domain1 = domain2 = self
            else:
                if domain1 is None: domain1 = self
                if domain2 is None: domain2 = self

            matrix0 = self.p0 if ct == CalculationType.PRESSURE else getattr(self,
                                                                             'u0' if bt == BoundaryType.HORIZONTAL else 'w0')
            if matrix1 is None:
                matrix1 = domain1.p0 if ct == CalculationType.PRESSURE else getattr(domain1,
                                                                                    'u0' if bt == BoundaryType.HORIZONTAL else 'w0')
            if matrix2 is None:
                matrix2 = domain2.p0 if ct == CalculationType.PRESSURE else getattr(domain2,
                                                                                    'u0' if bt == BoundaryType.HORIZONTAL else 'w0')

            a = 0 if ct == CalculationType.PRESSURE else 1
            b = 1 if bt == BoundaryType.HORIZONTAL else 0

            if dest:
                kc = dest['fact']
            else:
                kc = Kcalc.derfactp((Ntot, self.cfg.dx)) if ct == CalculationType.PRESSURE else Kcalc.derfactu(
                    (Ntot, self.cfg.dx))

            # Determine which rho matrix instance to use
            rmat = self.rho_matrices[rho_matrix_key][ct]

            if bt == BoundaryType.HORIZONTAL:
                matrix0_offset = self.top_left.y
                matrix0_indexed = matrix0[range_start - matrix0_offset:range_end - matrix0_offset, :]
                matrix1_offset = domain1.top_left.y
                matrix1_indexed = matrix1[range_start - matrix1_offset:range_end - matrix1_offset, :]
                matrix2_offset = domain2.top_left.y
                matrix2_indexed = matrix2[range_start - matrix2_offset:range_end - matrix2_offset, :]
            else:
                matrix0_offset = self.top_left.x
                matrix0_indexed = matrix0[:, range_start - matrix0_offset:range_end - matrix0_offset]
                matrix1_offset = domain1.top_left.x
                matrix1_indexed = matrix1[:, range_start - matrix1_offset:range_end - matrix1_offset]
                matrix2_offset = domain2.top_left.x
                matrix2_indexed = matrix2[:, range_start - matrix2_offset:range_end - matrix2_offset]

            matrix = spatderp3(matrix0_indexed, kc, self.cfg.Wlength, self.cfg.A, primary_dimension,
                               secondary_dimension, nearest_2power(Ntot), rmat, matrix1_indexed, matrix2_indexed, a, b)

            if bt == BoundaryType.HORIZONTAL:
                source[range_start - matrix0_offset:range_end - matrix0_offset, :] = matrix
            else:
                source[:, range_start - matrix0_offset:range_end - matrix0_offset] = matrix
        return source

    def rk_update(self, sub_frame):
        self.u0 = self.u0_old + (-self.cfg.dtRK * self.cfg.alfa[sub_frame] * ( 1 / self.rho * self.matrix_dict['Lpx'])).real
        self.w0 = self.w0_old + (-self.cfg.dtRK * self.cfg.alfa[sub_frame] * ( 1 / self.rho * self.matrix_dict['Lpz'])).real
        self.px0 = self.px0_old + (
            -self.cfg.dtRK * self.cfg.alfa[sub_frame] * (self.rho * pow(self.cfg.c1, 2.) * self.matrix_dict['Lvx'])).real
        self.pz0 = self.pz0_old + (
            -self.cfg.dtRK * self.cfg.alfa[sub_frame] * (self.rho * pow(self.cfg.c1, 2.) * self.matrix_dict['Lvz'])).real
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
        order = a.top_left < b.top_left  # delivers trouble in arbitrary domain configurations
        self.domain1 = a if order else b
        self.domain2 = b if order else a
        self.boundary_type = t
        if self.boundary_type == BoundaryType.HORIZONTAL:
            self.line = (self.domain1.top_left.x, self.domain1.bottom_right.x), (
            self.domain1.bottom_right.y, self.domain1.bottom_right.y)
        else:
            self.line = (self.domain1.bottom_right.x, self.domain1.bottom_right.x), (
            self.domain1.top_left.y, self.domain1.bottom_right.y)

    def create_line(self, fig, tl):
        if not has_matplotlib: return
        # Plots a line on the canvas, 
        # PML|PML is not drawn
        # air|PML in a dark gray
        # air|air in a light gray
        if self.domain1.is_pml and self.domain2.is_pml: return
        c = '#eeeeee' if self.domain1.is_pml == self.domain2.is_pml else '#bbbbbb'
        fig.add_line(lines.Line2D([x - tl.x for x in self.line[0]], [y - tl.y for y in self.line[1]], color=c))

    def __repr__(self):
        attrs = ('x', self.domain1.bottom_right.x) if self.boundary_type == BoundaryType.HORIZONTAL else (
        'y', self.domain1.bottom_right.y)
        return "Boundary at %s=%d" % attrs


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

            p = self.grid_location - self.container.top_left
            value = self.container.p0[p.y, p.x]
        else:
            # Spectral interpolation 

            top_domain = self.container.find_neighbor_at('top', self.location.x)
            bottom_domain = self.container.find_neighbor_at('bottom', self.location.x)

            def calc_fact(size, bt):
                primary_dimension = size.x if bt == BoundaryType.HORIZONTAL else size.y
                Ntot = 2. * self.cfg.Wlength + primary_dimension + 1  # Pressure matrix, hence +1
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

            dx = (self.grid_location - self.container.top_left).x
            dx_top = (self.grid_location - top_domain.top_left).x
            dx_bottom = (self.grid_location - bottom_domain.top_left).x

            zfact = calc_fact(Point(1, self.container.size.y), BoundaryType.VERTICAL)

            bottom_top_key = frozenset([bottom_domain, top_domain])
            rho = self.container.rho_matrices[bottom_top_key]['p']
            Ntot = 2. * self.cfg.Wlength + self.container.size.y + 1

            p0shift = spatderp3(p0dx[:, dx:dx + 1], zfact, self.cfg.Wlength, self.cfg.A, self.container.size.y, 1,
                                nearest_2power(Ntot), rho, p0dx_bottom[:, dx_bottom:dx_bottom + 1],
                                p0dx_top[:, dx_top:dx_top + 1], 0, 0)

            dy = self.grid_location.y - self.container.top_left.y

            value = p0shift[dy, 0]

        self.array.append(value)

        return value

    def create_annotation(self, fig, tl):
        if not has_matplotlib:
            return
        fig.add_patch(
            patches.Circle((self.location.x - tl.x, self.location.y - tl.y), radius=2, color='#8899bb', fill=False))

    def __repr__(self):
        return "Receiver at " + repr(self.location)


class Scene(object):
    def __init__(self, cfg):
        self.cfg = cfg
        self.domains = []
        self.boundaries = []
        self.receivers = []
        self.source_positions = []
        self.top_left = Point(1e9, 1e9)
        self.bottom_right = Point(-1e9, -1e9)
        self.size = Point(0, 0)
        self.subplot = None

    def __repr__(self):
        return "\n".join(repr(x) for x in self.domains) + "\n" + "\n".join(
            repr(x) for x in self.boundaries) + "\n" + "\n".join(
            repr(x) for x in self.receivers) + "\nSources:\n" + "\n".join(repr(x) for x in self.source_positions)

    def __getattr__(self, k):
        matches = [d for d in self.domains if d.id == k]
        assert len(matches) == 1
        return matches[0]

    def add_receiver(self, rx, ry):
        location = Point(rx / self.cfg.dx, ry / self.cfg.dx)
        container = None
        for d in self.domains:
            if not d.is_pml and d.contains_point(location): container = d
        assert container is not None
        receiver = Receiver(self.cfg, "receiver%d" % (len(self.receivers) + 1), location, container)
        self.receivers.append(receiver)

    def add_domain(self, domain: Domain):
        self.top_left = Point(min(self.top_left.x, domain.top_left.x), min(self.top_left.y, domain.top_left.y))
        self.bottom_right = Point(max(self.bottom_right.x, domain.bottom_right.x), max(self.bottom_right.y, domain.bottom_right.y))
        self.size = Point(self.bottom_right - self.top_left)

        opposite = {'left': 'right', 'top': 'bottom', 'right': 'left', 'bottom': 'top'}

        for other_domain in self.domains:
            # TK: Not entirely sure whether this is always correct, for
            # the use cases for openPSTD, in which the PML layers are
            # always added implicitly at the end, it is sufficient:
            # A PML domain can only be a neighbour of another PML domain
            # in case they both connect to a set of two non-PML domains
            # that are neighbours of each other too.
            if domain.is_sec_pml and other_domain.is_sec_pml:
                continue
            if domain.is_sec_pml and not other_domain.is_pml:
                continue
            if other_domain.is_sec_pml and not domain.is_pml:
                continue
            if domain.is_pml and other_domain.is_pml:
                if (domain.is_sec_pml and other_domain in domain.pml_for) or (other_domain.is_sec_pml and domain in other_domain.pml_for):
                    # secondary pml layers should have a single pml neighbour
                    pass
                elif not (len(other_domain.pml_for) == 1 and len(domain.pml_for) == 1 and other_domain.pml_for[0].is_neighbour_of(
                        domain.pml_for[0]) and other_domain.is_sec_pml == domain.is_sec_pml):
                    continue

            # Test for the following situations:
            #
            #   | a   #   b |   #  _____  #  a | b
            #   |--   #   --|   #  b | a  #  __|__
            #   | b   #   a |   #         # 
            #  (1)    #   (2)   #  (3)    #  (4)

            B = None
            orientation = None

            if other_domain.top_left.x == domain.bottom_right.x and len(set(range(int(other_domain.top_left.y), int(other_domain.bottom_right.y))) & set(
                    range(int(domain.top_left.y),
                          int(domain.bottom_right.y)))):  # and a.top_left.y == b.top_left.y and a.size.y == b.size.y: # (3)
                B = Boundary(other_domain, domain, BoundaryType.VERTICAL)
                orientation = 'left'
            if other_domain.bottom_right.x == domain.top_left.x and len(set(range(int(other_domain.top_left.y), int(other_domain.bottom_right.y))) & set(
                    range(int(domain.top_left.y),
                          int(domain.bottom_right.y)))):  # and a.top_left.y == b.top_left.y and a.size.y == b.size.y: # (4)
                B = Boundary(other_domain, domain, BoundaryType.VERTICAL)
                orientation = 'right'
            if other_domain.top_left.y == domain.bottom_right.y and len(set(range(int(other_domain.top_left.x), int(other_domain.bottom_right.x))) & set(
                    range(int(domain.top_left.x),
                          int(domain.bottom_right.x)))):  # and a.top_left.x == b.top_left.x and a.size.x == b.size.x: # (1)
                B = Boundary(other_domain, domain, BoundaryType.HORIZONTAL)
                orientation = 'bottom'
            if other_domain.bottom_right.y == domain.top_left.y and len(set(range(int(other_domain.top_left.x), int(other_domain.bottom_right.x))) & set(
                    range(int(domain.top_left.x),
                          int(domain.bottom_right.x)))):  # and a.top_left.x == b.top_left.x and a.size.x == b.size.x: # (2)
                B = Boundary(other_domain, domain, BoundaryType.HORIZONTAL)
                orientation = 'top'

            # PML layers can only have a single non-PML neighbour
            is_neighbour = bool(orientation)
            a_is_pml_for_other_domain = other_domain.is_pml and not domain.is_pml and domain not in other_domain.pml_for
            b_is_pml_for_other_domain = domain.is_pml and not other_domain.is_pml and other_domain not in domain.pml_for
            if orientation and not (a_is_pml_for_other_domain or b_is_pml_for_other_domain):
                self.boundaries.append(B)
                other_domain.neighbour_dict[orientation].append(domain)
                domain.neighbour_dict[opposite[orientation]].append(other_domain)

        self.domains.append(domain)

    def calc_rho_matrices(self):
        for d in self.domains: d.calc_rho_matrices()

    def add_pml_domains(self, n=None):
        if n is None:
            n = self.cfg.PMLcells
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
                    pml_id = "%s_%s" % (d.id, adj)
                    if len(rs) > 1: pml_id += '_%d' % idx
                    a = d.edges[adj[0]]['a']
                    pml_a = max(EPSILON, a)
                    if adj == 'left': pml_offset = Point(-n, range[0] - d.top_left.y)
                    if adj == 'right': pml_offset = Point(d.size.x, range[0] - d.top_left.y)
                    if adj == 'bottom': pml_offset = Point(range[0] - d.top_left.x, -n)
                    if adj == 'top': pml_offset = Point(range[0] - d.top_left.x, d.size.y)

                    if adj == 'left' or adj == 'right':
                        pml_size = Point(n, range[1] - range[0])
                    else:
                        pml_size = Point(range[1] - range[0], n)

                    # Secondary PML domains are only added for primary PML domains that
                    # completely overlap with their AIR domain boundary. Furthermore,
                    # otherwise the PML layer is set to be locally reacting.
                    bt = BoundaryType.ortho(BoundaryType.from_domain_adjacency(adj))
                    if bt == BoundaryType.VERTICAL:
                        full_overlap = range[0] == d.top_left.x and range[1] == d.bottom_right.x
                    else:
                        full_overlap = range[0] == d.top_left.y and range[1] == d.bottom_right.y

                    pml_domain = Domain(self.cfg, pml_id, pml_a, d.top_left + pml_offset, pml_size, {}, 'pml', [d])

                    if not full_overlap: pml_domain.local = True

                    D.append(pml_domain)
                    if a > 0 and full_overlap:
                        sec_adjs = set(Domain.ADJACENCIES) - set([adj, Domain.OPPOSITES[adj]])
                        for sec_adj in sec_adjs:
                            sec_pml_id = "%s_%s_%s" % (d.id, adj, sec_adj)
                            other_pml_a = d.edges[sec_adj[0]]['a']
                            # TK: An attempt to prevent refraction on the secondary
                            # PML corner. This is especially effective in the case
                            # of a fully absorbent connecting to a fully reflective
                            # edge. For arbitrary combinations this is more of an
                            # approximation:
                            sec_pml_a = min(
                                max(EPSILON, other_pml_a),
                                max(EPSILON, pml_a))
                            if sec_adj == 'left': sec_pml_offset = Point(-n, 0)
                            if sec_adj == 'right': sec_pml_offset = Point(pml_domain.size.x, 0)
                            if sec_adj == 'bottom': sec_pml_offset = Point(0, -n)
                            if sec_adj == 'top': sec_pml_offset = Point(0, pml_domain.size.y)
                            sec_pml_size = Point(n, n)
                            D2.append((pml_domain, sec_adj,
                                       Domain(self.cfg, sec_pml_id, sec_pml_a, d.top_left + pml_offset + sec_pml_offset,
                                              sec_pml_size, {}, 'pml', [pml_domain])))
        for d in D:
            self.add_domain(d)

        # Sort the domains according to their top-left position and size.
        # The sole purpose is to find duplicates in N log N time.
        # OR: Todo: Is there still a well defined ordering with arbitrary domain configurations...?
        def coord_pairs(d1, d2):
            return [
                (d1.top_left.x, d2.top_left.x),
                (d1.top_left.y, d2.top_left.y),
                (d1.bottom_right.x, d2.bottom_right.x),
                (d1.bottom_right.y, d2.bottom_right.y)
            ]

        def sort_func(d1t, d2t):
            d1, d2 = d1t[2], d2t[2]
            if d1[0].id != d2[0].id:
                return __cmp__(d1[0].id, d2[0].id)
            for a, b in coord_pairs(d1, d2):
                if a != b: return -1 if a < b else 1
            return 0

        def key_func(d):
            return d[0].id, d[2].top_left.x, d[2].top_left.y, d[2].bottom_right.x, d[2].bottom_right.y

        D3 = []
        prev = None

        try:
            sorted_domains = sorted(D2, sort_func)
        except:
            sorted_domains = sorted(D2, key=key_func)

        for D in sorted_domains:
            parent, adj, pml_domain = D
            # Check if the domain for which this domain is a PML layer
            # has been assigned a domain on that edge by now.
            if parent.neighbour_dict[adj]: continue

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

    def add_source(self, sx, sy):
        self.source_positions.append(Point(sx, sy))
        for d in [_d for _d in self.domains if not _d.is_pml]:
            Sx, Sy = sx - d.top_left.x * self.cfg.dx, sy - d.top_left.y * self.cfg.dx
            dS = np.fromfunction(lambda y, x: np.sqrt((x * self.cfg.dx - Sx) ** 2 + (y * self.cfg.dx - Sy) ** 2),
                                 (d.size.y, d.size.x))
            p0 = np.exp(-self.cfg.bwidth * np.power(np.abs(dS), 2.))
            d.p0 += p0
            d.px0 += np.power(np.cos(np.angle(dS)), 2.) * p0
            d.pz0 += np.power(np.sin(np.angle(dS)), 2.) * p0

    def draw_boundaries(self, fig):
        self.subplot = self.subplot if self.subplot else fig.add_subplot(111)
        for b in self.boundaries:
            b.create_line(self.subplot, self.top_left)

    def draw_receivers(self, fig):
        self.subplot = self.subplot if self.subplot else fig.add_subplot(111)
        for b in self.receivers:
            b.create_annotation(self.subplot, self.top_left)

    def draw_domain_ids(self, fig):
        self.subplot = self.subplot if self.subplot else fig.add_subplot(111)
        prefix_len = len(commonprefix([d.id for d in self.domains]))
        for d in self.domains:
            self.subplot.text(d.top_left.x - self.top_left.x + 1, d.bottom_right.y - self.top_left.y - 1, d.id[prefix_len:],
                              fontsize=10)

    def get(self, field):
        a = np.zeros((self.size.y, self.size.x))
        for b in self.domains:
            tl = b.top_left - self.top_left
            a[tl.y:tl.y + b.size.y, tl.x:tl.x + b.size.x] += getattr(b, field)
        return a

    def set(self, field, t, Lp):
        raise Exception("Do not use")
        A, B = sorted([b for b in self.boundaries if b.boundary_type == t])
        assert A.domain2 == B.domain1
        a, b, c = A.domain1, A.domain2, B.domain2
        if t == BoundaryType.HORIZONTAL:
            assert a.size.y == b.size.y == c.size.y
            h = a.size.y
            # TODO: Isn't it possible to replace '0:h' simply by ':'
            getattr(a, field)[0:h, 0:a.size.x] = Lp[0:h, 0:a.size.x]
            getattr(b, field)[0:h, 0:b.size.x] = Lp[0:h, a.size.x:a.size.x + b.size.x]
            getattr(c, field)[0:h, 0:c.size.x] = Lp[0:h, a.size.x + b.size.x:a.size.x + b.size.x + c.size.x]
        else:
            assert a.size.x == b.size.x == c.size.x
            w = a.size.x
            getattr(a, field)[0:a.size.y, 0:w] = Lp[0:w, 0:a.size.y].transpose()
            getattr(b, field)[0:b.size.y, 0:w] = Lp[0:w, a.size.y:a.size.y + b.size.y].transpose()
            getattr(c, field)[0:c.size.y, 0:w] = Lp[0:w, a.size.y + b.size.y:a.size.y + b.size.y + c.size.y].transpose()


# Class to write data from simulation to file (array to bin or plot to file)
class DataWriter:
    def __init__(self, cfgd, scene, write_plot, write_array):
        self.plotdir = cfgd['plotdir']
        self.scene = scene
        self.write_plot = write_plot
        self.write_array = write_array
        self.visualisation_subsampling = cfgd.get('visualisation_subsampling', 1)
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
    def _write_plot(self, frame):
        self.pp.set_array(self.scene.get('p0'))
        self.pp.autoscale()
        m = max(abs(self.pp.norm.vmin), abs(self.pp.norm.vmin))
        self.pp.norm.vmin = -m
        self.pp.norm.vmax = m

        temp_fileame = os.path.join(self.plotdir, 'temp.png')
        image_filename = os.path.join(self.plotdir, 'im-%06d.png' % (frame + 1))
        plt.savefig(temp_fileame, bbox_inches=0, pad_inches=0)
        # atomic operation
        shutil.move(temp_fileame, image_filename)

    # Write binary data to file
    def _write_array(self, frame):
        for d in (_d for _d in self.scene.domains if not _d.is_pml):
            array_filename = os.path.join(self.plotdir, '%s-%06d.bin' % (d.id, (frame + 1)))
            array_file = open(array_filename, 'wb')
            if self.visualisation_subsampling > 1:
                numpy_array = subsample(d.p0, self.visualisation_subsampling)
            else:
                numpy_array = d.p0
            pa = array.array('f', numpy_array.flatten(order='F'))
            pa.tofile(array_file)
            array_file.close()

    # Public method to write data to file
    def write_to_file(self, frame):
        if self.write_array:
            self._write_array(frame)
        if self.write_plot:
            self._write_plot(frame)
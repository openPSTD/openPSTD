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
Internationalisation.

Author: Thomas Krijnen
"""


class i18n:
    def __init__(self, lang): pass
    def __getitem__(self,k):
        return {
            'NON_ORTHO': 'The selected mesh is not orthogonal',
            'NON_2D': 'The selected mesh is not two-dimensional',
            'NO_MESH': 'The selected object is not a mesh object',
            'NO_SPEAKERS': 'There are no speaker objects in the scene',
            'NON_QUAD': 'The selected mesh contains non-quadrangular faces',
            'PML_LT_WINDOW': 'The amount of PML cells is less than the '
                'specified window size',
            'NO_EXECUTE': 'The specified Python executable path can not be '
                'ran',
            'DETERMINANT_0': 'The domain object has an non-invertable '
                'matrix'
        }.get(k, k)

i18n = i18n('en_US')

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

import bmesh

def layer(bm, type, name):
    try: return getattr(bm.edges.layers, type)[name]
    except: return getattr(bm.edges.layers, type).new(name)

def update(me, type, name, v):
    bm = bmesh.from_edit_mesh(me)
    l = layer(bm, type, name)
    for e in (_e for _e in bm.edges if _e.select):
        e[l] = v

def updater(type, bm_name, scn_name):
    return lambda self, context: update(context.edit_object.data, type, bm_name, getattr(context.scene, scn_name))

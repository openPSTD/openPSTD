//////////////////////////////////////////////////////////////////////////
//                                                                      //
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

uniform mat3 u_view;
attribute vec2 a_position;
attribute vec4 a_color;
varying vec4 v_color;

attribute vec2 a_texcoord;
varying vec2 v_texcoord;

void main()
{
    vec2 position = (u_view * vec3(a_position, 1)).xy;

    gl_Position = vec4(position, 0.0, 1.0);
    v_color = a_color;
    v_texcoord = a_texcoord;
}
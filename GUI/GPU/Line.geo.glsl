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
#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_view;

uniform float thickness;
void main()
{
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;

    vec4 line = (p1-p0);
    vec4 normal = vec4(normalize(vec2(-line.y, line.x)), 0, 0);

    vec4 a = p0 - thickness * normal;
    vec4 b = p0 + thickness * normal;
    vec4 c = p1 - thickness * normal;
    vec4 d = p1 + thickness * normal;

    gl_Position = u_view*b; EmitVertex();
    gl_Position = u_view*d; EmitVertex();
    gl_Position = u_view*a; EmitVertex();
    gl_Position = u_view*c; EmitVertex();
    EndPrimitive();
}

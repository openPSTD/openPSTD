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

//Simulation2D.frag

#version 330

uniform float vmin;
uniform float vmax;

in vec2 v_texcoord;

uniform sampler2D values;

out vec4 color;

void main()
{
    float value = texture(values, v_texcoord).r;

    if(value < vmin)
    {
        value = vmin;
    }
    else if(value > vmax)
    {
        value = vmax;
    }

    value = (value-vmin)/(vmax-vmin)*2-1;
    float valueAbs = abs(value);
    if(valueAbs != 0)
        valueAbs = pow(valueAbs, 0.5);
    if(0 < value)
    {
        color = vec4(valueAbs, 0, 0, 1);
    }
    else
    {
        color = vec4(0, 0, valueAbs, 1);
    }

}
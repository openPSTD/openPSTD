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
uniform float v_log_min;

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
    //value is between -1 and +1, also the result is between 0 and 1 (everything below 0 will be ignored)
    float result = 20*(log(abs(value))/log(10))/-v_log_min+1;
    //create a hot color map
    color = vec4(clamp(result*3, 0, 1), clamp(result*3-1.0/3.0, 0, 1), clamp(result*3-2.0/3.0, 0, 1), 1);
}
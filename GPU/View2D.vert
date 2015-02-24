uniform float scale;
attribute vec2 position;
attribute vec4 color;
varying vec4 v_color;

void main()
{
    gl_Position = vec4(position*scale, 0.0, 1.0);
    v_color = color;
}
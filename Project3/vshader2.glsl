#version 120 
attribute vec4 coord;
varying vec2 texpos;
void main(void) {
  gl_Position = vec4(coord.x, coord.y, 0);

  texpos = coord.zw;
}
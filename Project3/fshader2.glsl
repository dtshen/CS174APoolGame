#version 120 
varying vec2 txpos;
uniform sampler2D tex;
uniform vec4 color;

void main(void){
gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;
}
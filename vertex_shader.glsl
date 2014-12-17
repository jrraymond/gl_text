#version 330 core

in vec4 pos ;
out vec2 texture_pos ;

void main() {
    gl_Position = vec4(pos.xy, 0, 1) ;
    texture_pos = pos.zw ;
}
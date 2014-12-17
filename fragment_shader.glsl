#version 330 core
in vec2 texture_pos ;
uniform sampler2D texture ;
uniform vec4 color ;

void main() {
    gl_FragColor = vec4(1,1,1, texture2D(texture, texture_pos).a) * color ;
}

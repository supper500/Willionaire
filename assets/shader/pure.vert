#version 330 core
layout (location = 0) in vec2 Pos;
uniform mat2 linear;
uniform vec2 translation;
void main(){
	gl_Position = vec4(linear*Pos+translation, 0.0, 1.0);
}
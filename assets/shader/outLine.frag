#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D Texture;
uniform vec4 Color;
void main(){
	FragColor=Color;
	FragColor.w*=texture(Texture,TexCoords).w*0.8;
}
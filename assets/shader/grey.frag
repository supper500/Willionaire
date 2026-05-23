#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D Texture;
void main(){
	vec4 tmp=texture(Texture,TexCoords);
	FragColor.w=tmp.w;
	tmp.x=0.299*tmp.x+0.587*tmp.y+0.114*tmp.z;
	FragColor.x=FragColor.y=FragColor.z=tmp.x;
}
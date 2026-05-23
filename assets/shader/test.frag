#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D Texture;
void main(){
	FragColor=texture(Texture,TexCoords);
	FragColor=vec4(gl_FragCoord.x/1280.0,gl_FragCoord.y/1280.0,0.0,1.0);
}
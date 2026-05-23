#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D atlasTexture;
uniform vec3 textColor;

void main(){
    float glyphShape = texture(atlasTexture, TexCoords).r;
    FragColor = vec4(textColor, glyphShape);
	// FragColor = vec4(textColor, 1.0);
}
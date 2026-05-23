#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aFaceIndex;

uniform mat4 Mat;

out vec2 TexCoord;
flat out int FaceIndex;

void main() {
	gl_Position = Mat * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
	FaceIndex = int(aFaceIndex);
}
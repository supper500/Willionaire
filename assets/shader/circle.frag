#version 330 core
in vec4 pos_center;
in vec4 color;
in vec2 radius;
out vec4 FragColor;
void main(){
	float dx=(pos_center.x-pos_center.z)/radius.x;
	float dy=(pos_center.y-pos_center.w)/radius.y;
	if(dx*dx+dy*dy>1) discard;
	FragColor=color;
}
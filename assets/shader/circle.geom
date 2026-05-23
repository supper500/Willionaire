#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
in vec2 r[];
in vec4 c[];
out vec4 pos_center;
out vec4 color;
out vec2 radius;
void main(){
	vec4 Pos=gl_in[0].gl_Position;
	color=c[0];
	radius=r[0];
	gl_Position=Pos+vec4(-r[0].x,-r[0].y,0.0,0.0);
	pos_center=vec4(gl_Position.xy,Pos.xy);
	EmitVertex();
	gl_Position=Pos+vec4(-r[0].x,r[0].y,0.0,0.0);
	pos_center=vec4(gl_Position.xy,Pos.xy);
	EmitVertex();
	gl_Position=Pos+vec4(r[0].x,-r[0].y,0.0,0.0);
	pos_center=vec4(gl_Position.xy,Pos.xy);
	EmitVertex();
	gl_Position=Pos+vec4(r[0].x,r[0].y,0.0,0.0);
	pos_center=vec4(gl_Position.xy,Pos.xy);
	EmitVertex();
}
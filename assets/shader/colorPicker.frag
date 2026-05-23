#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
void main(){
	vec4 clr=vec4(0.0,0.0,0.0,1.0);

	float tx=TexCoords.x*6;
	if(tx<1.0 || tx>5.0) clr.x=1.0;
	else if(tx<2.0) clr.x=2.0-tx;
	else if(tx>4.0) clr.x=tx-4.0;
	if(tx>1.0 && tx<3.0) clr.y=1.0;
	else if(tx<1.1) clr.y=tx;
	else if(tx<4.0) clr.y=4.0-tx;
	if(tx>3.0 && tx<5.0) clr.z=1.0;
	else if(tx>4.9) clr.z=6.0-tx;
	else if(tx>2.0) clr.z=tx-2.0;

	if(TexCoords.y>0.5)
		FragColor=mix(clr,vec4(0.0,0.0,0.0,1.0),TexCoords.y*2-1.0);
	else FragColor=mix(clr,vec4(1.0,1.0,1.0,1.0),1.0-TexCoords.y*2);
}
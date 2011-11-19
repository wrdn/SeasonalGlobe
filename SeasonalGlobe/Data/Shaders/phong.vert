varying vec3 Nt, Lt, Vt;
varying vec2 tc;

void main(void)
{
	vec4 objectPos = gl_ModelViewMatrix * gl_Vertex;
	Nt.xyz = gl_NormalMatrix * gl_Normal;
	Lt.xyz = vec3(0,0,0) - objectPos.xyz;
	Vt.xyz = vec3(0,0.5,4) - objectPos.xyz;
	tc = gl_MultiTexCoord0.xy;

	gl_Position = ftransform();
}
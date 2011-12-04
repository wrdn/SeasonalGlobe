uniform vec3 eyePos;
varying vec3 viewDir, lightDir, Normalv;

void main()
{
	gl_Position = ftransform();
	vec4 objpos = gl_ModelViewMatrix * gl_Vertex;
	viewDir = eyePos - objpos.xyz;
	lightDir = vec3(0,10,-100) - objpos.xyz;
	Normalv = gl_NormalMatrix * gl_Normal;
};
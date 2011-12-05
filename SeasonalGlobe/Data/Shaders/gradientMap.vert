varying vec2 TexCoord;
uniform float t;
varying float HT;

void main()
{
	mat4 modMVM = gl_ModelViewMatrix;
	
	// Spherical billboarding
	modMVM[0][0] = 1.0;
	modMVM[2][2] = 1.0;
	modMVM[0][1] = 0.0;
	modMVM[0][2] = 0.0;
	modMVM[2][0] = 0.0;
	modMVM[2][1] = 0.0;
	
	vec4 pos = gl_ProjectionMatrix * modMVM * gl_Vertex;
	
	gl_Position = pos;
	TexCoord = gl_MultiTexCoord0.xy;
	
	float p = fract(gl_Vertex.y + 0.48 * t);
	HT = 1-p;
};
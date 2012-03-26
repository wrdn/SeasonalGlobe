varying vec2 TexCoord;
varying float k;
uniform float Time;

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
	
	vec4 translatedPos = gl_ModelViewMatrix * gl_Vertex;
	
	TexCoord = gl_Vertex.xy;
	float t = fract(translatedPos.z + 0.4 * Time);
	k = 1.0 - t;
}
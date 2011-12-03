varying vec4 Normal;
varying vec2 texcoord;
uniform vec3 camPos, lightPos;
uniform int K;
uniform vec3 cameraPosition2; 
void main(void)
{
	Normal = vec4(gl_Normal,1);
	texcoord.xy = gl_MultiTexCoord0.xy;
	gl_Position = ftransform();
};
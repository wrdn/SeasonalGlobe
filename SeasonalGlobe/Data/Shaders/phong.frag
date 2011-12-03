uniform sampler2D tex;
uniform vec4 mulCol;
uniform vec3 cameraPosition2; 
uniform vec3 timeVariable; 
varying vec4 Normal;
varying vec2 texcoord;

void main(void)
{
	if(timeVariable.x>2) { };

	vec2 tc = texcoord;
	tc.xy = tc.xy + cameraPosition2.xy;

	gl_FragColor.xyz =  texture2D(tex, tc).xyz;
	gl_FragColor.w = 1;
};
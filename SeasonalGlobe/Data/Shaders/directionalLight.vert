varying vec3 Normal;
varying vec3 LightDirection;
varying vec2 TextureCoordinate;

void main()
{
	gl_Position = ftransform();
	TextureCoordinate = gl_MultiTexCoord0.xy;
	Normal = gl_NormalMatrix * gl_Normal;
	
	vec4 lightPos = gl_LightSource[0].position;
	
	LightDirection = normalize(lightPos.xyz);
}
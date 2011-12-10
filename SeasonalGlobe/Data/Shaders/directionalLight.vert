varying vec3 Normal;
varying vec3 LightDirection;
varying vec2 TextureCoordinate;

void main()
{
	gl_Position = ftransform();
	TextureCoordinate = gl_MultiTexCoord0.xy;
	Normal = gl_NormalMatrix * gl_Normal;
	
	// 0 in w component denotes a directional light (affine)
	vec4 lightPos = vec4(gl_LightSource[0].position.xyz,0.0);
	
	LightDirection = normalize(lightPos.xyz);
}
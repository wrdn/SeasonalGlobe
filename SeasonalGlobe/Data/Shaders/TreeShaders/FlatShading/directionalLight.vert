flat varying vec3 Normal; // Don't interpolate normal (Flat shading)
varying vec3 LightDirection;

void main()
{
	gl_Position = ftransform();
	Normal = gl_NormalMatrix * gl_Normal;
	
	// 0 in w component denotes a directional light (affine)
	vec4 lightPos = vec4(gl_LightSource[0].position.xyz,0.0);
	
	LightDirection = normalize(lightPos.xyz);
}
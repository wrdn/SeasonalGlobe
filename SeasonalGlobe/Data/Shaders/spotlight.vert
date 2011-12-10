varying vec3 Normal;
varying vec3 LightDirection;
varying vec2 TextureCoordinate;
varying vec3 eyeVec;

void main()
{
	gl_Position = ftransform();
	TextureCoordinate = gl_MultiTexCoord0.xy;
	Normal = gl_NormalMatrix * gl_Normal;
	
	vec3 vertexPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	LightDirection = vec3(gl_LightSource[0].position.xyz - vertexPosition);
	eyeVec = -vertexPosition;
}
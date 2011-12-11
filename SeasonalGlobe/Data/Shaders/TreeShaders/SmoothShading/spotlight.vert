varying vec3 Normal;
varying vec2 TextureCoordinate;
varying vec3 eyeVec;
varying vec3 LightDir[4];

void main()
{
	gl_Position = ftransform();
	TextureCoordinate = gl_MultiTexCoord0.xy;
	Normal = gl_NormalMatrix * gl_Normal;
	
	vec3 vertexPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	eyeVec = -vertexPosition;
	
	LightDir[0] = vec3(gl_LightSource[0].position.xyz - vertexPosition);
	LightDir[1] = vec3(gl_LightSource[1].position.xyz - vertexPosition);
	LightDir[2] = vec3(gl_LightSource[2].position.xyz - vertexPosition);
	LightDir[3] = vec3(gl_LightSource[3].position.xyz - vertexPosition);
}
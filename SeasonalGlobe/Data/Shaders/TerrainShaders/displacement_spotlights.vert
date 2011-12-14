uniform sampler2D displacementMap;
uniform float vposmult;

varying vec3 Normal;
varying vec2 TextureCoordinate;
varying vec3 eyeVec;
varying vec3 LightDir[4];

void main(void)
{
	TextureCoordinate = gl_MultiTexCoord0.xy;

  vec4 dv = texture2D(displacementMap, TextureCoordinate.xy);
   
   // float df = 0.30 * dv.x + 0.59 + dv.y + 0.11 * dv.z;
   //float df = 0.3 * dv.x + 0.59 + dv.y + 0.08 * dv.z;
   float df = 0.30*dv.x + 0.59*dv.y + 0.11*dv.z;
   vec4 newVertexPos = vec4(normalize(gl_Normal) * df * vposmult, 0);
   
  gl_Position = gl_ModelViewProjectionMatrix * (newVertexPos + gl_Vertex);

	
	Normal = gl_NormalMatrix * gl_Normal;
	
	vec3 vertexPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
	eyeVec = -vertexPosition;
	
	LightDir[0] = vec3(gl_LightSource[0].position.xyz - vertexPosition);
	LightDir[1] = vec3(gl_LightSource[1].position.xyz - vertexPosition);
	LightDir[2] = vec3(gl_LightSource[2].position.xyz - vertexPosition);
	LightDir[3] = vec3(gl_LightSource[3].position.xyz - vertexPosition);
}
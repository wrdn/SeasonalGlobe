uniform sampler2D displacementMap;
uniform float vposmult;

//varying vec3 Normal, LightDirection;
//varying vec2 TextureCoordinate;

varying vec3 normal;
varying vec3 lightVector;
varying vec3 eyeVector;
varying vec3 position;
varying vec2 texture_coordinate;

void main(void)
{
	vec2 TextureCoordinate = gl_MultiTexCoord0.xy;
	vec4 dv = texture2D(displacementMap, TextureCoordinate);	
	float df = 0.30*dv.x + 0.59*dv.y + 0.11*dv.z;
   	vec4 newVertexPos = vec4(normalize(gl_Normal) * df * vposmult, 0);
	gl_Position = gl_ModelViewProjectionMatrix * (newVertexPos + gl_Vertex);
	
	position = vec3(gl_ModelViewMatrix * (newVertexPos+gl_Vertex));
	normal = normalize(gl_NormalMatrix * gl_Normal);
	texture_coordinate = TextureCoordinate;
	
	vec3 c1 = cross( gl_Normal, vec3(0.0, 0.0, 1.0) ); 
	vec3 c2 = cross( gl_Normal, vec3(0.0, 1.0, 0.0) ); 
	vec3 tangent;
	vec3 binormal;
	if( length(c1)>length(c2) ) { tangent = gl_NormalMatrix * c1; }
	else { tangent = gl_NormalMatrix * c2; }
	tangent = normalize(tangent);
	binormal = cross(tangent, normal); 
	binormal = normalize(binormal);
	lightVector = normalize(gl_LightSource[0].position.xyz - position);
	
	mat3 tangentSpace;
	tangentSpace[0][0] = tangent.x;
	tangentSpace[1][0] = tangent.y;
	tangentSpace[2][0] = tangent.z;
	tangentSpace[0][1] = binormal.x;
	tangentSpace[1][1] = binormal.y;
	tangentSpace[2][1] = binormal.z;
	tangentSpace[0][2] = normal.x;
	tangentSpace[1][2] = normal.y;
	tangentSpace[2][2] = normal.z;

	lightVector = tangentSpace * lightVector;
	eyeVector = normalize(-position);
	eyeVector = tangentSpace * eyeVector;
	
   	//Normal = gl_NormalMatrix * gl_Normal;
   	//vec4 lightPos = vec4(gl_LightSource[0].position.xyz,0.0);
   	//LightDirection = normalize(lightPos.xyz);   	
 
}
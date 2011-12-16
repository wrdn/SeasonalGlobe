varying vec3 normal;
varying vec3 eyeVector;
varying vec3 position;
varying vec2 texture_coordinate;
varying vec3 LightDir[4];

void main(void)
{
	// Displacement mapping
	texture_coordinate = gl_MultiTexCoord0.xy;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	position = vec3(gl_ModelViewMatrix * gl_Vertex);
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	// Normal mapping (required to fix lighting on displacement mapped terrain)
	vec3 c1 = cross( gl_Normal, vec3(0.0, 0.0, 1.0) ); 
	vec3 c2 = cross( gl_Normal, vec3(0.0, 1.0, 0.0) ); 
	vec3 tangent;
	vec3 binormal;
	if( length(c1)>length(c2) ) { tangent = gl_NormalMatrix * c1; }
	else { tangent = gl_NormalMatrix * c2; }
	tangent = normalize(tangent);
	binormal = cross(tangent, normal); 
	binormal = normalize(binormal);
	
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

	eyeVector = normalize(-position);
	eyeVector = tangentSpace * eyeVector;
	
	LightDir[0] = tangentSpace * vec3(gl_LightSource[0].position.xyz - position);
	LightDir[1] = tangentSpace * vec3(gl_LightSource[1].position.xyz - position);
	LightDir[2] = tangentSpace * vec3(gl_LightSource[2].position.xyz - position);
	LightDir[3] = tangentSpace * vec3(gl_LightSource[3].position.xyz - position);
}
#version 130
///////////////////////////////
// variables
///////////////////////////////
out vec3 normal;
out vec3 lightVector;
out vec3 eyeVector;
out vec3 position;
out vec2 texture_coordinate;

///////////////////////////////
// functions
///////////////////////////////

void main()
{
	
	// generate the variables for the phong lighting and texturing
	position = vec3(gl_ModelViewMatrix * gl_Vertex);
	normal = normalize(gl_NormalMatrix * gl_Normal);
	texture_coordinate = vec2(gl_MultiTexCoord0);

	vec3 c1 = cross( gl_Normal, vec3(0.0, 0.0, 1.0) ); 
	vec3 c2 = cross( gl_Normal, vec3(0.0, 1.0, 0.0) ); 

	vec3 tangent;
	vec3 binormal;

	if( length(c1)>length(c2) )
	{
		tangent = gl_NormalMatrix * c1;	
	}
	else
	{
		tangent = gl_NormalMatrix * c2;	
	}

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

	// Transforming The Vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
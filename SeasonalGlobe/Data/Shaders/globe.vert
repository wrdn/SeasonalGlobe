varying vec3 normal;
varying vec3 worldPos;

void main()
{
	worldPos = vec3(gl_Vertex);
	
	normal = gl_NormalMatrix * gl_Normal;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
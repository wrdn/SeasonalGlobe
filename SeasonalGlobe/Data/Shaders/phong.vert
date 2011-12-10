uniform vec3 lightPosition;
varying vec3 vlightDirection, vNormal;
varying vec2 vTexCoord;

void main()
{
	gl_Position = ftransform();
	vTexCoord = gl_MultiTexCoord0.xy;
	vec4 objectPos = gl_ModelViewMatrix * gl_Vertex;

	vlightDirection = vec3(0,1,1);
	//vlightDirection = normalize(lightPosition);
	//vlightDirection = lightPosition - objectPos.xyz;
	vNormal = normalize(gl_NormalMatrix * gl_Normal);
};
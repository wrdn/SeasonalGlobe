uniform sampler2D baseMap;

varying vec3 Normal;
varying vec3 LightDirection;
varying vec2 TextureCoordinate;

uniform bool useTextures;

void main()
{
	vec4 lightAmbient = gl_LightSource[0].ambient;
	vec4 lightDiffuse = gl_LightSource[0].diffuse;
	
	vec4 ka = gl_FrontMaterial.ambient;
	vec4 kd = gl_FrontMaterial.diffuse;
	
	vec3 N = normalize(Normal);
	vec3 L = normalize(LightDirection);
	
	float NdotL = max(dot(N,L),0.0);
	
	vec4 texCol;
	if(useTextures) { texCol = texture2D(baseMap, TextureCoordinate); }
	else { texCol = vec4(1,1,1,1); }
	
	gl_FragColor = (lightAmbient * ka * texCol) + (lightDiffuse * kd * NdotL * texCol);
}
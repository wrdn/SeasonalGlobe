uniform vec4 lightAmbient, lightDiffuse;
uniform vec4 matAmbient, matDiffuse;
uniform sampler2D baseMap;

varying vec3 Normal;
varying vec3 LightDirection;
varying vec2 TextureCoordinate;

void main()
{
	vec3 N = normalize(Normal);
	vec3 L = normalize(LightDirection);
	
	float NdotL = max(dot(N,L),0.0);
	
	vec4 texCol = texture2D(baseMap, TextureCoordinate);
	
	gl_FragColor = (lightAmbient * matAmbient * texCol) + (lightDiffuse * matDiffuse * NdotL);
}
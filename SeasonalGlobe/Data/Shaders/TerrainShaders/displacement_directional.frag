uniform sampler2D colorMap;
varying vec3 Normal, LightDirection;
varying vec2 TextureCoordinate;

void main(void)
{
	vec4 lightAmbient = gl_LightSource[0].ambient;
	vec4 lightDiffuse = gl_LightSource[0].diffuse;
	
	vec4 ka = gl_FrontMaterial.ambient;
	vec4 kd = gl_FrontMaterial.diffuse;
	
	vec3 N = normalize(Normal);
	vec3 L = normalize(LightDirection);
	
	float NdotL = max(dot(N,L),0.0);

	vec4 texCol = texture2D(colorMap, TextureCoordinate);
        gl_FragColor = (lightAmbient * ka * texCol) + (lightDiffuse * kd * NdotL * texCol);
}
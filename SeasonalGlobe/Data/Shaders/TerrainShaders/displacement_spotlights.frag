uniform sampler2D colorMap;

varying vec3 Normal;
varying vec2 TextureCoordinate;
varying vec3 eyeVec;
varying vec3 LightDir[4];

const float cos_outer_cone_angle = 0.5;

vec4 ApplySpotLight(int index, vec4 texCol, vec3 N, float ambientMultiplier)
{
	vec4 finalColor = ambientMultiplier * gl_LightSource[index].ambient * gl_FrontMaterial.ambient * texCol;
	vec3 L = normalize(LightDir[index]);
	vec3 D = normalize(gl_LightSource[index].spotDirection);
	
	float cos_cur_angle = dot(-L, D);
	float cos_inner_cone_angle = gl_LightSource[index].spotCosCutoff;
	float cos_inner_minus_outer_angle = 
	      cos_inner_cone_angle - cos_outer_cone_angle;
	float spot = 0.0;
	spot = clamp((cos_cur_angle - cos_outer_cone_angle) / 
	       cos_inner_minus_outer_angle, 0.0, 1.0);
	float NdotL = max(dot(N,L), 0.0);
	if(NdotL > 0.0)
	{
		finalColor += gl_LightSource[index].diffuse * gl_FrontMaterial.diffuse * NdotL * texCol * spot;
	}
	return finalColor;
};

void main(void)
{
	vec4 textureColor = texture2D(colorMap, TextureCoordinate);
	vec3 N = normalize(Normal);
	gl_FragColor =
		ApplySpotLight(0, textureColor, N, 0.3333) +
		ApplySpotLight(1, textureColor, N,1) +
		ApplySpotLight(2, textureColor, N,1) +
		ApplySpotLight(3, textureColor, N, 0.25);
}
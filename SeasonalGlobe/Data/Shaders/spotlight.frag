uniform sampler2D baseMap;

varying vec3 Normal;
varying vec3 LightDirection;
varying vec2 TextureCoordinate;
varying vec3 eyeVec;

const float cos_outer_cone_angle = 1.2;

void main()
{
	vec4 texCol = texture2D(baseMap, TextureCoordinate);
	vec4 finalColor = gl_LightSource[0].ambient * gl_FrontMaterial.ambient * texCol;
	
	vec3 L = normalize(LightDirection);
	vec3 D = normalize(gl_LightSource[0].spotDirection);
	
	float cos_cur_angle = dot(-L, D);
	float cos_inner_cone_angle = gl_LightSource[0].spotCosCutoff;
	float cos_inner_minus_outer_angle = 
	      cos_inner_cone_angle - cos_outer_cone_angle;
	
	float spot = 0.0;
	spot = clamp((cos_cur_angle - cos_outer_cone_angle) / 
	       cos_inner_minus_outer_angle, 0.0, 1.0);

	vec3 N = normalize(Normal);
	float NdotL = max(dot(N,L), 0.0);
	if(NdotL > 0.0)
	{
		finalColor += gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * NdotL * texCol * spot;
	}
	
	gl_FragColor = finalColor;
}
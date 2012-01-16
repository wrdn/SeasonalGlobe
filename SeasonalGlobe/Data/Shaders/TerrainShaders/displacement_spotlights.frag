uniform sampler2D colorMap, snowMap, normalMap;
uniform float t;

varying vec3 normal;
varying vec3 eyeVector;
varying vec3 position;
varying vec2 texture_coordinate;
varying vec3 LightDir[4];

uniform float normalLerpFactor;

const float cos_outer_cone_angle = 0.8f; // default: 0.8 (~36 degs)

vec4 ApplySpotLight(int index, vec4 texCol, vec3 _N, float ambientMultiplier, float diffuseMultiplier, float specularMultiplier)
{
	vec3 L = normalize(LightDir[index]);
	vec3 D = normalize(gl_LightSource[index].spotDirection);
	vec4 fcol = gl_LightSource[index].ambient * gl_FrontMaterial.ambient * texCol;
	
	float cos_cur_angle = dot(-L,D);
	float cos_inner_cone_angle = gl_LightSource[index].spotCosCutoff;
	float cos_inner_minus_outer_angle = cos_inner_cone_angle - cos_outer_cone_angle;
	
	float spot = clamp((cos_cur_angle - cos_outer_cone_angle) / 
	       cos_inner_minus_outer_angle, 0.0, 1.0);
	
	vec3 N = normalize(_N);
	float lambertTerm = max(dot(N,L),0);
	
	if(lambertTerm>0.0)
	{
		fcol += gl_LightSource[index].diffuse * gl_FrontMaterial.diffuse * lambertTerm * spot;
			
		vec3 E = normalize(eyeVector);
		vec3 R = reflect(-L, N);
			
		float specular = pow(max(dot(R,E),0), gl_FrontMaterial.shininess);
		fcol += gl_LightSource[index].specular * gl_FrontMaterial.specular * specular * spot;
	}
	return fcol;
};

void main(void)
{
	vec4 textureCoefficient = (1-t)*texture2D(colorMap, texture_coordinate) + t*texture2D(snowMap, texture_coordinate);
	
	vec3 normalVector = normalize( texture2D(normalMap, texture_coordinate).xyz * 2.0 - 1.0);
	normalVector = normalize(mix(normal, normalVector, normalLerpFactor));
	
	normalize(eyeVector);
	
	//normalVector = normalize(normal);
	
	gl_FragColor =
		ApplySpotLight(0, textureCoefficient, normalVector, 1,1,1) +
		ApplySpotLight(1, textureCoefficient, normalVector,1, 1, 1) +
		ApplySpotLight(2, textureCoefficient, normalVector,1, 1, 1) +
		ApplySpotLight(3, textureCoefficient, normalVector, 1, 1, 1);
}
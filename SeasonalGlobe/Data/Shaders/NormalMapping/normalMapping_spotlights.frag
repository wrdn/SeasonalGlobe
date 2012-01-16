uniform sampler2D baseMap, normalMap;

varying vec3 normal;
varying vec3 eyeVector;
varying vec3 position;
varying vec2 texture_coordinate;
varying vec3 LightDir[4];


const float cos_outer_cone_angle = 0.8;

vec4 ApplySpotLight(int index, vec4 texCol, vec3 N, float ambientMultiplier, float diffuseMultiplier, float specularMultiplier)
{
	vec3 L = normalize(LightDir[index]);
	vec3 D = normalize(gl_LightSource[index].spotDirection);
	
	vec4 ka = gl_FrontMaterial.ambient,
		 kd = gl_FrontMaterial.diffuse,
		 ks = gl_FrontMaterial.specular;
	float shininess = gl_FrontMaterial.shininess;
	
	vec4 lightAmbient  = gl_LightSource[index].ambient;
	vec4 lightDiffuse  = gl_LightSource[index].diffuse;
	vec4 lightSpecular = gl_LightSource[index].specular;
	
	vec3 reflectionVector = normalize(-reflect(L, N));  
	
	float cos_cur_angle = dot(-L, D);
	float cos_inner_cone_angle = gl_LightSource[index].spotCosCutoff;
	float cos_inner_minus_outer_angle = 
	      cos_inner_cone_angle - cos_outer_cone_angle;
	float spot = 0.0;
	spot = clamp((cos_cur_angle - cos_outer_cone_angle) / cos_inner_minus_outer_angle, 0.0, 1.0);
	float NdotL = max(dot(N,L), 0.0);
	
	vec4 finalColor = ambientMultiplier * lightAmbient * ka * texCol; // ambient
	if(NdotL > 0)
	{
		finalColor += diffuseMultiplier * lightDiffuse * kd * max(dot(N, L),0.0) * texCol; // Diffuse
		finalColor += specularMultiplier * lightSpecular * ks * pow(max(dot(reflectionVector,eyeVector),0.0),shininess)*texCol; // Specular
	}
	return finalColor;
};

void main(void)
{
	vec4 textureCoefficient = texture2D(baseMap, texture_coordinate);
	
	vec3 normalVector = normalize( texture2D(normalMap, texture_coordinate).xyz * 2.0 - 1.0);
	normalVector = normalize(normalVector);
	
	normalize(eyeVector);
	
	gl_FragColor =
		ApplySpotLight(0, textureCoefficient, normalVector, 1,1,1) +
		ApplySpotLight(1, textureCoefficient, normalVector,1, 1, 1) +
		ApplySpotLight(2, textureCoefficient, normalVector,1, 1, 1) +
		ApplySpotLight(3, textureCoefficient, normalVector, 1, 1, 1);
}
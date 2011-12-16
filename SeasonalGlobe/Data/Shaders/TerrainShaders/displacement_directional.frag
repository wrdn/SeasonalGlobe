uniform sampler2D colorMap, snowMap, normalMap;
uniform float t;

varying vec3 normal;
varying vec3 lightVector;
varying vec3 eyeVector;
varying vec3 position;
varying vec2 texture_coordinate;

uniform float normalLerpFactor;

void main(void)
{
	normalize(lightVector);
	normalize(eyeVector);
	
	vec4 textureCoefficient = (1-t)*texture2D(colorMap, texture_coordinate) + t*texture2D(snowMap, texture_coordinate);
	
	vec4 ka = gl_FrontMaterial.ambient,
		 kd = gl_FrontMaterial.diffuse,
		 ks = gl_FrontMaterial.specular;
	float shininess = gl_FrontMaterial.shininess;
	
	vec4 lightAmbient  = gl_LightSource[0].ambient;
	vec4 lightDiffuse  = gl_LightSource[0].diffuse;
	vec4 lightSpecular = gl_LightSource[0].specular;
	
	vec3 normalVector = normalize( texture2D(normalMap, texture_coordinate).xyz * 2.0 - 1.0);
	normalVector = normalize(mix(normal, normalVector, normalLerpFactor));
	
	vec3 reflectionVector = normalize(-reflect(lightVector,normalVector));  
 
	//calculate Ambient Term:  
	vec4 ambientLightIntensity = lightAmbient * ka * textureCoefficient;    

	//calculate Diffuse Term:  
	vec4 diffuseLightIntensity = lightDiffuse * kd * max(dot(normalVector,lightVector), 0.0) * textureCoefficient;   
   
	// calculate Specular Term:
	vec4 specularLightIntensity = lightSpecular * ks * pow(max(dot(reflectionVector,eyeVector),0.0),shininess)*textureCoefficient;   
  
	// write Total Color: 
	gl_FragColor =  ambientLightIntensity + diffuseLightIntensity + specularLightIntensity;
}
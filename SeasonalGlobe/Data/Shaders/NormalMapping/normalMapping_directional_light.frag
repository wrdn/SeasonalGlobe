#version 130
///////////////////////////////
// uniforms
///////////////////////////////
uniform sampler2D baseMap;
uniform sampler2D normalMap;

///////////////////////////////
// variables
///////////////////////////////
in vec3 normal;
in vec3 position;   
in vec3 lightVector;
in vec3 eyeVector; 
in vec2 texture_coordinate;

out vec4 fragColour;


void main()
{	
	
	normalize(lightVector);
	normalize(eyeVector);
	// get the colour from the texture
	vec4 textureCoefficient = texture2D(baseMap, texture_coordinate);
	
	vec4 fragmentColour = vec4(0.0,0.0,0.0,1.0); // initialise fragment(pixel) colour to black
		
	vec4 ambientIntensity	= vec4(0.0,0.0,0.0,1.0);
	vec4 diffuseIntensity	= vec4(1.0,1.0,1.0,1.0);
	vec4 specularIntensity  = vec4(1.0,1.0,1.0,1.0);

	float ambientCoefficient = 0.0;
	float diffuseCoefficient = 0.95;
	float specularCoefficient = 1.0;
	float specularShininess = 20.0;
	
	vec3 normalVector = normal;
	
	normalVector = normalize( texture2D(normalMap, texture_coordinate).xyz * 2.0 - 1.0);

	vec3 reflectionVector = normalize(-reflect(lightVector,normalVector));  
 
	//calculate Ambient Term:  
	vec4 ambientLightIntensity = ambientIntensity * ambientCoefficient * textureCoefficient;    

	//calculate Diffuse Term:  
	vec4 diffuseLightIntensity = diffuseIntensity * diffuseCoefficient * max(dot(normalVector,lightVector), 0.0) * textureCoefficient;   
   
	// calculate Specular Term:
	vec4 specularLightIntensity = specularIntensity * specularCoefficient * pow(max(dot(reflectionVector,eyeVector),0.0),specularShininess) * textureCoefficient;   
  
	// write Total Color: 
	vec4 phongModel =  ambientLightIntensity + diffuseLightIntensity + specularLightIntensity;
	fragmentColour += phongModel; 
	
	
	fragColour = fragmentColour;
}

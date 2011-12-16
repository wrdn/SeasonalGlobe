uniform sampler2D colorMap, snowMap, normalMap;
uniform float t;

//varying vec3 Normal, LightDirection;
//varying vec2 TextureCoordinate;

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
	// get the colour from the texture
	
	vec4 textureCoefficient = (1-t)*texture2D(colorMap, texture_coordinate) +
					t*texture2D(snowMap, texture_coordinate);
	
	vec4 fragmentColour = vec4(0.0,0.0,0.0,1.0); // initialise fragment(pixel) colour to black
		
	vec4 ambientIntensity	= vec4(0.6, 0.6,0.6,1.0);
	vec4 diffuseIntensity	= vec4(1.0,1.0,1.0,1.0);
	vec4 specularIntensity  = vec4(1.0,1.0,1.0,1.0);

	float ambientCoefficient = 0.5;
	float diffuseCoefficient = 0.95;
	float specularCoefficient = 1.0;
	float specularShininess = 20.0;
	
	vec3 normalVector = normalize( texture2D(normalMap, texture_coordinate).xyz * 2.0 - 1.0);
	normalVector = mix(normal, normalVector, normalLerpFactor);
	
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

	gl_FragColor = fragmentColour;
	//gl_FragColor = vec4(normalVector,1);
	
	/*float NdotL = max(dot(N,L),0.0);

		vec4 texCol = (1-t)*texture2D(colorMap, TextureCoordinate) +
					t*texture2D(snowMap, TextureCoordinate);
        gl_FragColor = (lightAmbient * ka * texCol) + (lightDiffuse * kd * NdotL * texCol);*/
}
uniform sampler2D baseMap;
varying vec2 TextureCoordinate;
uniform bool useTextures;

void main()
{
	vec4 lightAmbient = gl_LightSource[0].ambient;
	vec4 ka = gl_FrontMaterial.ambient;
	
	if(useTextures)
	{
		vec4 texCol = texture2D(baseMap, TextureCoordinate);
		gl_FragColor = (lightAmbient * ka * texCol);
	}
	else
	{
		gl_FragColor = (lightAmbient * ka);
	}
}
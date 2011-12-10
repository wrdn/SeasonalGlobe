uniform sampler2D baseMap;
varying vec2 TextureCoordinate;

void main()
{
	vec4 lightAmbient = gl_LightSource[0].ambient;
	vec4 ka = gl_FrontMaterial.ambient;
	vec4 texCol = texture2D(baseMap, TextureCoordinate);
	gl_FragColor = (lightAmbient * ka * texCol);
}
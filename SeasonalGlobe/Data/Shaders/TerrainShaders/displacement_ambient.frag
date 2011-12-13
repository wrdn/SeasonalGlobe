uniform sampler2D colorMap;
varying vec2 TextureCoordinate;

void main(void)
{
	vec4 lightAmbient = gl_LightSource[0].ambient;
	vec4 ka = gl_FrontMaterial.ambient;
	vec4 texCol = texture2D(colorMap, TextureCoordinate);
   	gl_FragColor = (lightAmbient * ka * texCol);
}
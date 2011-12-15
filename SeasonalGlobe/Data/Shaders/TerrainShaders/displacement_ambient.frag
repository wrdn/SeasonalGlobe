uniform sampler2D colorMap, snowMap;
uniform float t;
varying vec2 TextureCoordinate;

void main(void)
{
	vec4 lightAmbient = gl_LightSource[0].ambient;
	vec4 ka = gl_FrontMaterial.ambient;
	
	vec4 texCol = (1-t)*texture2D(colorMap, TextureCoordinate) +
					t*texture2D(snowMap, TextureCoordinate);
	gl_FragColor = (lightAmbient * ka * texCol);
}
uniform sampler2D AlphaMap;
varying vec2 TexCoord;
varying vec4 color;

void main()
{
	gl_FragColor = texture2D(AlphaMap, TexCoord) * color;
};
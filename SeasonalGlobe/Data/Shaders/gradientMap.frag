uniform sampler2D GradientMap;
uniform sampler2D AlphaMap;
varying vec2 TexCoord;
uniform float t;

void main()
{
	vec2 baseTexCoord = vec2( 1.0 - t, 0);
	vec4 alpha = texture2D(AlphaMap, TexCoord);
	vec4 base = texture2D(GradientMap, baseTexCoord);
	gl_FragColor = alpha*base;
};
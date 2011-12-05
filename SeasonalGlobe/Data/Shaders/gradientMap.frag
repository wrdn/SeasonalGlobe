uniform sampler2D GradientMap;
uniform sampler2D AlphaMap;
varying vec2 TexCoord;
varying float HT;

void main()
{
	float fade = pow(dot(TexCoord, TexCoord), 0.37000f);
	gl_FragColor = texture2D(AlphaMap, vec2( HT, 0));
};
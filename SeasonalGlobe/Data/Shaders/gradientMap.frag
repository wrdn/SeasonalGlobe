uniform sampler2D GradientMap;
uniform sampler2D AlphaMap;
varying vec2 TexCoord;
varying float k;

void main()
{
	float fade = pow(dot(TexCoord, TexCoord), 0.37f);
	
	vec4 alphaColor = texture2D(AlphaMap, TexCoord);
	vec4 gradientColor = texture2D(GradientMap, vec2(k,0));
	gl_FragColor = alphaColor * gradientColor;
	
	//
	//gl_FragColor = 1-fade * texture2D(GradientMap, vec2(k,0));
};
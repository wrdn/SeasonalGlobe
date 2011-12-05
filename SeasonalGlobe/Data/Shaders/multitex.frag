uniform sampler2D TextureA;
uniform sampler2D TextureB;
varying vec2 TexCoord;

void main()
{
	vec4 texares = texture2D(TextureA, TexCoord);
	vec4 texbres = texture2D(TextureB, TexCoord);
	
	gl_FragColor = texares * texbres;
};
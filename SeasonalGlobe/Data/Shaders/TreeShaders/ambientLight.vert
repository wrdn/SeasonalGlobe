varying vec2 TextureCoordinate;

void main()
{
	gl_Position = ftransform();
	TextureCoordinate = gl_MultiTexCoord0.xy;
}
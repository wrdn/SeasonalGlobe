varying vec2 TexCoord;
varying vec4 color;

void main()
{
	mat4 mat = gl_ModelViewMatrix;
	
	gl_Position = gl_ProjectionMatrix * mat * gl_Vertex;
	
	TexCoord = gl_MultiTexCoord0.xy;
	
	color = gl_Color;
}
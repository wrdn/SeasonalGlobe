uniform bool SphericalBillboarding;
varying vec2 TexCoord;
varying vec4 color;

void main()
{
	mat4 mat = gl_ModelViewMatrix;
	
	/*if(SphericalBillboarding) // Spherical billboarding
	{
		mat[0][0] = mat[2][2] = mat[1][1] = 1.0f;
		mat[0][1] = mat[0][2] = mat[0][3] = mat[1][0] = 0.0f;
		mat[1][2] = mat[1][3] = mat[2][0] = mat[2][1] = mat[2][3] = 0.0f;
	}
	else
	{
		mat[0][0] = mat[2][2] = 1.0f;
		mat[0][1] = mat[0][2] = mat[0][3] = mat[1][0] = 0.0f;
		mat[1][2] = mat[1][3] = mat[2][0] = mat[2][1] = mat[2][3] = 0.0f;
	}*/
	
	gl_Position = gl_ProjectionMatrix * mat * gl_Vertex;
	
	TexCoord = gl_MultiTexCoord0.xy;
	
	color = gl_Color;
};
uniform sampler2D displacementMap;
uniform float vposmult;

void main(void)
{
	// in the application, shift the mesh down by vposmult*10, draw it, then move it down again by the same amount
	// these transformations should be applied to the Y axis
   
   //float vposmult = 0.45f;
   
   gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
   vec4 dv = texture2D(displacementMap, gl_MultiTexCoord0.xy);
   
   // float df = 0.30 * dv.x + 0.59 + dv.y + 0.11 * dv.z;
   float df = 0.3 * dv.x + 0.59 + dv.y + 0.08 * dv.z;
   vec4 newVertexPos = vec4(normalize(gl_Normal) * df * vposmult, 0);
   
   gl_Position = gl_ModelViewProjectionMatrix * (newVertexPos + gl_Vertex);
}
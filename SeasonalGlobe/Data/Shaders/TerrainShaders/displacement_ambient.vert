uniform sampler2D displacementMap;
uniform float vposmult;
varying vec2 TextureCoordinate;

void main(void)
{
   TextureCoordinate = gl_MultiTexCoord0.xy;
   vec4 dv = texture2D(displacementMap, TextureCoordinate);
   
   //float df = 0.3 * dv.x + 0.59 + dv.y + 0.08 * dv.z;
   float df = 0.30*dv.x + 0.59*dv.y + 0.11*dv.z;
   vec4 newVertexPos = vec4(normalize(gl_Normal) * df * vposmult, 0);
   
   gl_Position = gl_ModelViewProjectionMatrix * (newVertexPos + gl_Vertex);
}
varying vec3 normal;
varying vec3 worldPos;

void main()
{
	/*if(worldPos.y < 0) { discard; }*/
	
	vec4 domeMaterial = vec4(1.0, 1.0, 1.0, pow(1-dot(vec3(0, 0, 1), normalize(normal)),2.0));
	gl_FragColor = domeMaterial;
}
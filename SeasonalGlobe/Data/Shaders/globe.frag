varying vec3 normal;

void main()
{
	vec4 domeMaterial = vec4(1.0, 1.0, 1.0, pow(1-dot(vec3(0, 0, 1), normalize(normal)),2.0));
	gl_FragColor = domeMaterial;
};
varying vec3 Nt, Lt, Vt;
varying vec2 tc;
uniform sampler2D tex;
uniform float specularPower;
uniform vec4 amb, diff, spec;
uniform float red;

void main(void)
{
	//vec4 amb = vec4(.2f,.2f,.2f,1);
	vec4 ka = vec4(.1f,.1f,.1f,1);
	//vec4 diff = vec4(1.0f, .0f, .0f, 1.0f);
	//vec4 spec = vec4(.6f, .6f, .6f, 1.0f);
	
	vec3 N = normalize(Nt);
	vec3 L = normalize(Lt);
	float dln = max(0.0,dot(N,L));
	vec3  R = normalize(((2.0*N)*dln)-L);
	vec3 V = normalize(Vt);
	float drv = max(0.0, dot(R,V));

	vec4 baseCol = texture2D(tex,tc);

	vec4 fAmb = amb * baseCol;
	vec4 fDiff = dln * diff * baseCol;
	vec4 fSpec = spec * pow(drv,specularPower);
	gl_FragColor = fAmb + fDiff + fSpec;
	gl_FragColor = vec4(red,0,0,1);
}
varying vec3 viewDir, lightDir, Normalv;

void main()
{
	vec3 LightDirection = normalize(lightDir);
	vec3 Normal = normalize(Normalv);
	float NdotL = dot(Normal, LightDirection);

	vec3 Reflection = normalize( ( ( 2.0 * Normal ) * NdotL ) - LightDirection );
	vec3 ViewDirection = normalize(viewDir);
	float RdotV = max(0.0, dot(Reflection, ViewDirection));

	float NdotV = dot(Normal, ViewDirection);
	gl_FragColor = vec4(NdotV);

	gl_FragColor = vec4(0.1, 0.1, 0.1, 0.8);
};
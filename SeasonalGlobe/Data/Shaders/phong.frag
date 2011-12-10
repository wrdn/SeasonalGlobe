uniform vec4 fAmbient, fDiffuse;
uniform sampler2D baseMap;
uniform bool applyTexture;
varying vec3 vlightDirection, vNormal;
varying vec2 vTexCoord;

void main()
{
	vec3 lightDir = normalize(vlightDirection);
	vec3 Normal = normalize(vNormal);
	float ndotl = max(0,dot(Normal, lightDir));

	vec4 baseCol = texture2D(baseMap, vTexCoord);

	//if(applyTexture)
	//{
	
	vec4 AmbResult = fAmbient * baseCol;
	vec4 DiffResult = fDiffuse * ndotl * baseCol;
	//DiffResult.w = 1;
	gl_FragColor = AmbResult + DiffResult;
	
	/*}
	else
	{
		vec4 AmbResult = fAmbient;
		vec4 DiffResult = fDiffuse * ndotl;
		DiffResult.w = 1;
		gl_FragColor = AmbResult + DiffResult;
	}*/

	gl_FragColor = vec4(Normal,1);
};
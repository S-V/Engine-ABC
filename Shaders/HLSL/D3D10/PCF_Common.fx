/*
=============================================================================
	File:	PCF_Common.fx
	Desc:	Common shader code for Percentage Closer Filtering.
=============================================================================
*/

// Calculates the shadow occlusion using bilinear PCF
// vTexCoord is the texture coordinate used to sample the shadowmap
//
float CalcShadowTermPCF( SamplerState shadowMapSampler, float fLightDepth, float2 vTexCoord )
{
	float fShadowTerm = 0.0f;

	// transform to texel space
	float2 vShadowMapCoord = g_vShadowMapSize * vTexCoord;
    
	// Determine the lerp amounts           
	float2 vLerps = frac(vShadowMapCoord);

	// read in bilerp stamp, doing the shadow checks
	float fSamples[4];
	
	fSamples[0] = (tex2D(shadowMapSampler, vTexCoord).x + BIAS < fLightDepth) ? 0.0f: 1.0f;  
	fSamples[1] = (tex2D(shadowMapSampler, vTexCoord + float2(1.0/g_vShadowMapSize.x, 0)).x + BIAS < fLightDepth) ? 0.0f: 1.0f;  
	fSamples[2] = (tex2D(shadowMapSampler, vTexCoord + float2(0, 1.0/g_vShadowMapSize.y)).x + BIAS < fLightDepth) ? 0.0f: 1.0f;  
	fSamples[3] = (tex2D(shadowMapSampler, vTexCoord + float2(1.0/g_vShadowMapSize.x, 1.0/g_vShadowMapSize.y)).x + BIAS < fLightDepth) ? 0.0f: 1.0f;  
    
	// lerp between the shadow values to calculate our light amount
	fShadowTerm = lerp( lerp( fSamples[0], fSamples[1], vLerps.x ),
							  lerp( fSamples[2], fSamples[3], vLerps.x ),
							  vLerps.y );							  
								
	return fShadowTerm;								 
}

//
// Edge-tap smoothing
//
// fLightDepth - normalized depth in light space
// vTexCoord is the texture coordinate used to sample the shadowmap
// iSqrtSamples is the square-root of the number of PCF samples to use. So for 25 samples, you'd use iSqrtSamples = 5.
//
float CalcShadowTermSoftPCF( SamplerState shadowMapSampler, float fLightDepth, float2 vTexCoord, int iSqrtSamples )
{
	float fShadowTerm = 0.0f;  
		
	float fRadius = (iSqrtSamples - 1.0f) / 2;
	float fWeightAccum = 0.0f;
	
	for (float y = -fRadius; y <= fRadius; y++)
	{
		for (float x = -fRadius; x <= fRadius; x++)
		{
			float2 vOffset = 0;
			vOffset = float2(x, y);				
			vOffset /= g_vShadowMapSize;
			float2 vSamplePoint = vTexCoord + vOffset;			
			float fDepth = tex2D(shadowMapSampler, vSamplePoint).x;
			float fSample = (fLightDepth <= fDepth + BIAS);
			
			// Edge tap smoothing
			float xWeight = 1;
			float yWeight = 1;
			
			if (x == -fRadius)
				xWeight = 1 - frac(vTexCoord.x * g_vShadowMapSize.x);
			else if (x == fRadius)
				xWeight = frac(vTexCoord.x * g_vShadowMapSize.x);
				
			if (y == -fRadius)
				yWeight = 1 - frac(vTexCoord.y * g_vShadowMapSize.y);
			else if (y == fRadius)
				yWeight = frac(vTexCoord.y * g_vShadowMapSize.y);
				
			fShadowTerm += fSample * xWeight * yWeight;
			fWeightAccum = xWeight * yWeight;
		}											
	}		
	
	fShadowTerm /= (iSqrtSamples * iSqrtSamples);
	
	return fShadowTerm;
}

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 WorldPos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct Light
{
	float3 Dir;
	float3 Position;
	float Range;
	float3 Attenuation;
	float4 Ambient;
	float4 Diffuse;
};

cbuffer ConstantBufferPerObject
{
	float4x4 WVP;	
	float4x4 World;
};

cbuffer ConstantBufferPerFrame
{
	Light TheLight;
}

Texture2D ObjectTexture;
SamplerState ObjectSamplerState;
float4 main(VS_OUTPUT Input) : SV_TARGET
{
	Input.Normal = normalize(Input.Normal);
	float4 Diffuse = ObjectTexture.Sample(ObjectSamplerState, Input.TexCoord);
	float3 FinalColor = float3(0.0f, 0.0f, 0.0f);
	float3 LightToPixelVec = (TheLight.Position - Input.WorldPos);
	const float Distance = length(LightToPixelVec);
	float3 FinalAmbient = (Diffuse * TheLight.Ambient);
	if(Distance > TheLight.Range)
	{
		return float4(FinalAmbient, Diffuse.a);
	}

	LightToPixelVec /= Distance;

	const float HowMuchLight = dot(LightToPixelVec, Input.Normal);
	if(HowMuchLight > 0.0f)
	{
		FinalColor += HowMuchLight * Diffuse * TheLight.Diffuse;
		FinalColor /= TheLight.Attenuation[0] + (TheLight.Attenuation[1] * Distance) + (TheLight.Attenuation[2] * (Distance * Distance));
	}
	
	FinalColor = saturate(FinalColor + FinalAmbient);
	return float4(FinalColor, Diffuse.a);
}
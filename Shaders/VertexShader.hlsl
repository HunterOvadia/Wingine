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

VS_OUTPUT main(float4 InPos : POSITION, float2 InTexCoord : TEXCOORD, float3 Normal : NORMAL)
{
	VS_OUTPUT Output;
	
	Output.Pos = mul(InPos, WVP);
	Output.WorldPos = mul(InPos, World);
	Output.Normal = mul(Normal, World);
	Output.TexCoord = InTexCoord;
	
	return Output;
}
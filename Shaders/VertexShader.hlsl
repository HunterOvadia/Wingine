struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

cbuffer ConstantBufferPerObject
{
	float4x4 WVP;	
};

VS_OUTPUT main(float4 InPos : POSITION, float2 InTexCoord : TEXCOORD)
{
	VS_OUTPUT Output;
	Output.Pos = mul(InPos, WVP);
	Output.TexCoord = InTexCoord;
	return Output;
}
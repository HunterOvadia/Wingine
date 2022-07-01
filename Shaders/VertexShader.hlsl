struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

cbuffer ConstantBufferPerObject
{
	float4x4 WVP;	
};

VS_OUTPUT main(float4 InPos : POSITION, float4 InColor : COLOR)
{
	VS_OUTPUT Output;
	Output.Pos = mul(InPos, WVP);
	Output.Color = InColor;
	return Output;
}
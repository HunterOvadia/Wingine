struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT main(float4 InPos : POSITION, float4 InColor : COLOR)
{
	VS_OUTPUT Output;
	Output.Pos = InPos;
	Output.Color = InColor;
	return Output;
}
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

float4 main(VS_OUTPUT Input) : SV_TARGET
{
	return Input.Color;
}
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

Texture2D ObjectTexture;
SamplerState ObjectSamplerState;
float4 main(VS_OUTPUT Input) : SV_TARGET
{
	return ObjectTexture.Sample(ObjectSamplerState, Input.TexCoord);
}
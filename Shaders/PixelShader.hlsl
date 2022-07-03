struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

Texture2D ObjectTexture;
SamplerState ObjectSamplerState;
float4 main(VS_OUTPUT Input) : SV_TARGET
{
	float4 Diffuse = ObjectTexture.Sample(ObjectSamplerState, Input.TexCoord);
	clip(Diffuse.a - 0.25);
	return Diffuse;
}
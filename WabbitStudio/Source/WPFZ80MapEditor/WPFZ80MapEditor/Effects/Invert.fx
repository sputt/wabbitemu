sampler2D input : register(s0);

float4 main(float2 uv : TEXCOORD) : COLOR
{
	float4 pix = tex2D(input, uv);
	if (pix.a > 0) {
		float4 result = float4(1.0, 1.0, 1.0, 0.0) - pix;
		result.a = pix.a;
		return result;
	} else {
		return pix;
	}
}

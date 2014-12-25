sampler2D input : register(s0);
float4 color : register(c0);

float4 main(float2 uv : TEXCOORD) : COLOR
{
	float4 pix = tex2D(input, uv);
	if (pix.a > 0) {
		return lerp(pix, color, 0.4);
	} else {
		return pix;
	}
}

sampler2D input : register(s0);
float4 color : register(c0);

float4 main(float2 uv : TEXCOORD) : COLOR
{
	float4 pix = tex2D(input, uv);
	if (pix.a > 0 && !(pix.r < 0.2 && pix.g < 0.2 && pix.b <  0.2)) {
		return lerp(pix, color, 0.60);
	} else {
		return pix;
	}
}

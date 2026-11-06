
extern sampler2D BaseTex;
extern sampler2D SpotLightTex;
extern sampler2D StringTex;

struct PS_INPUT
{
    float2 base : TEXCOORD0;
    float2 spotlight : TEXCOORD1;
    float2 tex : TEXCOORD2;
};

struct PS_OUTPUT
{
    float4 diffuse : COLOR0;
};


PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    
    float4 b = tex2D(BaseTex, input.base);
    float4 s = tex2D(SpotLightTex, input.spotlight);
    float4 t = tex2D(StringTex, input.tex);
    
    float4 c = b * s + t;
    c += 0.1f; 
    
    output.diffuse = c;
    
	return output;
}
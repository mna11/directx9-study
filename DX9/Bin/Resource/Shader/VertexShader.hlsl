float4x4 ViewProjMatrix;

float4 Blue = { 0.f, 0.f, 1.f, 1.f };

struct VS_INPUT
{
    float4 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 diffuse : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = mul(input.position, ViewProjMatrix);
    output.diffuse = Blue;
    return output;
}
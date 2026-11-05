extern matrix WorldViewMatrix;
extern matrix ProjMatrix;

static vector Black = { 0.f, 0.f, 0.f, 0.f };


struct VS_INPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL0;
    float4 faceNormal1 : NORMAL1;
    float4 faceNormal2 : NORMAL2;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 diffuse : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    input.position = mul(input.position, WorldViewMatrix);
    
    vector eyeToVertex = input.position;
    
    input.normal.w = 0.f;
    input.faceNormal1.w = 0.f;
    input.faceNormal2.w = 0.f;
    
    input.normal = mul(input.normal, WorldViewMatrix);
    input.faceNormal1 = mul(input.faceNormal1, WorldViewMatrix);
    input.faceNormal2 = mul(input.faceNormal2, WorldViewMatrix);
    
    float dot0 = dot(eyeToVertex, input.faceNormal1);
    float dot1 = dot(eyeToVertex, input.faceNormal2);
    
    if ((dot0 * dot1) < 0.f)
    {
        input.position += 0.1f * input.normal;
    }
    
    output.position = mul(input.position, ProjMatrix);
    output.diffuse = Black;
    
	return output;
}
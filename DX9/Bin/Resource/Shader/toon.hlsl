

extern float4x4 WorldViewMatrix;
extern float4x4 WorldViewProjMatrix;

extern float4 Color;

extern float4 LightDirection;

struct VS_INPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 uvCoords : TEXCOORD;
    float4 diffuse : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    // 클립 공간 좌표 구하기
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = mul(input.position, WorldViewProjMatrix); 
    
    // 빛 방향을 뷰 스페이스로 좌표 변환
    float4 lightDir = LightDirection;
    lightDir.w = 0.f;
    lightDir = mul(lightDir, WorldViewMatrix);

    // 법선을 뷰 스페이스로 좌표 변환 
    input.normal.w = 0.f;
    input.normal = mul(input.normal, WorldViewMatrix);
    
    float u = dot(lightDir, input.normal);
    if (u < 0.f)
        u = 0.f;
    float v = 0.5f;
    
    output.uvCoords.x = u;
    output.uvCoords.y = v;
    output.diffuse = Color;
  
    return output;
}
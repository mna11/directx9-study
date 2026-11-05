
float4x4 ViewMatrix;        // 월드 -> 뷰
float4x4 ViewProjMatrix;    // 월드 -> 클립

float4 AmbientMtrl;         // 재질이 환경광을 반사하는 색 비율
float4 DiffuseMtrl;         // 재질이 확산광을 반사하는 색 비율

float4 LightDirection;      // 표면에서 광원을 향하는 방향

float4 DiffuseLightIntensity = { 0.f, 0.f, 1.f,  1.f }; // 파란색 확산광
float4 AmbientLightIntensity = { 0.f, 0.f, 0.2f, 1.f }; // 약한 파란색 환경광 

struct VS_INPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 diffuse : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    // 클립 공간 좌표 구하기
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = mul(input.position, ViewProjMatrix); 
    
    // 빛 방향을 뷰 스페이스로 좌표 변환
    float4 lightDir = LightDirection;
    lightDir.w = 0.f;
    lightDir = mul(lightDir, ViewMatrix);

    // 법선을 뷰 스페이스로 좌표 변환 
    input.normal.w = 0.f;
    input.normal = mul(input.normal, ViewMatrix);

    // 월드 스페이스에서 해도 되긴함
    // 그런데 왜 클립 스페이스에서 안하고 굳이 ViewMatrix를 상수 테이블로 받아와서
    // 뷰 스페이스에서 할까? 
    // 이건 투영 변환은 벡터 사이의 각도를 보존하지 않기 때문이다.
    // -> 축별 크기 조정이 들어가서 그럼
    
    // [0, 90]을 [1, 0]에 매핑
    float s = dot(lightDir, input.normal);
    // s = max(0, cos{\theta})
    if (s < 0.f)
        s = 0.f;
    
    // 환경광 부분(표면 방향과 관계 없이 기본 밝기) + 확산광 부분(s에 따라 밝기가 달라짐)
    output.diffuse = (AmbientMtrl * AmbientLightIntensity) + s * (DiffuseMtrl * DiffuseLightIntensity);
    return output;
}
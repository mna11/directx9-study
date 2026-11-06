extern matrix WorldViewMatrix;
extern matrix WorldViewProjMatrix;

extern vector Color;
extern vector LightDirection;
extern texture ShadeTex;

struct VS_INPUT
{
    vector position : POSITION;
    vector normal   : NORMAL;
};

struct VS_OUTPUT
{
    vector position : POSITION;
    float2 uvCoords : TEXCOORD;
    vector diffuse  : COLOR;
};

VS_OUTPUT Main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
     output.position = mul(input.position, WorldViewProjMatrix);
    
    float4 lightDir = LightDirection;
    lightDir.w = 0.0f;
    input.normal.w   = 0.0f;
    lightDir = mul(LightDirection, WorldViewMatrix);
    input.normal     = mul(input.normal, WorldViewMatrix);
    
    float u = dot(lightDir, input.normal);
    if( u < 0.0f )
        u = 0.0f;
   
    float v = 0.5f;

    output.uvCoords.x = u;
    output.uvCoords.y = v;
    
    output.diffuse = Color;
    
    return output;
}

sampler ShadeSampler = sampler_state
{
    Texture   = (ShadeTex);
    MinFilter = POINT; // no filtering for cartoon shading
    MagFilter = POINT;
    MipFilter = NONE;
};


technique Toon
{
    pass P0
    {
        vertexShader = compile vs_1_1 Main();

        Sampler[0] = (ShadeSampler);
    }
}

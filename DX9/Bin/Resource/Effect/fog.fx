
technique Fog
{
    pass P0
    {
        pixelshader      = null;
        vertexshader     = null;
        fvf              = XYZ | Normal;
        Lighting         = true;
        NormalizeNormals = true;
        SpecularEnable   = false;

        FogVertexMode = LINEAR; // linear fog function
        FogStart      = 50.0f;  // fog starts 50 units away from viewpoint
        FogEnd        = 300.0f; // fog ends 300 units away from viewpoint

        FogColor      = 0x00CCFFCC; // gray
        FogEnable     = true;       // enable
    }
}

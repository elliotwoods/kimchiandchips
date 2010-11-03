//
// PADE TEST
// coefficients are accepted through
// the view matrix input
// this allows for different pade's per projector

//transforms
float4x4 tW: WORLD;        //the models world matrix
float4x4 tV: VIEW;         //view matrix as set via Renderer (EX9)
float4x4 tP: PROJECTION;   //projection matrix as set via Renderer (EX9)
float4x4 tWVP: WORLDVIEWPROJECTION;

//material properties
float4 cAmb : COLOR <String uiname="Color";>  = {1, 1, 1, 1};
float Alpha = 1;

//texture
texture Tex <string uiname="Texture";>;
sampler Samp = sampler_state    //sampler for doing the texture-lookup
{
    Texture   = (Tex);          //apply a texture to the sampler
    MipFilter = LINEAR;         //sampler states
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

float4x4 tTex: TEXTUREMATRIX <string uiname="Texture Transform";>;

//the data structure: "vertexshader to pixelshader"
//used as output data with the VS function
//and as input data with the PS function
struct vs2ps
{
    float4 Pos : POSITION;
    float4 TexCd : TEXCOORD0;
};

// --------------------------------------------------------------------------------------------------
// VERTEXSHADERS
// --------------------------------------------------------------------------------------------------

float coefficient(int iDimension, int iCoefficient)
{
   int iRow, iColumn;
   
   iRow = iDimension*2 + (iCoefficient/4);
   iColumn = iCoefficient % 4;
   
   return tV[iRow][iColumn];

}

float4 PadePosP(float4 PosW)
{

    //perform Pade
    float4 PosVP = float4(0,0,0,1);
    float Pos2D[2];
    
    for (int i=0; i<2; i++)
        Pos2D[i] = (PosW.x*coefficient(i,0) +
                    PosW.y*coefficient(i,1) +
                    PosW.z*coefficient(i,2) +
                    coefficient(i,3))
                   /
                   (PosW.x*coefficient(i,4) +
                    PosW.y*coefficient(i,5) +
                    PosW.z*coefficient(i,6) +
                    coefficient(i,7));

    
    return float4(Pos2D[0],Pos2D[1],0,1);//Pos2D[0], Pos2D[1], 0, 1);

}

vs2ps VS(
    float4 Pos : POSITION,
    float4 TexCd : TEXCOORD0)
{
    //inititalize all fields of output struct with 0
    vs2ps Out = (vs2ps)0;

    //transform to world space
    float4 PosW = mul(Pos, tW);

    Out.Pos = PadePosP(PosW);
    

    //transform texturecoordinates
    Out.TexCd = mul(TexCd, tTex);

    return Out;
}

// --------------------------------------------------------------------------------------------------
// PIXELSHADERS:
// --------------------------------------------------------------------------------------------------

float4 PS(vs2ps In): COLOR
{
    //In.TexCd = In.TexCd / In.TexCd.w; // for perpective texture projections (e.g. shadow maps) ps_2_0

    float4 col = tex2D(Samp, In.TexCd) * cAmb;
    col.a *= Alpha;
    return col;
}

// --------------------------------------------------------------------------------------------------
// TECHNIQUES:
// --------------------------------------------------------------------------------------------------

technique TConstant
{
    pass P0
    {
        //Wrap0 = U;  // useful when mesh is round like a sphere
        VertexShader = compile vs_1_1 VS();
        PixelShader = compile ps_1_0 PS();
    }
}

technique TConstantFF
{
    pass P0
    {
        //transformations
        WorldTransform[0]   = (tW);
        ViewTransform       = (tV);
        ProjectionTransform = (tP);

        //material
        MaterialAmbient  = {1, 1, 1, 1};
        MaterialDiffuse  = {1, 1, 1, 1};

        //texturing
        Sampler[0] = (Samp);
        TextureTransform[0] = (tTex);
        TexCoordIndex[0] = 0;
        TextureTransformFlags[0] = COUNT4 | PROJECTED;
        //Wrap0 = U;  // useful when mesh is round like a sphere

        //lighting
        LightEnable[0] = TRUE;
        Lighting       = TRUE;
        SpecularEnable = FALSE;

        LightType[0]     = DIRECTIONAL;
        LightAmbient[0]  = (cAmb);
        LightDiffuse[0]  = {0, 0, 0, 0};
        LightDirection[0] = {0, 0, 1, 1};

        //shading
        ShadeMode = FLAT;
        VertexShader = NULL;
        PixelShader  = NULL;
    }
}

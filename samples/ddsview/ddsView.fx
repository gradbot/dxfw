//--------------------------------------------------------------------------------------
// File: ddsview.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture1D tx1D : register( t0 );
Texture1DArray tx1DArray : register( t0 );

Texture2D tx2D : register( t0 );
Texture2D<uint> tx2D_r8uint : register( t0 );
Texture2D<uint4> tx2DStencil : register( t0 );
Texture2DArray tx2DArray : register( t0 );
TextureCube txCube : register( t0 );

Texture3D tx3D : register( t0 );

SamplerState samLinear : register( s0 );

cbuffer LogicControl  : register( b0 )
{
    float   index;
    float   scale;
    float   bias;
    bool    depthMode;          // depth texture
    int     renderMode;         // 0: RGBA, 1/2/3/4 -> R/G/B/A
    bool    normalized;
	float	width;
	float	height;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = input.Pos;
    output.Tex = input.Tex;
    return output;
}

float4 finalColor(float4 clr)
{
    float4 newClr;
    
    if (renderMode == 0)
    {
        newClr = scale*clr;
    }
    else
    {
        float k;
        if (renderMode == 1)
            k = clr.r;
        else if (renderMode == 2)
            k = clr.g;
        else if (renderMode == 3)
            k = clr.b;
        else /*if (renderMode == 1)*/
            k = clr.a;
        newClr = scale*float4(k, k, k, 1);
    }

    return newClr + bias;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_1D( PS_INPUT input) : SV_Target
{
    float4 clr = tx1D.Sample( samLinear, input.Tex.x);
    return finalColor(clr);
}

float4 PS_1DArray( PS_INPUT input) : SV_Target
{
    float4 clr = tx1DArray.Sample( samLinear, float2(input.Tex.x, index) );
    return finalColor(clr);
}

float4 PS_2D( PS_INPUT input) : SV_Target
{
    float4 clr = tx2D.Sample( samLinear, input.Tex.xy);
    return finalColor(clr);
}

float4 PS_2D_R8uint( PS_INPUT input) : SV_Target
{
    uint clr = tx2D_r8uint.Load( int3(input.Tex.xy*float2(width,height), 0) );
    return finalColor(float4(clr, 0, 0, 0));
}

float4 PS_Stencil( PS_INPUT input) : SV_Target
{
    uint stencil = tx2DStencil.Load( int3(input.Tex.xy*float2(width,height), 0) ).g;
    return finalColor(float4(0, stencil, 0, 0));
}

float4 PS_2DArray( PS_INPUT input) : SV_Target
{
    float4 clr = tx2DArray.Sample( samLinear, float3(input.Tex.xy, index) );
    return finalColor(clr);
}

float4 PS_3D( PS_INPUT input) : SV_Target
{
    int Width, Height, Depth;
    tx3D.GetDimensions( Width, Height, Depth);

    float4 clr =  tx3D.Sample( samLinear, float3(input.Tex.xy, index / Depth) );
    return finalColor(clr);
}

// PS_Cube is unused for now
float4 PS_Cube( PS_INPUT input) : SV_Target
{
#if 0
    return txCube.Sample( samLinear, float3(input.Tex.xy, input.Tex.z + (6*index)) );
#else
    return txCube.Sample( samLinear, float3(input.Tex.xy, input.Tex.z));
#endif
}
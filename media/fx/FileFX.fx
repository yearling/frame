//--------------------------------------------------------------------------------------
// File: 受光物体.fx
//
//--------------------------------------------------------------------------------------
uniform  float    g_iCount = 88888;
shared   float4   g_Out;

float3 lightDir< string UIDirectional = "Light Direction";> = {0.577, -0.577, 0.577};
string XFile = "这是FX文件内的字符";
#define MAX  100
float4 g_v4Vector[ MAX ];
//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;  
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    Out.Position = In.Position;
    //
    g_Out.xyzw = Out.Position.xyzw;
    //
    g_iCount = g_iCount + 1;
    return Out;
  }


//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        CullMode = CCW;
        FillMode = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = NULL;
      }
  }

technique TestTech
  {
    pass P0
      {
      }
    pass P1
      {
      }
    pass P2
      {
      }
  }

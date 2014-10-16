//--------------------------------------------------------------------------------------
// 效果文件
//
//--------------------------------------------------------------------------------------
uniform  float    g_iCount = 88888;
shared   float4   g_Out;

float3 lightDir< string UIDirectional = "Light Direction";> = {0.577, -0.577, 0.577};
string XFile = "这是FX文件内的字符";

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        CullMode = CCW;
        FillMode = WireFrame;
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

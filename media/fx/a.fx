//--------------------------------------------------------------------------------------
// Ч���ļ�
//
//--------------------------------------------------------------------------------------
uniform  float    g_iCount = 88888;
shared   float4   g_Out;

float3 lightDir< string UIDirectional = "Light Direction";> = {0.577, -0.577, 0.577};
string XFile = "����FX�ļ��ڵ��ַ�";

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

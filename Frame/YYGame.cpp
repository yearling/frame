#include "stdafx.h"
#include "YYGame.h"
#include <cstdio>

YYGame::YYGame(void)
{
}


YYGame::~YYGame(void)
{
}


void YYGame::GameResourceReset()
{
	GetD3D9Device()->SetRenderState(D3DRS_LIGHTING,FALSE);
}
#define D3DFVF_YYFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
void YYGame::GameResourceInit()
{
	HRESULT hr;
	YYFVF g_Vertices[] =
	{
		{ -1.0f,-1.0f, 0.0f, 0xffff0000, },
		{  1.0f,-1.0f, 0.0f, 0xff0000ff, },
		{  0.0f, 1.0f, 0.0f, 0xffffffff, },

	};
	if( FAILED( hr = GetD3D9Device()->CreateVertexBuffer( 3 * sizeof( YYFVF ),
		0 , D3DFVF_YYFVF , D3DPOOL_MANAGED , &vertex_buf, NULL ))) 
		return ;
	VOID* pVertices;
	if( FAILED( hr = vertex_buf->Lock( 0, sizeof( g_Vertices ), ( void** )&pVertices, 0 )))    
		return;
	memcpy( pVertices, g_Vertices, sizeof( g_Vertices ));
	vertex_buf->Unlock( );

	GetD3D9Device()->SetRenderState(D3DRS_LIGHTING,FALSE);
//	GetD3D9Device()->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	GetD3D9Device()->SetRenderState(D3DRS_STENCILENABLE,TRUE);
}

void YYGame::GameResourceLost()
{

}

void YYGame::GameMain(double timespan)
{
	HRESULT hr;
	GetD3D9Device()->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_STENCIL|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 100, 0, 0 ), 1.0f, 0 );
	if(SUCCEEDED(GetD3D9Device()->BeginScene()))	
	{
		float ftime=(float)timespan;
		UINT iTime = timeGetTime() % 1000;
		FLOAT fAngle = iTime * ( 2.0f * D3DX_PI ) / 1000.0f;
		D3DXMATRIX world;
		D3DXMatrixRotationY(&world,fAngle);
		GetD3D9Device()->SetTransform(D3DTS_WORLD,&world);
		D3DXVECTOR3 vEyePt( 0.0f , 3.0f , -5.0f );
		D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
		GetD3D9Device()->SetTransform(D3DTS_VIEW,&matView);
		D3DXMATRIX pro;
		D3DXMatrixPerspectiveFovLH(&pro,D3DX_PI/4,1.0f,1.0f,100.f);
		GetD3D9Device()->SetTransform(D3DTS_PROJECTION,&pro);

		GetD3D9Device()->SetStreamSource(0,vertex_buf,0,sizeof(YYFVF));
		GetD3D9Device()->SetFVF(D3DFVF_YYFVF);
		GetD3D9Device()->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,1);
		//ID3DXMesh *mesh=0;
		//hr=D3DXCreateTeapot(GetD3D9Device(),&mesh,nullptr);
		////if(SUCCEEDED(hr))
		//{
		//	mesh->DrawSubset(0);
		//	mesh->Release();
		//}
		GetD3D9Device()->EndScene();
	}
}

bool YYGame::GameInit()
{
	return true;
}

void YYGame::GameExit()
{

}

void YYGame::MouseProc(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos)
{
	if(bLeftButtonDown)
		ToggleFullScreen();
	if(bRightButtonDown)
	{
		if(GetRenderingPaused())
			Pause(false,false);
		else
			Pause(true,true);
	}
}


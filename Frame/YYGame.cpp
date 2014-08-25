#include "stdafx.h"
#include "YYGame.h"
#include <cstdio>
#include <boost/exception/all.hpp>
#include "YYUT.h"
namespace YYUT
{

	#define D3DFVF_YYFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
	const int  IDC_TOGGLEFULLSCREEN   =1;
	const int  IDC_TOGGLEREF          =2;
	const int  IDC_CHANGEDEVICE       =3;
	YYGame::YYGame(void)
	{
	}


	YYGame::~YYGame(void)
	{
	}


	void YYGame::GameResourceReset()
	try{
		HUDRest();
	}
	catch(YYUTGUIException &e)
	{
#if defined( DEBUG ) || defined( _DEBUG )
		cout<<"[GameResourceReset]:failed!!"<<endl;
#endif
		throw;
	}


	void YYGame::GameResourceInit()
	try{
		HRESULT hr;
		HUDInit();
		//////////////////////////////////////////////////////////////////////////
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
		GetD3D9Device()->SetRenderState(D3DRS_STENCILENABLE,TRUE);
		YYUTDialogResourceManager::GetInstance()->SetHWND(GetHWND());
		YYUTDialogResourceManager::GetInstance()->OnD3DCreateDevice(GetD3D9Device());
		robot_mesh_=make_shared<YYUTObjectX>();
		robot_mesh_->Init(GetD3D9Device());
		robot_mesh_->LoadObject(_T("..//media//robot.x"));
	}
	catch(YYUTGUIException &e)
	{
		throw e;
	}

	void YYGame::GameResourceLost()
	{
		YYUTDialogResourceManager::GetInstance()->OnD3DLostDevice();
	}

	void YYGame::GameMain(double time_span, double time_elapse)
{
		GetD3D9Device()->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_STENCIL|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 100, 0, 0 ), 1.0f, 0 );
		if(SUCCEEDED(GetD3D9Device()->BeginScene()))	
		{
			static float ftime=0;
			ftime+=(float)time_elapse*30;
			if(ftime>=360)
				ftime=0.0f;
			FLOAT fAngle = ftime*D3DX_PI/180;
			D3DXMATRIX world;
			D3DXMatrixRotationY(&world,fAngle);
			GetD3D9Device()->SetTransform(D3DTS_WORLD,&world);
			D3DXVECTOR3 vEyePt( 0.0f , 3.0f , -50.0f );
			D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
			D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
			D3DXMATRIX matView;
			D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
			GetD3D9Device()->SetTransform(D3DTS_VIEW,&matView);
			D3DXMATRIX pro;
			D3DXMatrixPerspectiveFovLH(&pro,D3DX_PI/4,1.0f,1.0f,100.f);
			GetD3D9Device()->SetTransform(D3DTS_PROJECTION,&pro);

			/*GetD3D9Device()->SetStreamSource(0,vertex_buf,0,sizeof(YYFVF));
			GetD3D9Device()->SetFVF(D3DFVF_YYFVF);
			GetD3D9Device()->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,1);*/
			GetD3D9Device()->SetRenderState(D3DRS_LIGHTING,FALSE);
			robot_mesh_->Draw();	
			hud_->OnRender(time_span);	
			GetD3D9Device()->EndScene();
		}
	}

	void YYGame::GameInit()
	{
	}

	void YYGame::GameExit()
	{
	}

	void YYGame::MouseProc(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos)
	{
	}

	HRESULT YYGame::PreMyProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,bool &further_process)
	{
		further_process=YYUTDialogResourceManager::GetInstance()->MsgProc(hWnd,uMsg,wParam,lParam);
		if(further_process)
			return 0;
		if(hud_)
		{
			further_process=hud_->ProcessMsg(hWnd,uMsg,wParam,lParam);
			if(further_process)
			return 0;
		}
		further_process=false;
		return 0;
	}

	void YYGame::HUDRest()
	{
		GetD3D9Device()->SetRenderState(D3DRS_LIGHTING,FALSE);
		YYUTDialogResourceManager::GetInstance()->OnD3DResetDevice();
		YYUTD3D9DeviceSettings *dev_seting=GetCurrentDeviceSettings();
		int width=dev_seting->pp.BackBufferWidth;
		int height=dev_seting->pp.BackBufferHeight;
		hud_->SetLocation(width-170,0);
		hud_->SetSize(170,170);
	}

	void YYGame::HUDInit()
	{
		HRESULT hr;
		hud_=YYUTDialog::MakeDialog();
		hud_->Init(YYUTDialogResourceManager::GetInstance(),true);
		int index_y=10;
		shared_ptr<YYUTButton> bt_fullscreen=hud_->AddButton(IDC_TOGGLEFULLSCREEN,L"Toggle full screen",35,index_y,125,22);
		bt_fullscreen->SetEvent(std::bind(&YYGame::ToggleFullScreen,this));
		hud_->AddButton(IDC_TOGGLEREF,L"Toggle REF(F3)",35,index_y+=24,125,22);
		hud_->AddButton(IDC_CHANGEDEVICE,L"Change device(F2)",35,index_y+=24,125,22,VK_F2);
	}


	
}

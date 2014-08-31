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
		int width=GetCurrentDeviceSettings()->pp.BackBufferWidth;
		int height=GetCurrentDeviceSettings()->pp.BackBufferHeight;
		float aspect=(float)width/(float)height;
		camera_.SetProjParam(D3DX_PI/4,aspect,1.0f,1000.0f);
		camera_.SetWindow(width,height);
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

		YYUTDialogResourceManager::GetInstance()->SetHWND(GetHWND());
		YYUTDialogResourceManager::GetInstance()->OnD3DCreateDevice(GetD3D9Device());
		D3DXVECTOR3 eye( 0.0f , 3.0f , -50.0f );
		D3DXVECTOR3 lookat( 0.0f, 0.0f, 0.0f );

		camera_.SetHWND(GetHWND());
		//camera_.SetButtonMasks();
		camera_.SetViewParam(&eye,&lookat);
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
		camera_.FrameMove(time_elapse);
		GetD3D9Device()->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_STENCIL|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 100, 0, 0 ), 1.0f, 0 );
		if(SUCCEEDED(GetD3D9Device()->BeginScene()))	
		{
			D3DXMATRIX world,view,proj;
			world=*camera_.GetWorldMatrix();
			view=*camera_.GetViewMatrix();
			proj=*camera_.GetProjMatrix();
			GetD3D9Device()->SetTransform(D3DTS_WORLD,&world);
			GetD3D9Device()->SetTransform(D3DTS_VIEW,&view);
			GetD3D9Device()->SetTransform(D3DTS_PROJECTION,&proj);

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
		camera_.HandleMessage(hWnd,uMsg,wParam,lParam);
		
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

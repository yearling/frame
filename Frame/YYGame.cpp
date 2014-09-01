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
		UNREFERENCED_PARAMETER(e);
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
		
		robot_mesh_=make_shared<YYUTObjectX>();
		robot_mesh_->Init(GetD3D9Device());
		robot_mesh_->LoadObject(_T("..//media//robot.x"));
		cell_mesh_=make_shared<YYUTObjectX>();
		cell_mesh_->Init(GetD3D9Device());
		cell_mesh_->LoadObject(_T("..//media//cell.x"));
		byte* vertex_buf=nullptr;
		auto mesh=robot_mesh_->GetMesh();
		mesh->LockVertexBuffer(0, (void**)&vertex_buf);
		D3DXVECTOR3 center;
		float radius;
		 hr = D3DXComputeBoundingSphere((D3DXVECTOR3*)vertex_buf, mesh->GetNumVertices(),
			D3DXGetFVFVertexSize(mesh->GetFVF()), &center, &radius);
		if(FAILED(hr))
			BOOST_THROW_EXCEPTION(YYUTException()<<err_str("caculation model radius error")<<err_hr(hr));
		if(radius<=0)
		{
			radius =100.0f;
		}
		cout<<radius<<endl;
		mesh->UnlockVertexBuffer();
		D3DXVECTOR3 eye(0.0f , 0.0f,-radius*3);
		D3DXVECTOR3 lookat( 0.0f, 0.0f, 1.0f );

		camera_.SetHWND(GetHWND());
		//camera_.SetButtonMasks();
		camera_.SetViewParam(&eye,&lookat);
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
		camera_.FrameMove((float)time_elapse);
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
			D3DXMatrixIdentity(&world);
			D3DXMATRIX scale;
			D3DXMatrixScaling(&scale,5.0f,5.0f,5.0f);
			world*=scale;
			GetD3D9Device()->SetTransform(D3DTS_WORLD,&world);
			cell_mesh_->Draw();
			hud_->OnRender((float)time_span);	
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
		hud_->SetLocation(width-170,0);
		hud_->SetSize(170,170);
	}

	void YYGame::HUDInit()
	{
		hud_=YYUTDialog::MakeDialog();
		hud_->Init(YYUTDialogResourceManager::GetInstance(),true);
		int index_y=10;
		shared_ptr<YYUTButton> bt_fullscreen=hud_->AddButton(IDC_TOGGLEFULLSCREEN,L"Toggle full screen",35,index_y,125,22);
		bt_fullscreen->SetEvent(std::bind(&YYGame::ToggleFullScreen,this));
		hud_->AddButton(IDC_TOGGLEREF,L"Toggle REF(F3)",35,index_y+=24,125,22);
		hud_->AddButton(IDC_CHANGEDEVICE,L"Change device(F2)",35,index_y+=24,125,22,VK_F2);
	}


	
}

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
		//camera_.SetWindow(width,height);
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
		mesh->UnlockVertexBuffer();
		D3DXVECTOR3 eye(0.0f , 0.0f,radius*3);
		D3DXVECTOR3 lookat( 0.0f, 0.0f, 1.0f );

		camera_.SetHWND(GetHWND());
		camera_.SetViewParam(&eye,&lookat);
		//camera_.SetRaius(3*radius);
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
			hud_->OnRender((float)time_elapse);	
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
		int height=GetHeight();
		float scalar_width=0.15f;
		float scalar_height=0.05f;
		float scalar_seperate=0.008f;
		int dialog_height=(int)(height*scalar_height);
		int dialog_widht=GetWidth();
		int control_width=(int)(scalar_width*dialog_widht);
		int control_height=dialog_height;
		int control_seperate=(int)(scalar_seperate*dialog_widht);
		hud_->SetLocation(0,height-dialog_height);
		hud_->SetSize(GetWidth(),dialog_height);
		int index_x=0;
		bt_fullscreen->ResetPosisionSize(index_x,0,control_width,control_height);
		bt_sample2->ResetPosisionSize(index_x+=(control_width+control_seperate),0,control_width,control_height);
		bt_sample3->ResetPosisionSize(index_x+=(control_width+control_seperate),0,control_width,control_height);
	}

	void YYGame::HUDInit()
	{
		hud_=YYUTDialog::MakeDialog();
		hud_->Init(YYUTDialogResourceManager::GetInstance(),true); 
		D3DXCOLOR dark_theme=D3DCOLOR_ARGB(100,14,17,42);
		hud_->SetBackgroundColor(dark_theme);
		int index_x=0;
		int height=GetHeight();
		float scalar=0.05f;
		int dialog_height=(int)(height*scalar);
		hud_->SetLocation(0,height-dialog_height);
		hud_->SetSize(GetWidth(),dialog_height);
		bt_fullscreen=hud_->AddButton(IDC_TOGGLEFULLSCREEN,L"Toggle full screen",index_x,0,300,dialog_height);
		bt_fullscreen->SetEvent(std::bind(&YYGame::ToggleFullScreen,this));
		bt_sample2=hud_->AddButton(IDC_TOGGLEREF,L"Sample(F2)",index_x+=310,0,300,dialog_height);
		bt_sample3=hud_->AddButton(IDC_CHANGEDEVICE,L"Change device(F2)",index_x+=310,0,300,dialog_height);
	}


	
}

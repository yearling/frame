#include "stdafx.h"
#include "YYGame.h"
#include <cstdio>
#include <boost/exception/all.hpp>
#include "YYUT.h"
#include "YYUTMsgPackege.h"
namespace YYUT
{

	#define D3DFVF_YYFVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
	const int  IDC_TOGGLEFULLSCREEN   =1;
	const int  IDC_TOGGLEREF          =2;
	const int  IDC_CHANGEDEVICE       =3;
	const int  IDC_FPS				  =4;
	YYGame::YYGame(void)
	{
	}


	YYGame::~YYGame(void)
	{
	}


	void YYGame::GameResourceReset()
	try{
		HUDRest();
		int width=GetWidth();
		int height=GetHeight();
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
		robot_mesh_->LoadObject(_T("..\\media\\robot\\robot.x"));
		cell_mesh_=make_shared<YYUTObjectX>();
		cell_mesh_->Init(GetD3D9Device());
		cell_mesh_->LoadObject(_T("..\\media\\cell\\cell.x"));
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
		int fps=(int)GetFPS();
		wstringstream ss;
		ss<<fps;
		wstring str_fps(ss.str());
		bt_fps_->SetText(str_fps);	
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
			//////////////////////////////////////////////////////////////////////////
			
			//////////////////////////////////////////////////////////////////////////
			robot_mesh_->Draw();	
			D3DXMatrixIdentity(&world);
			D3DXMATRIX scale;
			D3DXMatrixScaling(&scale,5.0f,5.0f,5.0f);
			world*=scale;
			GetD3D9Device()->SetTransform(D3DTS_WORLD,&world);
			cell_mesh_->Draw();
			//////////////////////////////////////////////////////////////////////////
			//draw controls eq: button animate_static dialog huds;
			hud_->OnRender((float)time_elapse);	
			hud_fps_->OnRender((float)time_elapse);
			//////////////////////////////////////////////////////////////////////////
				
			GetD3D9Device()->EndScene();
		}
	}

	void YYGame::GameInit()
	{
	}

	void YYGame::GameExit()
	{
	}



	void YYGame::HUDRest()
	{
		GetD3D9Device()->SetRenderState(D3DRS_LIGHTING,FALSE);
		YYUTDialogResourceManager::GetInstance()->OnD3DResetDevice(GetWidth(),GetHeight());
		int height=GetHeight();
		float scalar_width=0.18f;
		float scalar_height=0.0618f;
		float scalar_seperate=0.008f;
		D3DXCOLOR dark_theme=D3DCOLOR_ARGB(100,14,17,42);
		hud_->SetBackgroundColor(dark_theme);
		hud_->SetLocation(0.0f,1-scalar_height);
		hud_->SetSize(1.0f,scalar_height);
		hud_->SetFont("default_font",0.77f);
		float index_x=0.0f;
		bt_fullscreen->ResetPosisionSize(index_x,0.0f,scalar_width,1.0f);
		bt_sample2->ResetPosisionSize(index_x+=(scalar_width+scalar_seperate),0.0f,scalar_width,1.0f);
		bt_sample3->ResetPosisionSize(index_x+=(scalar_width+scalar_seperate),0.0f,scalar_width,1.0f);
		float hud_locate_x=0.9f;
		float hud_fps_width=1-hud_locate_x;
		float asper=(float)GetWidth()/(float)GetHeight();
		float hud_fps_height=hud_fps_width*asper;
		hud_fps_->SetLocation(hud_locate_x,0.0f);
		hud_fps_->SetSize(hud_fps_width,hud_fps_height);
		hud_fps_->SetBackgroundColor(D3DCOLOR_ARGB(0,14,17,42));
		hud_fps_->SetFont("default_font",0.50f);
		bt_fps_->SetLocation(0.0f,0.0f);
		bt_fps_->SetSize(1.0f,1.0f);
	}

	void YYGame::HUDInit()
	{
		hud_=YYUTDialog::MakeDialog();
		hud_fps_=YYUTDialog::MakeDialog();
		hud_->Init(YYUTDialogResourceManager::GetInstance(),true); 
		hud_fps_->Init(YYUTDialogResourceManager::GetInstance(),true);	
		bt_fullscreen=hud_->AddButton(IDC_TOGGLEFULLSCREEN,L"Full Screen");
		bt_fullscreen->SetEvent(std::bind(&YYGame::ToggleFullScreen,this));
		bt_sample2=hud_->AddButton(IDC_TOGGLEREF,L"Pause");
		bt_sample2->SetEvent(std::bind(&YYUTManager::Pause,this,false,true));
		bt_sample3=hud_->AddButton(IDC_CHANGEDEVICE,L"Resume");
		bt_sample3->SetEvent(std::bind(&YYUTManager::Pause,this,false,false));
		bt_fps_=hud_fps_->AddStaticAnimate(IDC_FPS);
	}

	void YYGame::KeyboardMouseProc()
	{
		for(;;)
		{
			YYUTWindowsMsg msg=GetKeyBoardMouseMSGQueue().Take();
			if(hud_)
			{
				hud_->ProcessMsg(GetHWND(),msg.uMsg,msg.wParam,msg.lParam);
			}
			camera_.HandleMessage(GetHWND(),msg.uMsg,msg.wParam,msg.lParam);
		}
	}

	void YYGame::ToggleFullScreen()
	{
		::SendMessage(GetHWND(),WM_FULLSCREEN,0,0);
	}
}

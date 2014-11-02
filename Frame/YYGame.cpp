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
		YYUTEffectManager::GetInstance().OnDeviceReset();
		for_each(balls_.begin(),balls_.end(),[&]( std::shared_ptr<YYUTHLSLRenderObject> & ptr){
			ptr->OnResetDevice();});
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
		YYUTEffectManager::GetInstance().OnDeviceCreate(GetD3D9Device());
		YYUTEffectManager::GetInstance().CreateEffect("ball.effect",_T("..\\media\\ball1\\FileFX.fx"));
		balls_.push_back(make_shared<YYUTHLSLRenderObject>());
		balls_.push_back(make_shared<YYUTHLSLRenderObject>());
		balls_.push_back(make_shared<YYUTHLSLRenderObject>());
		balls_.push_back(make_shared<YYUTHLSLRenderObject>());
		for_each(balls_.begin(),balls_.end(),[&]( std::shared_ptr<YYUTHLSLRenderObject> & ptr){
			ptr->Init(GetD3D9Device());});
		balls_[0]->LoadObject(_T("..\\media\\ball1\\Model.X"),"ball.effect");
		balls_[1]->LoadObject(_T("..\\media\\ball2\\Model.X"),"ball.effect");
		balls_[2]->LoadObject(_T("..\\media\\ball3\\Model.X"),"ball.effect");
		balls_[3]->LoadObject(_T("..\\media\\ball4\\Model.X"),"ball.effect");
		for(auto iter=balls_.begin();iter!=balls_.end();iter++)
		{
			(*iter)->SetFrameMoveEvent(std::bind(&YYGame::BallFrameMoveEvent,this,placeholders::_1));
			(*iter)->SetFrameResetEvent(std::bind(&YYGame::BallFrameReset,this,placeholders::_1));
		}
		float radius=balls_[0]->GetRadius();
		if(radius<=0)
		{
			radius =100.0f;
		}
		D3DXVECTOR3 eye(30.0f , 15.0f,0.0f);
		D3DXVECTOR3 lookat( 0.0f, 0.0f, 0.0f );
		camera_.SetHWND(GetHWND());
		camera_.SetViewParam(&eye,&lookat);
		//camera_.SetRaius(3*radius);
		//////////////////////////////////////////////////////////////////////////
		YYUTImportFactory im;
		im.CreateImport("hello.txt");
	}
	catch(YYUTGUIException &e)
	{
		throw e;
	}

	void YYGame::GameResourceLost()
	{
		YYUTDialogResourceManager::GetInstance()->OnD3DLostDevice();
		YYUTEffectManager::GetInstance().OnDeviceLost();
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
			//////////////////////////////////////////////////////////////////////////
			for_each(balls_.begin(),balls_.end(),[&]( std::shared_ptr<YYUTHLSLRenderObject> & ptr){
				ptr->OnFrameMove();ptr->Draw();});
			//////////////////////////////////////////////////////////////////////////
			
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
		//::SendMessage(GetHWND(),WM_FULLSCREEN,0,0);
		//注意不能用SendMessage,这样子的话windowProc函数会在处理输入的线程里执行，
		//也就是会在处理输入的线程里执行ToggleFullScreenImp()，这个函数里会调用IDirect3DDevice::Reset
		//文档上说明了，Reset，Create，Destroy必须在创建窗口的线程里，要么会死锁
		::PostMessage(GetHWND(),WM_FULLSCREEN,0,0);
	}

	
	void YYGame::BallFrameReset(YYUTHLSLRenderObject *objectx)
	{
		if(objectx)
		{
			CComPtr<IDirect3DTexture9> texture=objectx->GetMaterialTextureCombine()[0].second;
			CComPtr<ID3DXEffect> effect=objectx->GetEffect();
			effect->SetTechnique("DefaultTech");
			effect->SetTexture("g_ColorTexture",texture);
		}
	}

	void YYGame::BallFrameMoveEvent(YYUTHLSLRenderObject *objectx)
	{
		HRESULT hr=S_OK;
		if(objectx)
		{
			auto effect=objectx->GetEffect();	
			effect->SetMatrix("g_matWorld",camera_.GetWorldMatrix());
			effect->SetMatrix("g_matView",camera_.GetViewMatrix());
			effect->SetMatrix("g_matProject",camera_.GetProjMatrix());
			effect->SetFloat("g_fTime",GetElapsedTime());
			D3DXVECTOR4 eye(*camera_.GetEyePt(),0);
			effect->SetVector("g_vEyePosition",&eye);
			D3DXMATRIX mat_material;
			D3DMATERIAL9 mtr=objectx->GetMaterialTextureCombine()[0].first;
			mat_material._11=mtr.Ambient.r;
			mat_material._12=mtr.Ambient.g;
			mat_material._13=mtr.Ambient.b;
			mat_material._14=mtr.Ambient.a;
			mat_material._21=mtr.Diffuse.r;
			mat_material._22=mtr.Diffuse.g;
			mat_material._23=mtr.Diffuse.b;
			mat_material._24=mtr.Diffuse.a;
			mat_material._31=mtr.Specular.r;
			mat_material._32=mtr.Specular.g;
			mat_material._33=mtr.Specular.b;
			mat_material._34=mtr.Specular.a;
			effect->SetMatrix("g_matMaterial",&mat_material);
			float determinnat;
			D3DXMATRIX mat_out;
			D3DXMatrixInverse(&mat_out,&determinnat,camera_.GetWorldMatrix());
			D3DXMatrixTranspose(&mat_out,&mat_out);
			effect->SetMatrix("g_matWorldNormalInverseTranspose",&mat_out);
			float fTime=GetElapsedTime();
			D3DXVECTOR4 vLightPosition( 7.0f ,35.0f  , 7.0f , 1.0f );
			effect->SetVector("g_vLightPosition",&vLightPosition);
		}
	}

}

#include "stdafx.h"
#include "YYUT.h"
#include "YYUTGUI.h"
#include <algorithm>
#include <boost\throw_exception.hpp>
#include <iostream>
#include "YYUTTimer.h"
using std::cout;
namespace YYUT
{

	YYUTDialogResourceManager::YYUTDialogResourceManager()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		cout<<"[YYUTDialogResourceManager] Create!"<<endl;
#endif // _DEBUG
		width_=height_=0;
	}

	YYUTDialogResourceManager::~YYUTDialogResourceManager()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		cout<<"[YYUTDialogResourceManager] Destroy!"<<endl;
#endif // _DEBUG
	}

	bool YYUTDialogResourceManager::MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		return false;
	}

	void YYUTDialogResourceManager::OnD3DCreateDevice(LPDIRECT3DDEVICE9 d3d_device)
		try
	{
		HRESULT hr=S_OK;
		d3d_device_=d3d_device;
		std::for_each(vec_font_.begin(),vec_font_.end(),[&](std::shared_ptr<YYUTFontNode> &p)
		{
			YYUTDialogResourceManager::CreateFont(p);
		});
		std::for_each(vec_texture_.begin(),vec_texture_.end(),[&](std::shared_ptr<YYUTTextureNode>& p)
		{ 
			CreateTexture(p);
		});
		hr=D3DXCreateSprite(d3d_device_,&sprite_);
		if(FAILED(hr))
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_hr(hr));
	}
	catch (YYUTGUIException &e)
	{
		UNREFERENCED_PARAMETER(e);
#if defined( DEBUG ) || defined( _DEBUG )
		cout<<"[YYUTDialogResourceManager]: OnCreateDevice failed!!"<<endl;
#endif
		throw;
	}
	catch(...)
	{
		throw;
	}

	void YYUTDialogResourceManager::OnD3DResetDevice()
	{
		HRESULT hr=S_OK;
		std::for_each(vec_font_.begin(),vec_font_.end(),[&](shared_ptr<YYUTFontNode> &p){
			p->font_->OnResetDevice();
		});
		if(sprite_)
			sprite_->OnResetDevice();
		hr=d3d_device_->CreateStateBlock(D3DSBT_ALL,&state_block_);
		if(FAILED(hr))
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_hr(hr));
	}

	void YYUTDialogResourceManager::OnD3DResetDevice(int width,int height)
	{
		OnD3DResetDevice();
		SetWidth(width);
		SetHeight(height);
	}

	void YYUTDialogResourceManager::OnD3DLostDevice()
	{
		std::for_each(vec_font_.begin(),vec_font_.end(),[&](shared_ptr<YYUTFontNode> p){
			if(p) p->font_->OnLostDevice();
		});
		if(sprite_)
			sprite_->OnLostDevice();
		state_block_=nullptr;
	}

	void YYUTDialogResourceManager::OnD3DDestroyDevice()
	{
		d3d_device_=nullptr;
		std::for_each(vec_font_.begin(),vec_font_.end(),[&](shared_ptr<YYUTFontNode> p){
			if(p) p->font_=nullptr;
		});
		std::for_each(vec_texture_.begin(),vec_texture_.end(),[&](std::shared_ptr<YYUTTextureNode> p){ 
			if(p) p->texture_=nullptr;
		});
	}
	//throw YYUTGUIException;
	//首先把要创建的资源放入manager,如果d3d_device存在，则创建。
	shared_ptr<YYUTFontNode> YYUTDialogResourceManager::AddFont(wstring face_name,long height,long weight)
	{
		// 找到一个font
		auto pos=std::find_if(vec_font_.begin(),vec_font_.end(),[&](shared_ptr<YYUTFontNode> p)->bool{
			if(p->face_==face_name &&p->height_==height && p->weight_==weight)
				return true;
			else
				return false;
		});
		if(pos!=vec_font_.end())
			return *pos;
		auto new_font_node=std::make_shared<YYUTFontNode>();
		new_font_node->face_=face_name;
		new_font_node->height_=height;
		new_font_node->weight_=weight;
		vec_font_.push_back(new_font_node);
		if(d3d_device_)
			YYUTDialogResourceManager::CreateFont(new_font_node);
		return new_font_node;
	}

	shared_ptr<YYUTTextureNode> YYUTDialogResourceManager::AddTexture(wstring resource_name)
	{
		auto pos=find_if(vec_texture_.begin(),vec_texture_.end(),[&](shared_ptr<YYUTTextureNode> p)->bool{
			if(p->file_name_==resource_name)
				return true;
			return false;
		});
		if(pos!=vec_texture_.end())
			return *pos;
		shared_ptr<YYUTTextureNode> new_texture_node(new YYUTTextureNode);
		new_texture_node->file_name_=resource_name;
		vec_texture_.push_back(new_texture_node);
		if(d3d_device_)
			CreateTexture(new_texture_node);
		return new_texture_node;
	}

	void YYUTDialogResourceManager::EnableKeyboardInputForAllDialogs()
	{
	}
	//throw YYUTGUIException();
	void YYUTDialogResourceManager::CreateFont(std::shared_ptr<YYUTFontNode> &font)
	{
		HRESULT hr;
		font->font_=nullptr;
		hr=D3DXCreateFont(d3d_device_,font->height_,0,font->weight_,1,FALSE,DEFAULT_CHARSET,
			OUT_TT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,font->face_.c_str(),&font->font_);
		if(FAILED(hr))
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_hr(hr)<<err_str("D3DXCreateFont Failed!"));
	}

	void YYUTDialogResourceManager::CreateTexture(std::shared_ptr<YYUTTextureNode> texture_node)
		try
	{
		HRESULT hr=S_OK;
		D3DXIMAGE_INFO image_info;

		if(texture_node->file_name_.empty())
			return ;
		hr=D3DXCreateTextureFromFileEx( d3d_device_, texture_node->file_name_.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
			1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
			D3DX_DEFAULT, D3DX_DEFAULT, 0,
			&image_info, NULL, &texture_node->texture_);
		if(FAILED(hr))
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_hr(hr)<<err_str("D3DXCreateTextureFromFileEx failed!"));

		texture_node->width_=image_info.Width;
		texture_node->heigth_=image_info.Height;
	}
	catch(YYUTException& e)
	{
			e<<err_str("Create GUI Texture failed! : load frome resource");
		throw;
	}

	void YYUTDialogResourceManager::SetHWND(HWND hwnd)
	{
		hwnd_=hwnd;
	}


	YYUTDialogResourceManager YYUTDialogResourceManager::instance_;


	YYUTFontNode::YYUTFontNode()
	{
		height_=0;
		weight_=0;
	}


	YYUTTextureNode::YYUTTextureNode():
		width_(0),
		heigth_(0)
	{
	}


	void YYUTBlendColor::Init(D3DCOLOR default_color,D3DCOLOR disablecolor/*=D3DCOLOR_ARGB(200,128,128,128)*/,D3DCOLOR hidden_color/*=0*/)
	{
		std::fill(&state_[0],&state_[MAX_CONTROL_STATE],default_color);
		state_[YYUT_STATE_DISABLE]=disablecolor;
		state_[YYUT_STATE_HIDDEN]=hidden_color;
		current_=hidden_color;
	}

	void YYUTBlendColor::Blend(UINT state,float elapsed_time,float rate/*=0.7f*/)
	{
		D3DXCOLOR des_color=state_[state];
		D3DXColorLerp(&current_,&current_,&des_color,1.0f-powf(rate,30*elapsed_time));
	}

	YYUTBlendColor::YYUTBlendColor()
	{
		memset(state_,0,sizeof(D3DCOLOR)*MAX_CONTROL_STATE);
		ZeroMemory(&current_,sizeof(current_));
	}




	void YYUTElement::SetTexture(string index_texture,RECT *prc_textrue,D3DCOLOR default_texture_color/*=D3DCOLOR_ARGB(255,255,255,255)*/)
	{
		texture_id_=index_texture;
		if(prc_textrue)
			rect_texture_=*prc_textrue;
		else
			SetRectEmpty(&rect_texture_);
		texture_color_.Init(default_texture_color);
	}

	void YYUTElement::SetFont(string index_font,D3DCOLOR default_font_color/*=D3DCOLOR_ARGB(255,255,255,255)*/,DWORD text_format/*=DT_CENTER| DT_VCENTER*/)
	{
		font_id_=index_font;
		text_format_=text_format;
		font_color_.Init(default_font_color);
	}

	void YYUTElement::Refresh()
	{
		texture_color_.current_=texture_color_.state_[YYUT_STATE_HIDDEN];
		font_color_.current_=font_color_.state_[YYUT_STATE_HIDDEN];
	}

	YYUTElement::YYUTElement():
	font_id_(""),
		texture_id_(""),
		text_format_(0)
	{
		ZeroMemory(&rect_texture_,sizeof(rect_texture_));
	}





	void YYUTDialog::AddStatic(int ID,wstring text,int x,int y,int width,int height,bool is_default/*=false*/,YYUTStatic **create/*=nullptr*/)
	{

	}

	shared_ptr<YYUTButton> YYUTDialog::AddButton(int ID,wstring text,int x,int y,int width,int height,UINT hot_key/*=0U*/,bool is_default/*=false*/)
	{
		shared_ptr<YYUTButton> button=YYUTButton::MakeInstance(shared_from_this());
		AddControl(static_pointer_cast<YYUTControl>(button));
		button->SetID(ID);
		button->SetText(text);
		button->SetLocation(x,y);
		button->SetSize(width,height);
		button->SetHotKey(hot_key);
		button->is_default_=is_default;
		return button;
	}

	void YYUTDialog::AddControl(shared_ptr<YYUTControl> &control)
	{
		InitControl(control);
		controls_.push_back(control);

	}

	void YYUTDialog::InitControl(shared_ptr<YYUTControl>& control)
	{
		control->OnInit();
	}

	void YYUTDialog::InitControl(shared_ptr<YYUTControl>& control,YYUTElement const & ele)
	{
		control->OnInit();
	}

	shared_ptr<YYUTStatic> YYUTDialog::GetStatic(int ID)
	{
		for_each(controls_.begin(),controls_.end(),[&](shared_ptr<YYUTControl> &sp)
		{

		});

		return nullptr;
	}

	shared_ptr<YYUTButton> YYUTDialog::GetButton(int ID)
	{
		auto pos=std::find_if(controls_.begin(),controls_.end(),
			[&](shared_ptr<YYUTControl> &sp){
				return (sp->GetID()==ID	&&sp->GetType()==YYUT_CONTROL_BUTTON);
		});
		if(pos!=controls_.end())
			return dynamic_pointer_cast<YYUTButton>(*pos);
		return nullptr;
	}

	shared_ptr<YYUTControl> YYUTDialog::GetControl(int ID)
	{
		auto pos=std::find_if(controls_.begin(),controls_.end(),
			[&](shared_ptr<YYUTControl> &sp){
				return sp->GetID()==ID;	
		});
		if(pos!=controls_.end())
			return *pos;
		return nullptr;
	}

	shared_ptr<YYUTControl> YYUTDialog::GetControl(int ID,UINT control_type)
	{
		auto pos=std::find_if(controls_.begin(),controls_.end(),
			[&](shared_ptr<YYUTControl> &sp){
				return sp->GetID()==ID && sp->GetType()==control_type;	
		});
		if(pos!=controls_.end())
			return *pos;
		return nullptr;
	}

	shared_ptr<YYUTControl> YYUTDialog::GetControlAtPoint(POINT pt)
	{
		auto pos=std::find_if(controls_.begin(),controls_.end(),[&](shared_ptr<YYUTControl> & sp)
		{
			return sp->ContainPoint(pt) && sp->GetEnable() && sp->GetVisible();
		});
		if(pos!=controls_.end())
			return *pos;
		return nullptr;
	}

	bool YYUTDialog::GetControlEnable(int ID)
	{
		shared_ptr<YYUTControl> sp=GetControl(ID);
		if(sp==nullptr)
			return false;
		return sp->GetEnable();
	}

	void YYUTDialog::SetControlEnable(int ID,bool enable)
	{
		shared_ptr<YYUTControl> sp=GetControl(ID);
		if(sp)
			sp->SetEnable(enable);
	}

	
	void YYUTDialog::SetElemt(string element_id, const YYUTElement &element)
	{
		element_map_[element_id]=element;
	}

	

	void YYUTDialog::SendEvent(UINT event,bool trigger_by_user,shared_ptr<YYUTControl> control)
	{
		if(!trigger_by_user && !non_user_events_)
			return;
		if(gui_event_)
			gui_event_(event,control->GetID(),control);
	}

	void YYUTDialog::RequestFocus(shared_ptr<YYUTControl> &control)
	{
		if(control_focus_==control)
			return ;
		if(!control->CanHaveFocus())
			return;
		if(control_focus_)
			control_focus_->OnFocusOut();
		control->OnFocusIn();
		control_focus_=control;
	}

	void YYUTDialog::DrawRect(RECT *rect,D3DCOLOR color)
	{
		RECT rc_screen=*rect;
		OffsetRect(&rc_screen,x_,y_);
	}

	void YYUTDialog::DrawPolyLine(PINT * points,UINT number,D3DCOLOR color)
	{

	}
	inline int RectWidth(RECT &rc)
	{
		return rc.right-rc.left;
	}
	inline int RectHeight(RECT &rc)
	{
		return rc.bottom-rc.top;
	}
	void YYUTDialog::DrawSprite(YYUTElement &elemet,RECT &prc_dest)
	{
		//pre_dest是相对坐标
		RECT rc_texture=elemet.rect_texture_;
		//屏幕绝对坐标
		RECT rc_screen=prc_dest;
		OffsetRect(&rc_screen,x_,y_);
		shared_ptr<YYUTTextureNode> sp_text_node=GetTexture(elemet.texture_id_);
		float scale_x=(float)RectWidth(rc_screen)/RectWidth(rc_texture);
		float scale_y=(float)RectHeight(rc_screen)/RectHeight(rc_texture);
		D3DXMATRIXA16 mat_scalar;
		D3DXMatrixScaling(&mat_scalar,scale_x,scale_y,1.0f);
		D3DXMATRIXA16 mat_translation;
		D3DXMatrixTranslation(&mat_translation,(float)rc_screen.left,(float)rc_screen.top,1.0f);
		mat_scalar*=mat_translation;
		manager_->sprite_->SetTransform(&mat_scalar);
		manager_->sprite_->Draw(sp_text_node->texture_,&rc_texture,NULL,NULL,elemet.texture_color_.current_);

	}

	void YYUTDialog::DrawSprite(YYUTElement &elemet,RECT &prc_dest,float angle)
	{
		RECT rc_texture=elemet.rect_texture_;
		//屏幕绝对坐标
		RECT rc_screen=prc_dest;
		OffsetRect(&rc_screen,x_,y_);
		float width=(float)RectWidth(rc_screen);
		float height=(float)RectHeight(rc_screen);
		shared_ptr<YYUTTextureNode> sp_text_node=GetTexture(elemet.texture_id_);
		float scale_x=(float)RectWidth(rc_screen)/RectWidth(rc_texture);
		float scale_y=(float)RectHeight(rc_screen)/RectHeight(rc_texture);
		D3DXMATRIXA16 mat_scalar;
		D3DXMatrixScaling(&mat_scalar,scale_x,scale_y,1.0f);
		D3DXMATRIXA16 mat_to_origin;
		D3DXMatrixTranslation(&mat_to_origin,-width/2,-height/2,0);
		D3DXMATRIXA16 mat_rotate;
		D3DXMatrixRotationZ(&mat_rotate,angle);
		D3DXMATRIXA16 mat_back_to_position;
		D3DXMatrixTranslation(&mat_back_to_position,(float)rc_screen.left+width/2,(float)rc_screen.top+height/2,1);
		mat_scalar*=mat_to_origin*mat_rotate*mat_back_to_position;
		mat_scalar._43=1.0f;
		mat_scalar._44=1.0f;
		manager_->sprite_->SetTransform(&mat_scalar);
		manager_->sprite_->Draw(sp_text_node->texture_,&rc_texture,NULL,NULL,elemet.texture_color_.current_);
	}

	void YYUTDialog::CalcTextRect(wstring text,shared_ptr<YYUTElement> element,RECT *prc_dest,bool shadow/*=false*/,int count/*=-1*/)
	{
		HRESULT hr=S_OK;
	}

	void YYUTDialog::RemoveControl(int ID)
	{

	}

	void YYUTDialog::RemoveAllControl()
	{

	}

	void YYUTDialog::Refresh()
	{

	}
	struct YYUT_SCREEN_VERTEX
	{
		float x,y,z,h;
		D3DCOLOR color;
		float tu,tv;
		static DWORD FVF;
	};
	DWORD YYUT_SCREEN_VERTEX::FVF=D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	struct YYUT_SCREEN_VERTEX_UNTEX
	{
		float x,y,z,h;
		D3DCOLOR color;
		static DWORD FVF;
	};
	DWORD YYUT_SCREEN_VERTEX_UNTEX::FVF=D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	void YYUTDialog::OnRender(float elapsed_time)
		try{
			HRESULT hr;
			assert(manager_->GetD3D9Device() && manager_->state_block_);
			CComPtr<IDirect3DDevice9> com_d3ddevice=manager_->GetD3D9Device();
			CComPtr<IDirect3DStateBlock9> com_state_block=manager_->state_block_;
			//过一段时间刷新一下，刷新掉默认焦点
			if(last_time_refresh_ < time_refresh_)
			{
				last_time_refresh_=YYUTTimer::GetInstance().GetTime();
				Refresh();
			}
			CComPtr<IDirect3DDevice9> d3d_device=manager_->GetD3D9Device();
			//用来存储state_block,过会用来还原statae
			manager_->state_block_->Capture();
			d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			d3d_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			d3d_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			d3d_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE);
			d3d_device->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
			d3d_device->SetRenderState(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|
				D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED );
			d3d_device->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
			d3d_device->SetRenderState(D3DRS_FOGENABLE,FALSE);
			d3d_device->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			d3d_device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
			d3d_device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		
		
			d3d_device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
			d3d_device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
			d3d_device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
			d3d_device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
			d3d_device->SetTextureStageState(0,D3DTSS_RESULTARG,D3DTA_CURRENT);
			d3d_device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
			d3d_device->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
			d3d_device->SetRenderState(D3DRS_ZENABLE,FALSE);
			BOOL back_ground_is_visible=(color_top_left_ | color_top_right_ | color_bottom_left_ | 
				color_bottom_right_) &0xff0000;
			//back_ground_is_visible=true;
			if( back_ground_is_visible)
			{
				YYUT_SCREEN_VERTEX_UNTEX vertices[4]=
				{
					(float)x_,(float)y_,1.0f,1.0f,color_top_left_,
					(float)x_+width_,(float)y_,1.0f,1.0f,color_top_right_,
					(float)x_+width_,(float)y_+height_,1.0f,1.0f,color_bottom_right_,
					(float)x_,(float)y_+height_,1.0f,1.0f,color_bottom_left_,
				};
				d3d_device->SetVertexShader(NULL);
				d3d_device->SetPixelShader(NULL);
				d3d_device->SetFVF(YYUT_SCREEN_VERTEX_UNTEX::FVF);
				d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertices,sizeof(YYUT_SCREEN_VERTEX_UNTEX));
			}
			/*d3d_device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
			d3d_device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
			d3d_device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
			d3d_device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			d3d_device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
			d3d_device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
			d3d_device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);	*/
			
			hr=	manager_->sprite_->Begin(D3DXSPRITE_DONOTSAVESTATE|D3DXSPRITE_ALPHABLEND);
			if(FAILED(hr))
				BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_hr(hr));
			//d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			//d3d_device->SetRenderState(D3DRS_ALPHAREF,0x00000000);//240
			//d3d_device->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
				for_each(controls_.begin(),controls_.end(),[&](shared_ptr<YYUTControl> & sp_control)
				{
						sp_control->Render(elapsed_time);
				});
			manager_->sprite_->End();

			manager_->state_block_->Apply();
	}
	catch(YYUTGUIException &e)
	{
		string *msg=boost::get_error_info<err_str>(e);
		if(msg)
		{
			(*msg)+="\n!!throw from YYUTDdialog render!!";
		}
		else
			e<<err_str("!!throw from YYUTDdialog render!!");
		throw;
	}
	//这个index是相对于YYUTDialog而言的，对于Manger来说只用一个shared_ptr
	void YYUTDialog::SetFont(string name_id,wstring face_name,long height,long weight)
	{
		assert(manager_!=nullptr && L"Try to call YYUTDialog::Init first");
		font_map_.insert(make_pair<string,shared_ptr<YYUTFontNode>>
			(std::move(name_id),manager_->AddFont(face_name,height,weight)))	;
	}

	void YYUTDialog::SetTexture(string name_id,wstring file_name)
	{
		assert(manager_!=nullptr && L"Try to call YYUTDialog::Init first");
		shared_ptr<YYUTTextureNode> sp=manager_->AddTexture(file_name);
		texture_map_[name_id]=sp;
	}

	void YYUTDialog::ClearFocus()
	{
		if(control_focus_)
		{
			control_focus_->OnFocusOut();
			control_focus_=nullptr;
		}
		::ReleaseCapture();
	}

	void YYUTDialog::FucusDefaultControl()
	{

	}

	YYUTDialog::YYUTDialog()
	{
#if defined( DEBUG ) || defined( _DEBUG )
		std::cout<<"[YYUTDialog] Create!"<<std::endl;
#endif // _DEBUG
		x_=0;
		y_=0;
		width_=0;
		height_=0;
		visible_=true;
		be_pressed_=false;
		color_top_left_=0;
		color_top_right_=0;
		color_bottom_left_=0;
		color_bottom_right_=0;
		last_time_refresh_=0;
		default_control_id_=0xFFFF;
		non_user_events_=false;
		manager_=nullptr;
		font_scalar_=1.0f;
	}

	void YYUTDialog::Init(YYUTDialogResourceManager *manager,bool register_dialog/*=true*/)
	{
		manager_=manager;
		hwnd_=manager_->GetHWND();
		InitDefaultElemets();
	}

	void YYUTDialog::Init(YYUTDialogResourceManager* manager,bool register_dialog,wstring contorl_texture_file_name)
	{

	}
	void YYUTDialog::InitDefaultElemets()
	{
		//SetFont(0);
		SetFont("default_font",L"nasalization rg");
		SetTexture("bt_default",_T("..\\media\\UI\\default.jpg"));
		SetTexture("bt_move",_T("..\\media\\UI\\move.jpg"));
		SetTexture("bt_press",_T("..\\media\\UI\\press.jpg"));
		SetTexture("big_circle",_T("..\\media\\UI\\big_circle.png"));
		SetTexture("small_circle",_T("..\\media\\UI\\small_circle.png"));
		SetTexture("back_color",_T("..\\media\\UI\\back_color.png"));
		//SetTexture("as_fps_big_cicle")
		YYUTElement element;
		RECT texture_rc;
		element.SetFont("default_font");
		element.font_color_.state_[YYUT_STATE_DISABLE]=D3DCOLOR_ARGB(125,255,255,255);
		element.font_color_.state_[YYUT_STATE_NORMAL]=D3DCOLOR_ARGB(230,255,255,255);
		element.font_color_.state_[YYUT_STATE_FOCUS]=D3DCOLOR_ARGB(230,255,255,255);
		element.font_color_.state_[YYUT_STATE_HIDDEN]=D3DCOLOR_ARGB(120,255,255,255);
		element.font_color_.state_[YYUT_STATE_MOUSEOVER]=D3DCOLOR_ARGB(230,255,255,255);
		element.font_color_.state_[YYUT_STATE_PRESSED]=D3DCOLOR_ARGB(255,255,255,255);
		element.texture_color_.state_[YYUT_STATE_DISABLE]=D3DCOLOR_ARGB(125,255,255,255);
		element.texture_color_.state_[YYUT_STATE_NORMAL]=D3DCOLOR_ARGB(230,255,255,255);
		element.texture_color_.state_[YYUT_STATE_FOCUS]=D3DCOLOR_ARGB(230,255,255,255);
		element.texture_color_.state_[YYUT_STATE_HIDDEN]=D3DCOLOR_ARGB(120,255,255,255);
		element.texture_color_.state_[YYUT_STATE_MOUSEOVER]=D3DCOLOR_ARGB(230,255,255,255);
		element.texture_color_.state_[YYUT_STATE_PRESSED]=D3DCOLOR_ARGB(255,255,255,255);
		//YYUTButton
		::SetRect(&texture_rc,0,0,256,256);
		element.SetTexture("bt_default",&texture_rc);
		element.SetFont("default_font");
		SetElemt("button_up_elem",element);

		::SetRect(&texture_rc, 0, 0, 256, 256 );
		element.SetTexture("bt_press",&texture_rc,D3DCOLOR_ARGB(0,255,255,255));
		SetElemt("button_down_elem",element);

		::SetRect(&texture_rc, 0, 0, 256, 256 );
		element.SetTexture("bt_move",&texture_rc,D3DCOLOR_ARGB(0,255,255,255));
		SetElemt("button_move_elem",element);
		::SetRect(&texture_rc,0,0,1024,1024);
		element.SetTexture("back_color",&texture_rc,D3DCOLOR_ARGB(255,255,255,255));
		SetElemt("as_back_elem",element);
		element.SetTexture("big_circle",&texture_rc,D3DCOLOR_ARGB(255,255,255,255));
		SetElemt("as_big_circle",element);
		element.SetTexture("small_circle",&texture_rc,D3DCOLOR_ARGB(255,255,255,255));
		SetElemt("as_small_circle",element);
	}
	shared_ptr<YYUTDialog> YYUTDialog::MakeDialog()
	{
		//不能用make_shared<>，因constructor is protected
		return shared_ptr<YYUTDialog>(new YYUTDialog);
	}

	

	YYUTDialog::~YYUTDialog()
	{
		cout<<"[YYUTDialog] destroy!"<<endl;
	}

	void YYUTDialog::DrawText(wstring text,YYUTElement & element,RECT &prc_dest,bool shadow,int count/*=-1*/)
	{
		if(element.font_color_.current_.a==0)
			return;
		RECT rc_screen=prc_dest;
		OffsetRect(&rc_screen,x_,y_);
		D3DXMATRIX mat_transform;
		D3DXMatrixIdentity(&mat_transform);
		manager_->sprite_->SetTransform(&mat_transform);
		shared_ptr<YYUTFontNode> sp_font=GetFont(element.font_id_);
		sp_font->font_->DrawText(manager_->sprite_,text.c_str(),text.size(),&rc_screen,element.text_format_,element.font_color_.current_);
	}

	bool YYUTDialog::ProcessMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		bool handled=false;
		if(!visible_)
			return false;
		//YYUTDialog自身对鼠标的响应
		if(WM_LBUTTONUP && be_pressed_)
		{
			POINT mouse_point={short(LOWORD(lParam)),short(HIWORD(lParam))};
			if(mouse_point.x>=x_	&& mouse_point.x<x_+width_ &&
				mouse_point.y>=y_   && mouse_point.y<y_+height_)
			{
				ReleaseCapture();
				be_pressed_=false;
				return true;
			}
		}
		
		switch(uMsg)
		{
		case WM_SIZE:
		case WM_MOVE:
			{
				break;
			}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			{
				if(control_focus_)
				{
					if(shared_from_this()==control_focus_->dialog_.lock() && control_focus_->GetEnable())
					{
						if(control_focus_->HandleKeyboard(uMsg,wParam,lParam))
							return true;
					}
				}
				break;
			}
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_XBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
			{
				POINT mouse_point={short(LOWORD(lParam)),short(HIWORD(lParam))};
				mouse_point.x-=x_;
				mouse_point.y-=y_;
				if(control_focus_)
				{
					if(shared_from_this()==control_focus_->dialog_.lock() && control_focus_->GetEnable())
					{
						if(control_focus_->HandleMouse(uMsg,mouse_point,wParam,lParam))
							return true;
					}
				}
				shared_ptr<YYUTControl> control_todo=GetControlAtPoint(mouse_point);
				if(control_todo!=nullptr && control_todo->GetEnable())
				{
					handled=control_todo->HandleMouse(uMsg,mouse_point,wParam,lParam);
					if(handled)
						return true;
				}
				else
				{
					//在dialog上的控件外
					if(uMsg==WM_LBUTTONDOWN) 
					{
						if(control_focus_ && shared_from_this()==control_focus_->dialog_.lock())
						{
							control_focus_->OnFocusOut();
							control_focus_=nullptr;
						}
					}
				}
				if(uMsg==WM_MOUSEMOVE)
				{
					OnMouseMove(mouse_point);
					return false;
				}
				break;
			}
		case WM_CAPTURECHANGED:
			{
				if((HWND)lParam !=hwnd_)
				{
					be_pressed_=false;
				}
			}
		}
		return false;
	}

	void YYUTDialog::OnMOuseUp(POINT pt)
	{
		control_mouse_over_=nullptr;
	}

	void YYUTDialog::OnMouseMove(POINT pt)
	{
		shared_ptr<YYUTControl> sp=GetControlAtPoint(pt);
		if(sp==control_mouse_over_)
			return;
		if(control_mouse_over_)
			control_mouse_over_->OnMouseLeave();
		control_mouse_over_=sp;
		if(sp!=nullptr)
			control_mouse_over_->OnMouseEnter();
	}

	void YYUTDialog::SetBackgroundColor(D3DCOLOR top_left,D3DCOLOR top_right,D3DCOLOR bottom_left,D3DCOLOR bottom_right)
	{
		color_top_left_=top_left;
		color_top_right_=top_right;
		color_bottom_left_=bottom_left;
		color_bottom_right_=bottom_right;
	}

	void YYUTDialog::OnRender_imp(float elapsed_time)
	{

	}

	bool YYUTDialog::OnCycleFocus(bool forward)
	{
		return false;
	}

	shared_ptr<YYUTTextureNode> YYUTDialog::GetTexture(string name)
	{
		auto pos=texture_map_.find(name);
		if(pos==texture_map_.end())
		{
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str(string("can't find ")+name+"texture resource"));
		}
		return pos->second;
	}

	shared_ptr<YYUTFontNode> YYUTDialog::GetFont(string index)
	{
		auto pos=font_map_.find(index);
		if(pos==font_map_.end())
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("can't find "+index+" font resource"));
		return pos->second;
	}


	void YYUTDialog::SetLocation(float x,float y)
	{
		x_=static_cast<int>(ceilf(manager_->GetWidth()*x));
		y_=static_cast<int>(ceilf(manager_->GetHeigth()*y));
	}

	void YYUTDialog::SetSize(float width,float height)
	{
		width_=static_cast<int>(ceil(width*manager_->GetWidth()));
		height_=static_cast<int>(ceil(height*manager_->GetHeigth()));
	}

	YYUTElement & YYUTDialog::GetDefaultElem(string element_id)
	{
		auto pos=element_map_.find(element_id);
		if(pos==element_map_.end())
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("can't find the element_id:"+element_id));
		return pos->second;
	}

	void YYUTDialog::SetFont(string id,float scalar)
	{
		font_scalar_=scalar;
		int font_height=static_cast<int>(height_*font_scalar_);
		if(font_map_.find(id)==font_map_.end())
		{
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("can't find the ID:"+id+" font"));
		}
		auto font_exsit=font_map_[id];
		auto font_node=manager_->AddFont(font_exsit->face_,font_height);
		font_map_[id]=font_node;
	}

	shared_ptr<YYUTAnimationStatic> YYUTDialog::AddStaticAnimate(int ID)
	{
		shared_ptr<YYUTAnimationStatic> animate_static=YYUTAnimationStatic::MakeInstance(shared_from_this());
		AddControl(static_pointer_cast<YYUTControl>(animate_static));
		animate_static->SetID(ID);
		return animate_static;
	}

	shared_ptr<YYUTControl> YYUTDialog::control_focus_;

	double YYUTDialog::time_refresh_=0.0f;


	void YYUTControl::Refresh()
	{

	}

	void YYUTControl::SetTextColor(D3DCOLOR color)
	{

	}

	YYUTControl::~YYUTControl()
	{

	}

	void YYUTControl::UpdateRect()
	{
		SetRect(&bounding_box_,x_,y_,x_+width_,y_+height_);
	}

	YYUTControl::YYUTControl(weak_ptr<YYUTDialog> dialog/*=nullptr*/)
	{
		dialog_=dialog;
		type_=YYUT_CONTROL_BUTTON;
		ID_=0;
		index_=0;
		enable_=true;
		visible_=true;
		mouse_over_=false;
		has_focus_=false;
		is_default_=false;
		x_=0;
		y_=0;
		width_=0;
		height_=0;
		ZeroMemory(&bounding_box_,sizeof(bounding_box_));
	}

	void YYUTControl::SetEvent(CONTROL_EVENT e)
	{
		control_event_=e;
	}

	YYUT::YYUTElement YYUTControl::GetElemet(string ele_id)
	try	{
		auto dialog=dialog_.lock();
		if(dialog==nullptr)
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("can't get the dialog"));
		 return dialog->GetElemMap().at(ele_id);
	}
	catch(std::out_of_range &e)
	{
		BOOST_THROW_EXCEPTION(boost::enable_error_info(e)<<err_str("can't find the element"));
	}

	void YYUTControl::SetLocation(float x,float y)
	{
		auto dialog=dialog_.lock();
		if(dialog)
		{
			x_=static_cast<int>(ceilf(x*dialog->GetWidth()));
			y_=static_cast<int>(ceilf(y*dialog->Getheight()));
			UpdateRect();
		}
	}

	void YYUTControl::SetSize(float width,float height)
	{
		auto dialog=dialog_.lock();
		if(dialog)
		{
			width_=static_cast<int>(ceilf(width*dialog->GetWidth()));
			height_=static_cast<int>(ceilf(height*dialog->Getheight()));
			UpdateRect();
		}
	}

	void YYUTControl::ResetPosisionSize(float x,float y,float width,float height)
	{
		auto dialog=dialog_.lock();
		if(dialog)
		{
			x_=static_cast<int>(ceilf(x*dialog->GetWidth()));
			y_=static_cast<int>(ceilf(y*dialog->Getheight()));
			width_=static_cast<int>(ceilf(width*dialog->GetWidth()));
			height_=static_cast<int>(ceilf(height*dialog->Getheight()));
			UpdateRect();
		}
	}
	YYUTButton::YYUTButton(weak_ptr<YYUTDialog> dialog/*=nullptr*/):YYUTStatic(dialog)
	{
		type_=YYUT_CONTROL_BUTTON;
		pre_state=YYUT_STATE_MOUSEOVER;
		pressed_=false;
		hot_key_=0;
		memset(&rc_window,0,sizeof(rc_window));
		right_side=0.0f;
	}

	bool YYUTButton::HandleKeyboard(UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		if(!enable_ || !visible_)
			return false;
		switch(uMsg)
		{
		case WM_KEYDOWN:
			{
				switch(wParam)
				{
				case VK_SPACE:
					pressed_=true;
					return true;
				}
			}
			break;
		case WM_KEYUP:
			{
				switch(wParam)
				{
				case VK_SPACE:
					if(pressed_==true)
					{
						pressed_=false;

						shared_ptr<YYUTDialog> dialog=dialog_.lock();
						if(dialog)
							dialog->SendEvent(EVENT_BUTTON_CLICKED,true,shared_from_this());
						else
							BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("unexcept father dialog's life is over"));
					}
					return true;
				}
			}
			break;
		}
		return false;
	}

	bool YYUTButton::HandleMouse(UINT uMsg,POINT pt,WPARAM wParam,LPARAM lParam)
	{
		if(!enable_ ||!visible_)
			return false;
		switch(uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			{
				if(ContainPoint(pt))
				{
					pressed_=true;
					HWND tmp_hwnd;
					shared_ptr<YYUTDialog> dialog=dialog_.lock();
					if(dialog)
					{
						tmp_hwnd=dialog->GetHWND();	
						::SetCapture(tmp_hwnd);
						if(!has_focus_)
							dialog->RequestFocus(shared_from_this());
						return true;
					}
					else
						BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("unexcept father dialog's life is over"));

				}
				break;
			}
		case WM_LBUTTONUP:
			{
				if(pressed_)
				{
					pressed_=false;
					::ReleaseCapture();
					shared_ptr<YYUTDialog> dialog=dialog_.lock();
					if(dialog)
					{
							dialog->ClearFocus();
						if(ContainPoint(pt))
							{
								if(control_event_)
									control_event_();
							}
						return true;
					}
					else
						BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("unexcept father dialog's life is over"));
				}
				break;
			}
		}
		return false;
	}

	void YYUTButton::Render(float elapsed_time)
	{
		int offset_x=0;
		int offset_y=0;
		state=YYUT_STATE_NORMAL;
		if(visible_==false)
			state=YYUT_STATE_HIDDEN;
		else if(enable_==false)
			state=YYUT_STATE_DISABLE;
		else if(pressed_)
		{
			state=YYUT_STATE_PRESSED;
		}
		else if(mouse_over_)
		{
			state=YYUT_STATE_MOUSEOVER;
		}
		else if(has_focus_)
		{
			state=YYUT_STATE_FOCUS;
		}
		YYUTElement element_up=GetElemet("button_up_elem");
		YYUTElement element_move=GetElemet("button_move_elem");
		YYUTElement element_down=GetElemet("button_down_elem");
		shared_ptr<YYUTDialog> dialog=dialog_.lock();
		if(dialog)
		{
			if(pre_state!=state)
			{
				rc_window.left=bounding_box_.left;
				rc_window.right=bounding_box_.left;
				right_side=(float)bounding_box_.left;
				rc_window.top=bounding_box_.top;
				rc_window.bottom=bounding_box_.bottom;
			}
			else
			{
				rc_window.left=bounding_box_.left;
				right_side+=static_cast<float>(width_)*elapsed_time*5;
				rc_window.right=(long)right_side;
				rc_window.right=(std::min)(rc_window.right,bounding_box_.right);
				rc_window.top=bounding_box_.top;
				rc_window.bottom=bounding_box_.bottom;
			}
			switch(state)
			{
				case YYUT_STATE_NORMAL: 
					element_up.texture_color_.current_=D3DCOLOR_ARGB(200,255,255,255);
					dialog->DrawSprite(element_up,bounding_box_);
					element_up.font_color_.current_=element_up.font_color_.state_[YYUT_STATE_NORMAL];
					dialog->DrawText(text_,element_up,bounding_box_);
					break;
				case YYUT_STATE_FOCUS:
				case YYUT_STATE_MOUSEOVER:
					element_move.texture_color_.current_=D3DCOLOR_ARGB(200,255,255,255);
					dialog->DrawSprite(element_move,rc_window);
					element_move.font_color_.current_=element_move.font_color_.state_[YYUT_STATE_MOUSEOVER];
					dialog->DrawText(text_,element_move,bounding_box_);
					break;
				case YYUT_STATE_PRESSED:
					element_down.texture_color_.current_=D3DCOLOR_ARGB(255,255,255,255);
					dialog->DrawSprite(element_down,rc_window);
					element_down.font_color_.current_=element_down.font_color_.state_[YYUT_STATE_PRESSED];
					dialog->DrawText(text_,element_down,bounding_box_);
					break;
				default:
					break;
			}
			//dialog->DrawSprite(element,&rc_window);
			//dialog->DrawText(text_,element,rc_window);
		}
		else
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("unexcept father dialog's life is over"));
		pre_state=state;
	}

	shared_ptr<YYUTButton> YYUTButton::MakeInstance(weak_ptr<YYUTDialog> p)
	{
		return make_shared<YYUTButton>(p);
	}


	YYUTStatic::YYUTStatic(weak_ptr<YYUTDialog> dialog):YYUTControl(dialog)
	{
		type_=YYUT_CONTROL_STATIC;
	}

	void YYUTStatic::Render(float elapsed_time)
	{
		if(visible_==false)
			return;
		YYUT_CONTROL_STATE state=YYUT_STATE_NORMAL;
		if(enable_==false)
			state=YYUT_STATE_DISABLE;
		YYUTElement element=GetElemet("static");
		element.font_color_.Blend(state,elapsed_time);
		shared_ptr<YYUTDialog> dialog=dialog_.lock();
		if(dialog)
		{
			dialog->DrawText(text_,element,bounding_box_,true);
		}
	}

	void YYUTStatic::SetText(wstring text)
	{
		text_=text;
	}


	YYUTAnimationStatic::YYUTAnimationStatic(weak_ptr<YYUTDialog> dialog):YYUTStatic(dialog)
	{
		radius_=0.0f;
		center_.x=0.0f;
		center_.y=0.0f;
		big_circle_angle=0.0f;
		small_circle_angle=0.0f;
	}

	bool YYUTAnimationStatic::HandleMouse(UINT uMsg,POINT pt,WPARAM wParam,LPARAM lParam)
	{
		if(!enable_ ||!visible_)
			return false;
		return false;
	}

	bool YYUTAnimationStatic::ContainPoint(POINT pt)
	{
		int delta_x=(int)center_.x-pt.x;
		int delta_y=(int)center_.y-pt.y;
		if(radius_*radius_>=delta_x*delta_x+delta_y*delta_y)
		{
			return true;
		}
		else	
			return false;
	}

	void YYUTAnimationStatic::Render(float elapsed_time)
	{
		state=YYUT_STATE_NORMAL;
		if(visible_==false)
			state=YYUT_STATE_HIDDEN;
		else if(enable_==false)
			state=YYUT_STATE_DISABLE;
		else if(mouse_over_)
		{
			state=YYUT_STATE_MOUSEOVER;
		}
		else if(has_focus_)
		{
			state=YYUT_STATE_FOCUS;
		}
		big_circle_angle+=elapsed_time*5;
		small_circle_angle-=elapsed_time;
		if(small_circle_angle<=-D3DX_PI*2)
			small_circle_angle=0.0f;
		if(big_circle_angle>=D3DX_PI*2)
			big_circle_angle=0.0f;
		YYUTElement element_animate=GetElemet("as_back_elem");
		YYUTElement element_big_circle=GetElemet("as_big_circle");
		YYUTElement element_small_circle=GetElemet("as_small_circle");
		element_animate.font_color_.current_=D3DCOLOR_ARGB(255,255,255,255);
		element_animate.texture_color_.current_=D3DCOLOR_ARGB(100,255,255,255);
		element_big_circle.texture_color_.current_=D3DCOLOR_ARGB(255,255,255,255);
		element_small_circle.texture_color_.current_=D3DCOLOR_ARGB(255,255,255,255);

		shared_ptr<YYUTDialog> dialog=dialog_.lock();
		if(dialog)
		{
			dialog->DrawSprite(element_animate,bounding_box_);
			dialog->DrawSprite(element_big_circle,bounding_box_,big_circle_angle);
			dialog->DrawSprite(element_small_circle,bounding_box_,small_circle_angle);
			dialog->DrawText(text_,element_animate,bounding_box_);	

		}
	}

	shared_ptr<YYUTAnimationStatic> YYUTAnimationStatic::MakeInstance(weak_ptr<YYUTDialog> p)
	{
		return make_shared<YYUTAnimationStatic>(p);
	}

}

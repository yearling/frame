#include "stdafx.h"
#include "YYUT.h"
#include "YYUTGUI.h"
#include "YYUTres.h"
#include <algorithm>
#include <boost\throw_exception.hpp>
#include <iostream>
#include "YYUTTimer.h"
using std::cout;
namespace YYUT
{

	YYUTDialogResourceManager::YYUTDialogResourceManager()
	{
#ifdef _DEBUG
		cout<<"[YYUTDialogResourceManager] Create!"<<endl;
#endif // _DEBUG

		back_buffer_width_=back_buffer_height_=0;

	}

	YYUTDialogResourceManager::~YYUTDialogResourceManager()
	{
#ifdef _DEBUG
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
#ifdef _DEBUG
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
			size_t len=face_name.length();
			if(0==_wcsnicmp(p->face_.c_str(),face_name.c_str(),len) &&
				p->height_==height && p->weight_==weight)
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
			size_t len=0;
			len=resource_name.length();
			if(p->file_source_&&0==_wcsnicmp(p->file_name_.c_str(),resource_name.c_str(),len))
				return true;
			return false;
		});
		if(pos!=vec_texture_.end())
			return *pos;
		shared_ptr<YYUTTextureNode> new_texture_node(new YYUTTextureNode);
		new_texture_node->file_source_=true;
		new_texture_node->file_name_=resource_name;
		vec_texture_.push_back(new_texture_node);
		if(d3d_device_)
			CreateTexture(new_texture_node);
		return new_texture_node;
	}

	shared_ptr<YYUTTextureNode> YYUTDialogResourceManager::AddTexture(wstring file_name,int source_id,HMODULE module)
	{
		auto pos=find_if(vec_texture_.begin(),vec_texture_.end(),[&](shared_ptr<YYUTTextureNode> p)->bool{
			if(!p->file_source_ && p->resource_module_==module)
			{
				if(file_name.empty())
				{
					if(source_id==p->resource_id_)
						return true;
				}
				else
				{
					size_t len=0;
					len=file_name.length();
					if(0==_wcsnicmp(p->file_name_.c_str(),file_name.c_str(),len))
						return true;
				}
			}
			return false;
		});
		if(pos!=vec_texture_.end())
			return *pos;
		shared_ptr<YYUTTextureNode> new_texture_node(new YYUTTextureNode);
		new_texture_node->resource_module_=module;
		if(file_name.empty())
		{
			new_texture_node->resource_id_=source_id;
			new_texture_node->file_name_=_T("");
		}
		else
		{
			new_texture_node->resource_id_=0;
			new_texture_node->file_name_=file_name;
		}
		vec_texture_.push_back(new_texture_node);
		if(d3d_device_)
			CreateTexture(new_texture_node);
		return new_texture_node;
	}

	void YYUTDialogResourceManager::RegisterDialog(shared_ptr<YYUTDialog>& dialog)
	{
		dialogs_.insert(dialog);
	}

	void YYUTDialogResourceManager::UnRegisterDiaglog(shared_ptr<YYUTDialog>& dialog)
	{
		dialogs_.erase(dialog);
	}

	void YYUTDialogResourceManager::EnableKeyboardInputForAllDialogs()
	{
		for_each(dialogs_.begin(),dialogs_.end(),std::bind(&YYUTDialog::EnableKeyBoardInput,placeholders::_1,true));
	}
	//throw YYUTGUIException();
	void YYUTDialogResourceManager::CreateFont(std::shared_ptr<YYUTFontNode> &font)
	{
		HRESULT hr;
		font->font_=nullptr;
		hr=D3DXCreateFont(d3d_device_,font->height_,0,font->weight_,1,FALSE,DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,font->face_.c_str(),&font->font_);
		if(FAILED(hr))
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_hr(hr)<<err_str("D3DXCreateFont Failed!"));
	}

	void YYUTDialogResourceManager::CreateTexture(std::shared_ptr<YYUTTextureNode> texture_node)
		try
	{
		HRESULT hr=S_OK;
		D3DXIMAGE_INFO image_info;
		if(!texture_node->file_source_)
		{
			if(texture_node->resource_id_==0xFFFF && texture_node->resource_module_==(HMODULE)0xFFFF)
			{
				YYUTCreateGUITextureFromInternalArray(d3d_device_,&texture_node->texture_,&image_info);
			}
			else
			{
				LPCWSTR id=texture_node->resource_id_?(LPCWSTR)(size_t)texture_node->resource_id_:
					texture_node->file_name_.c_str();
			hr=D3DXCreateTextureFromResourceEx(d3d_device_,texture_node->resource_module_,id,D3DX_DEFAULT,
				D3DX_DEFAULT,1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0,&image_info,nullptr,&texture_node->texture_);
			if(FAILED(hr))
				BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_hr(hr)<<err_str("D3DXCreateTextureFromResourceEx failed!"));
			}
		}
		else
		{
			if(texture_node->file_name_.empty())
				return ;
			hr=D3DXCreateTextureFromFileEx( d3d_device_, texture_node->file_name_.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
				1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
				D3DX_DEFAULT, D3DX_DEFAULT, 0,
				&image_info, NULL, &texture_node->texture_);
			if(FAILED(hr))
				BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_hr(hr)<<err_str("D3DXCreateTextureFromFileEx failed!"));
		}
		texture_node->width_=image_info.Width;
		texture_node->heigth_=image_info.Height;
	}
	catch(YYUTException& e)
	{
		if(!texture_node->file_source_)
			e<<err_str("Create GUI Texture failed! : load frome internal arry or frome resource");
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


	YYUTTextureNode::YYUTTextureNode():file_source_(false),
		resource_module_(NULL),
		resource_id_(0),
		width_(0),
		heigth_(0)
	{
	}


	void YYUTBlendColor::Init(D3DCOLOR default_color,D3DCOLOR disablecolor/*=D3DCOLOR_ARGB(200,128,128,128)*/,D3DCOLOR hidden_color/*=0*/)
	{
		//for_each(&state_[0],&state_[MAX_CONTROL_STATE],[&](D3DCOLOR &c){c=default_color;});
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
		index_texture_=index_texture;
		if(prc_textrue)
			rect_texture_=*prc_textrue;
		else
			SetRectEmpty(&rect_texture_);
		texture_color_.Init(default_texture_color);
	}

	void YYUTElement::SetFont(string index_font,D3DCOLOR default_font_color/*=D3DCOLOR_ARGB(255,255,255,255)*/,DWORD text_format/*=DT_CENTER| DT_VCENTER*/)
	{
		index_font_=index_font;
		text_format_=text_format;
		font_color_.Init(default_font_color);
	}

	void YYUTElement::Refresh()
	{
		texture_color_.current_=texture_color_.state_[YYUT_STATE_HIDDEN];
		font_color_.current_=font_color_.state_[YYUT_STATE_HIDDEN];
	}

	YYUTElement::YYUTElement():
	index_font_(""),
		index_texture_(""),
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
		control->index_=controls_.size();
		for_each(element_holder_map.begin(),element_holder_map.end(),[&](const YYUTElementHolder & hold){
			if(hold.control_type_==control->GetType())
				control->SetElement(hold.element_index_,&hold.element_);
		});
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

	void YYUTDialog::SetDefaultElemt(UINT control_type,UINT index_element,YYUTElement *element)
	{
		YYUTElementHolder tmp;
		tmp.control_type_=control_type;
		tmp.element_index_=index_element;
		tmp.element_=*element;
		auto pos=element_holder_map.find(tmp);
		if(pos!=element_holder_map.end())
		{
			element_holder_map.erase(pos);
		}
		element_holder_map.insert(tmp);
	}

	const YYUTElement * YYUTDialog::GetDefaultElement(UINT control_type,UINT index_element)
	{
		auto pos=std::find_if(element_holder_map.begin(),element_holder_map.end(),[&](const YYUTElementHolder & holder)
		{
			return holder.control_type_==control_type &&holder.element_index_==index_element;
		});
		if(pos!=element_holder_map.end())
			return &pos->element_;
		return nullptr;
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
		if(caption_enable_)
			OffsetRect(&rc_screen,0,caption_height_);

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
	void YYUTDialog::DrawSprite(shared_ptr<YYUTElement> &elemet,RECT* prc_dest)
	{
		if(elemet->texture_color_.current_.a==0)
			return ;
		RECT rc_texture=elemet->rect_texture_;
		RECT rc_screen=*prc_dest;
		OffsetRect(&rc_screen,x_,y_);
		if(caption_enable_)
			OffsetRect(&rc_screen,0,caption_height_);
		shared_ptr<YYUTTextureNode> sp_text_node=GetTexture(elemet->index_texture_);
		float scale_x=(float)RectWidth(rc_screen)/RectWidth(rc_texture);
		float scale_y=(float)RectHeight(rc_screen)/RectHeight(rc_texture);
		D3DXMATRIXA16 mat_tranform;
		D3DXMatrixScaling(&mat_tranform,scale_x,scale_y,1.0f);
		manager_->sprite_->SetTransform(&mat_tranform);
		D3DXVECTOR3 pos((float)rc_screen.left,(float)rc_screen.top,0.0f);
		pos.x/=scale_x;
		pos.y/=scale_y;
		manager_->sprite_->Draw(sp_text_node->texture_,&rc_texture,NULL,&pos,elemet->texture_color_.current_);

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
			assert(manager_->GetD3D9Device() && manager_->state_block_);
			CComPtr<IDirect3DDevice9> com_d3ddevice=manager_->GetD3D9Device();
			CComPtr<IDirect3DStateBlock9> com_state_block=manager_->state_block_;
			//过一段时间刷新一下，刷新掉默认焦点
			if(last_time_refresh_ < time_refresh_)
			{
				last_time_refresh_=YYUTTimer::GetInstance().GetTime();
				Refresh();
			}
			if(!visible_ || (minimized_ && !caption_enable_))
				return ;
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
			BOOL back_ground_is_visible=(color_top_left_ | color_top_right_ | color_bottom_left_ | 
				color_bottom_right_) &0xff0000;
			if(!minimized_ && back_ground_is_visible)
			{
				YYUT_SCREEN_VERTEX_UNTEX vertices[4]=
				{
					(float)x_,(float)y_,0.5f,1.0f,color_top_left_,
					(float)x_+width_,(float)y_,0.5f,1.0f,color_top_right_,
					(float)x_+width_,(float)y_+height_,0.5f,1.0f,color_bottom_right_,
					(float)x_,(float)y_+height_,0.5f,1.0f,color_bottom_left_,
				};
				d3d_device->SetVertexShader(NULL);
				d3d_device->SetPixelShader(NULL);
				d3d_device->SetRenderState(D3DRS_ZENABLE,FALSE);
				d3d_device->SetFVF(YYUT_SCREEN_VERTEX_UNTEX::FVF);
				d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertices,sizeof(YYUT_SCREEN_VERTEX_UNTEX));
			}
			d3d_device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
			d3d_device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
			d3d_device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
			d3d_device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			d3d_device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
			d3d_device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
			d3d_device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
			shared_ptr<YYUTTextureNode> texture_node=GetTexture("default_texture");
			d3d_device->SetTexture(0,texture_node->texture_);
			manager_->sprite_->Begin(D3DXSPRITE_DONOTSAVESTATE);
			if(caption_enable_)
			{
				RECT rc={0,-caption_height_,width_,0};
				DrawSprite(cap_element_,&rc);
				rc.left+=5;
				if(minimized_)
					caption_text_+=L"(Minimized)";
				DrawText(caption_text_,cap_element_,rc,true);
			}
			if(!minimized_)
			{
				for_each(controls_.begin(),controls_.end(),[&](shared_ptr<YYUTControl> & sp_control)
				{
					if(sp_control!=control_focus_)
						sp_control->Render(elapsed_time);
				});
				if(control_focus_&& control_focus_->dialog_.lock()==shared_from_this())
					control_focus_->Render(elapsed_time);
			}
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
			(name_id,manager_->AddFont(face_name,height,weight)))	;
	}

	void YYUTDialog::SetTexture(string name_id,wstring file_name)
	{
		assert(manager_!=nullptr && L"Try to call YYUTDialog::Init first");
		shared_ptr<YYUTTextureNode> sp=manager_->AddTexture(file_name);
		texture_map_[name_id]=sp;
	}

	void YYUTDialog::SetTexture(string name_id,wstring file_name,int source_id,HMODULE resource_module)
	{
		assert(manager_!=nullptr && L"Try to call YYUTDialog::Init first");
		shared_ptr<YYUTTextureNode> sp=manager_->AddTexture(file_name,source_id,resource_module);
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
#ifdef _DEBUG
		std::cout<<"[YYUTDialog] Create!"<<std::endl;
#endif // _DEBUG
		x_=0;
		y_=0;
		width_=0;
		height_=0;
		visible_=true;
		caption_enable_=false;
		minimized_=false;
		drag_=false;
		caption_height_=18;
		color_top_left_=0;
		color_top_right_=0;
		color_bottom_left_=0;
		color_bottom_right_=0;
		last_time_refresh_=0;
		default_control_id_=0xFFFF;
		non_user_events_=false;
		keyboard_input_=false;
		mouse_input_=true;
		manager_=nullptr;
		cap_element_=make_shared<YYUTElement>();
	}

	void YYUTDialog::Init(YYUTDialogResourceManager *manager,bool register_dialog/*=true*/)
	{
		manager_=manager;
		if(register_dialog)
			manager_->RegisterDialog(shared_from_this());
		hwnd_=manager_->GetHWND();
		SetTexture("default_texture",_T(""),0xffff,(HMODULE)0xFFFF);
		SetTexture("desert",_T("Desert.jpg"));
		InitDefaultElemets();
	}

	void YYUTDialog::Init(YYUTDialogResourceManager* manager,bool register_dialog,wstring contorl_texture_file_name)
	{

	}
	void YYUTDialog::InitDefaultElemets()
	{
		//SetFont(0);
		SetFont("default_font",L"Arial",14,FW_NORMAL);
		YYUTElement element;
		RECT texture_rc;
		assert(cap_element_ && L"cap_element should not empty");
		cap_element_->SetFont("default_font");
		::SetRect(&texture_rc,17,269,241,287);
		cap_element_->SetTexture("default_texture",&texture_rc);
		cap_element_->texture_color_.state_[YYUT_STATE_NORMAL]=D3DCOLOR_ARGB(255,255,255,255);
		cap_element_->font_color_.state_[YYUT_STATE_NORMAL]=D3DCOLOR_ARGB(255,255,255,255);
		cap_element_->SetFont("default_font",D3DCOLOR_ARGB(255,255,255,255),DT_LEFT|DT_VCENTER);
		cap_element_->texture_color_.Blend(YYUT_STATE_NORMAL,10.0f);
		cap_element_->font_color_.Blend(YYUT_STATE_NORMAL,10.0f);
		//YYUTSTATIC
		element.SetFont("default_font");
		element.font_color_.state_[YYUT_STATE_DISABLE]=D3DCOLOR_ARGB(200,200,200,200);
		SetDefaultElemt(YYUT_CONTROL_STATIC,0,&element);
		//YYUTButton
		::SetRect(&texture_rc,0,0,136,54);
		element.SetTexture("default_texture",&texture_rc);
		element.SetFont("default_font");
		element.texture_color_.state_[YYUT_STATE_NORMAL]=D3DCOLOR_ARGB(150,255,255,255);
		element.texture_color_.state_[YYUT_STATE_PRESSED]=D3DCOLOR_ARGB(200,255,255,255);
		element.font_color_.state_[YYUT_STATE_MOUSEOVER]=D3DCOLOR_ARGB(255,0,0,0);
		SetDefaultElemt(YYUT_CONTROL_BUTTON,0,&element);

		::SetRect(&texture_rc, 136, 0, 252, 54 );
		element.SetTexture("default_texture",&texture_rc,D3DCOLOR_ARGB(0,255,255,255));
		element.texture_color_.state_[YYUT_STATE_NORMAL]=D3DCOLOR_ARGB(160,255,255,255);
		element.texture_color_.state_[YYUT_STATE_PRESSED]=D3DCOLOR_ARGB(60,0,0,0);
		element.font_color_.state_[YYUT_STATE_MOUSEOVER]=D3DCOLOR_ARGB(30,255,255,255);
		SetDefaultElemt(YYUT_CONTROL_BUTTON,1,&element);
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

	void YYUTDialog::DrawText(wstring text,shared_ptr<YYUTElement>& element,RECT &prc_dest,bool shadow,int count/*=-1*/)
	{
		if(element->font_color_.current_.a==0)
			return;
		RECT rc_screen=prc_dest;
		OffsetRect(&rc_screen,x_,y_);
		if(caption_enable_)
			OffsetRect(&rc_screen,0,caption_height_);
		D3DXMATRIX mat_transform;
		D3DXMatrixIdentity(&mat_transform);
		manager_->sprite_->SetTransform(&mat_transform);
		shared_ptr<YYUTFontNode> sp_font=GetFont(element->index_font_);
		if(shadow)
		{
			RECT rc_shadow=rc_screen;
			OffsetRect(&rc_shadow,1,1);
			sp_font->font_->DrawText(manager_->sprite_,text.c_str(),text.size(),&rc_shadow,element->text_format_,
				D3DCOLOR_ARGB(DWORD(element->font_color_.current_.a*255),0,0,0));
		}
		sp_font->font_->DrawText(manager_->sprite_,text.c_str(),text.size(),&rc_screen,element->text_format_,element->font_color_.current_);
	}

	bool YYUTDialog::ProcessMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		bool handled=false;
		if(!visible_)
			return false;
		/*if(WM_LBUTTONDOWN==uMsg|| WM_LBUTTONDBLCLK ==uMsg||WM_KEYDOWN==uMsg)
		{
			manager_->EnableKeyboardInputForAllDialogs();
		}*/
		if(caption_enable_)
		{
			if(WM_LBUTTONDOWN ==uMsg || WM_LBUTTONDBLCLK==uMsg)
			{
				POINT mouse_point={short(LOWORD(lParam)),short(HIWORD(lParam))};
				if(mouse_point.x>=x_	&& mouse_point.x<x_+width_ &&
					mouse_point.y>=y_   && mouse_point.y<y_+caption_height_)
				{
					drag_=true;
					SetCapture(hwnd_);
					return true;
				}
			}
		}
		else if(WM_LBUTTONUP && drag_)
		{
			POINT mouse_point={short(LOWORD(lParam)),short(HIWORD(lParam))};
			if(mouse_point.x>=x_	&& mouse_point.x<x_+width_ &&
				mouse_point.y>=y_   && mouse_point.y<y_+caption_height_)
			{
				ReleaseCapture();
				drag_=false;
				minimized_=!minimized_;
				return true;
			}
		}
		if(minimized_)
			return false;
		if(control_focus_)
		{
			if(control_focus_->dialog_.lock()==shared_from_this() && control_focus_->GetEnable())
			{
				if(control_focus_->MsgProc(uMsg,wParam,lParam))
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
				if(!mouse_input_)
					return false;
				POINT mouse_point={short(LOWORD(lParam)),short(HIWORD(lParam))};
				mouse_point.x-=x_;
				mouse_point.y-=y_;
				if(caption_enable_)
					mouse_point.y-=caption_height_;
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
					drag_=false;
				}
			}
		}
		return false;
	}

	void YYUTDialog::OnMOuseUp(POINT pt)
	{
		control_pressed_=nullptr;
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

	shared_ptr<YYUTControl> YYUTDialog::control_pressed_;

	shared_ptr<YYUTControl> YYUTDialog::control_focus_;

	double YYUTDialog::time_refresh_=0.0f;


	void YYUTControl::Refresh()
	{

	}

	void YYUTControl::SetTextColor(D3DCOLOR color)
	{

	}

	void YYUTControl::SetElement(UINT index,const YYUTElement * element)
	{
		assert(element);
		if(element==nullptr)
			BOOST_THROW_EXCEPTION(std::invalid_argument("element is nullptr"));
		elements_[index]=make_shared<YYUTElement>(*element);
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


	bool YYUTElementHolder::operator ==(const YYUTElementHolder &p) const
	{
		return control_type_==p.control_type_ && element_index_==p.element_index_;
	}

	YYUTButton::YYUTButton(weak_ptr<YYUTDialog> dialog/*=nullptr*/):YYUTStatic(dialog)
	{
		type_=YYUT_CONTROL_BUTTON;
		pressed_=false;
		hot_key_=0;
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
						if(!dialog->keyboard_input_)
							dialog->ClearFocus();
						if(ContainPoint(pt))
							{
								//dialog->SendEvent(EVENT_BUTTON_CLICKED,true,shared_from_this());
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
		YYUT_CONTROL_STATE state=YYUT_STATE_NORMAL;
		if(visible_==false)
			state=YYUT_STATE_HIDDEN;
		else if(enable_==false)
			state=YYUT_STATE_DISABLE;
		else if(pressed_)
		{
			state=YYUT_STATE_PRESSED;
			offset_x=1;
			offset_y=2;
		}
		else if(mouse_over_)
		{
			state=YYUT_STATE_MOUSEOVER;
			offset_x=-1;
			offset_y=-2;
		}
		else if(has_focus_)
		{
			state=YYUT_STATE_FOCUS;
		}
		shared_ptr<YYUTElement> element=GetElemet(0);
		float blend_rate=(state==YYUT_STATE_PRESSED)?0.0f:0.5f;
		RECT rc_window=bounding_box_;
		OffsetRect(&rc_window,offset_x,offset_y);

		element->texture_color_.Blend(state,elapsed_time,blend_rate);
		element->font_color_.Blend(state,elapsed_time,blend_rate);
		shared_ptr<YYUTDialog> dialog=dialog_.lock();
		if(dialog)
		{
			dialog->DrawSprite(element,&rc_window);
			dialog->DrawText(text_,element,rc_window);
			element=GetElemet(1);
			element->texture_color_.Blend(state,elapsed_time,blend_rate);
			element->font_color_.Blend(state,elapsed_time,blend_rate);
			dialog->DrawSprite(element,&rc_window);
			//dialog->DrawText(text_,element,rc_window);
		}
		else
			BOOST_THROW_EXCEPTION(YYUTGUIException()<<err_str("unexcept father dialog's life is over"));

	}

	shared_ptr<YYUTButton> YYUTButton::MakeInstance(weak_ptr<YYUTDialog> p)
	{
		return make_shared<YYUTButton>(p);
	}


	YYUTStatic::YYUTStatic(weak_ptr<YYUTDialog> dialog):YYUTControl(dialog)
	{
		type_=YYUT_CONTROL_STATIC;
		elements_.clear();
	}

	void YYUTStatic::Render(float elapsed_time)
	{
		if(visible_==false)
			return;
		YYUT_CONTROL_STATE state=YYUT_STATE_NORMAL;
		if(enable_==false)
			state=YYUT_STATE_DISABLE;
		shared_ptr<YYUTElement> element=GetElemet(0);
		element->font_color_.Blend(state,elapsed_time);
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

}

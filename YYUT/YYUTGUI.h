#ifndef	__YYUTGUI__
#define __YYUTGUI__
#pragma  once
#include "stdafx.h"
#include "YYUT.h"
#include <memory>
#include <vector>
#include <unordered_set>
#include <atlcomcli.h>
#include "YYUTenum.h"
#include <boost\exception\errinfo_file_name.hpp>
#include <boost\noncopyable.hpp>
#include <xfunctional>
#include <functional>
using namespace std;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::set;
using std::unordered_set;
#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_EDITBOX_STRING                0x0601
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703
namespace YYUT
{
	struct YYUTGUIException:virtual YYUTException{};
	class	YYUTDialogResourceManager;
	class   YYUTControl;
	class   YYUTButton;
	class   YYUTStatic;
	struct YYUTTextureNode
	{
		YYUTTextureNode();
		bool file_source_;
		HMODULE resource_module_;
		int resource_id_;
		wstring file_name_;
		long width_;
		long heigth_;
		CComPtr<IDirect3DTexture9> texture_;
	};
	struct YYUTFontNode
	{
		wstring face_;
		long height_;
		long weight_;
		CComPtr<ID3DXFont> font_;
		YYUTFontNode();
	};
	enum YYUT_CONTROL_TYPE
	{
		YYUT_CONTROL_BUTTON,
		YYUT_CONTROL_STATIC,
	};
	enum YYUT_CONTROL_STATE
	{
		YYUT_STATE_NORMAL=0,
		YYUT_STATE_DISABLE,
		YYUT_STATE_HIDDEN,
		YYUT_STATE_FOCUS,
		YYUT_STATE_MOUSEOVER,
		YYUT_STATE_PRESSED, 
	};
	const int MAX_CONTROL_STATE=6;

	struct YYUTBlendColor 
	{
		YYUTBlendColor();
		void Init(D3DCOLOR default_color,D3DCOLOR disablecolor=D3DCOLOR_ARGB(200,128,128,128),D3DCOLOR hidden_color=0);
		void Blend(UINT state,float elapsed_time,float rate=0.7f);
		D3DCOLOR state_[MAX_CONTROL_STATE];
		D3DXCOLOR current_;
	};
	class YYUTElement
	{
	public:
		YYUTElement();
		void SetTexture(string index_texture,RECT *prc_textrue,D3DCOLOR default_texture_color=D3DCOLOR_ARGB(255,255,255,255));
		void SetFont(string index_font,D3DCOLOR default_font_color=D3DCOLOR_ARGB(255,255,255,255),DWORD text_format=DT_CENTER| DT_VCENTER);
		void Refresh();
		string index_texture_;
		string index_font_;
		DWORD text_format_;
		RECT rect_texture_;
		YYUTBlendColor texture_color_;
		YYUTBlendColor font_color_;
	};
	struct YYUTElementHolder
	{
		UINT control_type_;
		UINT element_index_;
		YYUTElement element_;
		bool operator==(const YYUTElementHolder &p) const;		
	};
	class YYUTDialog:public std::enable_shared_from_this<YYUTDialog>,boost::noncopyable
	{
		friend class YYUTDialogResourceManager;
		typedef std::function<void(UINT,int,shared_ptr<YYUTControl>)> FUNCTION_GUI_EVENT;
	public:
		
		static shared_ptr<YYUTDialog> MakeDialog();
		void Init(YYUTDialogResourceManager* manager,bool register_dialog=true);
		void Init(YYUTDialogResourceManager* manager,bool register_dialog,wstring contorl_texture_file_name);
		void Init(YYUTDialogResourceManager* manager,bool register_dialog,wstring control_texture_file_name,
			HMODULE control_texture_resource_module);
		bool ProcessMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		void AddStatic(int ID,wstring text,int x,int y,int width,int height,bool is_default=false,YYUTStatic **create=nullptr);
		shared_ptr<YYUTButton> AddButton(int ID,wstring text,int x,int y,int width,int height,UINT hot_key=0U,bool is_default=false);
		void AddControl(shared_ptr<YYUTControl> &control);
		void InitControl(shared_ptr<YYUTControl>& control);
		shared_ptr<YYUTStatic> GetStatic(int ID);
		shared_ptr<YYUTButton> GetButton(int ID);
		shared_ptr<YYUTControl> GetControl(int ID);
		shared_ptr<YYUTControl> GetControl(int ID,UINT control_type);
		shared_ptr<YYUTControl> GetControlAtPoint(POINT pt);
		bool  GetControlEnable(int ID);
		void  SetControlEnable(int ID,bool enable);
		void  SetDefaultElemt(UINT control_type,UINT index_element,YYUTElement *element);
		const YYUTElement *GetDefaultElement(UINT control_type,UINT index_element);
		void SendEvent(UINT event,bool trigger_by_user,shared_ptr<YYUTControl> control);
		void RequestFocus(shared_ptr<YYUTControl> &control);
		void DrawRect(RECT *rect,D3DCOLOR color);
		void DrawPolyLine(PINT * points,UINT number,D3DCOLOR color);
		void DrawSprite(shared_ptr<YYUTElement> &elemet,RECT* prc_dest);
		void DrawText(wstring text,shared_ptr<YYUTElement>& element,RECT &prc_dest,bool shadow =false,int count=-1);
		void CalcTextRect(wstring text,shared_ptr<YYUTElement> element,RECT *prc_dest,bool shadow=false,int count=-1);
		bool GetVisible() { return visible_;}
		void SetVisible(bool visible) {visible_=visible;}
		bool GetMinimized() {return minimized_;}
		void SetMinimized(bool minimized) {minimized_=minimized;}
		void SetBackgroundColor(D3DCOLOR color_all_corners)
		{
			SetBackgroundColor(color_all_corners,color_all_corners,color_all_corners,color_all_corners);
		}
		void SetBackgroundColor(D3DCOLOR top_left,D3DCOLOR top_right,D3DCOLOR bottom_left,D3DCOLOR bottom_right);
		void EnableCaption(bool enable) {caption_enable_=enable;}
		int GetCaptionHeight() const {return caption_height_;}
		void SetCaptionHeight(int height){ height_=height;}
		void SetCaptionText(wstring text){ caption_text_=text;}
		void GetLocation(POINT &pt) const { pt.x=x_;pt.y=y_;}
		void SetLocation(int x,int y) {x_=x;y_=y;}
		void SetSize(int width,int height){width_=width;height_=height;}
		static void SetRefreshTime(float time) { time_refresh_=time;}
		void RemoveControl(int ID);
		void RemoveAllControl();
		void EnableNonUserEvents(bool enable){non_user_events_=enable; }
		void EnableKeyBoardInput(bool enable){keyboard_input_=enable;}
		void EnableMouseInput(bool enable) {mouse_input_=enable;}
		bool IsKeyboardInputEnable() const {return keyboard_input_;}
		bool non_user_events_;
		bool keyboard_input_;
		bool mouse_input_;
		void Refresh();
		void OnRender(float elapsed_time);
		void SetFont(string name_id,wstring face_name,long height,long weight);
		void SetTexture(string  name_id,wstring file_name);
		void SetTexture(string  name_id,wstring file_name,int source_id,HMODULE resource_module);
		shared_ptr<YYUTTextureNode> GetTexture(string index);
		shared_ptr<YYUTFontNode> GetFont(string index);
		YYUTDialogResourceManager* GetManager(){ return manager_;}
		static void ClearFocus();
		void FucusDefaultControl();
		~YYUTDialog();
		void SetGUIEvent(FUNCTION_GUI_EVENT &eve){gui_event_=eve;}
		inline HWND GetHWND(){return hwnd_;}
		
	protected:
		YYUTDialog();
		FUNCTION_GUI_EVENT gui_event_;
	private:
		
		int default_control_id_;
		void OnRender_imp(float elapsed_time);
		static double time_refresh_;
		double last_time_refresh_;
		void InitDefaultElemets();
		void OnMouseMove(POINT pt);
		void OnMOuseUp(POINT pt);
		bool OnCycleFocus(bool forward);
		static shared_ptr<YYUTControl> control_focus_;
		static shared_ptr<YYUTControl> control_pressed_;
		shared_ptr<YYUTControl>		control_mouse_over_;
		bool visible_;
		bool caption_enable_;
		bool minimized_;
		bool drag_;
		wstring caption_text_;
		int x_;
		int y_;
		int width_;
		int height_;
		int caption_height_;
		D3DCOLOR color_top_left_;
		D3DCOLOR color_top_right_;
		D3DCOLOR color_bottom_left_;
		D3DCOLOR color_bottom_right_;
		shared_ptr<YYUTElement> cap_element_;
		YYUTDialogResourceManager *manager_;
		vector<shared_ptr<YYUTControl>> controls_;	
		std::map<string,shared_ptr<YYUTFontNode>> font_map_;
		std::map<string,shared_ptr<YYUTTextureNode>> texture_map_;
		std::unordered_set<YYUTElementHolder> element_holder_map;
		HWND hwnd_;
	};
	class YYUTDialogResourceManager:boost::noncopyable
	{
	public:
		static YYUTDialogResourceManager* GetInstance(){ return &instance_;}
		bool MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		void OnD3DCreateDevice(LPDIRECT3DDEVICE9 d3d_device);
		void OnD3DResetDevice();
		void OnD3DLostDevice();
		void OnD3DDestroyDevice();
		void SetHWND(HWND hwnd);
		inline HWND GetHWND(){ return hwnd_;}
		CComPtr<IDirect3DDevice9> GetD3D9Device() const { return d3d_device_;}
		//std::shared_ptr<YYUTFontNode>  GetFontNode(int index) const{ return vec_font_.at(index); }
		//std::shared_ptr<YYUTTextureNode> GetTextureNode(int index) const{ return vec_texture_.at(index);}
		shared_ptr<YYUTFontNode> AddFont(wstring face_name,long height,long weight);
		shared_ptr<YYUTTextureNode> AddTexture(wstring file_name);
		shared_ptr<YYUTTextureNode> AddTexture(wstring file_name,int sourec_id,HMODULE module);
		void RegisterDialog(shared_ptr<YYUTDialog>& dialog);
		void UnRegisterDiaglog(shared_ptr<YYUTDialog>& dialog);
		void EnableKeyboardInputForAllDialogs();
	public:
		CComPtr<IDirect3DStateBlock9> state_block_;
		//IDirect3DStateBlock9* state_block_;
		CComPtr<ID3DXSprite> sprite_;
		unsigned int back_buffer_width_;
		unsigned int back_buffer_height_;
		set<std::shared_ptr<YYUTDialog> > dialogs_;
	protected:
		YYUTDialogResourceManager();
		~YYUTDialogResourceManager();
		void CreateFont(std::shared_ptr<YYUTFontNode> &);
		void CreateTexture(std::shared_ptr<YYUTTextureNode>);
		vector<std::shared_ptr<YYUTFontNode>> vec_font_;
		vector<std::shared_ptr<YYUTTextureNode>> vec_texture_;
		CComPtr<IDirect3DDevice9>  d3d_device_;
	private:
		static YYUTDialogResourceManager instance_;
		HWND hwnd_;
	};
	class YYUTControl:public enable_shared_from_this<YYUTControl>
	{
	public:
		typedef	std::function<void()> CONTROL_EVENT;
		YYUTControl();
		YYUTControl(weak_ptr<YYUTDialog> dialog);
		
		virtual ~YYUTControl();
		virtual void OnInit(){};
		virtual void Refresh();
		virtual void Render(float elapsed_time){};
		virtual bool MsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam) {return false;}
		virtual bool HandleKeyboard(UINT uMsg,WPARAM wParam,LPARAM lParam) {return false;}
		virtual bool HandleMouse(UINT uMsg,POINT pt,WPARAM wParam,LPARAM lParam) {return false;}
		virtual bool CanHaveFocus(){return false;}
		virtual void OnFocusIn(){has_focus_=true;}
		virtual void OnFocusOut(){ has_focus_=false;}
		virtual void OnMouseEnter(){mouse_over_=true;}
		virtual void OnMouseLeave(){mouse_over_=false;}
		virtual void OnHotKey(){}
		virtual bool ContainPoint(POINT pt){return !!PtInRect(&bounding_box_,pt);}
		virtual void SetEnable(bool enable){enable_=enable;}
		virtual bool GetEnable(){return enable_;}
		virtual void SetVisible(bool visible){visible_=visible;}
		virtual bool GetVisible(){return visible_;}
		UINT GetType() const {return type_;}
		int GetID() const {return ID_;}
		void SetID(int id){ID_=id;}
		void SetLocation(int x,int y){x_=x;y_=y;UpdateRect();}
		void SetSize(int width,int height){width_=width;height_=height;UpdateRect();}
		void SetHotKey(UINT hotKey){hot_key_=hotKey;}
		UINT GetHotKey(){return hot_key_;}
		virtual void SetTextColor(D3DCOLOR color);
		shared_ptr<YYUTElement> GetElemet(UINT i){return elements_.at(i);}
		void SetElement(UINT index,const YYUTElement * element);
		bool visible_;
		bool mouse_over_;
		bool has_focus_;
		bool is_default_;
		int x_;
		int y_;
		int width_;
		int height_;
		weak_ptr<YYUTDialog> dialog_;
		UINT index_;
		//vector<shared_ptr<YYUTElement>> elements_;
		map<UINT,shared_ptr<YYUTElement>> elements_;
		void SetEvent( CONTROL_EVENT );
	protected:
		CONTROL_EVENT control_event_;
		virtual void UpdateRect();
		int ID_;
		YYUT_CONTROL_TYPE type_;
		UINT hot_key_;
		bool enable_;
		RECT bounding_box_;
		
	};
	class YYUTStatic:public YYUTControl
	{
	public:
		YYUTStatic (weak_ptr<YYUTDialog> dialog);
		virtual void Render(float elapsed_time);
		virtual bool ContainPoint(POINT pt){return false;}
		wstring GetText() {return text_;}
		void SetText(wstring text);
	protected:
		wstring text_;
	};
	class YYUTButton:public YYUTStatic
	{
	public:
		YYUTButton(weak_ptr<YYUTDialog> dialog);
		virtual bool HandleKeyboard(UINT uMsg,WPARAM wParam,LPARAM lParam);
		virtual bool HandleMouse(UINT uMsg,POINT pt,WPARAM wParam,LPARAM lParam);
		virtual void OnHotKey()
		{
			shared_ptr<YYUTDialog> dialog=dialog_.lock();
			if(dialog)
			{
				if(dialog->IsKeyboardInputEnable())
				dialog->RequestFocus(shared_from_this());
				dialog->SendEvent(EVENT_BUTTON_CLICKED,true,shared_from_this());
			}
		}
		virtual bool ContainPoint(POINT pt)
		{
			return !!PtInRect(&bounding_box_,pt);
		}
		virtual bool CanHaveFocus()
		{
			return (visible_ && enable_);
		}
		virtual void Render(float elapsed_time);
		static shared_ptr<YYUTButton> MakeInstance(weak_ptr<YYUTDialog> p);
	protected:
		bool pressed_;
	};
}
namespace std
{
	template<>
	size_t std::hash<YYUT::YYUTElementHolder>::operator()(const YYUT::YYUTElementHolder & p) const
	{
		return std::hash<int>()(p.control_type_+p.element_index_+(UINT)p.element_.index_font_.length());
	}
}
#endif
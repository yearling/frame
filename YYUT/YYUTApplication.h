#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include <boost\exception\detail\error_info_impl.hpp>
#include <concrt.h>
namespace YYUT
{
	struct YYUTWidnowException:virtual YYUTException
	{
		YYUTWidnowException():std::exception("[YYUTWindow]:create window failed!")
		{
		}
	};
	class YYUTWindow
	{
	public:
		typedef boost::error_info< struct tag_window_err,DWORD> WindowCreateError;
		virtual ~YYUTWindow(){};
		YYUTWindow(HINSTANCE hinstance):hwnd(NULL),main_instance(hinstance),is_windowed(true){};
		void Init(int _width,int Height,const TCHAR *);
		operator HWND(){ return hwnd;}
	public:
		HINSTANCE main_instance;
		HWND hwnd;
		int width;
		int height;
		bool is_windowed;
	};
	class YYUTApplication
	{
	public:
		friend YYUTWindow;
		YYUTApplication(void);
		virtual ~YYUTApplication(void);
		virtual void WindowCreate(int width,int height,const TCHAR *,int=SW_SHOW);
		virtual void Initial();
		virtual int Run();
		virtual void Exit();
	protected:
		static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
		virtual LRESULT MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		void * operator new(size_t){};
		void * operator new[](size_t){};
		void operator delete(void *){}; 
		void operator delete[](void *){}; 

	public:
		std::shared_ptr<YYUTWindow> main_window;
		HINSTANCE main_instance;
		
	};

	extern YYUTApplication *YYUT_application;
	YYUTApplication *GetApplication();
}
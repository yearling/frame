#include "stdafx.h"
#include <assert.h>
#include <Windows.h>
#include <windowsx.h>
#include "YYUTApplication.h"
namespace YYUT
{
	YYUTApplication *YYUT_application;

	YYUTApplication::YYUTApplication(void)
	{
		YYUT_application=this;
	}


	YYUTApplication::~YYUTApplication(void)
	{
	}

	void YYUTApplication::WindowCreate(int width,int height,const TCHAR *title,int cmdshow)
	{
		assert(main_window);
		try
		{
			main_window->Init(width,height,title);
			ShowWindow(*main_window,cmdshow);
			UpdateWindow(*main_window);
		}
		catch(YYUTWidnowException &e)
		{
			UNREFERENCED_PARAMETER(e);
			throw;
		}
	}

	void YYUTApplication::Initial()
	{
		main_window.reset(new YYUTWindow(main_instance));
	}

	int YYUTApplication::Run()
	{
		MSG msg;
		while (1)
		{
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				if(WM_QUIT == msg.message)
					return static_cast<int>(msg.wParam);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			DWORD current_time=timeGetTime();
		}
		return static_cast<int>(msg.wParam);
	}

	void YYUTApplication::Exit()
	{
	}


	LRESULT CALLBACK YYUTApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		YYUTApplication *pApp=GetApplication();
		return pApp->MyProc(hWnd,message,wParam,lParam);
	}

	LRESULT YYUTApplication::MyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int wmId, wmEvent;
		PAINTSTRUCT ps;
		HDC hdc;

		switch (message)
		{
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此添加任意绘图代码...
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	YYUTApplication * GetApplication()
	{
		return YYUT_application;
	}
	//throw an exception 
	typedef boost::error_info<struct tag_err_wstring,wstring> err_wstr;
	void YYUTWindow::Init(int _width,int Height,const TCHAR *window_name)
	{
		DWORD $err_hr;
		width=_width;
		height=Height;
		WNDCLASSEX wcex;
		TCHAR _Regstr[]={_T("YYWINDOW")};
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= YYUTApplication::WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= main_instance;
		wcex.hIcon			=  LoadIcon(NULL,IDI_APPLICATION);
		wcex.hCursor		=  LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	=	(HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	=	 nullptr;
		wcex.lpszClassName	=	_Regstr;
		wcex.hIconSm		=	 LoadIcon(NULL, IDI_APPLICATION);
		$err_hr=RegisterClassEx(&wcex);
		hwnd=CreateWindow(_Regstr,window_name, WS_CAPTION|WS_SIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, main_instance, NULL);
		$err_hr=GetLastError();
		if(hwnd)
		{
			if (is_windowed)
			{

				RECT window_rect = {0,0,width-1,height-1};
				// make the call to adjust window_rect
				::AdjustWindowRectEx(&window_rect,GetWindowStyle(hwnd),GetMenu(hwnd) != NULL,GetWindowExStyle(hwnd));
				MoveWindow(hwnd,100, // x position
					50, // y position
					window_rect.right - window_rect.left, // width
					window_rect.bottom - window_rect.top, // height
					FALSE);
			}
		}
		else
			BOOST_THROW_EXCEPTION(YYUTWidnowException()<<YYUTWindow::WindowCreateError($err_hr));
	}

}

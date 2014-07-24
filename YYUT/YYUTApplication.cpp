#include "stdafx.h"
#include "YYUTApplication.h"
#include <assert.h>
#include <Windows.h>
#include <windowsx.h>
YYUTApplication *YYUT_application;

YYUTApplication::YYUTApplication(void)
{
	YYUT_application=this;
}


YYUTApplication::~YYUTApplication(void)
{
}

bool YYUTApplication::WindowCreate(int width,int height,const TCHAR *title,int cmdshow)
{
	assert(main_window);
	if(main_window->Init(width,height,title))
	{
		ShowWindow(*main_window,cmdshow);
		UpdateWindow(*main_window);
	}
	return true;
}

bool YYUTApplication::Initial()
{
	main_window.reset(new yyWindow(main_instance));
	return true;
}

int YYUTApplication::Run()
{
	MSG msg;
    DWORD last_time=timeGetTime();
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
		last_time=current_time;
	}
	return static_cast<int>(msg.wParam);
}

int YYUTApplication::Exit()
{

	return 0;
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

bool yyWindow::Init(int _width,int _height,const TCHAR *title)
{
	width=_width;
	height=_height;
	WNDCLASSEX wcex;
	TCHAR _Regstr[]={_T("YYWINDOW")};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= YYUTApplication::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= main_instance;
	wcex.hIcon			=  LoadIcon(NULL,IDI_APPLICATION);
	wcex.hCursor		=	LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	=	(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	=	 nullptr;
	wcex.lpszClassName	=	_Regstr;
	wcex.hIconSm		=	 LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wcex);
	hwnd=CreateWindow(_Regstr,title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, main_instance, NULL);
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
		return true;
	}
	else 
		return false;
}


#include "stdafx.h"
#include "MyApp.h"
bool MyAPP::Initial()
{
	YYUTApplication::Initial();
	return true;
}



int MyAPP::Exit()
{
	return 1;
}
MyAPP::~MyAPP()
{

}

MyAPP::MyAPP()
{
	
}
LRESULT MyAPP::MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
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
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if ( wParam == VK_ESCAPE )
			::DestroyWindow(hWnd);
		break;
	case WM_LBUTTONDOWN:

		// compute the ray in view space given the clicked screen point
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool MyAPP::GameInit()
{
	YYUT::YYSetConsole();
	
	if(!InitResource())
		return false;
	return true;
}

void MyAPP::GameMain(DWORD timespan)
{
	
}
void MyAPP::GameExit()
{
}

bool MyAPP::InitResource()
{
	
	return true;
}


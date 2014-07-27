#ifndef	__YYUTGUI__
#define __YYUTGUI__
#pragma  once
#include "stdafx.h"
class YYUTDialogResourceManager
{
public:
	YYUTDialogResourceManager();
	~YYUTDialogResourceManager();
	bool MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

};
#endif
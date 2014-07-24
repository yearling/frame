#pragma once
#include "stdafx.h"
class yyWindow
{
public:
	virtual ~yyWindow(){};
	yyWindow(HINSTANCE hinstance):hwnd(NULL),main_instance(hinstance),is_windowed(true){};
	bool Init(int _width,int Height,const TCHAR *);
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
	friend yyWindow;
	YYUTApplication(void);
	virtual ~YYUTApplication(void);
	virtual bool WindowCreate(int width,int height,const TCHAR *,int=SW_SHOW);
	virtual bool  Initial();
	virtual int Run();
	virtual int Exit();

protected:
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	virtual LRESULT MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	void * operator new(size_t){};
	void * operator new[](size_t){};
	void operator delete(void *){}; 
	void operator delete[](void *){}; 

public:
	std::shared_ptr<yyWindow> main_window;
	HINSTANCE main_instance;
};

extern YYUTApplication *YYUT_application;
YYUTApplication *GetApplication();
#include "stdafx.h"
#include "MyApp.h"
MyAPP theApp;
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	int result=0;
	YYUTApplication* yy_app=GetApplication();
	try {
		yy_app->Initial();
	yy_app->WindowCreate(1920,1080,_T("my first window"),nCmdShow);
	if(!yy_app->GameInit())
		return -1;
	result=yy_app->Run();
	yy_app->GameExit();
	yy_app->Exit();
	}
	catch(yyd3dException &e)
	{
		//cout<<*boost::get_error_info<boost::throw_file>(e)<<endl;
	}
	return result;
}


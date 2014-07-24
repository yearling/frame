#include "stdafx.h"
#include "YYGame.h"
YYGame theApp;
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
		if(!yy_app->Initial())
			return -1;
	result=yy_app->Run();
	yy_app->Exit();
	}
	catch(yyd3dException &e)
	{
		//cout<<*boost::get_error_info<boost::throw_file>(e)<<endl;
		e.what();
	}
	return result;
}


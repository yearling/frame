#include "stdafx.h"
#include "YYGame.h"
#include <boost\exception\diagnostic_information.hpp>
#include <iostream>
using namespace YYUT;
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
		yy_app->Initial();
		result=yy_app->Run();
		yy_app->Exit();
	}
	catch(YYUTException &e)
	{
		UNREFERENCED_PARAMETER(e);
		cout<<"catched in main"<<endl;
		std::cout<<boost::current_exception_diagnostic_information();
		system("pause");
	}
	system("pause");
	return result;
}


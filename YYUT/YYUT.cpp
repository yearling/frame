#include "stdafx.h"
#include "YYUT.h"
#include <assert.h>
LRESULT YYUTManager::MyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void YYUTManager::GameExit()
{

}

bool YYUTManager::GameInit()
{
	YYUT::YYSetConsole();
	return true;
}

void YYUTManager::GameMain(DWORD timespan)
{

}

int YYUTManager::Exit()
{
	return 0;
}

bool YYUTManager::Initial()
{
	YYUTApplication::Initial();
	//�� �������\���п��������\���ɷ�������\ʹ���̸�����ʹ��
	//����ճ�ͼ�,ճ�ͼ�������������ctrl+alt+delʱ������Ҫ����һ�鰴��������һ��һ���İ����У�����ͬʱ��
	STICKYKEYS sk = {sizeof( STICKYKEYS ), 0};
	SystemParametersInfo( SPI_GETSTICKYKEYS, sizeof( STICKYKEYS ), &sk, 0 );
	SetStartupStickyKeys( sk );
	//�����л������л��������ǣ���Capslk��ʱ���һ��
	TOGGLEKEYS tk = {sizeof( TOGGLEKEYS ), 0};
	SystemParametersInfo( SPI_GETTOGGLEKEYS, sizeof( TOGGLEKEYS ), &tk, 0 );
	SetStartupToggleKeys( tk );
	//����ɸѡ��,�����л�����������ʱ������
	FILTERKEYS fk = {sizeof( FILTERKEYS ), 0};
	SystemParametersInfo( SPI_GETFILTERKEYS, sizeof( FILTERKEYS ), &fk, 0 );
	SetStartupFilterKeys( fk );
	//������ֹ��DPI��ʱ������⡣
	HINSTANCE hUser32 = LoadLibrary( L"user32.dll" );
	if( hUser32 )
	{
		typedef BOOL ( WINAPI* LPSetProcessDPIAware )( void );
		LPSetProcessDPIAware pSetProcessDPIAware = ( LPSetProcessDPIAware )GetProcAddress( hUser32,
			"SetProcessDPIAware" );
		if( pSetProcessDPIAware )
		{
			pSetProcessDPIAware();
		}
		FreeLibrary( hUser32 );
	}

	return true;
}

YYUTManager::YYUTManager()
{

}

YYUTManager::~YYUTManager()
{

}

bool YYUTManager::IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}

bool YYUTManager::ModifyDeviceSettings(YYUTD3D9DeviceSettings* pDeviceSettings, void* pUserContext)
{
	return true;
}

HRESULT YYUTManager::OnCreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	return S_OK;
}

HRESULT YYUTManager::OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	return S_OK;
}

void YYUTManager::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{

}

void YYUTManager::OnFrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{

}

void YYUTManager::OnLostDevice(void* pUserContext)
{

}

void YYUTManager::OnDestroyDevice(void* pUserContext)
{

}



void YYUTManager::YYUTLock::Init()
{
	InitializeCriticalSectionAndSpinCount(&cs,1000);
}

YYUTManager::YYUTLock::~YYUTLock()
{

}

YYUTManager::YYUTLock::YYUTLock()
{

}

CRITICAL_SECTION YYUTManager::YYUTLock::cs;

YYUTTimer YYUTTimer::_instance;

YYUTTimer::YYUTTimer()
{
	timer_stopped=true;
	QPF_Ticks_per_sec=0;
	stop_time=0;
	last_elapsed_time=0;
	base_time=0;
	//��ʼ��QPF_Ticks_per_secΪϵͳʱ������
	LARGE_INTEGER _tick_per_sec={0};
	QueryPerformanceFrequency(&_tick_per_sec);
	QPF_Ticks_per_sec=_tick_per_sec.QuadPart;
}
//stop�󷵻�stop��ʱ�䣬���򷵻ص�ǰʱ�䡣
LARGE_INTEGER YYUTTimer::GetAdjustedCurrentTime()
{
	LARGE_INTEGER _time;
	if(stop_time!=0)
		_time.QuadPart=stop_time;
	else
		QueryPerformanceCounter(&_time);
	return _time;
}

bool YYUTTimer::IsStoped()
{
	return timer_stopped;
}
//ָ��ĳһ����ȥ��
void YYUTTimer::LimitThreadAffinityToCurrentProc()
{
	HANDLE process=GetCurrentProcess();
	DWORD_PTR process_affinity_mask=0;
	DWORD_PTR system_affinity_mask=0;
	if(GetProcessAffinityMask(process,&process_affinity_mask,&system_affinity_mask)!=0&&process_affinity_mask)
	{
		//��ȡ���λΪ1���Ǹ�λ
		DWORD_PTR low=(process_affinity_mask &&((~process_affinity_mask)+1));
		HANDLE thread=GetCurrentThread();
		if(INVALID_HANDLE_VALUE!=thread)
		{
			SetThreadAffinityMask(thread,low);
			CloseHandle(thread);
		}
	}
	CloseHandle(process);
}

void YYUTTimer::Start()
{
	LARGE_INTEGER time={0};
	QueryPerformanceCounter(&time);
	if(timer_stopped)
	{
		base_time+=time.QuadPart-stop_time;
	}
	stop_time=0;
	last_elapsed_time=time.QuadPart;
	timer_stopped=false;
}

void YYUTTimer::Stop()
{
	if(!timer_stopped)
	{
		LARGE_INTEGER time={0};
		QueryPerformanceCounter(&time);
		stop_time=time.QuadPart;
		last_elapsed_time=time.QuadPart;
		timer_stopped=true;
	}
}

void YYUTTimer::Reset()
{
	LARGE_INTEGER time=GetAdjustedCurrentTime();
	base_time=time.QuadPart;
	last_elapsed_time=time.QuadPart;
	stop_time=0;
	timer_stopped=false;
}

void YYUTTimer::Advance()
{
	stop_time+=QPF_Ticks_per_sec/10;
}

double YYUTTimer::GetAbsoluteTime()
{
	LARGE_INTEGER time={0};
	QueryPerformanceCounter(&time);
	double current_time=time.QuadPart/(double)QPF_Ticks_per_sec;
	return current_time;
}

double YYUTTimer::GetTime()
{
	LARGE_INTEGER time=GetAdjustedCurrentTime();
	double current_time=double(time.QuadPart-base_time)/(double)QPF_Ticks_per_sec;
	return current_time;
}

void YYUTTimer::GetTimeAll(double *_current_time,double *_absolute_time,float *_elapse_time)
{
	assert(_current_time && _absolute_time &&_elapse_time);
	LARGE_INTEGER time=GetAdjustedCurrentTime();
	float elapse=(float)((double)(time.QuadPart-last_elapsed_time)/(double)QPF_Ticks_per_sec);
	last_elapsed_time=time.QuadPart;
	if(elapse<0.0f)
		elapse=0.0f;
	*_absolute_time=time.QuadPart/(double)QPF_Ticks_per_sec;
	*_current_time=(time.QuadPart-base_time)/(double)QPF_Ticks_per_sec;
	*_elapse_time=elapse;
}

float YYUTTimer::GetElapseTime()
{
	LARGE_INTEGER time=GetAdjustedCurrentTime();
	float elapse=(float)((double)(time.QuadPart-last_elapsed_time)/(double)QPF_Ticks_per_sec);
	last_elapsed_time=time.QuadPart;
	if(elapse<0.0f)
		elapse=0.0f;
	return elapse;
}

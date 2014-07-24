#pragma once
#ifndef __YYUT_H__
#define __YYUT_H__
#include "stdafx.h"
#include "YYUTApplication.h"
#include <windows.h>
#include <initguid.h>
#include <assert.h>
#include <wchar.h>
#include <mmsystem.h>
#include <math.h>      
#include <limits.h>      
#include <stdio.h>
#include <vector>
#include <functional>
using namespace std;
using  std::tr1::function;
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#define  SET_ACCESSOR(x,y)  inline void Set##y( x t) {YYUTLock l;y=t;};
#define  GET_ACCESSOR(x,y)  inline x Get##y()  {YYUTLock l; return y;};
#define  GET_SET_ACCESSOR(x,y) SET_ACCESSOR(x,y) GET_ACCESSOR(x,y)

#define  SETP_ACCESSOR(x,y) inline void Set##y(x * t) {YYUTLock l; y=*t;};
#define  GETP_ACCESSOR(x,y) inline x* Get##y() {YYUTLock l; return &y;};
#define  GETP_SETP_ACCESSOR(x,y) SETP_ACCESSOR(x,y) GETP_ACCESSOR(x,y)
#ifdef   _DEBUG
#define  TRACE_FUNCTION  printf("call %s \n",__FUNCTION__); 
#else
#define  TRACE_FUNCTION  
#endif
		 
struct yyd3dException:std::exception
{

};
class YYUTTimer
{
public:
	YYUTTimer();
	void	Reset();
	void	Start();
	void	Stop();
	void	Advance();
	double  GetAbsoluteTime();
	double  GetTime();
	float   GetElapseTime();
	void	GetTimeAll(double *_current_time,double *_absolute_time,float *elapse_time);
	bool	IsStoped();
	void    LimitThreadAffinityToCurrentProc();
	inline static YYUTTimer& GetInstance()
	{
		return _instance;
	}
protected:
	LARGE_INTEGER GetAdjustedCurrentTime();
	bool using_QPF;
	bool timer_stopped;
	LONGLONG QPF_Ticks_per_sec;
	LONGLONG stop_time;
	LONGLONG last_elapsed_time;
	LONGLONG base_time;
private:
	static YYUTTimer _instance;
};
struct YYUTD3D9DeviceSettings
{
	UINT adapter_ordinal;
	D3DDEVTYPE device_type;
	D3DFORMAT adapter_format;
	DWORD behavior_flags;
	D3DPRESENT_PARAMETERS pp;
};
class YYUTManager:public YYUTApplication 
{
public:
	YYUTManager();
	virtual ~YYUTManager();

public:
	GET_SET_ACCESSOR(IDirect3D9*,D3D9);
	GET_SET_ACCESSOR(IDirect3DDevice9*,D3D9Device);
	GET_SET_ACCESSOR(YYUTD3D9DeviceSettings*,CurrentDeviceSettings);
	GETP_SETP_ACCESSOR(D3DSURFACE_DESC,BackBufferSurfaceDes9);
	GETP_SETP_ACCESSOR(D3DCAPS9,Caps);
	GET_SET_ACCESSOR(HWND,HWNDFocus);
	GET_SET_ACCESSOR(HWND,HWNDDeviceFullScreen);
	GET_SET_ACCESSOR(HWND,HWNDDeviceWindowed);
	GET_SET_ACCESSOR(HMONITOR,AdapterMonitor);
	GET_SET_ACCESSOR(HMENU,Menu);
	GET_SET_ACCESSOR(UINT,FullScreenBackBufferWidthAtModeChange);
	GET_SET_ACCESSOR(UINT,FULLScreenBackBufferHeightAtModeChage);
	GET_SET_ACCESSOR(UINT, WindowBackBufferWidthAtModeChange);
	GET_SET_ACCESSOR(UINT, WindowBackBufferHeightAtModeChange);
	GETP_SETP_ACCESSOR(WINDOWPLACEMENT,WindowedPlacement);
	GET_SET_ACCESSOR(DWORD,WindowedStyleAtModeChange);
	GET_SET_ACCESSOR(bool ,TopmostWhileWindowd);
	GET_SET_ACCESSOR(bool,Minimized);
	GET_SET_ACCESSOR(bool,Maximized);
	GET_SET_ACCESSOR(bool,MinimizedWhileFullscreen);
	GET_SET_ACCESSOR(bool,IgnoreSizeChange);
	GET_SET_ACCESSOR(double,Time);
	GET_SET_ACCESSOR(double,AbsoluteTime);
	GET_SET_ACCESSOR(float,ElapsedTime);
	GET_SET_ACCESSOR(HINSTANCE,HInstance);
	GET_SET_ACCESSOR(double,LastStatusUpdateTime);
	GET_SET_ACCESSOR(DWORD,LastStatusUpdateFrames);
	GET_SET_ACCESSOR(float,FPS);
	GET_SET_ACCESSOR(int,CurrentFrameNumber);
	GET_SET_ACCESSOR(HHOOK,KeyboardHook);
	GET_SET_ACCESSOR( bool, AllowShortcutKeys );
	GET_SET_ACCESSOR( STICKYKEYS, StartupStickyKeys );
	GET_SET_ACCESSOR( TOGGLEKEYS, StartupToggleKeys );
	GET_SET_ACCESSOR( FILTERKEYS, StartupFilterKeys );
	GET_SET_ACCESSOR( bool, AppSupportsD3D9Override );
	GET_SET_ACCESSOR( bool, UseD3DVersionOverride );
	GET_SET_ACCESSOR( bool, ClipCursorWhenFullScreen );   
	GET_SET_ACCESSOR( bool, ShowCursorWhenFullScreen );
	GET_SET_ACCESSOR( bool, ConstantFrameTime );
	GET_SET_ACCESSOR( float, TimePerFrame );
	GET_SET_ACCESSOR( bool, WireframeMode );   
	GET_SET_ACCESSOR( bool, AutoChangeAdapter );
	GET_SET_ACCESSOR( int, ExitCode );
	GET_SET_ACCESSOR(bool,TimePaused);
	GET_SET_ACCESSOR(bool,RenderingPaused);
	GET_SET_ACCESSOR(int,PauseRenderingCount);
	GET_SET_ACCESSOR(int,PauseTimeCount);
	GET_SET_ACCESSOR(bool,DeviceLost);
	GET_SET_ACCESSOR(bool,DeviceObjectsCreated);
	GET_SET_ACCESSOR(bool,DeviceObjectsReset);
	GET_ACCESSOR(bool *,Keys);
	GET_ACCESSOR(bool*,LastKeys);
	GET_ACCESSOR(bool*,MouseButtons);
	GET_ACCESSOR(WCHAR*,StaticFrameStats);
	GET_ACCESSOR(WCHAR*,FPSStats);
	GET_ACCESSOR(WCHAR*,FrameStats);
	GET_ACCESSOR(WCHAR*,DeviceStats);

public:
	virtual bool		Initial();
	virtual int			Exit();
	virtual bool		ModifyDeviceSettings( YYUTD3D9DeviceSettings* pDeviceSettings);
	virtual HRESULT		OnCreateDevice( IDirect3DDevice9* pd3dDevice, 
										const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
										void* pUserContext );
	virtual HRESULT		OnResetDevice( void* pUserContext );
	virtual void		OnFrameMove( double fTime, float fElapsedTime);
	virtual void		OnFrameRender(double fTime, float fElapsedTime);
	virtual void		OnLostDevice( void* pUserContext );
	virtual void		OnDestroyDevice( void* pUserContext );
	virtual void		CreateDevice(bool windowd,int width,int height);
	virtual void        Render3DEnvironment();
	virtual void        GameResourceReset();
	virtual void		GameResourceInit();
	virtual void		GameResourceLost();
	virtual void GameMain(double timespan);
	virtual bool		GameInit();
	virtual void		GameExit();
			void		Pause(bool _time,bool _render);
			void		ShutDown();
	inline  bool		IsRenderPaused();
			void		ToggleFullScreen();
protected:
	virtual HRESULT		MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual HRESULT		PreMyProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,bool &Further_process);
	virtual void		KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown);
	virtual void		MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
								   bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
								   int xPos, int yPos);
	virtual int			Run();
	inline  HWND		GetHWND();
	inline  bool		IsWindowed();
			void		WindowSizeChange();
			HRESULT		ChangeDevice(YYUTD3D9DeviceSettings* new_device_Settings, bool force_recreate, 
										bool clip_window_to_single_adpater);
			bool		CanDeviceBeReset(YYUTD3D9DeviceSettings *old_device_settings,
										YYUTD3D9DeviceSettings *new_device_settings);
			
			void		UpdateBackBufferDes();
			void		SetupCursor();
			HRESULT		SetD3D9DeviceCursor( HCURSOR hCursor, bool bpush_backWatermark );
			void		Cleanup3DEnvironment9(bool release_setting);
			HRESULT		Create3DEnvironment9();
			HRESULT		Reset3DEnvironment();
			bool		FindVaildDeviceSettings(YYUTD3D9DeviceSettings* dev_set);
private:
	class YYUTLock
	{
	public:
		inline YYUTLock();
		inline ~YYUTLock();
		static void Init();
	private:
		static CRITICAL_SECTION cs;
	};
private:
	IDirect3D9*					D3D9;
	IDirect3DDevice9*			D3D9Device;
	YYUTD3D9DeviceSettings*		CurrentDeviceSettings;
	D3DSURFACE_DESC				BackBufferSurfaceDes9;
	D3DCAPS9					Caps;
	HWND						HWNDFocus;
	HWND						HWNDDeviceFullScreen;
	HWND						HWNDDeviceWindowed;
    HMONITOR					AdapterMonitor;
	HMENU						Menu;
	UINT						FullScreenBackBufferWidthAtModeChange;
	UINT						FULLScreenBackBufferHeightAtModeChage;
	UINT						WindowBackBufferWidthAtModeChange;
	UINT						WindowBackBufferHeightAtModeChange;
	WINDOWPLACEMENT				WindowedPlacement;
	DWORD						WindowedStyleAtModeChange;
	bool						TopmostWhileWindowd;
	bool						Minimized;
	bool						Maximized;
	bool						MinimizedWhileFullscreen;
	bool						IgnoreSizeChange;
	double						Time;
	double						AbsoluteTime;
	float						ElapsedTime;
	HINSTANCE                   HInstance;
	double						LastStatusUpdateTime;
	DWORD						LastStatusUpdateFrames;
	float						FPS;
	int							CurrentFrameNumber;
	HHOOK						KeyboardHook;
	bool						AllowShortcutKeys ;
	STICKYKEYS					StartupStickyKeys ;
	TOGGLEKEYS					StartupToggleKeys ;
	FILTERKEYS					StartupFilterKeys;
	bool                        AppSupportsD3D9Override ;
	bool						UseD3DVersionOverride;
	bool						ClipCursorWhenFullScreen;
	bool						ShowCursorWhenFullScreen;
	bool						ConstantFrameTime ;
	float						TimePerFrame;
	bool						WireframeMode;
	bool                        AutoChangeAdapter;
    int                         ExitCode;
	bool						Keys[256];
	bool						LastKeys[256];
	bool						MouseButtons[5];
	WCHAR                       StaticFrameStats[256];
	WCHAR						FPSStats[64];
	WCHAR						FrameStats[256];
	WCHAR						DeviceStats[256];
	bool						TimePaused;                 // if true, then time is paused
	bool						RenderingPaused;            // if true, then rendering is paused
	int							PauseRenderingCount;        // pause rendering ref count
	int							PauseTimeCount;             // pause time ref count
	bool						DeviceLost;                 // if true, then the device is lost and needs to be reset
	bool						DeviceObjectsCreated;
	bool						DeviceObjectsReset;
};
#endif
#ifndef __YYUTMANAGER__
#define __YYUTMANAGER__
#pragma  once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTApplication.h"
namespace YYUT
{
	struct YYUTManagerException:virtual YYUTException{};
	//struct boost::error_info<struct tag_yyut_D3DPRESENT_PARAMETERS,_D3DPRESENT_PARAMETERS_> errinfo_D3DPRESENT_PARAMETERS; 
	class YYUTManager:public YYUTApplication 
	{
	public:
		YYUTManager();
		virtual ~YYUTManager();

	public:
		GET_SET_ACCESSOR(IDirect3D9*,D3D9);
		GET_SET_ACCESSOR(IDirect3DDevice9*,D3D9Device);
		
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
		const YYUTD3D9DeviceSettings *GetCurrentDeviceSettings()
		{
			YYUTLock l;
			 return &CurrentDeviceSettings_;
		}
		void SetCurrentDeviceSettings(YYUTD3D9DeviceSettings* setting) 
		{
			YYUTLock l;
			if(setting)
			CurrentDeviceSettings_=*setting;	
		}
	public:
		virtual void		Initial();
		virtual void		Exit();
		virtual bool		ModifyDeviceSettings( YYUTD3D9DeviceSettings* pDeviceSettings);
		virtual HRESULT		OnCreateDevice( IDirect3DDevice9* pd3dDevice, 
							const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
							void* pUserContext );
		virtual bool		OnResetDevice( void* pUserContext );
		virtual void		OnFrameMove( double fTime, float fElapsedTime);
		virtual void		OnFrameRender(double fTime, float fElapsedTime);
		virtual void		OnLostDevice( void* pUserContext );
		virtual void		OnDestroyDevice( void* pUserContext );
		virtual void		CreateDevice(bool windowd,int width,int height);
		virtual void        Render3DEnvironment();
		virtual void        GameResourceReset();
		virtual void		GameResourceInit();
		virtual void		GameResourceLost();
		virtual void		GameMain(double time_span, double time_elapse);
		virtual void		GameInit();
		virtual void		GameExit();
				void		Pause(bool _time,bool _render);
				void		ShutDown();
		inline  bool		IsRenderPaused();
				void		ToggleFullScreen();
				int         GetWidth();
				int			GetHeight();
	protected:
		virtual LRESULT		MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw();
		virtual HRESULT		PreMyProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,bool &Further_process);
		virtual void		KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown);
		virtual void		MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
							bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
							int xPos, int yPos);
		virtual int			Run();
		inline  HWND		GetHWND();
		inline  bool		IsWindowed();
				void		WindowSizeChange();	
				HRESULT		ChangeDevice(YYUTD3D9DeviceSettings* new_device_Settings, bool force_recreate);
				bool		CanDeviceBeReset(YYUTD3D9DeviceSettings *old_device_settings,
								YYUTD3D9DeviceSettings *new_device_settings);

				void		UpdateBackBufferDes();
				void		SetupCursor();
				void		SetD3D9DeviceCursor( HCURSOR hCursor, bool bpush_backWatermark );
				void		Cleanup3DEnvironment9(bool release_setting);
				void		Create3DEnvironment9();
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
		YYUTD3D9DeviceSettings		CurrentDeviceSettings_;
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
		int							width_;
		int							height_;
		bool                        not_first_time;
	};
}
#endif
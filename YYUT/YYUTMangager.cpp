#include "stdafx.h"
#include <assert.h>
#include <iostream>
#include "YYUT.h"
#include "YYUTMangager.h"
#include "YYUTenum.h"
#include "YYUTMutiScreen.h"
#include "YYUTTimer.h"
#include <boost\exception\get_error_info.hpp>
#include <boost/format.hpp>
#include <boost/exception/all.hpp>
using namespace YYUT;

LRESULT YYUTManager::MyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) throw()
{
	try{
	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		if( !IsWindowed() )
		{
			IDirect3DDevice9* d3d_device = GetD3D9Device();
			if( d3d_device)
			{
				POINT ptCursor;
				GetCursorPos( &ptCursor );
				d3d_device->SetCursorPosition( ptCursor.x, ptCursor.y, 0 );
			}
		}
		//不能用break，要向下传递
	case WM_KEYDOWN:
			switch( wParam )
			{
			case VK_ESCAPE:
				{
					SendMessage( hWnd, WM_CLOSE, 0, 0 );
					break;
				}
			}
		//不能用break，要向下传递
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		{
			YYUTWindowsMsg msg;
			msg.uMsg=uMsg;
			msg.lParam=lParam;
			msg.wParam=wParam;
			GetKeyBoardMouseMSGQueue().Put(msg);
		}
		break;

	case WM_PAINT:
		{
		}
		break;


	case WM_SIZE:
		//移动一个窗口的边框的时候
		//先发送一个WM_ENTERSIZEMOVE,用来说明开始变化窗口大小
		//之后一直发送WM_SIZE
		//最后才发送WM_EXITSIZEMOVE
		//所以最后再reset
		WindowSizeChange();
		break;
	case WM_GETMINMAXINFO:
		//窗口在拖动时最大最小的范围
		( ( MINMAXINFO* )lParam )->ptMinTrackSize.x = 1200;
		( ( MINMAXINFO* )lParam )->ptMinTrackSize.y = 700;
		break;

	case WM_ENTERSIZEMOVE:
		// Halt frame movement while the app is sizing or moving
		//Pause( true, true );
		break;

	case WM_EXITSIZEMOVE:
		//Pause( false, false );
		WindowSizeChange();
		break;

	

	case WM_SETCURSOR:
		if( !IsWindowed() )
		{
			IDirect3DDevice9* pd3dDevice = GetD3D9Device();
			if( pd3dDevice && GetShowCursorWhenFullScreen() )
				return true; // prevent Windows from setting cursor to window class cursor
		}
		break;

	case WM_NCHITTEST:
		// Prevent the user from selecting the menu in full screen mode
		if( !IsWindowed() )
			return HTCLIENT;
		break;

	case WM_SYSCOMMAND:
		// Prevent moving/sizing in full screen mode
		switch( ( wParam & 0xFFF0 ) )
		{
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
			if( !IsWindowed() )
				return 0;
			break;
		}
		break;
	
	case WM_CLOSE:
		{
			HMENU hMenu;
			hMenu = ::GetMenu( hWnd );
			if( hMenu != NULL )
				DestroyMenu( hMenu );
			DestroyWindow( hWnd );
			Pause(true,true);
			SetHWNDFocus( NULL );
			SetHWNDDeviceFullScreen( NULL );
			SetHWNDDeviceWindowed( NULL );
			return 0;
		}

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
		break;
		//自定义的全屏函数，必须在有MSGLOOP的线程下工作。
	case WM_FULLSCREEN:
		ToggleFullScreenImp();
		break;
	}

	// Don't allow the F10 key to act as a shortcut to the menu bar
	// by not passing these messages to the DefWindowProc only when
	// there's no menu present
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	catch(...)
	{
		std::cout<<"ERROR happend in 窗口函数TAT"<<endl;
		std::cout<<boost::current_exception_diagnostic_information();
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
}


void YYUTManager::GameExit()
{
}

void YYUT::YYUTManager::GameInit()
{
}

void YYUT::YYUTManager::GameMain(double time_span, double time_elapse)
{
}

void YYUT::YYUTManager::Exit()
{
}

void YYUT::YYUTManager::Initial()
{
	YYUTApplication::Initial();
	//见 控制面板\所有控制面板项\轻松访问中心\使键盘更易于使用
	//保存粘滞键,粘滞键是用来，比如ctrl+alt+del时，我们要三个一块按，开启后，一个一个的按就行，不用同时按
	STICKYKEYS sk = {sizeof( STICKYKEYS ), 0};
	SystemParametersInfo( SPI_GETSTICKYKEYS, sizeof( STICKYKEYS ), &sk, 0 );
	SetStartupStickyKeys( sk );
	//保存切换键，切换键，就是，按Capslk键时会叫一下
	TOGGLEKEYS tk = {sizeof( TOGGLEKEYS ), 0};
	SystemParametersInfo( SPI_GETTOGGLEKEYS, sizeof( TOGGLEKEYS ), &tk, 0 );
	SetStartupToggleKeys( tk );
	//保存筛选键,用来切换连续按键的时间间隔。
	FILTERKEYS fk = {sizeof( FILTERKEYS ), 0};
	SystemParametersInfo( SPI_GETFILTERKEYS, sizeof( FILTERKEYS ), &fk, 0 );
	SetStartupFilterKeys( fk );
	//用来防止高DPI的时候出问题。
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
	YYUTTimer::GetInstance().Reset();
	int default_x=1920;
	int default_y=1080;
	try
	{
		YYUTApplication::WindowCreate(default_x,default_y,_T("YYUT"));
#if defined( DEBUG ) || defined( _DEBUG )
		//把log打到指定的文件
		YYUT::YYSetConsoleA("out_bug.txt");		
		//把log在console里显示
		//YYUT::YYSetConsoleA(GetHWND());		
#endif // _DEBUG
		SetHInstance(main_instance);
		SetHWNDFocus(main_window->hwnd);
		SetHWNDDeviceWindowed(main_window->hwnd);
		SetHWNDDeviceFullScreen(main_window->hwnd);
		CreateDevice(true,default_x,default_y);
		GameInit();
		input_thread_=make_shared<YYUTThread>(std::bind(&YYUTManager::KeyboardMouseProc,this));
		input_thread_->Start();
		render_thread_=make_shared<YYUTThread>(std::bind(&YYUTManager::Render3DEnvironment,this));	
		render_thread_->Start();
	}
	catch(YYUTWidnowException &e)
	{
		DWORD *$err_code=boost::get_error_info<YYUTWindow::WindowCreateError>(e);
		cout<<e.what()<<endl;
		cout<<"the failed code is :"<<*$err_code<<endl;
		throw;
	}
	catch(YYUTManagerException &e)
	{
		cout<<e.what()<<endl;
		cout<<"Initial failed!"<<endl;
		throw;
	}
}

YYUTManager::YYUTManager():keyboard_mouse_msg_queue_(200),render_pause_lock_(),render_pause_condition_(render_pause_lock_),render_frame_finished_condition_(render_pause_lock_)
{
	D3D9=NULL;
	D3D9Device=NULL;
	ZeroMemory(&BackBufferSurfaceDes9,sizeof(BackBufferSurfaceDes9));
	ZeroMemory(&Caps,sizeof(Caps));
	CurrentDeviceSettings_.pp.Windowed=TRUE;
	HWNDFocus=NULL;
	HWNDDeviceFullScreen=NULL;
	HWNDDeviceWindowed=NULL;
	AdapterMonitor=NULL;
	AutoChangeAdapter=true;
	AllowShortcutKeys=true;
	SetIgnoreSizeChange(true);
	memset(&CurrentDeviceSettings_,0,sizeof(CurrentDeviceSettings_));
	not_first_time=false;
	SetTopmostWhileWindowd(true);
	render_frame_finished_=true;
}

YYUTManager::~YYUTManager()
{

}


HRESULT YYUTManager::OnCreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	return S_OK;
}

bool YYUTManager::OnResetDevice(void* pUserContext)
{
	GameResourceReset();
	return true;
}


void YYUTManager::OnFrameRender(double fTime, float fElapsedTime)
{
	GameMain(fTime, fElapsedTime);
}

void YYUTManager::OnLostDevice(void* pUserContext)
{
	GameResourceLost();
}

void YYUTManager::OnDestroyDevice(void* pUserContext)
{

}




void YYUTManager::Pause(bool time_stop,bool render_stop)
{
	
	{
		YYUTMutexLockGuard lock(mutex_);
		if(time_stop)
			PauseTimeCount++;
		else
			PauseTimeCount--;
		if(PauseTimeCount<0)
			PauseTimeCount=0;
		if(PauseTimeCount>0)
		{
			YYUTTimer::GetInstance().Stop();
			TimePaused=true;
		}
		else
		{
			YYUTTimer::GetInstance().Start();
			TimePaused=false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//render
	{
		YYUTMutexLockGuard lock(render_pause_lock_);
		
			if(render_stop)
			{
				PauseRenderingCount++;
				cout<<"begin pause"<<endl;
				while(!render_frame_finished_)
					render_frame_finished_condition_.Wait();
			}
			else
			{
				PauseRenderingCount--;
				if(PauseRenderingCount<0)
					PauseRenderingCount=0;
				cout<<"start pase"<<endl;
			}
			if(PauseRenderingCount==0)
				render_pause_condition_.Notify();
	}
	cout<<"end pause"<<endl;
}

HWND YYUTManager::GetHWND()
{
	return IsWindowed()?GetHWNDDeviceWindowed():GetHWNDDeviceFullScreen();
}

bool YYUTManager::IsWindowed()
{
	return !!GetCurrentDeviceSettings()->pp.Windowed;
}

void YYUTManager::WindowSizeChange()
{
	//防止在d3d没初始化完成时调用，窗口初始化的时候会有WM_SIZE消息
	if(GetIgnoreSizeChange())
		return;
	YYUTD3D9DeviceSettings ds=*GetCurrentDeviceSettings();
	RECT rcCurrentClient;
	GetClientRect( GetHWND(), &rcCurrentClient );

	if( ( UINT )rcCurrentClient.right != ds.pp.BackBufferWidth ||
		( UINT )rcCurrentClient.bottom != ds.pp.BackBufferHeight)
	{
		// A new window size will require a new backbuffer size size
		// Tell DXUTChangeDevice and D3D to size according to the HWND's client rect
		ds.pp.BackBufferWidth = 0;
		ds.pp.BackBufferHeight = 0;
		try
		{ChangeDevice( &ds,false);}
		catch(YYUTException &)
		{
			cout<<"in change size catched!"<<endl;
			throw;
		}
	}
}

HRESULT YYUT::YYUTManager::ChangeDevice(YYUTD3D9DeviceSettings* new_device_Settings, bool force_recreate)
{
#if (defined _DEBUG) || (defined DEBUG )
	cout<<"call ChangeDevice\n"<<endl;
#endif
	HRESULT hr;
	YYUTD3D9DeviceSettings old_d3ddevice_setting=*GetCurrentDeviceSettings();
	assert(new_device_Settings);
	if(!new_device_Settings)
		BOOST_THROW_EXCEPTION(boost::enable_error_info(std::invalid_argument("invalid new_device_settings")));	

	SetCurrentDeviceSettings(new_device_Settings);
	Pause(true,true);
	SetIgnoreSizeChange(true);
	bool keep_current_window_size=false;
	if(new_device_Settings->pp.BackBufferWidth==0 &&new_device_Settings->pp.BackBufferHeight==0)
		keep_current_window_size=true;
	if(new_device_Settings->pp.Windowed)//window mode
	{
		if(not_first_time &&!old_d3ddevice_setting.pp.Windowed)
		{
			//from fullscreen -> windowed
			SetFullScreenBackBufferWidthAtModeChange(old_d3ddevice_setting.pp.BackBufferWidth);
			SetFULLScreenBackBufferHeightAtModeChage(old_d3ddevice_setting.pp.BackBufferHeight);
			SetWindowLong(GetHWNDDeviceWindowed(),GWL_STYLE,GetWindowedStyleAtModeChange());
		}
		// If different device windows are used for windowed mode and fullscreen mode,
		// hide the fullscreen window so that it doesn't obscure the screen.
		if( GetHWNDDeviceFullScreen() != GetHWNDDeviceWindowed() )
			ShowWindow( GetHWNDDeviceFullScreen(), SW_HIDE );
	}
	else
	{
		//from windowed to fullscreen
		if(!not_first_time ||old_d3ddevice_setting.pp.Windowed)
		{
			WINDOWPLACEMENT *pwp=GetWindowedPlacement();
			ZeroMemory(pwp,sizeof(WINDOWPLACEMENT));
			pwp->length=sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(GetHWNDDeviceWindowed(),pwp);
			bool top_most=((GetWindowLong(GetHWNDDeviceWindowed(),GWL_EXSTYLE) & WS_EX_TOPMOST)!=0);
			SetTopmostWhileWindowd(top_most);
			DWORD style=GetWindowLong(GetHWNDDeviceWindowed(),GWL_STYLE);
			style &= ~WS_MAXIMIZE & ~WS_MINIMIZE;
			SetWindowedStyleAtModeChange(style);
			if(not_first_time)
			{
				SetWindowBackBufferWidthAtModeChange(old_d3ddevice_setting.pp.BackBufferWidth);
				SetWindowBackBufferHeightAtModeChange(old_d3ddevice_setting.pp.BackBufferHeight);
			}
		}
		//Hide the window to avoid animation of blank windows
		ShowWindow(GetHWNDDeviceFullScreen(),SW_HIDE);
		SetWindowLong(GetHWNDDeviceFullScreen(),GWL_STYLE,WS_POPUP);
		WINDOWPLACEMENT full_screen_wp;
		ZeroMemory(&full_screen_wp,sizeof(WINDOWPLACEMENT));
		full_screen_wp.length=sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(GetHWNDDeviceFullScreen(),&full_screen_wp);
		if((full_screen_wp.flags & WPF_RESTORETOMAXIMIZED)!=0)
		{
			// Restore the window to normal if the window was maximized then minimized.  This causes the 
			// WPF_RESTORETOMAXIMIZED flag to be set which will cause SW_RESTORE to restore the 
			// window from minimized to maxmized which isn't what we want
			full_screen_wp.flags &=~WPF_RESTORETOMAXIMIZED;
			full_screen_wp.showCmd=SW_RESTORE;
			SetWindowPlacement(GetHWNDDeviceFullScreen(),&full_screen_wp);
		}
	}
	if(!force_recreate &&CanDeviceBeReset(&old_d3ddevice_setting,new_device_Settings))
	{
		hr=Reset3DEnvironment();
		if(FAILED(hr))
		{
			if(D3DERR_DEVICELOST==hr)
			{
				SetDeviceLost(true);
			}
			else 
			{
				BOOST_THROW_EXCEPTION(YYUTException()<<err_str("can't changed the device to old state"));
			}
		}
		
	}
	else
	{
		if(not_first_time)
			Cleanup3DEnvironment9(false);
		try{
			Create3DEnvironment9();
		}
		catch(...){
			Cleanup3DEnvironment9(true);
			/*Pause(false,false);*/
			SetIgnoreSizeChange(false);
			throw;
		}
	}
	if(!not_first_time)//first time
	{
		SetWindowPos(GetHWNDDeviceWindowed(),HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
	}
	if(not_first_time&&!old_d3ddevice_setting.pp.Windowed&&new_device_Settings->pp.Windowed)
	{
		//from fullscreen to windowed, to restore window formal size and position
		WINDOWPLACEMENT *wp=GetWindowedPlacement();
		SetWindowPlacement(GetHWNDDeviceWindowed(),wp);
		HWND hwnd_insert_after=GetTopmostWhileWindowd()?HWND_TOPMOST:HWND_NOTOPMOST;
		SetWindowPos(GetHWNDDeviceWindowed(),hwnd_insert_after,0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
	}
	if(!IsWindowVisible(GetHWND()))
		ShowWindow(GetHWND(),SW_SHOW);
	Pause(false,false);
	SetIgnoreSizeChange(false);
	not_first_time=true;
	return S_OK	;
}



bool YYUTManager::CanDeviceBeReset(YYUTD3D9DeviceSettings *old_device_settings,YYUTD3D9DeviceSettings *new_device_settings)
{
	if(GetD3D9Device() && 
		(old_device_settings->adapter_ordinal==new_device_settings->adapter_ordinal) &&
		(old_device_settings->device_type==new_device_settings->device_type)&&
		(old_device_settings->behavior_flags==new_device_settings->behavior_flags)
		)
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
//Reset the 3D environment by:
//		1call the device lost;
//		2Reset the device
//		3Stores the back buffer description
//		Sets up the full screen Direct3D cursor if requested
//		calls the device reset
HRESULT YYUTManager::Reset3DEnvironment()
{
	TRACE_FUNCTION
	HRESULT hr;
	IDirect3DDevice9 *d3d_device=GetD3D9Device();
	assert(d3d_device);
	if(GetDeviceObjectsReset()==true)
	{
		this->OnLostDevice(NULL);
		SetDeviceObjectsReset(false);
	}
	//Reset the device
	YYUTD3D9DeviceSettings device_setting=*GetCurrentDeviceSettings();
	hr=d3d_device->Reset(&device_setting.pp);
	if(FAILED(hr))
	{
		BOOST_THROW_EXCEPTION(YYUTManagerException()<<err_hr(hr)<<err_str("Try to reset device failed!"));
	}
	SetCurrentDeviceSettings(&device_setting);
	UpdateBackBufferDes();
	SetupCursor();
	if(!OnResetDevice(NULL))
	{
		BOOST_THROW_EXCEPTION(YYUTManagerException()<<err_str("try to reset user defined reset function failed!"));
	}
	SetDeviceObjectsReset(true);
	return S_OK;
}

void YYUTManager::UpdateBackBufferDes()
{
	HRESULT hr;
	IDirect3DSurface9* back_buffer;
	hr=GetD3D9Device()->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back_buffer);
	D3DSURFACE_DESC *buffer_surface_des=GetBackBufferSurfaceDes9();
	ZeroMemory(buffer_surface_des,sizeof(D3DSURFACE_DESC));
	if(SUCCEEDED(hr))
	{
		back_buffer->GetDesc(buffer_surface_des);
		
		back_buffer->Release();
	}
}

void YYUTManager::SetupCursor()
{
	IDirect3DDevice9 *d3d_device=GetD3D9Device();
	if(!IsWindowed() && d3d_device)
	{
		if(GetShowCursorWhenFullScreen())
		{
			SetCursor(NULL);//turn off windows cursor in full screen mode;
			HCURSOR cursor=(HCURSOR)(ULONG_PTR)GetClassLongPtr(GetHWNDDeviceFullScreen(),GCLP_HCURSOR);
			SetD3D9DeviceCursor(cursor,true);
			d3d_device->ShowCursor(TRUE);
		}
		else
		{
			SetCursor(NULL);
			d3d_device->ShowCursor(FALSE);
		}
	}
	if(!IsWindowed()&& GetClipCursorWhenFullScreen())
	{
		RECT window_rect;
		GetWindowRect(GetHWNDDeviceFullScreen(),&window_rect);
		ClipCursor(&window_rect);
	}
	else
		ClipCursor(NULL);
}
//An operating system cursor is created and used under either of these conditions:
//
//The hardware has set D3DCURSORCAPS_COLOR (see D3DCURSORCAPS), and the cursor size is 32x32 (which is the cursor size in the operating system). 
//	The application is running in windowed mode. 
//	Otherwise, DirectX uses an emulated cursor. An application uses IDirect3DDevice9::SetCursorPosition to move an emulated cursor to follow mouse movement.
//
//	It is recommended for applications to always trap WM_MOUSEMOVE events and call DXSetCursorPosition.
//
//	Direct3D cursor functions use either GDI cursor or software emulation, depending on the hardware. Users typically want to respond to a WM_SETCURSOR message. For example, they might want to write the message handler as follows:
//
//
//case WM_SETCURSOR:
//	// Turn off window cursor. 
//	SetCursor( NULL );
//	m_pd3dDevice->ShowCursor( TRUE );
//	return TRUE; // Prevent Windows from setting cursor to window class cursor.
//	break;
//
//
//	Or, users might want to call the IDirect3DDevice9::SetCursorProperties method if they want to change the cursor. 
//
//		The application can determine what hardware support is available for cursors by examining appropriate members of the D3DCAPS9 structure. Typically, hardware supports only 32x32 cursors and, when windowed, the system might support only 32x32 cursors. In this case, IDirect3DDevice9::SetCursorProperties still succeeds but the cursor might be reduced to that size. The hot spot is scaled appropriately.
//
//		The cursor does not survive when the device is lost. This method must be called after the device is reset.
//

//由于dx9的原因，全屏下鼠标cursor有问题。在dx10时 DXGI就没问题了。TAT

//Mouse Cursors
//	On previous versions of Windows, the standard GDI mouse cursor routines did not operate correctly on all full-screen exclusive devices. The SetCursorProperties, ShowCursor, and SetCursorPosition APIs were push_backed to handle these cases. Since Windows Vista's version of GDI fully understands DXGI surfaces, there is no need for this specialized mouse cursor API so there is no Direct3D 10 equivalent. Direct3D 10 applications should instead use the standard GDI mouse cursor routines for mouse cursors.
//
//
void YYUT::YYUTManager::SetD3D9DeviceCursor(HCURSOR hCursor, bool bpush_backWatermark)
{
	HRESULT hr=E_FAIL;
	ICONINFO icon_info;
	bool BW_Cursor=false;
	LPDIRECT3DSURFACE9 Cursor_surface=NULL;
	LPDIRECT3DDEVICE9 d3d_device=GetD3D9Device();
	HDC hdc_color=NULL;
	HDC hdc_mask=NULL;
	HDC hdc_screen=NULL;
	BITMAP bm;
	DWORD width=0;
	DWORD height=0;
	DWORD height_src=0;
	DWORD height_des=0;
	COLORREF color;
	COLORREF mask;
	UINT x;
	UINT y;
	BITMAPINFO bmi;
	COLORREF * array_color=NULL;
	COLORREF * array_mask=NULL;
	DWORD *bitmap;
	HGDIOBJ gid_object_old;
	ZeroMemory(&icon_info,sizeof(icon_info));
	if(!GetIconInfo(hCursor,&icon_info))
		goto End;
	if(0==GetObject((HGDIOBJ)icon_info.hbmMask,sizeof(BITMAP),(LPVOID)&bm))
		goto End;
	width=bm.bmWidth;
	height_src=bm.bmHeight;
	if(icon_info.hbmColor==NULL)
	{
		BW_Cursor=true;
		height_des=height_src/2;
	}
	else
	{
		BW_Cursor=false;
		height_des=height_src;
	}
	//Create a surface for the fullscreen cursor

	//D3DPOOL_SCRATCH 
	//Resources are placed in system RAM and do not need to be recreated when a device is lost. 
	//These resources are not bound by device size or format restrictions.
	//Because of this, these resources cannot be accessed by the Direct3D device nor set as textures or render targets. 
	//However, these resources can always be created, locked, and copied.

	//IDirect3DDevice8::CreateImageSurface was renamed CreateOffscreenPlainSurface. 
	//An push_backitional parameter that takes a D3DPOOL type was push_backed. 
	//D3DPOOL_SCRATCH will return a surface that has identical characteristics to a surface created by IDirect3DDevice8::CreateImageSurface. 
	//D3DPOOL_DEFAULT is the appropriate pool for use with StretchRect and ColorFill.


	if(FAILED(hr=d3d_device->CreateOffscreenPlainSurface(width,height_des,D3DFMT_A8R8G8B8,D3DPOOL_SCRATCH,&Cursor_surface,NULL)))
		goto End;
	array_mask=new DWORD[width*height_src];
	ZeroMemory( &bmi,sizeof(bmi));
	bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth=width;
	bmi.bmiHeader.biHeight=height_src;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=32;
	bmi.bmiHeader.biCompression=BI_RGB;
	hdc_screen=GetDC(NULL);
	hdc_mask=CreateCompatibleDC(hdc_screen);
	if(hdc_mask==nullptr)
	{
		hr=E_FAIL;
		goto End;
	}
	gid_object_old=SelectObject(hdc_mask,icon_info.hbmMask);
	GetDIBits(hdc_mask,icon_info.hbmMask,0,height_src,array_mask,&bmi,DIB_RGB_COLORS);
	SelectObject(hdc_mask,gid_object_old);
	if(!BW_Cursor)
	{
		array_color=new DWORD[width*height_des];
		hdc_color=CreateCompatibleDC(hdc_screen);
		if(hdc_color==nullptr)
		{
			hr=E_FAIL;
			goto End;
		}
		SelectObject(hdc_color,icon_info.hbmColor);
		GetDIBits(hdc_color,icon_info.hbmColor,0,height_des,array_color,&bmi,DIB_RGB_COLORS);
	}
	//Transfer cursor image into the surface
	D3DLOCKED_RECT lr;
	Cursor_surface->LockRect(&lr,NULL,0);
	bitmap=(DWORD*)lr.pBits;
	for(y=0;y<height_des;y++)
	{
		for(x=0;x<width;x++)
		{
			if(BW_Cursor)
			{
				color=array_mask[width*(height_des-1-y)+x];
				mask=array_mask[width*(height_src-1-y)+x];
			}
			else
			{
				color=array_color[width*(height_des-1-y)+x];
				mask=array_mask[width*(height_des-1-y)+x];
			}
			if(mask==0)
				bitmap[width*y+x]=0xff000000 | color;
			else
				bitmap[width*y+x]=0x00000000;
			//push_back some water mark to differ from windowss cursor;
			if( bpush_backWatermark && x < 12 && y < 5 )
			{
				// 11.. 11.. 11.. .... CCC0
				// 1.1. ..1. 1.1. .... A2A0
				// 1.1. .1.. 1.1. .... A4A0
				// 1.1. ..1. 1.1. .... A2A0
				// 11.. 11.. 11.. .... CCC0

				const WORD wMask[5] = { 0xccc0, 0xa2a0, 0xa4a0, 0xa2a0, 0xccc0 };
				if( wMask[y] & ( 1 << ( 15 - x ) ) )
				{
					bitmap[width * y + x] |= 0xff808080;
				}
			}
		}
	}
	Cursor_surface->UnlockRect();
	if(FAILED(hr=d3d_device->SetCursorProperties(icon_info.xHotspot,icon_info.yHotspot,Cursor_surface)))
	{
		goto End;
	}
	hr=S_OK;
End:
	if(icon_info.hbmMask!=NULL)
		DeleteObject(icon_info.hbmMask);
	if(icon_info.hbmColor!=NULL)
		DeleteObject(icon_info.hbmColor);
	if(hdc_screen!=NULL)
		ReleaseDC(NULL,hdc_screen);
	if(hdc_color!=NULL)
		DeleteDC(hdc_color);
	if(hdc_mask!=NULL)
		DeleteDC(hdc_mask);
	delete[] array_mask;array_mask=NULL;
	delete[] array_color;array_color=NULL;
	Cursor_surface->Release();
	Cursor_surface=NULL;
	//return hr;
}

void YYUTManager::ShutDown()
{

}

void YYUTManager::Cleanup3DEnvironment9(bool release_setting)
{
	TRACE_FUNCTION
	IDirect3DDevice9* d3d_device=GetD3D9Device();
	if(d3d_device!=NULL)
	{
		if(GetDeviceObjectsReset()==true)
		{
			OnLostDevice(NULL);
		}
		if(GetDeviceObjectsCreated()==true)
		{
			OnDestroyDevice(nullptr);
		}
		if(d3d_device)
			d3d_device->Release();
	}
	SetD3D9Device(nullptr);
	if(release_setting)
	{
		SetCurrentDeviceSettings(nullptr);
		
	}
	D3DSURFACE_DESC * back_buffer_surface_des=GetBackBufferSurfaceDes9();
	ZeroMemory(back_buffer_surface_des,sizeof(D3DSURFACE_DESC));
	D3DCAPS9 *d3dCaps=GetCaps();
	ZeroMemory(d3dCaps,sizeof(D3DCAPS9));
	SetDeviceObjectsCreated(false);
	not_first_time=false;
}

void YYUT::YYUTManager::Create3DEnvironment9()
{
	TRACE_FUNCTION
	HRESULT hr=S_OK;
	IDirect3DDevice9 *d3d_device=GetD3D9Device();
	YYUTD3D9DeviceSettings new_device_setting=*GetCurrentDeviceSettings();
	if(d3d_device==nullptr)
	{
		IDirect3D9 * d3d=GetD3D9();
		assert(d3d);
		hr=d3d->CreateDevice(new_device_setting.adapter_ordinal,new_device_setting.device_type,GetHWNDFocus(),new_device_setting.behavior_flags,
			&new_device_setting.pp,&d3d_device);
		if(hr==D3DERR_DEVICELOST)
		{
			SetDeviceLost(true);
		}
		else if(FAILED(hr))
		{
			BOOST_THROW_EXCEPTION(YYUTManagerException()<<err_hr(hr)<<err_str("create device failed!"));
		}
	}
	else
	{
		d3d_device->AddRef();	
	}
	SetD3D9Device(d3d_device);
	UpdateBackBufferDes();
	SetupCursor();
	D3DCAPS9 *d3d_caps=GetCaps();
	d3d_device->GetDeviceCaps(d3d_caps);
	GameResourceInit();
	OnResetDevice(nullptr);
	SetDeviceObjectsReset( true );
	SetDeviceObjectsCreated( true );
}

 int YYUT::YYUTManager::GetWidth() 
{
	return GetCurrentDeviceSettings()->pp.BackBufferWidth;
}

 int YYUT::YYUTManager::GetHeight() 
 {
	return GetCurrentDeviceSettings()->pp.BackBufferHeight;
 }

 void YYUT::YYUTManager::UpdateFrameStatus()
 {
	 double last_update_time=GetLastStatusUpdateTime();
	 double now_time=YYUTTimer::GetInstance().GetAbsoluteTime();
     DWORD frame_num=GetLastStatusUpdateFrames();
	 frame_num++;
	 SetLastStatusUpdateFrames(frame_num);
	 if(now_time-last_update_time>1.0)
	 {
		 float fps=static_cast<float>(frame_num/(now_time-last_update_time));
		 SetFPS(fps);
		 SetLastStatusUpdateFrames(0);
		 SetLastStatusUpdateTime(now_time);
	 }
 }

 YYUTMsgQueue<YYUTWindowsMsg> & YYUT::YYUTManager::GetKeyBoardMouseMSGQueue()
 {
	 YYUTMutexLockGuard lock(mutex_);
	 return keyboard_mouse_msg_queue_;
 }

 void YYUT::YYUTManager::KeyboardMouseProc(void)
 {

 }



void YYUTManager::CreateDevice(bool windowd,int width,int height)
{
	TRACE_FUNCTION
		try{
			if(GetD3D9()==NULL)
			{
				IDirect3D9 *pd3d=Direct3DCreate9(D3D_SDK_VERSION);
				if(pd3d==NULL)
					BOOST_THROW_EXCEPTION(YYUTD3DException()<<err_str("create d3d failed"));
				SetD3D9(pd3d);
			}
			YYUTD3D9DeviceSettings device_setting;
			ZeroMemory(&device_setting,sizeof(device_setting));
			device_setting.pp.hDeviceWindow=GetHWNDFocus();
			device_setting.adapter_ordinal=D3DADAPTER_DEFAULT;
			device_setting.pp.Windowed=windowd;
			device_setting.pp.BackBufferWidth=width;
			device_setting.pp.BackBufferHeight=height;
			device_setting.behavior_flags=D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;
			if(FindVaildDeviceSettings(&device_setting))
			{
#if defined( DEBUG ) || defined( _DEBUG )
				wcout<<"find property device settings"<<endl;
#endif
			}
			else
				throw YYUTManagerException()<<err_str("can't find property device settings");
			ChangeDevice(&device_setting,false);
	}
	catch(YYUTException &e)
	{
		UNREFERENCED_PARAMETER(e);
		throw;
	}
}

int YYUTManager::Run()
{
	HWND hwnd=GetHWNDFocus();
	MSG msg;
	//一定要是null,因为主窗口distroy之后hwnd就是无效了的。
	while(GetMessage(&msg,NULL,0,0))
	{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
	}
	return msg.message;
}

void YYUTManager::Render3DEnvironment()
{
	for(;;)
	{
		cout<<"render begin"<<endl;	
		{
			YYUTMutexLockGuard lock(render_pause_lock_);
			while(PauseRenderingCount)
				render_pause_condition_.Wait();
			render_frame_finished_=false;
		}
		HRESULT hr;
		
		if(GetDeviceLost())
		{
			Sleep(50);
		}
		// If no device created yet because device was lost (ie. another fullscreen exclusive device exists), 
		// then wait and try to create every so often.
		CComPtr<IDirect3DDevice9> d3d_device(GetD3D9Device());
		assert(d3d_device);
		if(NULL==d3d_device)
		{
			BOOST_THROW_EXCEPTION(YYUTManagerException()<<err_str("render error :not found d3d_device"));
		}

		double time,abs_time;
		float elapsed_time=0;
		YYUTTimer::GetInstance().GetTimeAll(&time,&abs_time,&elapsed_time);
		if(GetConstantFrameTime())
		{
			elapsed_time=GetTimePerFrame();
			time=GetTime()+elapsed_time;
		}
		SetTime(time);
		SetAbsoluteTime(abs_time);
		SetElapsedTime(elapsed_time);
		UpdateFrameStatus();
		
		OnFrameRender(time,elapsed_time);
		hr=d3d_device->Present(nullptr,nullptr,nullptr,nullptr);
		cout<<"after ender"<<endl;
		if(FAILED(hr))
		{
			if(D3DERR_DEVICELOST==hr)
			{
				SetDeviceLost(true);
			}
			else if( D3DERR_DRIVERINTERNALERROR == hr )
			{
				SetDeviceLost(true);
			}
		}
	/*	int nFrame=GetCurrentFrameNumber();
		nFrame++;
		SetCurrentFrameNumber(nFrame);
	*/	//////////////////////////////////////////////////////////////////////////
		//pause
		{
			YYUTMutexLockGuard lock(render_pause_lock_);
			render_frame_finished_=true;
			render_frame_finished_condition_.Notify();
		}
	}
}

bool YYUTManager::FindVaildDeviceSettings(YYUTD3D9DeviceSettings* dev_set)
{
	YYUTenum & enum_set=YYUTenum::GetInstance();
	enum_set.SetHWND(GetHWNDFocus());
	enum_set.SetD3D9(GetD3D9());
	enum_set.Enumerate();
	if(enum_set.FindPropertySettings(dev_set))
		return true;
	else
		return false;
}

void YYUTManager::GameResourceReset()
{

}

void YYUTManager::GameResourceInit()
{

}

void YYUTManager::GameResourceLost()
{

}

void YYUTManager::ToggleFullScreenImp()
{
//	HRESULT hr;
	YYUTD3D9DeviceSettings dev_setting=*GetCurrentDeviceSettings();
	YYUTD3D9DeviceSettings old_device_settings=*GetCurrentDeviceSettings();
	dev_setting.pp.Windowed=!dev_setting.pp.Windowed;
	bool isWindowed=!!dev_setting.pp.Windowed;
	UINT width=isWindowed? GetWindowBackBufferWidthAtModeChange():
						   GetFullScreenBackBufferWidthAtModeChange();
	UINT height=isWindowed?GetWindowBackBufferHeightAtModeChange():
							GetFULLScreenBackBufferHeightAtModeChage();
	if(width>0 &&height>0)
	{
		dev_setting.pp.BackBufferWidth=width;
		dev_setting.pp.BackBufferHeight=height;
	}
	if(FindVaildDeviceSettings(&dev_setting))
	{
		try
		{
			ChangeDevice(&dev_setting,false);
		}
		catch(YYUTManagerException &e)
		{
			UNREFERENCED_PARAMETER(e);
			ChangeDevice(&old_device_settings,false);
		}
	}
}



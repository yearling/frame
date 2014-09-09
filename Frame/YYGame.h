#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTMangager.h"
#include "YYUTGUI.h"
#include "YYUTObject.h"
#include "YYUTCamera.h"
#include <memory>
using std::shared_ptr;
namespace YYUT
{
	class YYGame :public YYUTManager
	{
	public:
		YYGame(void);
		virtual ~YYGame(void);
		virtual void        GameResourceReset();
		virtual void		GameResourceInit();
		virtual void		GameResourceLost();
		virtual void GameMain(double time_span, double time_elapse);
		virtual void		GameInit();
		virtual void		GameExit();
		virtual void		MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
			bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
			int xPos, int yPos);
		virtual HRESULT		PreMyProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,bool &Further_process);
				void		OnGUIEvent();
	protected:
		void HUDRest();
		void HUDInit();
		YYUTModelViewerCamera camera_;
	public:
		std::shared_ptr<YYUTDialog> hud_;
		std::shared_ptr<YYUTDialog> hud_fps_;
		std::shared_ptr<YYUTObjectX> robot_mesh_;
		std::shared_ptr<YYUTObjectX> cell_mesh_;
		shared_ptr<YYUTButton> bt_fullscreen;
		shared_ptr<YYUTButton> bt_sample2;
		shared_ptr<YYUTButton> bt_sample3;
		shared_ptr<YYUTAnimationStatic> bt_fps_;
	};

}
#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTMangager.h"
#include "YYUTGUI.h"
#include "YYUTObject.h"
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
		virtual void		GameMain(double timespan);
		virtual void		GameInit();
		virtual void		GameExit();
		virtual void		MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
			bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
			int xPos, int yPos);
		virtual HRESULT		PreMyProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,bool &Further_process);
				void       OnGUIEvent();
		struct YYFVF
		{
			FLOAT x,y,z;
			DWORD color;
		};
		void LoadMesh();
	private:
		LPDIRECT3DVERTEXBUFFER9 vertex_buf;
	protected:
		std::shared_ptr<YYUTDialog> hud_;
		std::shared_ptr<YYUTObjectX> robot_mesh_;
	};

}
#pragma once
#include "stdafx.h"
#include "yyut.h"
class YYGame :public YYUTManager
{
public:
	YYGame(void);
	virtual ~YYGame(void);
	virtual void        GameResourceReset();
	virtual void		GameResourceInit();
	virtual void		GameResourceLost();
	virtual void GameMain(double timespan);
	virtual bool		GameInit();
	virtual void		GameExit();
	virtual void		MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, 
		bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, 
		int xPos, int yPos);
	struct YYFVF
	{
		FLOAT x,y,z;
		DWORD color;
	};
private:
	LPDIRECT3DVERTEXBUFFER9 vertex_buf;
};


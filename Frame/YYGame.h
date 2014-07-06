#pragma once
#include "stdafx.h"
#include "yyut.h"
class YYGame :public YYUTManager
{
public:
	YYGame(void);
	virtual ~YYGame(void);
	virtual bool Initial();
};


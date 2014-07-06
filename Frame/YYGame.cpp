#include "stdafx.h"
#include "YYGame.h"


YYGame::YYGame(void)
{
}


YYGame::~YYGame(void)
{
}

bool YYGame::Initial()
{
	if(!YYUTManager::Initial())
		return false;
}

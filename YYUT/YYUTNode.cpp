#include "stdafx.h"
#include "YYUTNode.h"

namespace YYUT
{


	YYUTNodeDataBase::~YYUTNodeDataBase()
	{

	}


	YYUTNode::YYUTNode():father_ptr(nullptr)
	{
		D3DXMatrixIdentity(&transformation_);
	}

}
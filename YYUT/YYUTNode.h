#ifndef __YYUTNODE_H__
#define __YYUTNODE_H__
#pragma once
#include "stdafx.h"
#include <memory>
#include <boost/ptr_container/ptr_vector.hpp>
namespace YYUT
{
	class YYUTSenceManager
	{
	public:
		YYUTSenceManager();

	};
	class YYUTNode
	{
	public:
		YYUTNode();

	private:
		D3DXMATRIX transformation_;
		boost::ptr_vector<YYUTNode> children_vec;
		YYUTNode *father_ptr;
	};
	class YYUTNodeDataBase:public std::enable_shared_from_this<YYUTNodeDataBase>
	{
	public:
		enum NodeType{mesh=0,skelenton=1,camera=2,light=3};
		virtual ~YYUTNodeDataBase()=0;
		virtual NodeType GetType() const=0;
	};
}
#endif
#ifndef	 __YYUTUTILITY_H__
#define  __YYUTUTILITY_H__
#pragma once
#include "stdafx.h"
#include <boost/exception/all.hpp>

namespace YYUT
{
	typedef boost::error_info<struct tag_stack_info,string> err_stack_info;
	void ExceptionTraceInformation(boost::exception &e);
}
#endif
#pragma once
#ifndef __YYUT_H__
#define __YYUT_H__
#include "stdafx.h"
#include <windows.h>
#include <initguid.h>
#include <assert.h>
#include <wchar.h>
#include <mmsystem.h>
#include <math.h>      
#include <limits.h>      
#include <stdio.h>
#include <vector>
#include <functional>
#include <boost/exception/all.hpp>
#include <boost/exception/detail/error_info_impl.hpp>
#include <string>
//C语言接口用法，C++用unique_ptr
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#define  SET_ACCESSOR(x,y)  inline void Set##y( x t) {YYUTLock l;y=t;};
#define  GET_ACCESSOR(x,y)  inline x Get##y()  {YYUTLock l; return y;};
#define  GET_SET_ACCESSOR(x,y) SET_ACCESSOR(x,y) GET_ACCESSOR(x,y)

#define  SETP_ACCESSOR(x,y) inline void Set##y(x * t) {YYUTLock l; y=*t;};
#define  GETP_ACCESSOR(x,y) inline x* Get##y() {YYUTLock l; return &y;};
#define  GETP_SETP_ACCESSOR(x,y) SETP_ACCESSOR(x,y) GETP_ACCESSOR(x,y)
#if defined( DEBUG ) || defined( _DEBUG )
#define  TRACE_FUNCTION  printf("call %s \n",__FUNCTION__); 
#else
#define  TRACE_FUNCTION  
#endif

#if defined( DEBUG ) || defined( _DEBUG )
#define TRACE_EXCEPTION(e)  e<<boost::throw_function(__FUNCTION__)<<boost::throw_file(__FILE__)\
							<<boost::throw_line(__LINE__);
#else
#define TRACE_EXCEPTION(e)  
#endif // _DEBUG
namespace YYUT{

	struct YYUTException:virtual boost::exception,virtual std::exception{};
	struct YYUTD3DException:virtual YYUTException{};
	typedef boost::error_info<struct tag_err_HRESULT,HRESULT> err_hr;
	typedef boost::error_info<struct tag_err_string,string> err_str;
	struct YYUTD3D9DeviceSettings
	{
		UINT adapter_ordinal;
		D3DDEVTYPE device_type;
		D3DFORMAT adapter_format;
		DWORD behavior_flags;
		D3DPRESENT_PARAMETERS pp;
	};
}
#endif
#ifndef __YYUTEFFECTMANAGER_H__
#define __YYUTEFFECTMANAGER_H__
#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTMutiThread.h"
#include <boost/exception/all.hpp>
#include <boost/noncopyable.hpp>
namespace YYUT
{

	struct YYUTEffectException:virtual YYUTException{};
	struct YYUTEffectNode
	{
		YYUTEffectNode():used_(0){};
		CComPtr<ID3DXEffect> effect_;
		wstring effect_file_name_;
		int used_;
	};
	class YYUTEffectManager:boost::noncopyable
	{
	public:
		static YYUTEffectManager & GetInstance() { return instatnce_;}
		void OnDeviceCreate(CComPtr<IDirect3DDevice9> d3d_device);
		void OnDeviceReset();
		void OnDeviceLost();
		void OnDeviceDestroy();
		void CreateEffect(string id,wstring file_path);
		void RemoveEffect(string id);
		CComPtr<ID3DXEffect> AddEffect(string id);
	protected:
		YYUTEffectManager();
		
	private:
		static YYUTEffectManager instatnce_;
		CComPtr<IDirect3DDevice9> d3d_device_;
		std::map<string,YYUTEffectNode > map_effect_;
		typedef	std::map<string,YYUTEffectNode >::value_type MAP_EFFECT_PAIR;
		YYUTMutexLock mutex_;
	};

}

#endif
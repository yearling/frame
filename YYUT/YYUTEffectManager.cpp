#include "stdafx.h"
#include "YYUTEffectManager.h"
#include <utility>
#include <algorithm>

namespace YYUT
{

	YYUTEffectManager YYUTEffectManager::instatnce_;



	YYUTEffectManager::YYUTEffectManager()
	{

	}

	void YYUTEffectManager::OnDeviceCreate(CComPtr<IDirect3DDevice9> d3d_device)
	{
		d3d_device_=d3d_device;
	}

	void YYUTEffectManager::OnDeviceReset()
	{
		std::for_each(map_effect_.begin(),map_effect_.end(),[&](MAP_EFFECT_PAIR &p){
			if(p.second.effect_!=nullptr) p.second.effect_->OnResetDevice();
		});
	}

	void YYUTEffectManager::OnDeviceLost()
	{
		std::for_each(map_effect_.begin(),map_effect_.end(),[&](MAP_EFFECT_PAIR &p){
			if(p.second.effect_!=nullptr) p.second.effect_->OnLostDevice();
		});
	}

	void YYUTEffectManager::OnDeviceDestroy()
	{

	}

	void YYUTEffectManager::CreateEffect(string id,wstring file_path)
	{
		YYUTMutexLockGuard lock(mutex_);
		map_effect_[id].effect_file_name_=file_path;
	}

	void YYUTEffectManager::RemoveEffect(string id)
	{
		YYUTMutexLockGuard lock(mutex_);
		auto pos=map_effect_.find(id);
		if(pos!=map_effect_.end())
		{
			if(pos->second.used_>0)
				pos->second.used_--;
		}
		if(pos->second.used_==0)
			pos->second.effect_=0;
	}

	CComPtr<ID3DXEffect> YYUTEffectManager::AddEffect(string id)
	{
			assert(d3d_device_);
			YYUTMutexLockGuard lock(mutex_);
			auto pos=map_effect_.find(id);
			if(pos==map_effect_.end())
			{
				BOOST_THROW_EXCEPTION(YYUTEffectException()<<err_str("Add effect id is "+id));
			}
			if((pos->second.used_++)==0) //´´½¨
			{
			    HRESULT hr=S_OK;
				CComPtr<ID3DXBuffer> err_code;
				if(FAILED(hr=D3DXCreateEffectFromFile(d3d_device_,pos->second.effect_file_name_.c_str(),nullptr,nullptr,0,nullptr,&pos->second.effect_,&err_code)))
				{
					if(err_code)
					{
						string err_str_code((char*)err_code->GetBufferPointer());
						BOOST_THROW_EXCEPTION(YYUTEffectException()<<err_str("compile the "+id+" effect file error,the error code is "+err_str_code));
					}
					else
						BOOST_THROW_EXCEPTION(YYUTEffectException()<<err_str("compile the "+id+" effect file error"));
				}
				return pos->second.effect_;
			}
			else
				return pos->second.effect_;
	}

}
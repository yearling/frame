#include "stdafx.h"
#include "YYUTTimer.h"
#include <assert.h>

namespace YYUT
{
	
	YYUTTimer YYUTTimer::instance_;

	YYUTTimer::YYUTTimer()
	{
		timer_stopped_=true;
		QPF_Ticks_per_sec_=0;
		stop_time_=0;
		last_elapsed_time_=0;
		base_time_=0;
		//初始化QPF_Ticks_per_sec为系统时钟周期
		LARGE_INTEGER _tick_per_sec={0};
		QueryPerformanceFrequency(&_tick_per_sec);
		QPF_Ticks_per_sec_=_tick_per_sec.QuadPart;
	}
	//stop后返回stop的时间，否则返回当前时间。
	LARGE_INTEGER YYUTTimer::GetAdjustedCurrentTime()
	{
		YYUTMutexLockGuard lock(mutex_);
		return GetAdjustedCurrentTimeWithHold();
	}

	bool YYUTTimer::IsStoped()
	{
		YYUTMutexLockGuard lock(mutex_);
		return timer_stopped_;
	}
	//指定某一个核去跑
	void YYUTTimer::LimitThreadAffinityToCurrentProc()
	{
		HANDLE process=GetCurrentProcess();
		DWORD_PTR process_affinity_mask=0;
		DWORD_PTR system_affinity_mask=0;
		if(GetProcessAffinityMask(process,&process_affinity_mask,&system_affinity_mask)!=0&&process_affinity_mask)
		{
			//获取最低位为1的那个位
			DWORD_PTR low=(process_affinity_mask &&((~process_affinity_mask)+1));
			HANDLE thread=GetCurrentThread();
			if(INVALID_HANDLE_VALUE!=thread)
			{
				SetThreadAffinityMask(thread,low);
				CloseHandle(thread);
			}
		}
		CloseHandle(process);
	}

	void YYUTTimer::Start()
	{
		
		YYUTMutexLockGuard lock(mutex_);
		LARGE_INTEGER tmp_time={0};
		QueryPerformanceCounter(&tmp_time);
		if(timer_stopped_)
		{
			base_time_+=tmp_time.QuadPart-stop_time_;
		}
		stop_time_=0;
		last_elapsed_time_=tmp_time.QuadPart;
		timer_stopped_=false;
	}

	void YYUTTimer::Stop()
	{
		YYUTMutexLockGuard lock(mutex_);
		if(!timer_stopped_)
		{
			LARGE_INTEGER time={0};
			QueryPerformanceCounter(&time);
			stop_time_=time.QuadPart;
			last_elapsed_time_=time.QuadPart;
			timer_stopped_=true;
		}
	}

	void YYUTTimer::Reset()
	{
		YYUTMutexLockGuard lock(mutex_);
		LARGE_INTEGER time=GetAdjustedCurrentTimeWithHold();
		base_time_=time.QuadPart;
		last_elapsed_time_=time.QuadPart;
		stop_time_=0;
		timer_stopped_=false;
	}

	void YYUTTimer::Advance()
	{
		YYUTMutexLockGuard lock(mutex_);
		stop_time_+=QPF_Ticks_per_sec_/10;
	}

	double YYUTTimer::GetAbsoluteTime()
	{
		YYUTMutexLockGuard lock(mutex_);
		LARGE_INTEGER time={0};
		QueryPerformanceCounter(&time);
		double current_time=time.QuadPart/(double)QPF_Ticks_per_sec_;
		return current_time;
	}
	//得到运行的时间
	double YYUTTimer::GetTime()
	{
		YYUTMutexLockGuard lock(mutex_);
		LARGE_INTEGER time=GetAdjustedCurrentTimeWithHold();
		double current_time=double(time.QuadPart-base_time_)/(double)QPF_Ticks_per_sec_;
		return current_time;
	}

	void YYUTTimer::GetTimeAll(double *current_time,double *absolute_time,float *elapse_time)
	{
		YYUTMutexLockGuard lock(mutex_);
		assert(current_time && absolute_time &&elapse_time);
		LARGE_INTEGER time=GetAdjustedCurrentTimeWithHold();
		float elapse=(float)((double)(time.QuadPart-last_elapsed_time_)/(double)QPF_Ticks_per_sec_);
		last_elapsed_time_=time.QuadPart;
		if(elapse<0.0f)
			elapse=0.0f;
		*absolute_time=time.QuadPart/(double)QPF_Ticks_per_sec_;
		*current_time=(time.QuadPart-base_time_)/(double)QPF_Ticks_per_sec_;
		*elapse_time=elapse;
	}

	float YYUTTimer::GetElapseTime()
	{
		YYUTMutexLockGuard lock(mutex_);
		LARGE_INTEGER time=GetAdjustedCurrentTimeWithHold();
		float elapse=(float)((double)(time.QuadPart-last_elapsed_time_)/(double)QPF_Ticks_per_sec_);
		last_elapsed_time_=time.QuadPart;
		if(elapse<0.0f)
			elapse=0.0f;
		return elapse;
	}

	LARGE_INTEGER YYUTTimer::GetAdjustedCurrentTimeWithHold()
	{
		LARGE_INTEGER tmp_time;
		if(stop_time_!=0)
			tmp_time.QuadPart=stop_time_;
		else
			QueryPerformanceCounter(&tmp_time);
		return tmp_time;
	}



}
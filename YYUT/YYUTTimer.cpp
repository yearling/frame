#include "stdafx.h"
#include "YYUTTimer.h"
#include <assert.h>

namespace YYUT
{
	
	YYUTTimer YYUTTimer::_instance;

	YYUTTimer::YYUTTimer()
	{
		timer_stopped=true;
		QPF_Ticks_per_sec=0;
		stop_time=0;
		last_elapsed_time=0;
		base_time=0;
		//初始化QPF_Ticks_per_sec为系统时钟周期
		LARGE_INTEGER _tick_per_sec={0};
		QueryPerformanceFrequency(&_tick_per_sec);
		QPF_Ticks_per_sec=_tick_per_sec.QuadPart;
	}
	//stop后返回stop的时间，否则返回当前时间。
	LARGE_INTEGER YYUTTimer::GetAdjustedCurrentTime()
	{
		LARGE_INTEGER _time;
		if(stop_time!=0)
			_time.QuadPart=stop_time;
		else
			QueryPerformanceCounter(&_time);
		return _time;
	}

	bool YYUTTimer::IsStoped()
	{
		return timer_stopped;
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
		LARGE_INTEGER time={0};
		QueryPerformanceCounter(&time);
		if(timer_stopped)
		{
			base_time+=time.QuadPart-stop_time;
		}
		stop_time=0;
		last_elapsed_time=time.QuadPart;
		timer_stopped=false;
	}

	void YYUTTimer::Stop()
	{
		if(!timer_stopped)
		{
			LARGE_INTEGER time={0};
			QueryPerformanceCounter(&time);
			stop_time=time.QuadPart;
			last_elapsed_time=time.QuadPart;
			timer_stopped=true;
		}
	}

	void YYUTTimer::Reset()
	{
		LARGE_INTEGER time=GetAdjustedCurrentTime();
		base_time=time.QuadPart;
		last_elapsed_time=time.QuadPart;
		stop_time=0;
		timer_stopped=false;
	}

	void YYUTTimer::Advance()
	{
		stop_time+=QPF_Ticks_per_sec/10;
	}

	double YYUTTimer::GetAbsoluteTime()
	{
		LARGE_INTEGER time={0};
		QueryPerformanceCounter(&time);
		double current_time=time.QuadPart/(double)QPF_Ticks_per_sec;
		return current_time;
	}

	double YYUTTimer::GetTime()
	{
		LARGE_INTEGER time=GetAdjustedCurrentTime();
		double current_time=double(time.QuadPart-base_time)/(double)QPF_Ticks_per_sec;
		return current_time;
	}

	void YYUTTimer::GetTimeAll(double *_current_time,double *_absolute_time,float *_elapse_time)
	{
		assert(_current_time && _absolute_time &&_elapse_time);
		LARGE_INTEGER time=GetAdjustedCurrentTime();
		float elapse=(float)((double)(time.QuadPart-last_elapsed_time)/(double)QPF_Ticks_per_sec);
		last_elapsed_time=time.QuadPart;
		if(elapse<0.0f)
			elapse=0.0f;
		*_absolute_time=time.QuadPart/(double)QPF_Ticks_per_sec;
		*_current_time=(time.QuadPart-base_time)/(double)QPF_Ticks_per_sec;
		*_elapse_time=elapse;
	}

	float YYUTTimer::GetElapseTime()
	{
		LARGE_INTEGER time=GetAdjustedCurrentTime();
		float elapse=(float)((double)(time.QuadPart-last_elapsed_time)/(double)QPF_Ticks_per_sec);
		last_elapsed_time=time.QuadPart;
		if(elapse<0.0f)
			elapse=0.0f;
		return elapse;
	}



}
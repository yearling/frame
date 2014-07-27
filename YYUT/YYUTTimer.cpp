#include "stdafx.h"
#include "YYUTTimer.h"
#include <assert.h>

namespace YYUT
{
	 unsigned __int64 YTimer::GetCycleCount(void)
	{
		_asm    _emit 0x0F
		_asm    _emit 0x31
	}
	unsigned __int64 YTimer::Start()
	{
		
		::QueryPerformanceCounter(&m_StartCount);	
		return m_StartCount.QuadPart;
	}
	unsigned __int64 YTimer::Stop()
	{
		
		::QueryPerformanceCounter(&m_StopCount);
		unsigned __int64 i= m_StopCount.QuadPart-m_StartCount.QuadPart-m_avgTime;
		return i;
	}

	YTimer::YTimer():m_avgTime(0),m_avgRDTSC(0)
	{
		m_StartCount.QuadPart=0;
		m_StopCount.QuadPart=0;
		m_frequence.QuadPart=0;
		Start();
		m_avgTime=Stop();
		StartRDTSC();
		m_avgRDTSC=StopRDTSC();
		::QueryPerformanceFrequency(&m_frequence);
	}
	unsigned __int64 YTimer::GetCurrentFrequence()
	{
		LARGE_INTEGER fre={0};
		::QueryPerformanceFrequency(&fre);
		return fre.QuadPart;
	}
	unsigned __int64 YTimer::StartRDTSC()
	{
		m_StartRDTSC=GetCycleCount();
		return m_StartRDTSC;
	}

	double YTimer::TimeEclipseSecond(__int64 timespan)
	{
		double t=(double)(timespan)/(double)(m_frequence.QuadPart);
		return t;
	}
	double YTimer::TimeEclipseMicrosecond(__int64 timespan)
	{
		double t=(double)(timespan)*1000*1000/(double)(m_frequence.QuadPart);
		return t;
	}

	unsigned __int64 YTimer::StopRDTSC()
	{
		::QueryPerformanceFrequency(&m_frequence);
		m_StopRDTSC=GetCycleCount()-m_StartRDTSC-m_avgRDTSC;
		return m_StopRDTSC;
	}
	double YTimer::TimeEclipseMicrosecond()
	{
		double t=(double)(Stop())*1000*1000/(double)(m_frequence.QuadPart);
		return t;
	}

	double YTimer::TimeEclipseSecond()
	{
		double t=(double)(Stop())/(double)(m_frequence.QuadPart);
		return t;
	}

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
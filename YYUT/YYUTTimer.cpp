#include "stdafx.h"
#include "YYUTTimer.h"

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


}
#ifndef __YY_TIMER_H__
#define __YY_TIMER_H__

#include "stdafx.h"
namespace YYUT
{
class YTimer
{
public:
	YTimer();
	unsigned __int64 Start();//返回的是开始的时间
	unsigned __int64 Stop();//返回的是用时
	 unsigned __int64 GetCycleCount(void);//返回的是RDTSC，
	 unsigned __int64 StartRDTSC();
	 unsigned __int64 StopRDTSC();
	 unsigned __int64 GetCurrentFrequence();
	
	 double TimeEclipseMicrosecond(__int64 timespan);
	 double TimeEclipseMicrosecond();
	 double TimeEclipseSecond(__int64 timespan);
	 double TimeEclipseSecond();
private:
	unsigned __int64 m_avgTime;//调用开销
	unsigned __int64 m_avgRDTSC;
	LARGE_INTEGER m_frequence;
	LARGE_INTEGER m_StartCount;
	LARGE_INTEGER m_StopCount;
	unsigned __int64 m_StartRDTSC;
	unsigned __int64 m_StopRDTSC;
};
}

#endif




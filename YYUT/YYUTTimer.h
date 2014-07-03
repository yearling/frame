#ifndef __YY_TIMER_H__
#define __YY_TIMER_H__

#include "stdafx.h"
namespace YYUT
{
class YTimer
{
public:
	YTimer();
	unsigned __int64 Start();//���ص��ǿ�ʼ��ʱ��
	unsigned __int64 Stop();//���ص�����ʱ
	 unsigned __int64 GetCycleCount(void);//���ص���RDTSC��
	 unsigned __int64 StartRDTSC();
	 unsigned __int64 StopRDTSC();
	 unsigned __int64 GetCurrentFrequence();
	
	 double TimeEclipseMicrosecond(__int64 timespan);
	 double TimeEclipseMicrosecond();
	 double TimeEclipseSecond(__int64 timespan);
	 double TimeEclipseSecond();
private:
	unsigned __int64 m_avgTime;//���ÿ���
	unsigned __int64 m_avgRDTSC;
	LARGE_INTEGER m_frequence;
	LARGE_INTEGER m_StartCount;
	LARGE_INTEGER m_StopCount;
	unsigned __int64 m_StartRDTSC;
	unsigned __int64 m_StopRDTSC;
};
}

#endif




#ifndef __YY_TIMER_H__
#define __YY_TIMER_H__

#include "stdafx.h"
namespace YYUT
{
	class YYUTTimer
	{
	public:
		YYUTTimer();
		void	Reset();
		void	Start();
		void	Stop();
		void	Advance();
		double  GetAbsoluteTime();
		double  GetTime();
		float   GetElapseTime();
		void	GetTimeAll(double *_current_time,double *_absolute_time,float *elapse_time);
		bool	IsStoped();
		void    LimitThreadAffinityToCurrentProc();
		inline static YYUTTimer& GetInstance()
		{
			return _instance;
		}
	protected:
		LARGE_INTEGER GetAdjustedCurrentTime();
		bool using_QPF;
		bool timer_stopped;
		LONGLONG QPF_Ticks_per_sec;
		LONGLONG stop_time;
		LONGLONG last_elapsed_time;
		LONGLONG base_time;
	private:
		static YYUTTimer _instance;
	};

	
}

#endif




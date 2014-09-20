#ifndef __YY_TIMER_H__
#define __YY_TIMER_H__

#include "stdafx.h"
#include "YYUTMutiThread.h"
namespace YYUT
{
	//thread safty
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
			return instance_;
		}
	protected:
		LARGE_INTEGER GetAdjustedCurrentTime();
	   inline LARGE_INTEGER GetAdjustedCurrentTimeWithHold();
		bool using_QPF_;
		bool timer_stopped_;
		LONGLONG QPF_Ticks_per_sec_;//Hz
		LONGLONG stop_time_;
		LONGLONG last_elapsed_time_;
		LONGLONG base_time_;
	private:
		static YYUTTimer instance_;
		mutable YYUTMutexLock mutex_;
	};

	
}

#endif




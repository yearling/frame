#ifndef __YYUTMUTITHREAD_H__
#define  __YYUTMUTITHREAD_H__
#pragma once 
#include "stdafx.h"
#include "YYUT.h"
#include <assert.h>
#include <boost/noncopyable.hpp>
namespace YYUT
{
	//因为自vista以来提供的读写锁比关键段有更好的性能，
	//所里这里用默认写锁来提供不可重入的mutex,
	//容易死锁，但容易排查，
	//读写锁不用销毁
	//use as
	// int Foo::Size() const
	//{
	//		YYUTMutexLockGuard lock(mutex_);
	//		return data_.size();
	//}
	struct YYUTMutiThreadException:virtual std::exception,virtual boost::exception{};
	class YYUTMutexLock:boost::noncopyable
	{
	public:
		YYUTMutexLock() throw()
		{
			InitializeSRWLock(&srwlock_);//没有返回值。
		}
		~YYUTMutexLock(){}//系统自动销毁，不用手动
		void Lock() throw()
		{
			::AcquireSRWLockExclusive(&srwlock_);		
		}
		void Unlock() throw()
		{
			::ReleaseSRWLockExclusive(&srwlock_);
		}
		SRWLOCK *GetInside()
		{
			return &srwlock_;
		}
	private:
		SRWLOCK srwlock_;
	};
	class YYUTMutexLockGuard:boost::noncopyable
	{
	public:
		explicit YYUTMutexLockGuard(YYUTMutexLock & mu)throw():mutex_(mu)
		{
			mutex_.Lock();
		}
		~YYUTMutexLockGuard()
		{
			mutex_.Unlock();
		}
	private:
		YYUTMutexLock & mutex_;
	};
	class YYUTCondition:boost::noncopyable
	{
	public:
		explicit YYUTCondition(YYUTMutexLock &lock) throw():mutex_(lock)
		{
			InitializeConditionVariable(&cond_);	
		}
		~YYUTCondition(){} //系统自动销毁，不用手动
		void Wait()
		{
			if(::SleepConditionVariableSRW(&cond_,mutex_.GetInside(),INFINITE,0)==0)
				BOOST_THROW_EXCEPTION(YYUTMutiThreadException()<<err_str("condition sleep error")<<
				err_hr(GetLastError()));
		}
		void Notify()
		{
			::WakeConditionVariable(&cond_);
		}
		void NotifyAll()
		{
			::WakeAllConditionVariable(&cond_);
		}
	private:
		YYUTMutexLock &mutex_;
		CONDITION_VARIABLE cond_;
	};
}
//防止下面的情况
//YYUTMutexLockGuard(mutex_)
//这样子的话刚创建生命周期就结束了
#define YYUTMutexLockGuard(x)  static_assert(false,"Missing guard object name");

#endif
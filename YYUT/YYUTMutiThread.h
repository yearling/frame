#ifndef __YYUTMUTITHREAD_H__
#define  __YYUTMUTITHREAD_H__
#pragma once 
#include "stdafx.h"
#include "YYUT.h"
#include <assert.h>
#include <boost/noncopyable.hpp>
namespace YYUT
{
	//��Ϊ��vista�����ṩ�Ķ�д���ȹؼ����и��õ����ܣ�
	//����������Ĭ��д�����ṩ���������mutex,
	//�����������������Ų飬
	//��д����������
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
			InitializeSRWLock(&srwlock_);//û�з���ֵ��
		}
		~YYUTMutexLock(){}//ϵͳ�Զ����٣������ֶ�
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
		~YYUTCondition(){} //ϵͳ�Զ����٣������ֶ�
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
//��ֹ��������
//YYUTMutexLockGuard(mutex_)
//�����ӵĻ��մ����������ھͽ�����
#define YYUTMutexLockGuard(x)  static_assert(false,"Missing guard object name");

#endif
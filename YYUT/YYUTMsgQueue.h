#ifndef __YYUTMSGQUEUE_H__
#define __YYUTMSGQUEUE_H__
#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTAotmic.h"
#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>
#include <assert.h>
namespace YYUT
{
	template<typename T>
	class YYUTMsgQueue:boost::noncopyable
	{
	public:
		YYUTMsgQueue(int max_size):mutex_(),not_empty_(mutex_),queue_(max_size)
		{
		}
		void Put(const T & x)
		{
			YYUTMutexLockGuard lock(mutex_);
			queue_.push_back(x);
			not_empty_.Notify();
		}
		T Take()
		{
			YYUTMutexLockGuard lock(mutex_);
			while(queue_.empty())
				not_empty_.Wait();
			assert(!queue_.empty());
			T front(queue_.front());
			queue_.pop_front();
			return front;
		}
		bool Empty() const
		{
			YYUTMutexLockGuard lock(mutex_);
			return queue_.empty();
		}
		bool Full() const
		{
			YYUTMutexLockGuard lock(mutex_);
			return queue_.full();
		}
		size_t Size() const
		{
			YYUTMutexLockGuard lock(mutex_);
			return queue_.size();
		}
		size_t Capacity() const
		{
			YYUTMutexLockGuard lock(mutex_);
			return queue_.capacity();
		}
	private:
		//声明顺序与初始化列表相关，注意位置。
		mutable YYUTMutexLock mutex_;
		YYUTCondition not_empty_;
		boost::circular_buffer<T> queue_;
	};
}
#endif
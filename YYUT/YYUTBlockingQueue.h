#ifndef __YYUTBLOCKINGQUEUE_H__
#define __YYUTBLOCKINGQUEUE_H__
#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTAotmic.h
#include <boost/noncopyable.hpp>
#include <deque>
#include <assert.h>
namespace YYUT
{
	template<typename T>
	class YYUTBlockingQueue:boost noncopyable
	{
	public:
		YYUTBlockingQueue():mutex_(),not_empty_(mutex_),queue_()
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
		size_t Size() const
		{
			YYUTMutexLockGuard lock(mutex_);
			return queue_.size();
		}
	private:
		//声明顺序与初始化列表相关，注意位置。
		mutable YYUTMutexLock mutex_;
		YYUTCondition not_empty_;
		std::deque<T> queue_;
	};
}
#endif
#ifndef __YYUTTHREAD_H__
#define __YYUTTHREAD_H__
#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTAotmic.h"
#include <boost/noncopyable.hpp>
#include <functional>
namespace YYUT
{
	struct YYUTThreadException:virtual std::exception,virtual boost::exception{};
	
	class YYUTThread:boost::noncopyable
	{
	public:
		typedef std::function<void()> ThreadFunc;	
		explicit YYUTThread(const ThreadFunc &,const string &name=string());
		explicit YYUTThread(ThreadFunc&&,const string &name=string());
		~YYUTThread();
		void Start();
		void Join();
		bool Started() const{return started_;};
		operator HANDLE() const
		{
			return thread_handle_;
		}
		const string & Name() const { return name_;}
		static long ThreadCount() { return count_;}
	private:
		void SetDefaultName();
		bool started_;
		bool joined_;
		ThreadFunc func_;
		static volatile long count_;
		HANDLE thread_handle_;
		string name_;
		unsigned int thread_id_;
	};
	struct YYUTThreadData
	{
		typedef YYUTThread::ThreadFunc ThreadFunc;
		ThreadFunc func_;
		string name_;
		YYUTThreadData(const ThreadFunc& func,const string &name)
			:func_(func),name_(name)
		{}
		void RunThread()
		{
			try
			{
				func_();
			}
			catch(YYUTMutiThreadException & e)
			{
				UNREFERENCED_PARAMETER(e);
#if defined( DEBUG ) || defined( _DEBUG )
				std::cout<<"Thread Run Error,Thread "<<std::endl;
				std::cout<<boost::current_exception_diagnostic_information();
#endif
			}
			catch(...)
			{
				cout<<"Unexpected exception in thread :"<<name_<<endl;
			}
		}
		static unsigned int __stdcall StartThread(void *obj)
		{
		    YYUTThreadData* data=static_cast<YYUTThreadData*>(obj);
			data->RunThread();
			delete data;
			return 0;
		}
	};

}





#endif
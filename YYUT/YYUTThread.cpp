#include "stdafx.h"
#include "YYUTThread.h"
#include <iosfwd>
#include <process.h>



volatile long YYUT::YYUTThread::count_=0;

YYUT::YYUTThread::YYUTThread(const ThreadFunc &func,const string &name/*=string()*/)
	:started_(false),joined_(false),thread_handle_(INVALID_HANDLE_VALUE),func_(func),name_(name),thread_id_(0)
{
	SetDefaultName();
}

YYUT::YYUTThread::YYUTThread(ThreadFunc&&func,const string &name/*=string()*/)
	:started_(false),joined_(false),thread_handle_(INVALID_HANDLE_VALUE),func_(std::move(func)),name_(name),thread_id_(0)
{
	SetDefaultName();
}

void YYUT::YYUTThread::SetDefaultName()
{
	int num=count_;
	std::stringstream ss;
	ss<<"Thread"<<num;
	ss>>name_;
}

YYUT::YYUTThread::~YYUTThread()
{
	::CloseHandle(thread_handle_);
}
;
void YYUT::YYUTThread::Start()
{
	assert(!started_);
	started_=true;
	YYUTThreadData *data=new YYUTThreadData(func_,name_);
	typedef unsigned int( __stdcall *thread_fun_type )( void * );
	if((thread_handle_=(HANDLE)::_beginthreadex(nullptr,0,
	reinterpret_cast<thread_fun_type>(&YYUTThreadData::StartThread),
		(void*)data,0,&thread_id_))==INVALID_HANDLE_VALUE)
	{
		started_=false;
		delete data;
		BOOST_THROW_EXCEPTION(YYUTThreadException()<<err_str("Create Thread failed!")<<err_hr(GetLastError()));
	}
}

void YYUT::YYUTThread::Join()
{
	assert(started_);
	DWORD dw=::WaitForSingleObject(thread_handle_,INFINITE);
	switch(dw)
	{
	case WAIT_OBJECT_0:
		return;
	case WAIT_TIMEOUT:
		BOOST_THROW_EXCEPTION(YYUTThreadException()<<err_str("wait time out"));
	case WAIT_FAILED:
		BOOST_THROW_EXCEPTION(YYUTThreadException()<<err_str("Unexpected exception!"));
	default:
		return;
	}
	return ;
}

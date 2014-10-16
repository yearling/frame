#include "stdafx.h"
#include "YYUTUtility.h"


namespace YYUT
{
	void YYUT::ExceptionTraceInformation(boost::exception &e)
	{
		string inside_information=boost::current_exception_diagnostic_information();
		if(inside_information==string("No diagnostic information available."))
			inside_information.clear();
		string *info=boost::get_error_info<err_stack_info>(e);	
		if(info)
		{
			if(!inside_information.empty())
				*info+=inside_information;
		}
		else
		{
			if(!inside_information.empty())
				e<<err_stack_info(inside_information);
		}
	}
}



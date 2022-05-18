/********************************************************************
	created:	2008/09/20
	created:	20:9:2008   17:31
	file ext:	cpp
	author:		Guo Wei

*********************************************************************/
#include "stdafx.h"

#include "orsXExe.h"

//进度，在0.0-1.0之间
bool orsXProcessMsg::process(double status)
{
	_getPlatform()->logPrint(ORS_LOG_INFO, _T("process %.1lf%%"), status*100 );

	return true;
}


#include <stdarg.h>


//消息打印
int orsXProcessMsg::logPrint(orsLogLEVEL loglevel, const orsChar *fmt, ...)
{
	orsChar buf[ORS_MAX_LOG_ITEM_SIZE];

	int count;
	va_list args;
	va_start(args,fmt);
	count = vsprintf(buf,fmt,args);
	va_end(args);

	return _getPlatform()->logPrint(loglevel,  _T("%s"), buf);
}

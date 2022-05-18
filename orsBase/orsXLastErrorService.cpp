/********************************************************************
	created:	2008/09/24
	created:	24:9:2008   0:25
	filename: 	G:\OpenRS\SVN\openRS\desktop\src\orsBase\orsILastErrorServiceImp.cpp
	file path:	G:\OpenRS\SVN\openRS\desktop\src\orsBase
	file base:	orsILastErrorServiceImp
	file ext:	cpp
	author:		Guowei

	purpose:	最近错误服务
*********************************************************************/
#include "stdafx.h"

#include "orsXLastErrorService.h"

ors_int32 orsXLastErrorService::getLastErrorID()
{
	return m_uid;
}

ors_int32 orsXLastErrorService::getLastErrorTime()
{
	return m_time;
}


ors_string orsXLastErrorService::getLastErrorDescribe()
{
	return m_describe;
}



#include <time.h>

#ifndef _WINDOWS

ors_int32 GetTickCount()
{
    //time_t t = gettime();

}

#endif

void orsXLastErrorService::setErrorInfo(ors_int32 uid,ors_string describe)
{
	m_uid = uid;
	m_describe = describe;

#ifdef _WINDOWS
	m_time = GetTickCount();
#endif

}



void orsXLastErrorService::clearErrorStatus()
{
	m_uid = ORS_OK;

	m_describe = "No Error";
}
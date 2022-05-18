#ifndef _ORS_LAST_ERROR_SERVICE_H_
#define _ORS_LAST_ERROR_SERVICE_H_


#include "orsBase/orsIService.h"

#define ORS_OK		0
#define ORS_ERROR	1


//最近错误查看服务
interface orsILastErrorService : public orsIService
{
public:
	//得到最近错误的ID
	virtual ors_int32 getLastErrorID() = 0;

	//得到最最近错误发生时间戳
	virtual ors_int32 getLastErrorTime() = 0;

	//得到详细描述
	virtual ors_string getLastErrorDescribe() = 0;

	//输入错误，时间戳将自动加上
	virtual void setErrorInfo(ors_int32 uid, ors_string describe) = 0;

	virtual void clearErrorStatus() = 0;

	//接口名称
	ORS_INTERFACE_DEF( orsIService, _T("lastError" ))
};

#define ORS_SERVICE_LASTERROR _T("ors.service.lastError")


// 获取几何服务的宏定义
orsILastErrorService *getLastErrorService();
void setErrorStatus();
void clearErrorStatus();
ors_int32 getLastErrorID();

#define ORS_GET_LAST_ERROR_SERVICE_IMPL()\
	static orsILastErrorService *s_lastErrorService = NULL;\
	orsILastErrorService *getLastErrorService()\
{\
	if( NULL != s_lastErrorService )\
		return s_lastErrorService;\
	\
	s_lastErrorService = ORS_PTR_CAST(orsILastErrorService, getPlatform()->getService( ORS_SERVICE_LASTERROR) );\
	\
	return s_lastErrorService;\
};\
\
ors_int32 getLastErrorID()\
{\
	return getLastErrorService()->getLastErrorID();\
};\
\
void setErrorStatus()\
{\
	getLastErrorService()->setErrorInfo( ORS_ERROR, "Error");\
};\
\
void clearErrorStatus()\
{\
	getLastErrorService()->clearErrorStatus();\
};


#endif

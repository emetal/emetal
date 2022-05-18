#ifndef _ORS_LAST_ERROR_SERVICE_H_
#define _ORS_LAST_ERROR_SERVICE_H_


#include "orsBase/orsIService.h"

#define ORS_OK		0
#define ORS_ERROR	1


//�������鿴����
interface orsILastErrorService : public orsIService
{
public:
	//�õ���������ID
	virtual ors_int32 getLastErrorID() = 0;

	//�õ������������ʱ���
	virtual ors_int32 getLastErrorTime() = 0;

	//�õ���ϸ����
	virtual ors_string getLastErrorDescribe() = 0;

	//�������ʱ������Զ�����
	virtual void setErrorInfo(ors_int32 uid, ors_string describe) = 0;

	virtual void clearErrorStatus() = 0;

	//�ӿ�����
	ORS_INTERFACE_DEF( orsIService, _T("lastError" ))
};

#define ORS_SERVICE_LASTERROR _T("ors.service.lastError")


// ��ȡ���η���ĺ궨��
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

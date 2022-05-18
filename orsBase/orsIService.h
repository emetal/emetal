#ifndef _ORS_SERVICE_H_
#define _ORS_SERVICE_H_

#include "orsBase/orsIObject.h"

interface orsIPlatform;

interface orsIService : public orsIObject
{
public:
	//����ʱ������
	virtual bool startup( orsIPlatform *platform ) = 0;

	//�ر�ʱ������
	virtual void shutdown() = 0;

	//�Ƿ������ɹ�
	virtual bool isok() = 0;

	//����
	ORS_INTERFACE_DEF(orsIObject, _T("service"))
};

#define ORS_SERVICE _T("ors.service")

#endif


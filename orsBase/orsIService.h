#ifndef _ORS_SERVICE_H_
#define _ORS_SERVICE_H_

#include "orsBase/orsIObject.h"

interface orsIPlatform;

interface orsIService : public orsIObject
{
public:
	//启动时被调用
	virtual bool startup( orsIPlatform *platform ) = 0;

	//关闭时被调用
	virtual void shutdown() = 0;

	//是否启动成功
	virtual bool isok() = 0;

	//名称
	ORS_INTERFACE_DEF(orsIObject, _T("service"))
};

#define ORS_SERVICE _T("ors.service")

#endif


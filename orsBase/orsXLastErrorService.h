#ifndef ORS_LAST_ERROR_SERVICE_IMP_H
#define ORS_LAST_ERROR_SERVICE_IMP_H

#include "orsBase/orsIPlatform.h"
#include "orsBase/orsILastErrorService.h"

//最近错误查看服务
class orsXLastErrorService : public orsILastErrorService, public orsObjectBase
{
public:
	//启动时被调用
	virtual bool startup(orsIPlatform *platform){return true;};

	//关闭时被调用
	virtual void shutdown(){return ;}

	//是否启动成功
	virtual bool isok(){return true;}

	//得到最近错误的ID
	virtual ors_int32 getLastErrorID();

	//得到最最近错误发生时间戳
	virtual ors_int32 getLastErrorTime();

	//得到详细描述
	virtual ors_string getLastErrorDescribe();

	//输入错误，时间戳将自动加上
	virtual void setErrorInfo( ors_int32 uid, ors_string describe);

	virtual void clearErrorStatus();

	//Object接口宏实现
	ORS_OBJECT_IMP1(orsXLastErrorService,orsILastErrorService, "default", "lastErrorService")

public:
	ors_uint32	m_uid;
	ors_string	m_describe;
	ors_int32	m_time;
};


#endif

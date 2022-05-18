#ifndef ORS_IPLATFORM_IMP_H
#define ORS_IPLATFORM_IMP_H

#include <string>
#include <map>

#include "orsCommon.h"
#include "orsBase/orsIPlatform.h"
#include "orsBase/orsIConnectableObject.h"

#include "orsBase/orsUtil.h"

#include "orsXProperty.h"

// 
// #if defined kyLINUX64
// #include <vector>
// using namespace std;
// #define orsArray std::vector
// #endif

class orsXPlatform: public orsIPlatform
{
	typedef std::map<stdString , ref_ptr<orsIService> > orsServiceMap;

public:
	orsXPlatform(){
		int i=0;
	};

	//得到版本号
	virtual const orsChar *getVersion(){return ORS_PLATFORM_VERSION;}

	//注册服务（每个serviceName只对应一个表示）
	virtual bool registerService(const orsChar * serviceName, orsIService *service);

	//反注册服务
	virtual void unregisterService(const orsChar * serviceName);

	//得到所有的服务
	virtual orsArray<orsIService *> getAllService();

	//通过服务名得到服务单例
	virtual orsIService *getService(const orsChar * serviceName);

	//得到服务列表
	virtual orsArray<ors_string> getServiceNames();

	//
	// 得到几个固定服务（加快访问速度), 改为普通指针，这singleton服务用户不能释放
	// 全局日志服务
	virtual orsILogService *getLogService();

	//得到注册服务
	virtual orsIRegisterService *getRegisterService();

	//得到错误查看服务
	virtual orsILastErrorService *getLastErrorService();

public:
	// 通过 注册的对象名称 创建对象, 调用者需要用ref_ptr保存
	virtual orsIObject *createObject( const orsChar * objName  );

	// 创建属性对象
	virtual orsIProperty *createProperty();

	// 根据 “处理对象” 自动创建 对应“处理节点”的对象
	virtual orsIConnectableObject *CreateConnectableObject( orsIObject *logicObject);

public:
	//打印日志
	int logPrint(orsLogLEVEL loglevel, const orsChar *fmt, ...);

	// 取给点类型的无效值
	ors_float64 getDefaultNULL(orsDataTYPE type)
	{
		return orsGetDefaultNULL( type );
	}

	// 取类型的的范围
	ors_float64 getDefaultMin(orsDataTYPE type)
	{
		return orsGetDefaultMin( type);
	}

	ors_float64 getDefaultMax(orsDataTYPE type)
	{
		return orsGetDefaultMax( type );
	}

	//辅助方法，得到类型的字节数sizeof
	int getSizeOfType(orsDataTYPE type)
	{
		return orsGetSizeOfType( type );
	}

	void *malloc_( size_t size ) 
	{
		return malloc( size );
	}

	void free_( void *ptr ) 
	{
		free( ptr );	
	}

public:
	//启动平台
	bool startup(ors_string &errorInfo);

	//关闭平台，卸载资源
	void shutdown();

protected:
	//服务对应表
	orsServiceMap m_services;

	//缺省log服务
	ref_ptr<orsILogService> m_logService;

	//缺省注册服务
	ref_ptr<orsIRegisterService> m_registerService;

	//最近错误服务
	ref_ptr<orsILastErrorService> m_lastErrorService;

	//是否启动
	bool m_bStartup;
};

#endif

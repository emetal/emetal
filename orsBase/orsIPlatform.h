#ifndef _ORS_PLATFORM_INTERFACE_H
#define _ORS_PLATFORM_INTERFACE_H

#	if defined _WIN32 &&  _MSC_VER > 1000
#   if !defined(_DLL) || !defined(_MT)
#       error "Only multi-threaded C++ runtime DLL libraries can be used with OpenRS!"
#   endif
#	endif

#include <assert.h>
#include <stdio.h>
#include "warningdisable.h"

#include "orsBase/orsTypedef.h"
#include "orsBase/orsConstDef.h"
#include "orsBase/orsIObject.h"

interface orsIPlatform;
//interface orsIObject;
interface orsIPropertySerialize;
interface orsIProcessMsg;
interface orsIService;
interface orsILogService;
interface orsIRegisterService;
interface orsILastErrorService;
interface orsIProperty;
interface orsIConnectableObject;

// #include "orsIProperty.h"
// #include "orsILogService.h"
// #include "orsILastErrorService.h"
// #include "orsIRegisterService.h"
// 
// #include "orsIConnectableObject.h"

#ifdef ORS_PLATFORM_LINUX
#define ORS_PLATFORM_VERSION	"1.6.3"
#else 
#define ORS_PLATFORM_VERSION	"1.6.3"
#endif

enum orsLicenseServiceTYPE	{
	orsLstNodeLOCK,		// 本地
		orsLstFLOATING			// 浮动
};




//平台服务，注意服务名不等于服务本身的名字，服务本身名称是UUID，而服务名代表功能模块标识
interface orsIPlatform
{
public:
	//得到版本号
	virtual const orsChar *getVersion() = 0;

// 	//启动平台
// 	virtual const orsChar *startup( bool &ret ) = 0;
// 
// 	//关闭平台，卸载资源
// 	virtual void shutdown() = 0;

public:
	//注册服务（每个moudle只对应一个表示
	//needStartup是给插件注册服务的，因为在插件启动前，平台已经循环调用其内部服务启动了
	//插件必须用needstartup标签为true
	virtual bool registerService( const orsChar * service_name, orsIService *service) = 0;

// 	//得到服务列表
// 	virtual orsArray<const orsChar *> getServiceNames() = 0;
	//通过服务名得到服务单例
	virtual orsIService *getService( const orsChar * serviceName) = 0;

	//得到几个固定服务（加快访问速度）
	//全局日志服务
	virtual orsILogService *getLogService() = 0;
	//得到注册服务
	virtual orsIRegisterService *getRegisterService() = 0;
	//得到错误查看服务
	virtual orsILastErrorService *getLastErrorService() = 0;

public:
	// 通过 注册的对象名称 创建对象
	virtual orsIObject *createObject( const orsChar * objIdStr ) = 0;

	// 创建属性对象
	virtual orsIProperty *createProperty() = 0;

	// 根据 “处理对象” 自动创建 对应“处理节点”的对象
	virtual orsIConnectableObject *CreateConnectableObject( orsIObject *logicObject) = 0;


	// utilities
public:
	// 可变参数日志
	virtual int logPrint(orsLogLEVEL loglevel,  const orsChar *fmt, ...) = 0;

	// 取给点类型的无效值
	virtual ors_float64 getDefaultNULL(orsDataTYPE type) = 0;
	// 取类型的的范围
	virtual ors_float64 getDefaultMin(orsDataTYPE type) = 0;
	virtual ors_float64 getDefaultMax(orsDataTYPE type) = 0;

	//辅助方法，得到类型的字节数sizeof
	virtual int getSizeOfType(orsDataTYPE type) = 0;

	virtual void *malloc_( size_t size ) = 0;
	virtual void free_( void *ptr ) = 0;
	
};


// 取平台对象
orsIPlatform* getPlatform();

#define orsMalloc( size )	getPlatform()->malloc_(size)
#define orsFree( ptr )	getPlatform()->free_( ptr )

template<class T>
T* ors_create_object(T* nullval, const orsChar * id, orsIPlatform *pPlatform,  const orsChar * interfaceName)
{
	assert( NULL != pPlatform );
//	printf("ID:%s\n", id);
	orsIObject *obj = pPlatform->createObject(id);
	if(obj != NULL)
		return (T*)obj->queryInterface(interfaceName);
	else
		return NULL;
}

#define ORS_CREATE_OBJECT(T, id) (ors_create_object((T*)NULL, id, getPlatform(), #T))


#endif

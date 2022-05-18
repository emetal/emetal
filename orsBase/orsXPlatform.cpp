/********************************************************************
	created:	2008/09/20
	created:	20:9:2008   18:35
	filename: 	E:\OpenRS\DESKTOP\src\orsBase\orsIPlatformImp.cpp
	file path:	E:\OpenRS\DESKTOP\src\orsBase
	file base:	orsIPlatformImp
	file ext:	cpp
	author:		Guo Wei

	purpose:	平台实现

*********************************************************************/
#include "stdafx.h"

#include <vector>

#include "orsBase/orsConstDef.h"

#include "orsBase/orsIRegisterService.h"
#include "orsBase/orsILastErrorService.h"

#include "orsXPlatform.h"
#include "orsXProperty.h"


//反注册服务
void orsXPlatform::unregisterService( const orsChar * serviceName)
{
	orsServiceMap::iterator iter = m_services.find( serviceName );
	if(iter != m_services.end())
	{
		ref_ptr<orsIService> service = iter->second;
		service->shutdown();
		m_services.erase(iter);
	}
}

orsArray<ors_string> orsXPlatform::getServiceNames()
{
	orsArray<ors_string> servicelist;
	orsServiceMap::iterator iter = m_services.begin();
	while(iter != m_services.end()){
		servicelist.push_back(iter->first.c_str());
		iter++;
	}
	return servicelist;
}

//得到所有的服务
orsArray<orsIService *> orsXPlatform::getAllService()
{
	orsArray<orsIService *> servicelist;
	orsServiceMap::iterator iter = m_services.begin();

	while(iter != m_services.end()){
		servicelist.push_back( iter->second.get() );
		iter++;
	}
	return servicelist;
}

orsIService *orsXPlatform::getService(const orsChar * serviceName)
{
	orsServiceMap::iterator iter = m_services.find( serviceName );
	if(iter == m_services.end()) {
		logPrint( ORS_LOG_ERROR,  _T("Can not get service: %s"), serviceName );
		return NULL;
	}
	else{
		return iter->second.get();
	}
}

orsILogService *orsXPlatform::getLogService()
{
	if( NULL == m_logService.get() )
		m_logService = (orsILogService*)getService(ORS_logServiceName);

	return m_logService.get();
}

orsIRegisterService *orsXPlatform::getRegisterService()
{
	if(  NULL == m_registerService.get() )
		m_registerService = (orsIRegisterService*)getService(ORS_registerServiceName);

	return m_registerService.get();
}

orsILastErrorService *orsXPlatform::getLastErrorService()
{
	if(  NULL == m_lastErrorService.get() )
	 	m_lastErrorService = (orsILastErrorService*)getService(ORS_lastErrorServiceName);

	return m_lastErrorService.get();
}



orsIObject *orsXPlatform::createObject(const orsChar * name)
{
	return m_registerService->createObject(name); 
}

orsIProperty *orsXPlatform::createProperty()
{
	return new orsXProperty();
}


#define _ORS_CREATE_OBJECT(T, id) (ors_create_object((T*)NULL, id, _getPlatform(), #T))

orsIConnectableObject *orsXPlatform::CreateConnectableObject( orsIObject *logicObject )
{
	orsIRegisterService *registerService = _getPlatform()->getRegisterService();

	orsArray<ref_ptr<orsIObjectDesc> > objDescs =
		registerService->getObjectDescsByInterface( "orsIConnectableObject" );

	// 逐个试探
	for(unsigned int i=0;i<objDescs.size();i++)
	{
		orsIObjectDesc *desc = objDescs[i].get();
		orsIConnectableObject *connectObj =
			_ORS_CREATE_OBJECT( orsIConnectableObject, desc->getID() );

		if(connectObj != NULL){
			if( connectObj->setLogicObject( logicObject ) )
				return connectObj;
		}
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////////

bool orsXPlatform::registerService( const orsChar * serviceName, orsIService *service)
{
	unregisterService( serviceName );
	m_services.erase( serviceName );
	m_services.insert( orsServiceMap::value_type(serviceName, service));

	if( m_logService.get() ) {
		orsString msg = service->getClassID() + ": registered";
//		m_logService->debug( "orsBase", msg.c_str() );
	}
	else	{
// 		orsString msg = service->getClassID() + ": registered";
// 
// 		printf("debug: %s\n", msg.c_str() );
	}

	bool needStartup = m_bStartup;

	//判断之前是否已经有该服务的指针
	orsServiceMap::iterator iter;
	for (iter = m_services.begin();iter != m_services.end();iter++)
	{
		ref_ptr<orsIService> oldservice = iter->second;
		if(oldservice == service)
		{
			needStartup = false;
			break;
		}
	}

	if(needStartup){
		//需要错误处理
		if( !service->startup( this ) ) {
			orsString error_str = service->getClassID() + ": startup failed";

			m_logService->warn( "orsBase", error_str.c_str() );

			getLastErrorService()->setErrorInfo(0,  error_str.c_str() );
			return false;
		}
		else	{
			orsString msg = service->getClassID() + ": startup";
			m_logService->debug( "orsBase", msg.c_str() );
		}
	}

	//得到固定的服务名
	//if( NULL == m_logService.get() )
	m_logService = (orsILogService*)getService(ORS_logServiceName);

// 	m_registerService = (orsIRegisterService*)getService(ORS_registerServiceName);
// 	m_lastErrorService = (orsILastErrorService*)getService(ORS_lastErrorServiceName);

	return true;
}

void LoadSystemModules();

bool orsXPlatform::startup(ors_string &errorInfo)
{
	//得到固定的服务名
	m_logService = (orsILogService*)getService(ORS_logServiceName);
	m_registerService = (orsIRegisterService*)getService(ORS_registerServiceName);
	m_lastErrorService = (orsILastErrorService*)getService(ORS_lastErrorServiceName);

	if(m_logService == NULL)
	{
		errorInfo = "log service can not be found";
		return false;
	}

	if(m_registerService == NULL)
	{
		errorInfo = "register service can not be found";
		return false;
	}

	if(m_lastErrorService == NULL)
	{
		errorInfo = "last error service can not be found";
		return false;
	}

	//最先启动日志服务
	m_logService->startup(this);

	// 	//////////////////////////////////////////////////////////////////////////
	// 	// 对于注册过程可能无法监视
	// 	
	// 	m_logService->loadExtension( this );
	
	LoadSystemModules();

	//循环调用
	std::vector<orsIService *> servicesToBeStart;

	orsServiceMap::iterator iter = m_services.begin();
	while(iter != m_services.end()) {
		orsIService *service = iter->second.get();

		// 不是logService
		if( service != m_logService.get())
		{
			bool needStartup = true;

			// 已经在列表中 ？
			for (unsigned int i=0; i< servicesToBeStart.size(); i++)
			{
				if( service == servicesToBeStart[i] )
				{
					needStartup = false;
					break;
				}
			}

			// 放入列表
			if( needStartup )
				servicesToBeStart.push_back(service);
		}
		iter++;
	}

	for ( unsigned int i=0; i<servicesToBeStart.size(); i++)
	{
		ref_ptr<orsIService> service = servicesToBeStart[i];

		//需要错误处理
		if( !service->startup( this ) ) {
			orsString error_str = service->getClassID() + ": startup failed";
			m_logService->info( "orsBase", error_str.c_str() );
			return false;
		}
		else	{
			orsString msg = service->getClassID() + ": startup";
			m_logService->debug( "orsBase", msg.c_str() );
		}
	}

	m_bStartup = true;
	return true;
}


void unLoadSystemModules();
void orsXPlatform::shutdown()
{
	orsArray< ref_ptr<orsIService> > stopservices;

//	m_logService->unloadExtension();

	orsServiceMap::iterator iter = m_services.begin();
	while(iter != m_services.end()){
		ref_ptr<orsIService> service = iter->second;
		if(service.get() != m_logService.get())
		{
			bool needShutdown = true;
			for ( unsigned int i=0;i<stopservices.size();i++)
			{
				if(service == stopservices[i])
				{
					needShutdown = false;
					break;
				}
			}

			if(needShutdown)
				stopservices.push_back(service);
		}
		iter++;
	}

	for ( unsigned int i=0;i<stopservices.size();i++)
	{
		ref_ptr<orsIService> service = stopservices[i];
		if(service.get() != m_registerService.get() &&
			service.get() != m_logService.get())
			service->shutdown();
	}

	// 必须在插件释放前，释放插件内的日志对象
	m_logService->shutdown();

	stopservices.clear();
	m_services.clear();

	// 释放插件对象，卸载插件动态库
	m_registerService->shutdown();
	
	//////
	unLoadSystemModules();
	//////////////////////////////////////////////////////////////////////////

	m_registerService = NULL;
	m_lastErrorService = NULL;
	m_logService = NULL;

	m_bStartup = false;
}


#include <stdarg.h>

int orsXPlatform::logPrint(orsLogLEVEL loglevel, const orsChar *fmt, ...)
{
	orsIPlatform *platform = this;

	char buf[ORS_MAX_LOG_ITEM_SIZE];

	int count;
	va_list args;
	va_start(args,fmt);
	count = vsprintf(buf,fmt,args);
	va_end(args);

	orsILogService *service =  platform->getLogService();
	if(service != NULL)
	{
		switch(loglevel)
		{
		case ORS_LOG_WARNING:
			service->warn("orsBase",buf);
			break;
		case ORS_LOG_INFO:
			service->info("orsBase",buf);
			break;
		case ORS_LOG_ERROR:
			service->error("orsBase",buf);
			break;
		case ORS_LOG_FATAL:
			service->fatal("orsBase",buf);
			break;
		case ORS_LOG_DEBUG:
			service->debug("orsBase",buf);
			break;
		default:
			break;
		}
	}

	return count;
}



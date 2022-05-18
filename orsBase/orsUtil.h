/************************************************************************/
/* orsUtil.h 
                                                                     */
/************************************************************************/

#ifndef OPENRS_ORS_UTIL_H
#define OPENRS_ORS_UTIL_H

//只提供给系统开发者，不提供给插件开发者，系统DLL唯一导出，系统开发者通过平台指针调用

#include "orsBase/orsIPlatform.h"
#include "orsBase/orsString.h"

//启动openRS主体
extern "C" ORS_BASE_API orsIPlatform* orsInitialize(ors_string &errorinfo, bool bInteractiveLog = false );

//关闭openRS主体
extern "C" ORS_BASE_API void orsUninitialize();

//////////////////////////////////////////////////////////////////////////

#include "orsBase/orsILogService.h"

#define OrsLOG_DEBUG( strModule, message) { \
	getLogService()->debug( strModule, message, __FILE__, __LINE__); }

#define OrsLOG_INFO(strModule, message) { \
	getLogService()->info( strModule, message, __FILE__, __LINE__); }

#define OrsLOG_WARN(strModule, message) { \
	getLogService()->warn( strModule, message, __FILE__, __LINE__); }

#define OrsLOG_ERROR(strModule, message) { \
	getLogService()->error( strModule, message, __FILE__, __LINE__); }

#define OrsLOG_FATAL(strModule, message) { \
	getLogService()->fatal( strModule, message, __FILE__, __LINE__); }
	

#endif

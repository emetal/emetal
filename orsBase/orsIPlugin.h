#ifndef _ORS_PLUGIN_DEF_H_
#define _ORS_PLUGIN_DEF_H_

#include "orsTypedef.h"

#define ORS_PLUGIN_INSTANCE_GET			"orsGetPluginInstance"
#define ORS_PLUGIN_MAJOR_VERSION_GET	"orsGetPlatformMajorVersion"

// 插件导出函数声明
#define PLUGIN_API ORS_EXPORT


interface orsIObject;

// 对象创建函数指针类型
typedef orsIObject* (*ORS_OBJECT_CREATOR_FUN)(bool bForRegister);

enum orsLicenseTYPE	{
	orsltFREE,				// 免费，无锁
	orsltBUSINESS			// 商业，无license时为demo版，可能在影像上打上水印等
};

interface orsIPlatform;

interface orsIPlugin
{
	// plugin id, "org.openRS...."
	virtual const orsChar *getID() = 0;
	
	// plugin mame
	virtual	const orsChar *getName() = 0;

	// 得到插件版本号
	virtual const orsChar *getVersion() = 0;

	// "org.openRS...."
	virtual const orsChar *getProvider() = 0;

	//插件初始化,用户可以在该函数内，注册插件对象
	virtual bool initialize(orsIPlatform*  platform) = 0;

	//卸载插件时，做相关clear工作
	virtual void finalize() = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual orsLicenseTYPE GetLicenseType()	{	return orsltFREE; };

	// 从插件取加密锁的ID, 加密、解密由插件完成，此函数在license服务器上调用
	virtual bool GetLockID( const char *cryptKey, char cryptedLockID[2048] ) 
	{ 
		cryptedLockID[0]=0;
		return false;
	};
};

#endif

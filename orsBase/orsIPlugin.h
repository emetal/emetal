#ifndef _ORS_PLUGIN_DEF_H_
#define _ORS_PLUGIN_DEF_H_

#include "orsTypedef.h"

#define ORS_PLUGIN_INSTANCE_GET			"orsGetPluginInstance"
#define ORS_PLUGIN_MAJOR_VERSION_GET	"orsGetPlatformMajorVersion"

// ���������������
#define PLUGIN_API ORS_EXPORT


interface orsIObject;

// ���󴴽�����ָ������
typedef orsIObject* (*ORS_OBJECT_CREATOR_FUN)(bool bForRegister);

enum orsLicenseTYPE	{
	orsltFREE,				// ��ѣ�����
	orsltBUSINESS			// ��ҵ����licenseʱΪdemo�棬������Ӱ���ϴ���ˮӡ��
};

interface orsIPlatform;

interface orsIPlugin
{
	// plugin id, "org.openRS...."
	virtual const orsChar *getID() = 0;
	
	// plugin mame
	virtual	const orsChar *getName() = 0;

	// �õ�����汾��
	virtual const orsChar *getVersion() = 0;

	// "org.openRS...."
	virtual const orsChar *getProvider() = 0;

	//�����ʼ��,�û������ڸú����ڣ�ע��������
	virtual bool initialize(orsIPlatform*  platform) = 0;

	//ж�ز��ʱ�������clear����
	virtual void finalize() = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual orsLicenseTYPE GetLicenseType()	{	return orsltFREE; };

	// �Ӳ��ȡ��������ID, ���ܡ������ɲ����ɣ��˺�����license�������ϵ���
	virtual bool GetLockID( const char *cryptKey, char cryptedLockID[2048] ) 
	{ 
		cryptedLockID[0]=0;
		return false;
	};
};

#endif

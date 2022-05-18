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
	orsLstNodeLOCK,		// ����
		orsLstFLOATING			// ����
};




//ƽ̨����ע������������ڷ���������֣�������������UUID����������������ģ���ʶ
interface orsIPlatform
{
public:
	//�õ��汾��
	virtual const orsChar *getVersion() = 0;

// 	//����ƽ̨
// 	virtual const orsChar *startup( bool &ret ) = 0;
// 
// 	//�ر�ƽ̨��ж����Դ
// 	virtual void shutdown() = 0;

public:
	//ע�����ÿ��moudleֻ��Ӧһ����ʾ
	//needStartup�Ǹ����ע�����ģ���Ϊ�ڲ������ǰ��ƽ̨�Ѿ�ѭ���������ڲ�����������
	//���������needstartup��ǩΪtrue
	virtual bool registerService( const orsChar * service_name, orsIService *service) = 0;

// 	//�õ������б�
// 	virtual orsArray<const orsChar *> getServiceNames() = 0;
	//ͨ���������õ�������
	virtual orsIService *getService( const orsChar * serviceName) = 0;

	//�õ������̶����񣨼ӿ�����ٶȣ�
	//ȫ����־����
	virtual orsILogService *getLogService() = 0;
	//�õ�ע�����
	virtual orsIRegisterService *getRegisterService() = 0;
	//�õ�����鿴����
	virtual orsILastErrorService *getLastErrorService() = 0;

public:
	// ͨ�� ע��Ķ������� ��������
	virtual orsIObject *createObject( const orsChar * objIdStr ) = 0;

	// �������Զ���
	virtual orsIProperty *createProperty() = 0;

	// ���� ��������� �Զ����� ��Ӧ������ڵ㡱�Ķ���
	virtual orsIConnectableObject *CreateConnectableObject( orsIObject *logicObject) = 0;


	// utilities
public:
	// �ɱ������־
	virtual int logPrint(orsLogLEVEL loglevel,  const orsChar *fmt, ...) = 0;

	// ȡ�������͵���Чֵ
	virtual ors_float64 getDefaultNULL(orsDataTYPE type) = 0;
	// ȡ���͵ĵķ�Χ
	virtual ors_float64 getDefaultMin(orsDataTYPE type) = 0;
	virtual ors_float64 getDefaultMax(orsDataTYPE type) = 0;

	//�����������õ����͵��ֽ���sizeof
	virtual int getSizeOfType(orsDataTYPE type) = 0;

	virtual void *malloc_( size_t size ) = 0;
	virtual void free_( void *ptr ) = 0;
	
};


// ȡƽ̨����
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

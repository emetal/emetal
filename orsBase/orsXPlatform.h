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

	//�õ��汾��
	virtual const orsChar *getVersion(){return ORS_PLATFORM_VERSION;}

	//ע�����ÿ��serviceNameֻ��Ӧһ����ʾ��
	virtual bool registerService(const orsChar * serviceName, orsIService *service);

	//��ע�����
	virtual void unregisterService(const orsChar * serviceName);

	//�õ����еķ���
	virtual orsArray<orsIService *> getAllService();

	//ͨ���������õ�������
	virtual orsIService *getService(const orsChar * serviceName);

	//�õ������б�
	virtual orsArray<ors_string> getServiceNames();

	//
	// �õ������̶����񣨼ӿ�����ٶ�), ��Ϊ��ָͨ�룬��singleton�����û������ͷ�
	// ȫ����־����
	virtual orsILogService *getLogService();

	//�õ�ע�����
	virtual orsIRegisterService *getRegisterService();

	//�õ�����鿴����
	virtual orsILastErrorService *getLastErrorService();

public:
	// ͨ�� ע��Ķ������� ��������, ��������Ҫ��ref_ptr����
	virtual orsIObject *createObject( const orsChar * objName  );

	// �������Զ���
	virtual orsIProperty *createProperty();

	// ���� ��������� �Զ����� ��Ӧ������ڵ㡱�Ķ���
	virtual orsIConnectableObject *CreateConnectableObject( orsIObject *logicObject);

public:
	//��ӡ��־
	int logPrint(orsLogLEVEL loglevel, const orsChar *fmt, ...);

	// ȡ�������͵���Чֵ
	ors_float64 getDefaultNULL(orsDataTYPE type)
	{
		return orsGetDefaultNULL( type );
	}

	// ȡ���͵ĵķ�Χ
	ors_float64 getDefaultMin(orsDataTYPE type)
	{
		return orsGetDefaultMin( type);
	}

	ors_float64 getDefaultMax(orsDataTYPE type)
	{
		return orsGetDefaultMax( type );
	}

	//�����������õ����͵��ֽ���sizeof
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
	//����ƽ̨
	bool startup(ors_string &errorInfo);

	//�ر�ƽ̨��ж����Դ
	void shutdown();

protected:
	//�����Ӧ��
	orsServiceMap m_services;

	//ȱʡlog����
	ref_ptr<orsILogService> m_logService;

	//ȱʡע�����
	ref_ptr<orsIRegisterService> m_registerService;

	//����������
	ref_ptr<orsILastErrorService> m_lastErrorService;

	//�Ƿ�����
	bool m_bStartup;
};

#endif

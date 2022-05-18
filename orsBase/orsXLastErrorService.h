#ifndef ORS_LAST_ERROR_SERVICE_IMP_H
#define ORS_LAST_ERROR_SERVICE_IMP_H

#include "orsBase/orsIPlatform.h"
#include "orsBase/orsILastErrorService.h"

//�������鿴����
class orsXLastErrorService : public orsILastErrorService, public orsObjectBase
{
public:
	//����ʱ������
	virtual bool startup(orsIPlatform *platform){return true;};

	//�ر�ʱ������
	virtual void shutdown(){return ;}

	//�Ƿ������ɹ�
	virtual bool isok(){return true;}

	//�õ���������ID
	virtual ors_int32 getLastErrorID();

	//�õ������������ʱ���
	virtual ors_int32 getLastErrorTime();

	//�õ���ϸ����
	virtual ors_string getLastErrorDescribe();

	//�������ʱ������Զ�����
	virtual void setErrorInfo( ors_int32 uid, ors_string describe);

	virtual void clearErrorStatus();

	//Object�ӿں�ʵ��
	ORS_OBJECT_IMP1(orsXLastErrorService,orsILastErrorService, "default", "lastErrorService")

public:
	ors_uint32	m_uid;
	ors_string	m_describe;
	ors_int32	m_time;
};


#endif

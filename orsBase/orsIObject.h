/******************************************************************************
* $Id: orsIObject.h 2008/10/05 9:58 JWS $
*
* Project:  OpenRS
* Purpose:  ���ڶ�������ü����Ķ���
*
* Author:   JIANG Wanshou, jws@lmars.whu.edu.cn
******************************************************************************
* Copyright (c) 2008, Liesmars, Wuhan University
* History:
*      
****************************************************************************/

#ifndef _ORS_OBJECT_INTERFACE_H
#define _ORS_OBJECT_INTERFACE_H

#include "orsTypedef.h"
#include "orsMatrix.h"

#include "orsArray.h"

#include "orsString.h"


#include "orsRefPtr.h"
#include "orsMacro.h"

interface orsIProperty;

struct orsIObject
{
	// ��������������
protected:
	virtual ~orsIObject(){};

public:	
	//////////////////////////////////////////////////////////////////////////
	//������������������ͨ���̳�������orsObjectBase��ʵ�֣�����Ҫ�Լ�ʵ��
	//������
	virtual void addRef() = 0;
	virtual void release() = 0;

	//////////////////////////////////////////////////////////////////////////
	// ����4����������ͨ��ϵͳ�ṩ�ĺ�ORS_OBJECT_IMP1,ORS_OBJECT_IMP2...��ʵ��
	
	// ����, ������
	virtual ors_string	getClassName()	const	{ return _T("Unknown");	};

	// ��λ�����ID, ���ֶ����Ψһ���, ʵ��ID�ɽӿ�ID�Ͷ���ID���Ӷ���
	// ����������eclipse���ƣ��ɶ���ID�Ĳ�����ֿ��Թ��������
	virtual ors_string	getClassID()	const { return _T("ors"); }

	// �õ�detailed description
	virtual ors_string	getClassDesc() const { return _T("empty");}
	
	// �õ�ʵ�ֵĽӿ�����
	virtual orsArray<ors_string> getInterfaceNames() const = 0;

	// ���ݽӿ���ȡ����ָ��
	virtual void *queryInterface( const orsChar *className)  const = 0;
	
	///////////////////// ���Բ��� ////////////////////////////////////////////
	// �õ�����, �������ڻ�ȡ�ڲ�����, ���л���
	virtual const orsIProperty *getProperty() const	{	return NULL; };

	// ���ڴ�xml�ָ�����״̬��xml->property->object
	virtual bool initFromProperty( orsIProperty *property ) {	return true;	};

	orsIProperty *getProperty()	
	{
		return const_cast<orsIProperty *> ( (static_cast<const orsIObject&>(*this)).getProperty() );
	}; 


// 	//��������
// 	virtual void propertyChangeNotify() {} ;
// 	
// 	virtual bool checkAttr(const orsChar * name,	ors_int16 value, ors_int32 index  ) { return true;	};
// 	virtual bool checkAttr(const orsChar * name,	ors_int32 value, ors_int32 index  ) { return true;	};
// 	virtual bool checkAttr(const orsChar * name,	ors_float64 value, ors_int32 index  )  { return true;	};
// 	virtual bool checkAttr(const orsChar * name,	const orsChar * value, ors_int32 index  )  { return true;	} ;
// 	
// 	// �õ�����Լ��
// 	virtual ors_string getAttrContrainedDescription( const orsChar * name ) const { return _T("empty");}

};



//���ڸ������ü���ʵ��
//�Ƿ�Ӧ�ÿ��Ƕ��߳�֧��???
class orsObjectBase
{
public:
	orsObjectBase(){m_refCount = 0;}
	virtual ~orsObjectBase(){};
	virtual void internalAddRef() {
		m_refCount++;
	}
	
	virtual void internalRelease() {
		m_refCount--;
		if(m_refCount <= 0) 
			delete this;
	}
protected:
	mutable ors_int32 m_refCount;
};


//
// �����������Ϣ
//
interface orsIObjectDesc
{
public:	// for ref_ptr
	virtual void		addRef() = 0;
	virtual void		release() = 0;

public:
	virtual const orsChar *getName() = 0;
	virtual const orsChar *getID() = 0;
	virtual const orsChar *getDesc() = 0;
	virtual const orsChar *getPluginName() = 0;
	virtual const orsArray<orsString> &getInterfaceNames() const = 0;
};


#include "orsIProperty.h"

#endif

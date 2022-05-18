//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIConnectableObject.h 2008/6/07 12:22 JWS $
//
// Project: OpenRS
//
// Purpose: ���ڶ�������Ӷ���
//
// Author: GUO Wei, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIConnectableObject.h,v $
//
// Revision 1.0 date: 2008/6/07 by GUO Wei
// new
//
#ifndef _ORS_CONNECTABLE_OBJECT_H_
#define _ORS_CONNECTABLE_OBJECT_H_

#include "orsBase/orsTypedef.h"

#include "orsBase/orsIObject.h"


//�����Ӷ���ѭ�����ûᷢ���ڴ�й©���ʲ�������ָ��
interface orsIConnectableObject	: public orsIObject
{
public:
	//
//	virtual bool numOfInputAcceptable(orsIConnectableObject* object) = 0;

	//�ж��Ƿ�ǰ�����Ӷ����Ƿ�����objectIdx��λ����������object(���㷨����)
	virtual bool canConnect(orsIConnectableObject* object) = 0;

	//����������
	virtual bool connect(orsIConnectableObject* object) = 0;

	//�������жϿ�һ������
	virtual bool disconnect(orsIConnectableObject* object) = 0;

	//�Ͽ���������
	virtual void disconnectAll() = 0;

	//���ø����ӵ����������߼��㷨���ö���ʵ��
	virtual bool setLogicObject( orsIObject *obj) = 0;

// 	//�õ��߼�����
//	virtual ref_ptr<orsIObject> getLogicObject() = 0;

	//�����ӿ�
	virtual ors_int32 getNumberOfInput() = 0;

	//ͨ�������õ��������Ӷ���
	virtual orsIConnectableObject* getInputObjectByIndex(unsigned idx) = 0;

	// ��ϣ������������
//	ORS_INTERFACE_DEF(orsIObject, "connectable")
};

#endif	// _ORS_CONNECTABLE_OBJECT_H_


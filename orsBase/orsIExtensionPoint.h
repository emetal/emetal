//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIExtensionPoint.h 2008/10/07 12:18 JWS $
//
// Project: OpenRS 
//
// Purpose: ����һ����չ�� 
//
// Author: Wanshou Jiang, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIExtensionPoint.h,v $
//
// Revision 1.0 date: 2008/10/07 by JIANG Wanshou
// new
//

#ifndef _ORS_EXTENSION_POINT_24234546546598098_H_
#define _ORS_EXTENSION_POINT_24234546546598098_H_

#include "orsBase\orsTypedef.h"

//�����Ӷ���ѭ�����ûᷢ���ڴ�й©���ʲ�������ָ��
interface orsIExtensionPoint: public orsIObject
{
public:
	// 
//	virtual bool numOfInputAcceptable(orsIExtensionPoint* object) = 0;

	//�ж��Ƿ�ǰ�����Ӷ����Ƿ�����objectIdx��λ����������object(���㷨����)
	virtual bool canConnect(orsIExtensionPoint* object) = 0;

	//����������
	virtual bool connect(orsIExtensionPoint* object) = 0;  

	//�������жϿ�һ������
	virtual bool disconnect(orsIExtensionPoint* object) = 0;

	//�Ͽ���������
	virtual void disconnectAll() = 0;

	//���ø����ӵ����������߼��㷨���ö���ʵ��
	virtual bool setLogicObject( orsIObject *obj) = 0;
	
// 	//�õ��߼�����
//	virtual ref_ptr<orsIObject> getLogicObject() = 0;

	//�����ӿ� 
	virtual ors_int32 getNumberOfInput() = 0;

	//ͨ�������õ��������Ӷ���
	virtual orsIExtensionPoint* getInputObjectByIndex(int idx) = 0; 

	ORS_INTERFACE_DEF(orsIObject, "extensionPoint")
};

#endif	// _ORS_EXTENSION_H_
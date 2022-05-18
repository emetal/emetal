//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIConnectableObjectNoInputHelper.h 2008/10/05 17:59 JWS $
//
// Project: OpenRS
//
// Purpose: ���ڰ���ʵ��ֻ��һ������Ŀ����Ӷ��󣬴����Ӱ���������������
//
// Author: Wanshou Jiang, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIConnectableObjectNoInputHelper.h,v $
//
// Revision 1.0 date: 2008/10/05 by JIANG Wanshou
// new
//

#ifndef _ORS_CONNECTABLE_OBJECT_NO_INPUT_HELPER_H_
#define _ORS_CONNECTABLE_OBJECT_NO_INPUT_HELPER_H_


template <typename _T>
class orsIConnectableObjectNoInputHelper : public _T
{
public:
	virtual bool canConnect(orsIConnectableObject* object)	{	return false; }

	virtual bool connect(orsIConnectableObject* object) 	{	return false; }

	virtual bool disconnect(orsIConnectableObject* object) 	{	return false; }

	virtual void disconnectAll()	{};

	virtual bool setLogicObject( orsIObject *obj) 	{	return false; }

	// 	//�õ��߼�����
	//	virtual ref_ptr<orsIObject> getLogicObject() = 0;

	//�����ӿ�
	virtual ors_int32 getNumberOfInput() 	{	return 0; }

	//ͨ�������õ��������Ӷ���
	virtual orsIConnectableObject* getInputObjectByIndex(unsigned idx) 	{ return NULL; }

protected:


};

#endif	// _ORS_CONNECTABLE_OBJECT_NO_INPUT_HELPER_H_

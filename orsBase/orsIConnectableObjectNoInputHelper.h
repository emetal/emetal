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
// Purpose: 用于帮助实现只有一个输入的可连接对象，大多数影像处理都属于这种情况
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

	// 	//得到逻辑对象
	//	virtual ref_ptr<orsIObject> getLogicObject() = 0;

	//遍历接口
	virtual ors_int32 getNumberOfInput() 	{	return 0; }

	//通过索引得到输入链接对象
	virtual orsIConnectableObject* getInputObjectByIndex(unsigned idx) 	{ return NULL; }

protected:


};

#endif	// _ORS_CONNECTABLE_OBJECT_NO_INPUT_HELPER_H_

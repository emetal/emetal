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
// Purpose: 用于定义可连接对象
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


//可链接对象，循环引用会发生内存泄漏，故不用智能指针
interface orsIConnectableObject	: public orsIObject
{
public:
	//
//	virtual bool numOfInputAcceptable(orsIConnectableObject* object) = 0;

	//判断是否当前的链接对象是否能在objectIdx的位置上链接上object(如算法对象)
	virtual bool canConnect(orsIConnectableObject* object) = 0;

	//链接上输入
	virtual bool connect(orsIConnectableObject* object) = 0;

	//在输入中断开一个对象
	virtual bool disconnect(orsIConnectableObject* object) = 0;

	//断开所有输入
	virtual void disconnectAll() = 0;

	//设置该链接点对象的真正逻辑算法内置对象实现
	virtual bool setLogicObject( orsIObject *obj) = 0;

// 	//得到逻辑对象
//	virtual ref_ptr<orsIObject> getLogicObject() = 0;

	//遍历接口
	virtual ors_int32 getNumberOfInput() = 0;

	//通过索引得到输入链接对象
	virtual orsIConnectableObject* getInputObjectByIndex(unsigned idx) = 0;

	// 不希望出现在树上
//	ORS_INTERFACE_DEF(orsIObject, "connectable")
};

#endif	// _ORS_CONNECTABLE_OBJECT_H_


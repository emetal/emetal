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
// Purpose: 定义一个扩展点 
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

//可链接对象，循环引用会发生内存泄漏，故不用智能指针
interface orsIExtensionPoint: public orsIObject
{
public:
	// 
//	virtual bool numOfInputAcceptable(orsIExtensionPoint* object) = 0;

	//判断是否当前的链接对象是否能在objectIdx的位置上链接上object(如算法对象)
	virtual bool canConnect(orsIExtensionPoint* object) = 0;

	//链接上输入
	virtual bool connect(orsIExtensionPoint* object) = 0;  

	//在输入中断开一个对象
	virtual bool disconnect(orsIExtensionPoint* object) = 0;

	//断开所有输入
	virtual void disconnectAll() = 0;

	//设置该链接点对象的真正逻辑算法内置对象实现
	virtual bool setLogicObject( orsIObject *obj) = 0;
	
// 	//得到逻辑对象
//	virtual ref_ptr<orsIObject> getLogicObject() = 0;

	//遍历接口 
	virtual ors_int32 getNumberOfInput() = 0;

	//通过索引得到输入链接对象
	virtual orsIExtensionPoint* getInputObjectByIndex(int idx) = 0; 

	ORS_INTERFACE_DEF(orsIObject, "extensionPoint")
};

#endif	// _ORS_EXTENSION_H_
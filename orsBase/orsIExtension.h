//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIExtension.h 2008/10/07 12:19 JWS $
//
// Project: OpenRS
//
// Purpose: 定义扩展, 每个扩展对应一个扩展点
//
// Author: Wanshou Jiang, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIExtension.h,v $
//
// Revision 1.0 date: 2008/10/07 by JIANG Wanshou
// new
//

#ifndef _ORS_EXTENSION_1212423454654659809_H_
#define _ORS_EXTENSION_1212423454654659809_H_

#include "orsBase/orsTypedef.h"

#include "orsBase/orsIObject.h"

interface orsIExtension	: public orsIObject
{
public:
	ORS_INTERFACE_DEF(orsIObject, _T("extension") )
};

#endif	// _ORS_EXTENSION_H_

#ifndef _ORS_EVENT_INTERFACE_H
#define _ORS_EVENT_INTERFACE_H

/******************************************************************************
* Project:  OpenRS
* Purpose:  用于事件处理
*
* Author:   JWS, Guowei, guowei98032@gmail.com
******************************************************************************
* Copyright (c) 2009, Liesmars, Wuhan University
* History:
*      
*
****************************************************************************/

#include "orsIObject.h"

//////////////////////////////////////////////////////////////////////////
//
// 粗粒度算法的处理消息接口，如进度条，错误信息等
//
interface orsIEvent : public orsIObject
{
public:

	virtual void EnableEvent() = 0;
	virtual void DisableEvent() = 0;

	//接口名称
	ORS_INTERFACE_DEF( orsIObject, _T("event") )
};


#endif

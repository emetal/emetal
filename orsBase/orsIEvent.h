#ifndef _ORS_EVENT_INTERFACE_H
#define _ORS_EVENT_INTERFACE_H

/******************************************************************************
* Project:  OpenRS
* Purpose:  �����¼�����
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
// �������㷨�Ĵ�����Ϣ�ӿڣ����������������Ϣ��
//
interface orsIEvent : public orsIObject
{
public:

	virtual void EnableEvent() = 0;
	virtual void DisableEvent() = 0;

	//�ӿ�����
	ORS_INTERFACE_DEF( orsIObject, _T("event") )
};


#endif

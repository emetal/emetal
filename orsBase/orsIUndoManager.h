//*************************************************************************
//
// Copyright (C) 2010, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIUndoManager.h 2010/03/17 16:00 JWS $
//
// Project: OpenRS
//
// Purpose: Undo/Redo ������
//
// Author: JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIUndoManager.h,v $
//
// Revision 0.1 date: 2010/03/17 by JIANG Wanshou
// new
//

#ifndef _ORS_UNDO_MANAGER_INTERFACE_H
#define _ORS_UNDO_MANAGER_INTERFACE_H

#include "orsBase/orsIObject.h"
#include "orsBase/orsFastDelegate.h"


using namespace fastdelegate;

typedef FastDelegate2 <long,long> orsUndoRedoHandler;


struct orsAction
{
	long actionID;	// ��������ID, ��ģ���Լ�����
	long objID;		// ��������ID, ������ָ�룬Ҳ������ID

    orsUndoRedoHandler Undo;	// Undo ������
	orsUndoRedoHandler Redo;	// Redo ������
};


interface orsIUndoManager: public orsIObject
{
public:
	virtual void addAction( orsAction &action ) = 0;
    virtual void clear() = 0;

    virtual void Undo() = 0;
    virtual void Redo() = 0;

	virtual bool IsUndoable() = 0;
    virtual bool IsRedoable() = 0;

public:
	ORS_INTERFACE_DEF( orsIObject, _T("undoManager") )
};

#define ORS_UNDO_MANAGER_DEFAULT	_T("ors.undoManager.default")


#endif


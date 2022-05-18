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
// Purpose: Undo/Redo 管理器
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
	long actionID;	// 操作动作ID, 各模块自己定义
	long objID;		// 操作对象ID, 可以是指针，也可以是ID

    orsUndoRedoHandler Undo;	// Undo 处理函数
	orsUndoRedoHandler Redo;	// Redo 处理函数
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


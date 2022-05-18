//*************************************************************************
//
// Copyright (C) 2010, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsXUndoManager.h 2010/03/17 16:00 JWS $
//
// Project: OpenRS
//
// Purpose: Undo/Redo π‹¿Ì∆˜
//
// Author: JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsXUndoManager.h,v $
//
// Revision 0.1 date: 2010/03/17 by JIANG Wanshou
// new
//

#ifndef _ORS_UNDO_MANAGER_IMPL_H
#define _ORS_UNDO_MANAGER_IMPL_H


#include <vector>

#include "orsBase/orsIUndoManager.h"

interface orsXUndoManager: public orsIUndoManager, orsObjectBase
{
private:
	std::vector<orsAction>	m_undoList;
	std::vector<orsAction>	m_redoList;

public:
	virtual void addAction( orsAction &action );
    virtual void clear();

    virtual void Undo();
    virtual void Redo();

	virtual bool IsUndoable();
    virtual bool IsRedoable();

public:
	ORS_OBJECT_IMP1( orsXUndoManager, orsIUndoManager, _T("default"), _T("OpenRS Undo Manager") )

};


#endif


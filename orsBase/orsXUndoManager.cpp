//*************************************************************************
//
// Copyright (C) 2010, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsXUndoManager.cpp 2010/03/17 16:00 JWS $
//
// Project: OpenRS
//
// Purpose: Undo/Redo 管理器
//
// Author: JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsXUndoManager.cpp,v $
//
// Revision 0.1 date: 2010/03/17 by JIANG Wanshou
// new
//

#include "orsXUndoManager.h"

void orsXUndoManager::addAction( orsAction &action )
{
	m_undoList.push_back( action );

	// 添加新动作的时候，要清除redo队列

	m_redoList.clear();
}

void orsXUndoManager::clear()
{
	m_undoList.clear();
	m_redoList.clear();
}

void orsXUndoManager::Undo()
{
	int n = m_undoList.size();

	if( n > 0 )
	{
		orsAction action = m_undoList[n-1];
			
		m_undoList.pop_back();

		action.Undo( action.actionID, action.objID );

		m_redoList.push_back( action );
	}
	
}

void orsXUndoManager::Redo()
{
	int n = m_redoList.size();
	
	if( n > 0 )
	{
		orsAction action = m_redoList[n-1];
		
		m_redoList.pop_back();
		
		action.Redo( action.actionID, action.objID );
		
		m_undoList.push_back( action );
	}
}



bool orsXUndoManager::IsUndoable()	
{	
	return	m_undoList.size() > 0;	
};



bool orsXUndoManager::IsRedoable()	
{	
	return	m_redoList.size() > 0;	
};

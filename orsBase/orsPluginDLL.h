//*************************************************************************
//
// Copyright (C) 2008-2018, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsPluginDLL.h 2009/8/31 11:08 JWS $
//
// Project: OpenRS
//
// Purpose: 
//
// Author: GUO Wei, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsPluginDLL.h,v $
//
// Revision 1.0 date: 2008/09/20 by GUO Wei
// new
//

#ifndef ORS_PLUGIN_DLL_H
#define ORS_PLUGIN_DLL_H

#include <map>


#include "orsCommon.h"

#include "orsBase/orsIPlugin.h"

#include "orsBase/orsIObject.h"

typedef std::map<stdString ,ORS_OBJECT_CREATOR_FUN> orsObjCreatorMap;

class orsPluginDLL : public orsIObject,public orsObjectBase
{
public:
	orsPluginDLL(const orsChar * name,const orsChar * path);
	virtual ~orsPluginDLL();

//插件对象相关方法
public:
	bool load(orsIPlatform* platform);
	void unload();

public:
	stdString 			m_pluginName;
	stdString 			m_pathName;

	orsObjCreatorMap	m_objectCreators;

	orsPluginHandle		m_hDLL;

	orsIPlugin*			m_plugin;

public:

	//接口定义
	ORS_OBJECT_IMP0(orsPluginDLL, _T("plugindll"), _T("plugindll") );
};


typedef std::map<stdString , ref_ptr<orsPluginDLL> >	 orsPluginDLLMap;

#endif
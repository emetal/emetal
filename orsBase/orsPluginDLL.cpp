//*************************************************************************
//
// Copyright (C) 2008-2018, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsPluginDLL.cpp 2008/8/31 11:08 JWS $
//
// Project: OpenRS
//
// Purpose:
//
// Author: GUO Wei, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsPluginDLL.cpp,v $
//
// Revision 1.0 date: 2008/09/20 by GUO Wei
// new
//
#include "stdafx.h"

#include "orsPluginDLL.h"

#include "orsBase/orsIPlatform.h"

#include "orsBase/orsILogService.h"

orsPluginDLL::orsPluginDLL(const orsChar * name, const orsChar * path)
{
	m_pluginName = name;
	m_pathName = path;

	m_hDLL = NULL;

	m_plugin = NULL;
}

orsPluginDLL::~orsPluginDLL()
{
	unload();
}



typedef orsIPlugin *(*orsGetPluginInstancePtr)(orsIPlatform* platform);
typedef const char *(*orsgetPlatformMajorVersionPtr)();

bool orsPluginDLL::load(orsIPlatform* platform)
{
	platform->getLogService()->debug("load plugin :%s\n", m_pathName.c_str());

	//1)加载动态库
	m_hDLL = loadDynamicLib( m_pathName.c_str() );
	if( m_hDLL  == NULL)
		return false;


	stdString  msg = m_pathName + " loaded" ;
	platform->getLogService()->debug( "orsBase", msg.c_str() );


	//2)加载symbol函数
	orsgetPlatformMajorVersionPtr versionFun =
		(orsgetPlatformMajorVersionPtr)getDynamicSymbol( m_hDLL , ORS_PLUGIN_MAJOR_VERSION_GET);


	orsGetPluginInstancePtr	instanceFun =
		(orsGetPluginInstancePtr)getDynamicSymbol( m_hDLL , ORS_PLUGIN_INSTANCE_GET);
	if(versionFun == NULL || instanceFun == NULL){
		platform->getLogService()->warn("host", "versionFun == NULL || instanceFun == NULL");
		return false;
	}


	orsString platformMajorVersion;
	platformMajorVersion = versionFun();
	m_plugin = instanceFun( platform );

	//3) 插件加载?
	//判断插件的主平台号和当前平台号是否相符合
	if(platformMajorVersion != ORS_PLATFORM_VERSION)
	{
		_getPlatform()->logPrint(ORS_LOG_ERROR, _T("Version of %s differs from platform"), m_pluginName.c_str());
		return false;
	}

	if( !m_plugin->initialize( _getPlatform() ) )
	{
		_getPlatform()->logPrint(ORS_LOG_ERROR, _T("Failed Initializing %s "), m_pluginName.c_str());
		return false;
	}

	return true;
}

void orsPluginDLL::unload()
{
	if( NULL != m_hDLL )	{
		if(m_plugin != NULL)
		{
			m_plugin->finalize();
		}

		unloadDynamicLib( m_hDLL );

		stdString  msg = m_pathName + " unloaded" ;
		_getPlatform()->getLogService()->debug( "orsBase", msg.c_str() );

		m_plugin = NULL;
		m_hDLL = NULL;
	}
}

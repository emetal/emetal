//*************************************************************************
//
// Copyright (C) 2008-2018, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsSystemModules.cpp 2009/8/31 11:40 JWS $
//
// Project: OpenRS
//
// Purpose: 
//
// Author: GUO Wei, Wanshou Jiang, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsSystemModules.cpp,v $
//
// Revision 1.0 date: 2009/8/31 by JIANG Wanshou
// 改用orsPlugDLL进行管理
//
// Revision 1.0 date: 2008/9/23 by GUO Wei
// new
//
#include "stdafx.h"

#include <vector>
#include "orsBase/orsIPlatform.h"
#include "orsPluginDLL.h"
#include <string>

#define ORS_MODULE_INIT_DIR		"modules"

// #define ORS_MODULE_INIT_FUN		"orsModuleInitialize"
// #define ORS_MODULE_UNINIT_FUN	"orsModuleUninitialize"
// 
// typedef void (*orsModuleInitializeFun)(orsIPlatform* platform);
// typedef void (*orsModuleUninitializeFun)();
// 
// struct SystemModuleInfo
// {
// 	orsPluginHandle handle;
// 	orsModuleInitializeFun initFun;
// 	orsModuleUninitializeFun uninitFun;
// };
// 
// 
// std::vector<SystemModuleInfo> s_moduleHandles;

orsPluginDLLMap *s_loadedModules=NULL;

//////////////////////////////////////////////////////////////////////////

const orsChar *g_curModuleName = NULL;

bool LoadModule(const orsChar * pluginName )
{

	if(s_loadedModules==NULL)
	{
		s_loadedModules=new orsPluginDLLMap;
	}

	//得到插件的路径名
	orsString pathName = pluginName;	//getPluginPathByName(pluginName);
	
	//创建插件
	ref_ptr<orsPluginDLL> plugin = new orsPluginDLL( pluginName, pathName.c_str() );
	
	g_curModuleName = pathName.c_str();

	// 加载插件
	if( plugin->load( _getPlatform() ) )
	{
		//放入到插件实体map中
		s_loadedModules->erase( pluginName );
		s_loadedModules->insert( orsPluginDLLMap::value_type( pluginName, plugin) );
		
		return true;
	}
	else	{
		orsString msg = "Fail to add plugin: ";
		msg = msg + pluginName;
		
		_getPlatform()->logPrint( ORS_LOG_WARNING, msg );
		return false;
	}
}


//加载系统级模块
void LoadSystemModules()
{
	stdString  sysModuleDir = getExeDir() + "/" + ORS_MODULE_INIT_DIR;

	//printf("sysModuleDir==%s\n",sysModuleDir.c_str());
	//找到目录下所有的dll
	orsArray<orsString> allDlls;
	
	findAllFiles( sysModuleDir.c_str(), PLUGIN_EXTENSION, allDlls );

	if (allDlls.size()==0)
	{
		char *ExePathEnv = getenv( "EXE_PATH" );
		stdString ExePathEnv0 = ExePathEnv;
		sysModuleDir = ExePathEnv0 + "/" + ORS_MODULE_INIT_DIR;

		//printf("sysModuleDir==%s\n",sysModuleDir.c_str());

		findAllFiles( sysModuleDir.c_str(), PLUGIN_EXTENSION, allDlls );
	}
	
	//for (int i = 0;i<allDlls.size();i++)
	//{
	//	printf("allDlls %d ==%s\n",i,allDlls[i].c_str());
	//}

	//加载插件
	for(int i=0;i<allDlls.size();i++)
	{
		LoadModule( allDlls[i].c_str() );
	}
}

void unLoadSystemModules()
{
	delete s_loadedModules;
}
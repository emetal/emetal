/********************************************************************
	created:	2008/09/20
	created:	20:9:2008   17:30
	filename: 	e:\OpenRS\DESKTOP\src\orsBase\orsUtil.cpp
	file path:	e:\OpenRS\DESKTOP\src\orsBase
	file base:	orsUtil
	file ext:	cpp

	author:		Guo Wei

	purpose:	常用函数
*********************************************************************/
#include "stdafx.h"

#include "orsBase/orsUtil.h"

#include <vector>

#include <assert.h>

#include "orsBase/orsConstDef.h"

#include "orsXPlatform.h"
#include "orsXRegisterService.h"
#include "orsXLastErrorService.h"
//#include "orsXBinarySerialize.h"
#include "orsXLogService.h"
#include "orsXProperty.h"
#include "orsXXMLSerialize.h"
#include "orsXRDFService.h"

#include "orsXExe.h"

//////////////////////////////////////////////////////////////////////////


static	int	SizeOfDataType[13]={
	0,		//
	1,		// BYTE
	2,		// UINT16
	2,		// INT16
	4,		// UINT32
	4,		// INT32
	4,		// FLOAT32
	8,		// FLOAT64
	4,		// CINT16
	8,		// CINT32
	8,		// CFLOAT32
	16,		// CFLOAT64
	1       // INT8
};

//////////////////////////////////////////////////////////////////////////

int orsGetSizeOfType(orsDataTYPE type)
{
	if( type < 0 || type >= ORS_DT_COUNT )
		return 0;

	return SizeOfDataType[type];
}

double orsGetDefaultMin(orsDataTYPE scalarType)
{
	switch(scalarType)
	{
	case ORS_DT_BYTE:
		{
			return ORS_DEFAULT_MIN_PIX_UCHAR;
		}
 	case ORS_DT_INT8:
 		{
 			return ORS_DEFAULT_MIN_PIX_INT8;
 		}
	case ORS_DT_UINT16:
		{
			return ORS_DEFAULT_MIN_PIX_UINT16;
		}
	case ORS_DT_INT16:
		{
			return ORS_DEFAULT_MIN_PIX_INT16;
		}
	case ORS_DT_UINT32:
		{
			return ORS_DEFAULT_MIN_PIX_UINT32;
		}
	case ORS_DT_INT32:
		{
			return ORS_DEFAULT_MIN_PIX_INT32;
		}
	case ORS_DT_FLOAT32:
		{
			return ORS_DEFAULT_MIN_PIX_FLOAT;
		}
	case ORS_DT_FLOAT64:
		{
			return ORS_DEFAULT_MIN_PIX_DOUBLE;
		}
	default:
		{
			assert(0);
			break;
		}
	}

	return ORS_DBL_NAN;
}

double orsGetDefaultMax(orsDataTYPE scalarType)
{
	switch(scalarType)
	{
	case ORS_DT_BYTE:
		{
			return ORS_DEFAULT_MAX_PIX_UCHAR;
		}
 	case ORS_DT_INT8:
 		{
 			return ORS_DEFAULT_MAX_PIX_INT8;
 		}
	case ORS_DT_UINT16:
		{
			return ORS_DEFAULT_MAX_PIX_UINT16;
		}
	case ORS_DT_INT16:
		{
			return ORS_DEFAULT_MAX_PIX_INT16;
		}
	case ORS_DT_UINT32:
		{
			return ORS_DEFAULT_MAX_PIX_UINT32;
		}
	case ORS_DT_INT32:
		{
			return ORS_DEFAULT_MAX_PIX_INT32;
		}
	case ORS_DT_FLOAT32:
		{
			return ORS_DEFAULT_MAX_PIX_FLOAT;
		}
	case ORS_DT_FLOAT64:
		{
			return ORS_DEFAULT_MAX_PIX_DOUBLE;
		}
	default:
		{
			assert(0);
			break;
		}
	}
	return ORS_DBL_NAN;
}
double orsGetDefaultNULL(orsDataTYPE scalarType)
{
	switch(scalarType)
	{
	case ORS_DT_BYTE:
		{
			return 0;
		}
 	case ORS_DT_INT8:
		{
 			return ORS_DEFAULT_MIN_PIX_INT8;
		}
	case ORS_DT_UINT16:
		{
			return 0;
		}
	case ORS_DT_INT16:
	case ORS_DT_CINT16:
		{
			return ORS_SSHORT_NAN;
		}
	case ORS_DT_UINT32:
		{
			return 0;
		}
	case ORS_DT_INT32:
	case ORS_DT_CINT32:
		{
			return ORS_INT_NAN;
		}
	case ORS_DT_FLOAT32:
	case ORS_DT_CFLOAT32:
		{
			return ORS_FLT_NAN;
		}
	case ORS_DT_FLOAT64:
	case ORS_DT_CFLOAT64:
		{
			return ORS_DBL_NAN;
		}	
	default:
		{
			assert(0);
			break;
		}
    }

	return ORS_DBL_NAN;
}


//////////////////////////////////////////////////////////////////////////
static orsXPlatform	*s_platform = NULL;

orsIPlatform *_getPlatform()
{
	
	return s_platform;

	//return (orsIPlatform*)(&s_platform);
}

orsIObject* create_keywordProperty(bool bForRegister)
{
	return new orsXProperty;
}

/*
orsIObject* create_binaryPropertySerialize(bool bForRegister)
{
	return new orsXBinarySerialize;
}*/

orsIObject* create_xmlPropertySerialize(bool bForRegister)
{
	return new orsXXMLSerialize;
}

orsIObject* create_cmdProcessMsg(bool bForRegister)
{
	return new orsXProcessMsg;
}


orsIObject* create_cmdProcessMsg_ms(bool bForRegister)
{
	return new orsXProcessMsg_multiStage;
}


#include "orsXUndoManager.h"

orsIObject* create_undoManager(bool bForRegister)
{
	return new orsXUndoManager;
}

#include "orsXUtilityService.h"


//系统内部对象注册
void orsRegistCommonObjects()
{
	//系统常用对象工厂注册
	s_platform->getRegisterService()->registerObject(create_keywordProperty);
	//s_platform->getRegisterService()->registerObject(create_binaryPropertySerialize);
	s_platform->getRegisterService()->registerObject(create_xmlPropertySerialize);
}


//注册内部三大核心服务
void orsRegistCoreServices()
{
	s_platform->registerService( ORS_logServiceName, new orsXLogService);

	//系统服务注册
	orsXRegisterService *registerService = new orsXRegisterService;

	s_platform->registerService( ORS_registerServiceName, registerService );
	s_platform->registerService( ORS_pluginManagerName,  registerService );
	s_platform->registerService( ORS_lastErrorServiceName, new orsXLastErrorService);

	s_platform->registerService( ORS_SERVICE_RDF, new orsXRDFService );
	s_platform->registerService( ORS_SERVICE_UTILITY, new orsXUtilityService );

	s_platform->getRegisterService()->registerObject( create_cmdProcessMsg );
	s_platform->getRegisterService()->registerObject( create_cmdProcessMsg_ms );

	s_platform->getRegisterService()->registerObject( create_undoManager );
}


//////////////////////////////////////////////////////////////////////////
#ifdef WIN32

extern HMODULE g_hModule;
std::string GetSelfDllDir()
{
	char exeDir[512]= {0};
	char selfDir[512] = {0};
	
	HMODULE hHandle = g_hModule;
	
	GetModuleFileName( hHandle, exeDir, 512);
	char* temp_str = strrchr(exeDir,'\\');
	memcpy(selfDir,exeDir,temp_str-exeDir);
	return selfDir;
}
#endif

void orsSetEnv()
{
	//将固定的位置加入系统变量Path中
	//char pathenv[4096];
	//GetEnvironmentVariable("Path",pathenv,4096);
	
	char *pathEnv;
	pathEnv = getenv( "PATH" );
	
	stdString  plugindir = getExeDir() + "/plugins";
	stdString  moudledir = getExeDir() + "/modules";
	stdString  dependentsdir = getExeDir() + "/dependents";
	stdString  mydir = getExeDir();

	stdString  newPathEnv = "PATH=";
	newPathEnv = newPathEnv + mydir + ";" + newPathEnv + plugindir + ";" + moudledir + ";" + dependentsdir;
	
	if(pathEnv != NULL)
		newPathEnv = newPathEnv + ";" + pathEnv;

    pathEnv = ( char *)newPathEnv.c_str();
	//SetEnvironmentVariable("Path",pathenvs.c_str());
	putenv( pathEnv );

	// 
	char *gdalEnv = getenv( "GDAL_DATA" );

	if( NULL == gdalEnv)	{
		orsIUtilityService *pUtilService = (orsIUtilityService *)_getPlatform()->getService( ORS_SERVICE_UTILITY );
		
		if( NULL != pUtilService )	{
			orsString gdal_data;
			pUtilService->GetDirectory_ETC( gdal_data );
			
			gdal_data = gdal_data + _T("gdal_data");
			
			//orsString setStr = _T("GDAL_DATA=") + gdal_data;
			
			// putenv的字符串必须为全局字符串
			// putenv( (char*)(setStr.c_str()) );	

#ifdef ORS_PLATFORM_WINDOWS
			SetEnvironmentVariable("GDAL_DATA", gdal_data.c_str());
#else
			setenv("GDAL_DATA", gdal_data.c_str(), 1);
#endif

			printf("GDAL_DATA not set, set as  %s\n", gdal_data.c_str());
		}
	}
}

extern bool g_lazyLoad;

static int g_nInitializedTimes = 0;
extern "C" orsIPlatform* orsInitialize( ors_string &errorinfo, bool bInteractiveLog )
{
	if (0 == g_nInitializedTimes) {
		s_platform = new orsXPlatform;

		//注册内部对象
		orsRegistCoreServices();

		orsSetEnv();

		s_platform->getLogService()->enableInteractive(bInteractiveLog);

		orsRegistCommonObjects();

		// 非交互，开启懒加载

		g_lazyLoad = !bInteractiveLog;

		g_lazyLoad = false;

		if (!s_platform->startup(errorinfo))
			return NULL;
	}

	g_nInitializedTimes++;

	return s_platform;
}


extern "C" void orsUninitialize()
{
	//调用插件的初始化函数
	--g_nInitializedTimes;

	if( 0 == g_nInitializedTimes )	{
		s_platform->shutdown();
		delete s_platform;
	}
}



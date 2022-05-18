//*************************************************************************
//
// Copyright (C) 2010, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIUtilityService.h 2010/4/21 22:13 JWS $
//
// Project: OpenRS
//
// Purpose: OpenRS 便利服务
//
// Author: JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIUtilityService.h,v $
//
// Revision 0.1 date:2010/4/21 by JIANG Wanshou
// new
//

#ifndef _ORS_Utility_SERVICE_INTERFACE_H
#define _ORS_Utility_SERVICE_INTERFACE_H

#include "orsIProperty.h"

#include "orsIService.h"


// #if defined kyLINUX64
// #include <vector>
// using namespace std;
// #define orsArray std::vector
// #endif

// 便利服务
interface orsIUtilityService : public orsIService
{
public:
	virtual bool GetDirectory_EXE( orsString &dir )	= 0;
	virtual bool GetDirectory_ETC( orsString &dir )	= 0;
	
	virtual bool ShellExecute( const orsChar *cmdLine, bool bWaite = true ) = 0;

	// replace MACRO prop in outputProp with inputProp
	virtual bool ReplaceMacroString( orsIProperty *outputProp, orsIProperty *inputProp, orsIProperty *paraProp = NULL ) = 0;
	
	virtual void AddMarco( const orsChar *macroName, const orsChar *macroValue ) = 0;
	virtual bool GetMarco( const orsChar *macroName, orsString &macroValue  ) = 0;

	// 从"1,2,3", "1-3" 等数字字符串中提取数字
	virtual orsArray<ors_int32> String2Integers( orsString &strIntegers ) =0;
	
	// 整数数组转换为"1,2,3", "1-3"形式的字符串表示
	virtual orsString Integers2String( orsArray<ors_int32> &integers ) =0;

	virtual ors_int64 GetAvailableMemory() = 0;
	virtual orsDataTYPE GetOutputDataType(ors_string &strOutputDataType) =0;

	virtual bool CheckFileExist( const orsChar *filePath ) = 0;
	virtual bool CheckFolderExist( const orsChar *filePath ) = 0;

	virtual bool CopyFile( const orsChar *filePath,  const orsChar *newFilePath, bool bFailIfExists ) = 0;
	virtual bool DeleteFile( const orsChar *filePath ) = 0;

	virtual bool CreateFolder( const orsChar *filePath ) = 0;

	virtual bool CopyFolder( const orsChar *srcDir, const orsChar *dstDir, bool bFailIfExists ) = 0;
	virtual bool DeleteFolder( const orsChar *filePath ) = 0;

	virtual bool EmptyFolder( const orsChar *filePath ) = 0;

	// 移动文件夹或目录：在同一盘符时可提高效率
	virtual bool MoveFile( const orsChar *filePath,  const orsChar *newFilePath, bool bFailIfExists ) = 0;
	virtual bool MoveFolder( const orsChar *srcDir, const orsChar *dstDir, bool bFailIfExists ) = 0;

	virtual bool IsSupportedImageFile(const orsChar* filePath) = 0;
	//
	virtual int FindFiles(const orsChar *pszDir, orsArray<orsString> &vFiles, const orsChar *filters = "*.*") = 0;
public:
	//接口
	ORS_INTERFACE_DEF( orsIService, _T("utility"))
};


#define ORS_SERVICE_UTILITY				_T("ors.service.utility")
#define ORS_SERVICE_UTILITY_DEFAULT		_T("ors.service.utility.default")


// 获取几何服务的宏定义
orsIUtilityService *getUtilityService();

#define ORS_GET_UTILITY_SERVICE_IMPL()\
	static orsIUtilityService *s_UtilityService = NULL;\
	orsIUtilityService *getUtilityService()\
{\
	if( NULL != s_UtilityService )\
	return s_UtilityService;\
	\
	s_UtilityService =\
	ORS_PTR_CAST(orsIUtilityService, getPlatform()->getService( ORS_SERVICE_UTILITY) );\
	\
	return s_UtilityService;\
};


#endif


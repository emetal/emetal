//*************************************************************************
//
// Copyright (C) 2010, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsXUtilityService.h 2010/4/21 22:13 JWS $
//
// Project: OpenRS
//
// Purpose: OpenRS 便利服务
//
// Author: JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsXUtilityService.h,v $
//
// Revision 0.1 date:2010/4/21 by JIANG Wanshou
// new
//

#ifndef _ORS_Utility_SERVICE_IMPL_H
#define _ORS_Utility_SERVICE_IMPL_H

#include "orsBase/orsIProperty.h"

#include "orsBase/orsIUtilityService.h"

class orsXUtilityService : public orsIUtilityService, public orsObjectBase
{
public:
	//启动时被调用
	virtual bool startup( orsIPlatform *platform );
	
	//关闭时被调用
	virtual void shutdown();
	
	//是否启动成功
	virtual bool isok();

public:
	bool GetDirectory_EXE( orsString &dir );
	bool GetDirectory_ETC( orsString &dir );

	bool ShellExecute( const orsChar *cmdLine, bool bWaite = true );

	// replace MACRO prop in outputProp with inputProp
	bool ReplaceMacroString( orsIProperty *outputProp, orsIProperty *inputProp, orsIProperty *paraProp = NULL );
	
	void AddMarco( const orsChar *macroName, const orsChar *macroValue );
	bool GetMarco( const orsChar *macroName, orsString &macroValue );

	// 从"1,2,3", "1-3" 等数字字符串中提取数字
	orsArray<ors_int32> String2Integers( orsString &strIntegers );
	orsArray<ors_float32> String2Float( orsString &strIntegers ) ;
	orsString Integers2String( orsArray<ors_int32> &integers );
	
	ors_int64 GetAvailableMemory();
	orsDataTYPE GetOutputDataType(ors_string &strOutputDataType);

	bool CheckFileExist( const orsChar *filePath );
	bool CheckFolderExist( const orsChar *filePath );

	bool CopyFile( const orsChar *filePath,  const orsChar *newFilePath, bool bFailIfExists );
	bool DeleteFile( const orsChar *filePath );
	
	bool CreateFolder( const orsChar *filePath );
	
	/************
	* 功能：将源文件夹下所有文件包括子文件夹拷贝至目标文件夹下。若目标文件夹不存在，则创建目标文件夹。
	************/
	bool CopyFolder( const orsChar *srcDir, const orsChar *dstDir, bool bFailIfExists );
	bool DeleteFolder( const orsChar *filePath );
	
	bool EmptyFolder( const orsChar *filePath );

	bool MoveFile( const orsChar *filePath,  const orsChar *newFilePath, bool bFailIfExists );
	bool MoveFolder( const orsChar *srcDir, const orsChar *dstDir, bool bFailIfExists );
	
	// add by yanyuan
	bool IsSupportedImageFile(const orsChar* fileName);

	int FindFiles(const orsChar* pszDir, orsArray<orsString>& vFiles, const orsChar* filters = "*.*");
	
private:
	ref_ptr<orsIProperty> m_macroSet;

public:
	//接口
	ORS_OBJECT_IMP2( orsXUtilityService, orsIUtilityService, orsIService, _T("default"), _T("orsUtilityService") )
	
};

#endif


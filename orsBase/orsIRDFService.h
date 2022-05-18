//*************************************************************************
//
// Copyright (C) 2010, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIRDFService.h 2008/11/27 8:40 JWS $
//
// Project: OpenRS
//
// Purpose: OpenRS 资源描述服务
//
// Author: JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIRDFService.h,v $
//
// Revision 0.1 date: 2010/3/8 by JIANG Wanshou
// new
//

#ifndef _ORS_RDF_SERVICE_INTERFACE_H
#define _ORS_RDF_SERVICE_INTERFACE_H

#include "orsIProperty.h"

#include "orsIService.h"


// 资源描述服务
interface orsIRDFService : public orsIService
{
public:

	virtual orsIProperty *getRdfTree() = 0;

	// 返回主体的属性
	virtual orsIProperty  *getSubject( const orsChar *subjectName ) const = 0;

	// 查询 满足条件的父类
	virtual bool getParentSubject( const orsChar *subject, const orsChar *verb, const orsChar *objectWanted, orsString &parent ) const = 0;

	// 按推理模式查询主体的属性
	virtual bool getObject( const orsChar *subject, const orsChar *verb, orsString &object ) const = 0;

	// 从xml文件中读取元数据，并存到 属性树 中
	virtual ref_ptr<orsIProperty> readMetaDataFromXmlFile( const orsChar *xmlFileName ) = 0;

public:
	//接口
	ORS_INTERFACE_DEF( orsIService, _T("rdf"))
};


#define ORS_SERVICE_RDF				_T("ors.service.rdf")
#define ORS_SERVICE_RDF_DEFAULT		_T("ors.service.rdf.default")


// 获取几何服务的宏定义
orsIRDFService *getRDFService();

#define ORS_GET_RDF_SERVICE_IMPL()	\
	static orsIRDFService *s_RDFService = NULL;\
	orsIRDFService *getRDFService()\
{\
	if( NULL != s_RDFService )\
	return s_RDFService;\
	\
	s_RDFService =\
	ORS_PTR_CAST(orsIRDFService, getPlatform()->getService( ORS_SERVICE_RDF) );\
	\
	return s_RDFService;\
}


//////////////////////////////////////////////////////////////////////////
// OpenRS Basic Vocabulary ( 应用无关词汇 )
//
//-------------------------------- subject/object -------------------------
#define rdfSTRING				_T("String")
#define rdfINTEGER				_T("Integer")
#define rdfFLOAT				_T("Float")

// 枚举字符串
#define rdfEnumSTRING			_T("EnumString")

// 枚举
#define rdfENUM					_T("Enum")

// 文件名
#define rdfFileNAME				_T("FileName")
// 接口类名	
#define rdfClassID				_T("ClassID")

#define rdfDESC					_T("Desc")


//--------------------------------- verbs ----------------------------------
// 继承关系
#define rdfIsA					_T("IsA")

#define rdfClassINTERFACE		_T("ClassInterface")

// 数据类型
#define rdfDataTYPE				_T("DataType")

// 数据约束
#define rdfMaxLENGTH			_T("MaxLength")
#define rdfMinValue				_T("MinValue")
#define rdfMaxValue				_T("MaxValue")

// 文件相关
#define rdfFileTYPE				_T("FileType")
#define rdfFileExt				_T("FileExt")

/////////////////////// property constraints //////////////////

#define minVALUE		_T("MinValue")
#define maxVALUE		_T("MaxValue")
#define valueENUM		_T("ValueEnum")

#define stringTYPE		_T("StringType")
#define fileNAME		_T("FileName")
#define colorTABLE		_T("ColorTable")
#define bandLIST		_T("BandList")

#define imgFILE			_T("ImageFile")
#define sfFILE			_T("SimgleFeatureFile")
#define txtFILE			_T("TextFile")
#define csvFILE			_T("csvFile")
#define roiFILE			_T("RoiFILE")

#endif


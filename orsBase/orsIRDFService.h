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
// Purpose: OpenRS ��Դ��������
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


// ��Դ��������
interface orsIRDFService : public orsIService
{
public:

	virtual orsIProperty *getRdfTree() = 0;

	// �������������
	virtual orsIProperty  *getSubject( const orsChar *subjectName ) const = 0;

	// ��ѯ ���������ĸ���
	virtual bool getParentSubject( const orsChar *subject, const orsChar *verb, const orsChar *objectWanted, orsString &parent ) const = 0;

	// ������ģʽ��ѯ���������
	virtual bool getObject( const orsChar *subject, const orsChar *verb, orsString &object ) const = 0;

	// ��xml�ļ��ж�ȡԪ���ݣ����浽 ������ ��
	virtual ref_ptr<orsIProperty> readMetaDataFromXmlFile( const orsChar *xmlFileName ) = 0;

public:
	//�ӿ�
	ORS_INTERFACE_DEF( orsIService, _T("rdf"))
};


#define ORS_SERVICE_RDF				_T("ors.service.rdf")
#define ORS_SERVICE_RDF_DEFAULT		_T("ors.service.rdf.default")


// ��ȡ���η���ĺ궨��
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
// OpenRS Basic Vocabulary ( Ӧ���޹شʻ� )
//
//-------------------------------- subject/object -------------------------
#define rdfSTRING				_T("String")
#define rdfINTEGER				_T("Integer")
#define rdfFLOAT				_T("Float")

// ö���ַ���
#define rdfEnumSTRING			_T("EnumString")

// ö��
#define rdfENUM					_T("Enum")

// �ļ���
#define rdfFileNAME				_T("FileName")
// �ӿ�����	
#define rdfClassID				_T("ClassID")

#define rdfDESC					_T("Desc")


//--------------------------------- verbs ----------------------------------
// �̳й�ϵ
#define rdfIsA					_T("IsA")

#define rdfClassINTERFACE		_T("ClassInterface")

// ��������
#define rdfDataTYPE				_T("DataType")

// ����Լ��
#define rdfMaxLENGTH			_T("MaxLength")
#define rdfMinValue				_T("MinValue")
#define rdfMaxValue				_T("MaxValue")

// �ļ����
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


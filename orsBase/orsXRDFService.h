//*************************************************************************
//
// Copyright (C) 2010, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsXRDFService.h 2008/11/27 8:40 JWS $
//
// Project: OpenRS
//
// Purpose: OpenRS ��Դ��������
//
// Author: JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsXRDFService.h,v $
//
// Revision 0.1 date: 2010/3/8 by JIANG Wanshou
// new
//

#ifndef _ORS_RDF_SERVICE_IMPL_H
#define _ORS_RDF_SERVICE_IMPL_H

#include "orsBase/orsIRDFService.h"
#include <string>

class TiXmlNode;

// ��Դ��������
class orsXRDFService : public orsIRDFService, orsObjectBase
{
private:
	ref_ptr<orsIProperty> m_rdf;

private:

	void LoadRdf( orsString &rdfFileName );

	// ��ȡ��ָ�����͵���ͨxml
	bool readXmlNode( TiXmlNode * pNode, orsIProperty *record );

public:

	virtual orsIProperty *getRdfTree()	
	{	
		return m_rdf.get();
	};

	// �������������
	virtual orsIProperty *getSubject( const orsChar *subjectName ) const;

	// ��ѯ ���������ĸ���
	virtual bool getParentSubject( const orsChar *subject, const orsChar *verb, const orsChar *objectWanted, orsString &parent ) const ;
	
	// ������ģʽ��ѯ���������
	virtual bool getObject( const orsChar *subject, const orsChar *verb, orsString &object ) const ;

	// Ԫ���ݱ���Ķ���û�����͵��ַ���
	virtual ref_ptr<orsIProperty> readMetaDataFromXmlFile( const orsChar *xmlStringOrFileName );

public:

	//����ʱ������
	virtual bool startup( orsIPlatform *platform );
	
	//�ر�ʱ������
	virtual void shutdown();
	
	//�Ƿ������ɹ�
	virtual bool isok();




public:
	//�ӿ�
	ORS_OBJECT_IMP2( orsXRDFService, orsIRDFService, orsIService, _T("default"), _T("RDF Service") )
};



#endif


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
// Purpose: OpenRS 资源描述服务
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

// 资源描述服务
class orsXRDFService : public orsIRDFService, orsObjectBase
{
private:
	ref_ptr<orsIProperty> m_rdf;

private:

	void LoadRdf( orsString &rdfFileName );

	// 读取无指定类型的普通xml
	bool readXmlNode( TiXmlNode * pNode, orsIProperty *record );

public:

	virtual orsIProperty *getRdfTree()	
	{	
		return m_rdf.get();
	};

	// 返回主体的属性
	virtual orsIProperty *getSubject( const orsChar *subjectName ) const;

	// 查询 满足条件的父类
	virtual bool getParentSubject( const orsChar *subject, const orsChar *verb, const orsChar *objectWanted, orsString &parent ) const ;
	
	// 按推理模式查询主体的属性
	virtual bool getObject( const orsChar *subject, const orsChar *verb, orsString &object ) const ;

	// 元数据保存的都是没有类型的字符串
	virtual ref_ptr<orsIProperty> readMetaDataFromXmlFile( const orsChar *xmlStringOrFileName );

public:

	//启动时被调用
	virtual bool startup( orsIPlatform *platform );
	
	//关闭时被调用
	virtual void shutdown();
	
	//是否启动成功
	virtual bool isok();




public:
	//接口
	ORS_OBJECT_IMP2( orsXRDFService, orsIRDFService, orsIService, _T("default"), _T("RDF Service") )
};



#endif


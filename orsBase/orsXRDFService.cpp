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

#include "stdafx.h"

#include "tinyxml2/tinyxml2.h"

#define TiXmlDocument tinyxml2::XMLDocument
#define TiXmlPrinter  tinyxml2::XMLPrinter

#define TiXmlNode tinyxml2::XMLNode
#define TiXmlText  tinyxml2::XMLText
#define TiXmlComment tinyxml2::XMLComment
#define TiXmlDeclaration tinyxml2::XMLDeclaration
#define TiXmlUnknown tinyxml2::XMLUnknown

#define TiXmlElement tinyxml2::XMLElement
#define TiXmlAttribute tinyxml2::XMLAttribute


#include "orsBase/orsIPlatform.h"

#include "orsCommon.h"
#include "orsXRDFService.h"

#include "orsBase/orsIUtilityService.h"


// #if defined kyLINUX64
// #include <vector>
// using namespace std;
// #define orsArray std::vector
// #endif

//启动时被调用
bool orsXRDFService::startup( orsIPlatform *platform )
{
	orsString path;
	
	orsIUtilityService *pUtilityService = (orsIUtilityService *)_getPlatform()->getService( ORS_SERVICE_UTILITY);

	assert( NULL != pUtilityService );

	orsString etcDir;

	pUtilityService->GetDirectory_ETC( etcDir );

	path = etcDir + "RDF";
	//////////////////////////////////////////////////////////////////////////
	
	//找到目录下所有的dll
	orsArray<orsString> allRdfs;
	
	findAllFiles( path, "*.rdf.txt", allRdfs );
	
	if( allRdfs.size() > 0 )
		m_rdf = platform->createProperty();

	//加载插件
	ors_uint32 i = 0;
	for(i=0;i<allRdfs.size();i++)
	{
		LoadRdf( allRdfs[i] );
	}
	
	return true;
}

//关闭时被调用
void orsXRDFService::shutdown()
{
}

//是否启动成功
bool orsXRDFService::isok()
{
	if( NULL != m_rdf.get() )
		return true;

	return false;
}



void orsXRDFService::LoadRdf( orsString &rdfFileName )
{
	FILE *fp;

	fp = fopen( rdfFileName.c_str(), "rt" );
	
	if( NULL == fp )
		return ;
	
	orsChar lineBuf[256];
	
	while( !feof(fp) )	{
		fgets( lineBuf, 256, fp );
		
		// find subject
		orsChar *pSubject, *pEnd;

		pSubject = ( orsChar *)orsString::findSubStr_i( lineBuf, _T("<") );
		pEnd = ( orsChar *)orsString::findSubStr_i( lineBuf, _T(">") );

		if( pSubject && pEnd )	{
			pSubject++;
			*pEnd++ = 0;

			orsChar *pVerb = ( orsChar *)orsString::findSubStr_i( pEnd, _T("<") );
			pEnd = ( orsChar *)orsString::findSubStr_i( pEnd, _T(">") );

			if( pVerb && pEnd ) {
				pVerb++;
				*pEnd++ = 0;

				orsChar *pObject= ( orsChar *)orsString::findSubStr_i( pEnd, _T("<") );
				pEnd = ( orsChar *)orsString::findSubStr_i( pEnd, _T(">") );

				pObject++;
				*pEnd = 0;
		
				// 有效的三元组

				ref_ptr<orsIProperty> prop;

				m_rdf->getAttr( pSubject, prop );

				orsString strSubject;
				strSubject = pSubject;

				// 不存在，创建一个
				if( NULL == prop.get() ) {
					prop = _getPlatform()->createProperty();
					m_rdf->addAttr( pSubject, prop );
				}

				prop->addAttr( pVerb, pObject );
			}
		}
	}
	
	fclose( fp );
}


orsIProperty *orsXRDFService::getSubject( const orsChar *subjectName ) const
{
	ref_ptr<orsIProperty> prop;

	if( NULL == m_rdf.get() )
		return NULL;

	m_rdf->getAttr( subjectName, prop );

	return prop.get();
}

// 查询 满足条件的父类
bool orsXRDFService::getParentSubject( const orsChar *subject0, const orsChar *verb, const orsChar *objectWanted, orsString &parent ) const
{
	if( NULL == m_rdf.get() )
		return false;

	ref_ptr<orsIProperty> prop;
	orsString subject = subject0;
	orsString object;

	while( true ) 
	{
		// 主体不存在
		if( !m_rdf->getAttr( subject, prop ) )
			return false;
		
		if( prop->getAttr( verb, object ) )	{
			if( 0 == _tcsicmp( objectWanted, object.c_str() ) )
			{	
				parent = subject;
				return true;
			}
		}
		
		// 找父类
		if( prop->getAttr( rdfIsA, object ) )
			subject = object;
		else	
			return false;

	} ;

	return false;
}

// 按推理模式查询主体的属性
bool orsXRDFService::getObject( const orsChar *subject, const orsChar *verb, orsString &object ) const
{
	if( NULL == m_rdf.get() )
		return false;

	ref_ptr<orsIProperty> prop;
	orsString parent;

	while( true ) 
	{
		// 主体不存在
		if( !m_rdf->getAttr( subject, prop ) )
			return false;
				
		if( prop->getAttr( verb, object ) )	
			return true;
		
		// 找父类
		if( prop->getAttr( rdfIsA, parent ) )
			subject = parent;
		else
			return false;
		
	} ;
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

// 不需要 每个 属性都有 类型
bool orsXRDFService::readXmlNode( TiXmlNode * pNode, orsIProperty *record )
{
	TiXmlElement* xmlElement = 0 ;
	const char*	key;
	const char*	type;
	const char* text;
	
	while (pNode) {
		//判断节点类型
		TiXmlNode *pChildNode = pNode->FirstChild();

		if( pChildNode == NULL ) {
			//叶节点
			xmlElement = pNode->ToElement();

			if( NULL == xmlElement ) {
				pNode = pNode->NextSibling();
				continue;
			}

			key = xmlElement->Value();
			text = NULL;
			type = xmlElement->Attribute("type");
		}
		else if ( pNode->ToDocument() ) {
			//xml根节点
			readXmlNode(pChildNode,record);
			return true;
		}
		else if( pChildNode->ToElement()) {
			//嵌套子节点
			xmlElement = pNode->ToElement();
			
			key = xmlElement->Value();

			type = "property";
		}
		else {
			//叶节点
			xmlElement = pNode->ToElement();
			key = xmlElement->Value();
			text = xmlElement->GetText();
			type = xmlElement->Attribute("type");
		}
	

		// 根据节点数据类型读取,
		
		// 无类型, 按照字符串存储
		if(type == NULL) {
			//////////////////////////////////////////////////////////////////////////
			// 允许有attribute
			const TiXmlAttribute *attr;
			
			attr = xmlElement->FirstAttribute();

			if( NULL == attr )	{
				if( NULL != text && strlen(text) > 0 )
					record->addAttr(key,text);
			}
			else	{
				ref_ptr<orsIProperty> child_value = _getPlatform()->createProperty();

				if( NULL != text && strlen(text) > 0 )
					child_value->addAttr(key, text);
				
				while( attr )	{
					// 假定都是字符串
					if( NULL != attr->Value() && orsStrLen( attr->Value() ) > 0 )	{
						child_value->addAttr( attr->Name(), attr->Value() );
					}
					attr = attr->Next();
				}
				record->addAttr( key, child_value);
			}
		}
		else	{
			// 新节点
			if( 0==stricmp(type,"property") ) {
				ref_ptr<orsIProperty> child_value = _getPlatform()->createProperty();
				
				int bOk = readXmlNode( pChildNode, child_value.get() );
				
				if( !bOk ) 
					return bOk;

				//////////////////////////////////////////////////////////////////////////
				// 允许有attribute
				const TiXmlAttribute *attr;
				
				attr = xmlElement->FirstAttribute();
				
				while( attr )	{
					// 假定都是字符串
					if( NULL != attr->Value() && orsStrLen( attr->Value() ) > 0 )	{
						child_value->addAttr( attr->Name(), attr->Value() );
					}
					
					attr = attr->Next();
				}
				//////////////////////////////////////////////////////////////////////////

				record->addAttr( key, child_value);
			}
			else if (0==stricmp(type,"string")) {
				record->addAttr(key,text);
			}
			else if( 0==stricmp(type, "int16") ) {
				int  value;
				sscanf(text,"%d",&value);
				record->addAttr(key,(ors_int16)value);
			}
			else if( 0==stricmp(type, "int32") ||  0==stricmp(type, "int") ){
				ors_int32 value;
				sscanf(text,"%d", &value);
				record->addAttr(key,value);
			}
			else if( 0==stricmp(type,"float64") || 0==stricmp(type,"float") ) {
				ors_float64 value;
				sscanf(text,"%lf", &value);
				record->addAttr(key, value);
			}
			else {
				return false;
			}			
		}
		
		pNode = pNode->NextSibling();
	}
	
	return true;
}

ref_ptr<orsIProperty> orsXRDFService::readMetaDataFromXmlFile(const orsChar *xmlStringOrFileName )
{
	FILE *fp = fopen( xmlStringOrFileName, "r");
	
	if( NULL == fp )
		return NULL;
	
	//得到文件大小
	fseek(fp,0,SEEK_END);
	
	long file_len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	
	//读取信息
	char *buf = new char[file_len+10];

	if( NULL == buf )	{
		_getPlatform()->logPrint(ORS_LOG_ERROR, "Can not alloc %ld bytes", file_len );
		fclose( fp );
		return NULL;
	}

	memset(buf, 0, file_len+10 );
	
	fread(buf,file_len,sizeof(char),fp);
	
	fclose( fp );
	
	//////////////////////////////////////////////////////////////////////////

	ref_ptr<orsIProperty> root = _getPlatform()->createProperty();
	
	TiXmlDocument xmlDoc;

	xmlDoc.Parse( buf ) ;

	readXmlNode( &xmlDoc, root.get() );

	//////////////////////////////////////////////////////////////////////////
	delete buf;
	
	return root;
}

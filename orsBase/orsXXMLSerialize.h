#ifndef ORS_XXML_SERIALIZE_IMP_H
#define ORS_XXML_SERIALIZE_IMP_H

#include <string>

#include "orsBase/orsIProperty.h"
#include "orsBase/orsIPlatform.h"

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


class ORS_BASE_API orsXXMLSerialize : public orsIPropertySerialize, public orsObjectBase
{		
private:
	bool m_bFromUTF8;

private:

	void AddAttr_UTF8( orsIProperty *pProp, const char *name, const char *pStr );
	//添加真实的属性，如：<task name="ImageGeoQualityCheck">
	void AddAttribute_UTF8( orsIProperty *pProp, const char *name, const char *pStr );

public:
	virtual bool import( const char *bytes,long length, orsIProperty *info);
	virtual bool outport( char *&bytes,long *length, const orsIProperty *info);
	virtual bool outport_noUTF_8Head( char *&bytes,long *length, const orsIProperty *info);
	
	// 从文件读取，info需要事先分配内存
	virtual bool importFromFile(const char *xmlFileName, orsIProperty *info);

	// 保存到文件
	virtual bool outportToFile(const char *xmlFileName, const orsIProperty *info, bool bWriteValueTypeAttribute = true);

	virtual bool free(char *&bytes);

private:
	int readXmlNode(TiXmlNode* pNode, orsIProperty *record);
	int writeXmlNode(TiXmlDocument *pDoc, TiXmlNode* pNode, const orsIProperty *record, bool bWriteValueTypeAttribute);

public:
	//接口宏定义
	ORS_OBJECT_IMP1(orsXXMLSerialize, orsIPropertySerialize, _T("xml"), _T("xmlSerializer"))
};


#endif

/********************************************************************
	created:	2008/09/20
	created:	20:9:2008 17:00
	file ext:	cpp
	author:		Guo Wei

	purpose:
				属性序列化

  BOM（Byte Order Mark）是一个字符，它表明UNICODE文本的UTF-16,UTF-32的编码字节顺序（高字节低字节顺序）和编码方式
  Encoding Representation 
		UTF-8 EF BB BF 
		UTF-16 Big Endian FE FF 
		UTF-16 Little Endian FF FE 
		UTF-32 Big Endian 00 00 FE FF 
		UTF-32 Little Endian FF FE 00 00
	
  其中UTF-8编码是字节顺序无关的

  有些utf8编码没有这个BOM,该怎么区分了,是utf8还是ansi(根本就没有BOM这个东西),下面先了解下utf8:
  UTF-8是UNICODE的一种变长字符编码，由Ken Thompson于1992年创建。现在已经标准化为RFC 3629。UTF-8用1到6个字节编码UNICODE字符。
  如果UNICODE字符由2个字节表示，则编码成UTF-8很可能需要3个字节，而如果UNICODE字符由4个字节表示，则编码成UTF-8可能需要6个字节。
*********************************************************************/

#include "stdafx.h"

#include <string>

#include "orsBase/orsIProperty.h"
#include "orsCommon.h"

#include "orsXXMLSerialize.h"
#include "orsXProperty.h"

using namespace tinyxml2;

#ifdef ORS_PLATFORM_WINDOWS

static std::string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	unsigned short * wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);

	//strUTF8 = szGBK;

	std::string strTemp(szGBK);

	delete[]szGBK;
	delete[]wszGBK;

	return strTemp;
}


static std::string GBKToUTF8(const std::string& mbcsStr)
{
	wchar_t*  wideStr;
	char*   utf8Str;
	int   charLen;

	charLen = MultiByteToWideChar(936, 0, mbcsStr.c_str(), -1, NULL, 0);    ////////936 ----- gb2312
	wideStr = (wchar_t*)malloc(sizeof(wchar_t)*charLen);

	MultiByteToWideChar(936, 0, mbcsStr.c_str(), -1, wideStr, charLen);

	charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);

	utf8Str = (char*)malloc(charLen);

	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);

	std::string strTemp(utf8Str);

	free(wideStr);
	free(utf8Str);

	return strTemp;
}

#else


#include <iconv.h>

std::string UTF8ToGBK(const std::string& strUTF8)
{
	iconv_t cd = iconv_open("GBK", "UTF-8");

	const char *pin = strUTF8.c_str();
	size_t inlen = strlen(pin);

	size_t outlen = inlen * 4;

	char *outbuf = (char *)malloc(outlen);

	//bzero( outbuf, outlen );
	memset(outbuf, 0, outlen);

	char *in = (char *)pin;
	char *out = outbuf;

	// 注意in和out会变化

	size_t ret = iconv(cd, &in, &inlen, &out, &outlen);

	if (ret == -1)
	{
		printf("iconv failed\n");
		perror("iconv");
	}
	std::string strTemp(outbuf);

	free(outbuf);

	iconv_close(cd);

	return strTemp;
}


std::string GBKToUTF8(const std::string& strGBK)
{
	iconv_t cd = iconv_open("UTF-8", "GBK");

	const char *pin = strGBK.c_str();
	size_t inlen = strlen(pin);

	size_t outlen = inlen * 4;

	char *outbuf = (char *)malloc(outlen);

	//bzero( outbuf, outlen );
	memset(outbuf, 0, outlen);
	char *in = (char *)pin;
	char *out = outbuf;

	// 注意in和out会变化

	iconv(cd, &in, &inlen, &out, &outlen);

	std::string strTemp(outbuf);

	free(outbuf);

	iconv_close(cd);

	return strTemp;
}


#endif



bool orsXXMLSerialize::import( const char *bytes,long length, orsIProperty *info)
{
	TiXmlDocument xmlDoc;

	unsigned char *pBytes = (unsigned char *)bytes;

	if( orsString::findSubStr_i(bytes, "UTF-8" ) || ( 0xEF == pBytes[0] && 0xBB == pBytes[1] && 0xBF == pBytes[2] ) )	
	{
		m_bFromUTF8 = true;
	}
	else	{
		m_bFromUTF8 = false;
	}

	xmlDoc.Parse( bytes ) ;

	readXmlNode( &xmlDoc, info );

	return true ;
}


bool orsXXMLSerialize::importFromFile(const char *xmlFileName, orsIProperty *info)
{
	TiXmlDocument xmlDoc;

	if ( XML_SUCCESS == xmlDoc.LoadFile(xmlFileName)) {

		if (xmlDoc.HasBOM())
			m_bFromUTF8 = true;
		else {
			m_bFromUTF8 = false;

			char buf[256];
			FILE* fp = fopen(xmlFileName,"rt");
			if (NULL != fp) {
				fgets(buf, 256, fp);

				if (NULL != orsString::findSubStr_i(buf, "UTF-8"))
					m_bFromUTF8 = true;
				
				fclose(fp);
			}
		}

		readXmlNode(&xmlDoc, info);

		return true;
	}

	return false;
}


bool orsXXMLSerialize::outportToFile(const char *xmlFileName, const orsIProperty *info, bool bWriteValueTypeAttribute)
{
	TiXmlDocument xmlDoc;

	XMLDeclaration *declare = xmlDoc.NewDeclaration();
	xmlDoc.LinkEndChild(declare);

	xmlDoc.SetBOM(true);

	writeXmlNode(&xmlDoc, &xmlDoc, info, bWriteValueTypeAttribute);

	if( XML_SUCCESS == xmlDoc.SaveFile(xmlFileName) )
		return true;

	return false;
}


bool orsXXMLSerialize::outport(char *&bytes,long *length, const orsIProperty *info)
{
	m_bFromUTF8 = true;

	TiXmlDocument xmlDoc;

	writeXmlNode(&xmlDoc, &xmlDoc, info, true);

	TiXmlPrinter printer;

#ifdef TINYXML2_INCLUDED
	printer.PushText("\t");
#else
	printer.SetIndent( "\t" );
#endif 

	xmlDoc.Accept( &printer );
	const char* charXmlDoc = printer.CStr();
	*length =   strlen(charXmlDoc) ;

	const char *pHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	int headLen = strlen(pHead);

	bytes = new char[ headLen + *length + 1];
	strcpy( bytes, pHead);
	memcpy (bytes+ headLen, charXmlDoc, *length) ;

	*length += headLen;

	bytes[*length] = '\0';

	return true ;
}

bool orsXXMLSerialize::outport_noUTF_8Head(char *&bytes,long *length, const orsIProperty *info)
{
	m_bFromUTF8 = true;

	TiXmlDocument xmlDoc;

	writeXmlNode(&xmlDoc, &xmlDoc,info,true);

	TiXmlPrinter printer;

#ifdef TINYXML2_INCLUDED
	printer.PushText("\t");
#else
	printer.SetIndent("\t");
#endif 

	xmlDoc.Accept( &printer );
	const char* charXmlDoc = printer.CStr();
	*length =   strlen(charXmlDoc) ;

	bytes = new char[ *length + 1];
	memcpy (bytes, charXmlDoc, *length) ;

	bytes[*length] = '\0';

	return true ;
}

bool orsXXMLSerialize::free(char *&bytes)
{
	delete bytes;

	return true;
}


static char *NodeTypes[] =
{
	"TINYXML_DOCUMENT",
		"TINYXML_ELEMENT",
		"TINYXML_COMMENT",
		"TINYXML_UNKNOWN",
		"TINYXML_TEXT",
		"TINYXML_DECLARATION",
		"TINYXML_TYPECOUNT"
};



bool HasElementChildren( TiXmlNode * pNode )
{
#ifdef TINYXML2_INCLUDED
	return (NULL != pNode->FirstChildElement());
#else
	TiXmlNode *pChildNode = pNode->FirstChild();

	while (pChildNode) {
		if (TiXmlNode::TINYXML_ELEMENT == pChildNode->Type())
			return true;

		pChildNode = pChildNode->NextSibling();
	}
#endif

	return false;
}


void orsXXMLSerialize::AddAttr_UTF8( orsIProperty *pProp, const char *name, const char *pStr )
{
	if( NULL == pStr || strlen(pStr)==0)//当为空时，直接添加， by whx
	{
		pProp->addAttr( name,pStr); 
		return ;
	}

	if( m_bFromUTF8 )	{
		std::string gbk = UTF8ToGBK( pStr );
		pProp->addAttr( name, gbk.c_str() );
	}
	else 
		pProp->addAttr( name, pStr );
}


void orsXXMLSerialize::AddAttribute_UTF8( orsIProperty *pProp, const char *name, const char *pStr )
{
	if( NULL == pStr || strlen(pStr)==0 ) //当pStr为“”时，UTF8ToGBK函数会出错，所以这里进行判断，by whx，2017.1.25
	{
		pProp->addAttribute( name, pStr );
		return ;
	}

	if( m_bFromUTF8 )	{
		std::string gbk = UTF8ToGBK( pStr );
		pProp->addAttribute( name, gbk.c_str() );
	}
	else 
		pProp->addAttribute( name, pStr );
}


int orsXXMLSerialize::readXmlNode(TiXmlNode * pNode, orsIProperty *record)
{
	TiXmlElement* xmlElement = 0;
	const char*	key;
	const char*	type;
	const char* text;

	while (pNode) {
		if (pNode->ToDocument())
		{
			readXmlNode(pNode->FirstChild(), record);
		}
		else if (pNode->ToComment())
		{
			TiXmlComment *doc = pNode->ToComment();
			const char *value = doc->Value();
		}
		else if (pNode->ToUnknown())
		{
		}
		else if (pNode->ToText()) {
			TiXmlText *xmlText = pNode->ToText();
			const char *value = xmlText->Value();
			//assert(false);
		}
		else if (pNode->ToDeclaration()) {
			TiXmlDeclaration *xmlDeclr = pNode->ToDeclaration();
			const char *value = xmlDeclr->Value();
		}
		else if (pNode->ToElement()) {
			// 嵌套子节点
			xmlElement = pNode->ToElement();
			key = xmlElement->Value();

			if (HasElementChildren(pNode)) {
				ref_ptr<orsIProperty> children = new orsXProperty;

				// 按第一个传递，会自动遍历兄弟
				int nErr = readXmlNode(pNode->FirstChild(), children.get());

				if (nErr == 1)
					return nErr;

				///////////////////////////////////
				// 允许有attribute，转换为子属性
				const TiXmlAttribute *attr;

				attr = xmlElement->FirstAttribute();

				while (attr) {
					// 假定都是字符串
					// children->addAttr( attr->Name(), attr->Value() );
					AddAttribute_UTF8(children.get(), attr->Name(), attr->Value());

					attr = attr->Next();
				}

				//////////////////////////////////
				record->addAttr(key, children);
			}
			else {
				//叶节点
				xmlElement = pNode->ToElement();
				key = xmlElement->Value();
				text = xmlElement->GetText();
				type = xmlElement->Attribute("type");

				// 无类型, 按照字符串存储
				if (type == NULL) {
					// 分有attribute和无attribute 2种情况
					if (NULL != xmlElement->FirstAttribute()) {

						ref_ptr<orsIProperty> child_value = new orsXProperty;

						// 看有没有attribute, 作为child属性来实现
						const TiXmlAttribute *attr;

						attr = xmlElement->FirstAttribute();

						while (attr) {
							// 假定都是字符串
							if (NULL != attr->Value() && orsStrLen(attr->Value()) > 0)
							{
								//child_value->addAttr( attr->Name(), attr->Value() );
								AddAttribute_UTF8(child_value.get(), attr->Name(), attr->Value());
							}
							attr = attr->Next();
						}

						// 如果有文本，改为value保存
						//if( NULL != text && strlen(text) > 0 )
						//{
							//child_value->addAttr( _T("value"), text ) ;
						AddAttr_UTF8(child_value.get(), _T("value"), text);
						//}

						record->addAttr(key, child_value);
					}
					else {
						//if( NULL != text && strlen(text) > 0 )
						//{
							//record->addAttr(key,text);
						AddAttr_UTF8(record, key, text);
						//}
					}
				}
				else {
					//根据节点数据类型读取
					if (0 == stricmp(type, "bool")) {
						char buf[32];
						sscanf(text, "%s", buf);

						bool value;

						if (0 == _tcsicmp(buf, "true"))
							value = true;
						else
							value = false;

						record->addAttr(key, value);
					}
					else if (0 == stricmp(type, "int16")) {
						int value;
						sscanf(text, "%d", &value);
						record->addAttr(key, (ors_int16)value);
					}
					else if (0 == stricmp(type, "int32")) {
						ors_int32 value;
						sscanf(text, "%d", &value);
						record->addAttr(key, value);
					}
					else if (0 == stricmp(type, "float64")) {
						ors_float64 value;
						sscanf(text, "%lf", &value);
						record->addAttr(key, value);
					}
					else if (0 == stricmp(type, "string"))
					{
						//record->addAttr( key, text );
						AddAttr_UTF8(record, key, text);
					}
					else if (0 == stricmp(type, "blob") || 0 == stricmp(type, "vector") || 0 == stricmp(type, "matrix")) {
						ors_byte * val = NULL;
						ors_int32 len = 0;

						// 如果后面没有内容，则忽略
						if (NULL != text) {
							int v_len = orsBase64::getDataLength(strlen(text));
							unsigned char* data = new unsigned char[v_len];

							int realLen = orsBase64::decode(text, strlen(text), data);

							if (0 == stricmp(type, "blob"))
								record->addAttr(key, data, v_len);
							else if (0 == stricmp(type, "vector")) {
								ors_int32 *pInfo = (ors_int32 *)data;
								orsDataTYPE type = (orsDataTYPE)pInfo[0];
								ors_int32 nRows = pInfo[1];

								int headBytes = 2 * sizeof(ors_int32);
								int dataBytes = nRows * orsGetSizeOfType(type);

								switch (type) {
								case ORS_DT_FLOAT32:
								{
									orsVector<ors_float32>	vector;
									vector.Alloc(nRows);
									memcpy(vector.Buf(), data + headBytes, dataBytes);

									record->addAttr(key, vector);
								}
								break;
								case ORS_DT_FLOAT64:
								{
									orsVector<ors_float64> vector;
									vector.Alloc(nRows);
									memcpy(vector.Buf(), data + headBytes, dataBytes);

									record->addAttr(key, vector);
								}
								break;
								default:
									assert(false);
								}
							}
							else if (0 == stricmp(type, "matrix")) {
								ors_int32 *pInfo = (ors_int32 *)data;
								orsDataTYPE type = (orsDataTYPE)pInfo[0];
								ors_int32 nRows = pInfo[1];
								ors_int32 nCols = pInfo[2];

								int headBytes = 3 * sizeof(ors_int32);
								int dataBytes = nRows * nCols*orsGetSizeOfType(type);

								switch (type) {
								case ORS_DT_FLOAT32:
								{
									orsMatrix<ors_float32>	matrix;
									matrix.Alloc(nRows, nCols);
									memcpy(matrix.Buf(), data + headBytes, dataBytes);

									record->addAttr(key, matrix);
								}
								break;
								case ORS_DT_FLOAT64:
								{
									orsMatrix<ors_float64>matrix;
									matrix.Alloc(nRows, nCols);
									memcpy(matrix.Buf(), data + headBytes, dataBytes);
									record->addAttr(key, matrix);
								}
								break;
								default:
									assert(false);
								}
							}
							delete[]data;
						}
					}
				}
			}
		}

		// 遍历兄弟
		pNode = pNode->NextSibling();
	}

	return 0 ;
}

int orsXXMLSerialize::writeXmlNode(TiXmlDocument *pDoc, TiXmlNode * pNode, const orsIProperty *record, bool bWriteValueTypeAttribute)
{
	unsigned int i=0;
	for (i=0;i<record->size();i++)
	{
		ors_string name;
		orsVariantType type;
		ors_int32 valuenum;
		record->getAttributeInfo(i,name,type,valuenum, false);

		switch(type){
		case ORS_V_BOOL:
			{
				bool value;
				record->getAttr(name, value);

				TiXmlElement* pChildeElm = pDoc->NewElement(name);
				
				if( bWriteValueTypeAttribute )
					pChildeElm->SetAttribute("type", "bool");

				char text[32];
				memset(text,0,32);

				if( value )
					sprintf(text,"true");
				else
					sprintf(text,"false");

				TiXmlText* pText = pDoc->NewText(text);

				pChildeElm->InsertEndChild(pText);
				pNode->InsertEndChild(pChildeElm);
			}
			break;
		case ORS_V_I2:
			{
				orsArray<ors_int16> values;
				record->getAttr(name, values);
				for (unsigned j=0;j<values.size();j++)
				{
					TiXmlElement* pChildeElm = pDoc->NewElement(name);
					
					if (bWriteValueTypeAttribute)
						pChildeElm->SetAttribute("type", "int16");

					ors_int16 i_value = values[j];
					char text[32];
					memset(text,0,32);
					sprintf(text,"%d",i_value);

					TiXmlText* pText = pDoc->NewText(text);

					pChildeElm->InsertEndChild(pText);
					pNode->InsertEndChild(pChildeElm);
				}
			}
			break;
		case ORS_V_I4:
			{
				orsArray<ors_int32> values;
				record->getAttr(name, values);
				for (unsigned j=0;j<values.size();j++)
				{
					TiXmlElement* pChildeElm = pDoc->NewElement(name);
					
					if (bWriteValueTypeAttribute)
						pChildeElm->SetAttribute("type", "int32");
	
					ors_int32 i_value = values[j];
					
					char text[32];
					memset(text,0,32);
					sprintf(text,"%d",i_value);

					TiXmlText* pText = pDoc->NewText(text);

					pChildeElm->InsertEndChild(pText);
					pNode->InsertEndChild(pChildeElm);
				}
			}
			break;
		case ORS_V_R8:
			{
				orsArray<ors_float64> values;
				record->getAttr(name, values);
				for (unsigned j=0;j<values.size();j++)
				{
					TiXmlElement* pChildeElm = pDoc->NewElement(name);
					
					if (bWriteValueTypeAttribute)
						pChildeElm->SetAttribute("type", "float64");
					
					ors_float64 i_value = values[j];

					char text[200];
					memset(text,0,200);
					sprintf(text,"%.8lf",i_value);

					TiXmlText* pText = pDoc->NewText(text);

					pChildeElm->InsertEndChild(pText);
					pNode->InsertEndChild(pChildeElm);
				}
			}
			break;
		case ORS_V_STR:
			{
				orsArray<ors_string> values;
				
				record->getAttr(name, values);

				//////////////////////////////////////////////////////////////////////////
				//如果name是value,即创建属性时添加的关键字，则在写出时去掉这个关键词，只输出值
				if (name=="value"&&values.size()==1)
				{
					TiXmlElement *xmlElement = pNode->ToElement();
					if (!values[0].isEmpty())
					{
						if (xmlElement->ToText()==NULL)//说明没有其他的值
							xmlElement->SetText(values[0].c_str());	
					}
					else
					{
						if (xmlElement->ToText()==NULL)//说明没有其他的值
							xmlElement->SetText("");	
					}

				}
				else
				{
					for (unsigned j=0;j<values.size();j++)
					{
						if( !values[j].isEmpty() )	{
							TiXmlElement* pChildeElm = pDoc->NewElement(name);

							//默认就是string, 减少空间浪费；还可和其它软件兼容
							//pChildeElm->SetAttribute("type", "string");	

							std::string s_value = GBKToUTF8( values[j].c_str() );

							TiXmlText* pText = NULL;

							if( 0 == s_value.length() )
							{
								pText = pDoc->NewText("");
							}
							else
								pText = pDoc->NewText(s_value.c_str());

							pChildeElm->InsertEndChild(pText);
							pNode->InsertEndChild(pChildeElm);
						}
						else //当value为空时，by whx,2016/01/21
						{
							TiXmlElement* pChildeElm = pDoc->NewElement(name);

							TiXmlText* pText = NULL;

							pText = pDoc->NewText("");

							pChildeElm->InsertEndChild(pText);
							pNode->InsertEndChild(pChildeElm);
						}
					}
				}


			}
			break;
		case ORS_V_BLOB:
			{
				orsArray<const ors_byte *> values;
				orsArray<ors_int32> nLengths;
				record->getAttr(name, values,nLengths);

				for (unsigned j=0;j<values.size();j++)
				{
					TiXmlElement* pChildeElm = pDoc->NewElement(name);
					pChildeElm->SetAttribute("type", "blob");

					const ors_byte *val = values[j];
					ors_int32 len = nLengths[j];
					std::string blob_str;
					orsBase64::encode(val,len,blob_str);

					TiXmlText* pText = pDoc->NewText(blob_str.c_str());

					pChildeElm->InsertEndChild(pText);
					pNode->InsertEndChild(pChildeElm);
				}
			}
			break;
		case ORS_V_VECTOR:
		case ORS_V_MATRIX:
			{
				const ors_byte *value;
				ors_int32 nLength;

				record->getAttr(name, value, nLength);

				TiXmlElement* pChildeElm = pDoc->NewElement(name);

				if( ORS_V_VECTOR == type )
					pChildeElm->SetAttribute("type", "vector");
				else
					pChildeElm->SetAttribute("type", "matrix");

				std::string blob_str;
				orsBase64::encode(value, nLength, blob_str);

				TiXmlText* pText = pDoc->NewText( blob_str.c_str() );

				pChildeElm->InsertEndChild(pText);
				pNode->InsertEndChild(pChildeElm);
			}
			break;

		case ORS_V_CHILD:
			{
				orsArray<ref_ptr<orsIProperty> > children;
				record->getAttr(name, children );
				for (unsigned i=0;i<children.size();i++)
				{
					TiXmlElement* pChildeElm = pDoc->NewElement(name);

					TiXmlNode *pChildNode = pNode->InsertEndChild(pChildeElm);

					writeXmlNode( pDoc, pChildNode, children[i].get(), bWriteValueTypeAttribute );
				}
			}
			break;
		case ORS_V_LISTNER:
			break;
		case ORS_V_OBJECT:
			{
				orsIObject *obj = NULL;
				record->getAttr(name,obj);
				
				TiXmlElement* pChildeElm = pDoc->NewElement(name);
				pChildeElm->SetAttribute("type", "object");
				ors_string s_value;
				TiXmlText* pText = NULL;
				
				if( s_value.isEmpty() )
				{
					pText =  pDoc->NewText("");
				}
				else
					pText =  pDoc->NewText(s_value.c_str());
				
				pChildeElm->InsertEndChild(pText);
				pNode->InsertEndChild(pChildeElm);
			}
			break;
		case ORS_V_Attribute:
			{
				orsArray<ors_string> values;

				record->getAttr(name, values);

				for (unsigned j=0;j<values.size();j++)
				{
					if( !values[j].isEmpty() )	{
						TiXmlElement *xmlElement = pNode->ToElement();

						//默认就是string, 减少空间浪费；还可和其它软件兼容
						xmlElement->SetAttribute(name, values[j].c_str());	

					}
				}
			}
			break;
		default:
			//assert( false );

			break ;
		}
	}

	return 0 ;
}

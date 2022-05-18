//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIProperty.h 2008/11/27 8:40 JWS $
//
// Project: OpenRS
//
// Purpose: <Key,Value>����ʵ��
//
// Author: GUO Wei, JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIProperty.h,v $
//
// Revision 0.9 date: 2008/11/27 by JIANG Wanshou
// ���Ӿ����ʸ��
//
// Revision 0.8 date: 2008/9/24 by GUO Wei
// new
//

#ifndef _ORS_PROPERTY_INTERFACE_H
#define _ORS_PROPERTY_INTERFACE_H


#include "orsIObject.h"
#include "orsBase/orsFastDelegate.h"
#include <vector>
using namespace std;


// #if defined kyLINUX64
// #include <vector>
// using namespace std;
// #define orsArray std::vector
// #endif

//��������
enum orsVariantType{
	ORS_V_UNKOWN = 0,
		ORS_V_BOOL = 1,
		ORS_V_I2 = 2,
		ORS_V_I4 = 3,
		ORS_V_R8 = 4,
		ORS_V_STR = 5,		//
		ORS_V_BLOB = 6,		//
		ORS_V_CHILD = 7,	// orsIProperty
		ORS_V_VECTOR = 8,
		ORS_V_MATRIX = 9,
		ORS_V_OBJECT = 10,	// orsIObject
		ORS_V_LISTNER = 11,	// prop Listener
		ORS_V_Attribute = 12 //Node��ʵ������
};

//
// �������Լ�����, �������Ա仯֪ͨ�������������Ե�ȡֵ��Χ��Ĭ��ȡֵ��
//
//	bool OnPropChanged( const char *propName, orsIProperty *propControlList )
//		propName --- �仯��������	
//		propControlList	--- ���ڱ༭�����Կؼ��б�
//	���أ�true == ����, false == �ܾ�
//

interface orsIPropControlList;
interface orsIPropControl;

using namespace fastdelegate;
typedef FastDelegate2<orsIPropControlList *, orsIPropControl *, bool> orsPropertyListener;

#define ORS_PROPERTY_LISTNER _T("PropertyListner")

//
//
// OpenRS��orsIProperty ʵ������һ�� �����б�attrs)�ӿ�
//
//	
interface orsIProperty : public orsIObject
{
public:
	// ȫ������
	virtual void copy( const orsIProperty *pProperty ) = 0;

	// ֻ�����Լ��е�
	virtual void update( const orsIProperty *pProperty ) = 0;

public:		//���ӿ�
	virtual bool getAttr(const orsChar * name,	bool & value) const = 0;

	virtual bool getAttr(const orsChar * name,	ors_int16 & value) const = 0;
	virtual bool getAttr(const orsChar * name,	ors_int32 &value) const = 0;
	virtual bool getAttr(const orsChar * name,	ors_float64 &value) const = 0;

	// ���ؿ���
	virtual bool getAttr(const orsChar * name,	ors_string &value) const = 0;
	virtual bool getAttr(const orsChar * name,	string &value) const = 0;
	virtual bool getAttr(const orsChar * name,	orsVectorBase &value ) const = 0;
	virtual bool getAttr(const orsChar * name,	orsMatrixBase &value ) const = 0;

	// �����ڲ�ָ��
	virtual bool getAttr(const orsChar * name,	const ors_byte* &pValue, ors_int32 &nLength) const = 0;
	virtual bool getAttr(const orsChar * name,	ref_ptr<orsIProperty> &value ) const = 0;

public:		//���ӿ�
	virtual bool getAttr(const orsChar * name,	orsArray<ors_int16> &values) const = 0;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_int32> &values) const = 0;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_float64> &values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_int16> &values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_int32> &values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_float64> &values) const = 0;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_string>	&values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<string>  &values) const = 0;

	// �����ڲ�ָ��
	virtual bool getAttr(const orsChar * name,	orsArray<const ors_byte *> &values,orsArray<ors_int32> &vLength) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<const ors_byte *> &values,std::vector<ors_int32> &vLength) const = 0;
	virtual bool getAttr(const orsChar * name,	orsArray<ref_ptr<orsIProperty> > &values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<ref_ptr<orsIProperty> > &values) const = 0;

public:		//��ӿ�
	virtual bool setAttr(const orsChar * name,	bool value ) = 0;
	virtual bool setAttr(const orsChar * name,	ors_int16 value, ors_uint32 index = 0) = 0;
	virtual bool setAttr(const orsChar * name,	ors_int32 value, ors_uint32 index = 0) = 0;
	virtual bool setAttr(const orsChar * name,	ors_float64 value, ors_uint32 index = 0) = 0;
	virtual bool setAttr(const orsChar * name,	const orsChar * value, ors_uint32 index = 0) = 0;
	virtual bool setAttribute(const orsChar * name1, const orsChar * value, ors_uint32 index = 0) = 0;
	virtual bool setAttr(const orsChar * name,	const ors_byte* pValue, ors_int32 nLength,ors_uint32 index = 0) = 0;
	virtual bool setAttr(const orsChar * name,	const orsVectorBase &value ) = 0;
	virtual bool setAttr(const orsChar * name,	const orsMatrixBase &value ) = 0;
	virtual bool setAttr(const orsChar * name,	ref_ptr<orsIProperty> value, ors_uint32 index = 0 ) = 0;

public:		//���ӿ�
	virtual void addAttr(const orsChar * name,	bool value, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	ors_int16 value, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	ors_int32 value, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	ors_float64 value, bool bUnique = false)= 0;

	// �ڲ�����
	virtual void addAttr(const orsChar * name,	const orsChar * value, bool bUnique = false) = 0;
	//�����ʵ�����ԣ��磺<task name="ImageGeoQualityCheck">
	virtual void addAttribute(const orsChar * name,	const orsChar * value, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	const ors_byte *pValue, ors_int32 nLength, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	const orsVectorBase &value, bool bUnique = false ) = 0;
	virtual void addAttr(const orsChar * name,	const orsMatrixBase &value, bool bUnique = false ) = 0;

	// �ڲ�����
	virtual void addAttr(const orsChar * name,	ref_ptr<orsIProperty> value, bool bUnique = false) = 0;

public:	
	// ��չ������ָ��
	virtual void addAttr(const orsChar * name, orsIObject *value, bool bUnique = false ) = 0;
	virtual bool setAttr(const orsChar * name, orsIObject *value ) = 0;
 	virtual bool getAttr(const orsChar * name, orsIObject *&value)  const= 0;

	// ���Լ�����
	virtual void addAttr(const orsChar * name, orsPropertyListener *value, bool bUnique = false ) = 0;
	virtual bool setAttr(const orsChar * name, orsPropertyListener *value ) = 0;
	virtual bool getAttr(const orsChar * name, orsPropertyListener *&value)  const= 0;

public:
	virtual bool remove( const orsChar * name ) = 0;
	virtual bool removeAll() = 0;

	virtual orsIProperty *createChild( const orsChar * name, bool bUnique = false) = 0;

	// �������Խڵ�, nodePath��ʽ�磬//ClssHrchy/AllClss
	virtual orsIProperty *findNode( const orsChar *nodePath, orsIProperty *&pParent ) = 0;
	////�ж��Ƿ���ڽڵ�name
	virtual bool findAttrName(const orsChar * name) = 0;

	// �������Ը���
	virtual ors_uint32 size() const = 0;

	//�������ƺ����ͣ��������л�
	//ind�����Ե������ţ���ΧΪ0 --- (size()-1);
	virtual void getAttributeInfo(ors_uint32 ind,ors_string &name,orsVariantType &type,
		ors_int32 &numOfValues, bool bSort = true ) const = 0;

	// ȡ��������
	virtual orsVariantType getAttrType( const orsChar *attrName ) const = 0;

	//�ӿ�
	ORS_INTERFACE_DEF(orsIObject, _T("property"))
};

#define ORS_PROPERTY		  _T("ors.property")
#define ORS_PROPERTY_DEFAULT  _T("ors.property.default")


//�������л���
interface orsIPropertySerialize : public orsIObject
{
public:
	// ��xml�ֽ����������ԣ�info��Ҫ���ȷ����ڴ�
	virtual bool import(const char *bytes, long length, orsIProperty *info) = 0;

	// �������Ե�xml�ֽ���
	// Ĭ���������char* bytes�ڶ��Ϸ������ڴ棬Ҫ���û���������ʱҪ����һ��
	virtual bool outport(char *&bytes, long *length, const orsIProperty *info) = 0;
	//������const char *pHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";���ͷ

	virtual bool outport_noUTF_8Head( char *&bytes,long *length, const orsIProperty *info) = 0;

	// ���ļ���ȡ��info��Ҫ���ȷ����ڴ�
	virtual bool importFromFile(const char *xmlFileName, orsIProperty *info) = 0;

	// ���浽�ļ�
	virtual bool outportToFile(const char *xmlFileName, const orsIProperty *info, bool bWriteValueTypeAttribute = true) = 0;

	// �ͷź���
	virtual bool free(char *&bytes) = 0;

	//�ӿ�
	ORS_INTERFACE_DEF(orsIObject, _T("propertySerialize") );
};

#define ORS_PROPERTYSERIALIZE		   _T("ors.propertySerialize")
#define ORS_PROPERTYSERIALIZE_DEFAULT  _T("ors.propertySerialize.xml")

#endif


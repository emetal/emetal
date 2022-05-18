//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsXProperty.h 2008/9/24 8:41 JWS $
//
// Project: OpenRS
//
// Purpose: <Key,Value>����ʵ��
//
// Author:  GUO Wei, JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsXProperty.h,v $
//
//
// Revision 0.9 date: 2008/11/27 by JIANG Wanshou
// ���Ӿ����ʸ��
//
// Revision 0.8 date: 2008/9/24 by GUO Wei
// new
//
#ifndef ORS_PROPERTYS_IMP_H
#define ORS_PROPERTYS_IMP_H

#include <map>
#include <string>
#include <vector>

#include "orsBase/orsIPlatform.h"
#include "orsCommon.h"

struct orsBLOB
{
	void *pData;
};


class orsVariant
{
public:
	orsVariantType type;

private:
	union {
		bool			vBool;
		ors_int16		vInt16;
		ors_int32		vInt32;
		ors_float64		vFloat64;
		orsBLOB			vBlob;
		orsIProperty	*vProperty;
		orsIObject		*vObject;
		orsPropertyListener	*vPropListener;
	};

// 	long b_value_length ;
// 	orsString				s_value ;
// 	ref_ptr<orsIProperty>	child_value;

public:
	orsVariant();
	orsVariant(orsVariant & var) ;
	~orsVariant();

	void set( bool v )	{ vBool = v;	}
	void set( ors_int16 v )	{ vInt16 = v;	}
	void set( ors_int32 v )	{ vInt32 = v;	}
	void set( ors_float64 v )	{ vFloat64 = v;	}
	void set( const orsChar * str );
	void set( ref_ptr<orsIProperty> pProperty );

	void set( orsIObject *pObject );
	void set( orsPropertyListener *pObject )	{ vPropListener = pObject;	};

	void set( const ors_byte *data, ors_int32 length );

	void set( const orsVectorBase &value );
	void set( const orsMatrixBase &value );

	const ors_byte *getBlob( ors_int32 &length );
	const orsChar * getString();

	void getVector( orsVectorBase &vector );
	void getMatrix( orsMatrixBase &matrix );

	bool getBool()	{	return vBool;	}
	ors_int16 getInt16()	{	return vInt16;	}
	ors_int32 getInt32()	{	return vInt32;	}
	ors_float64 getFloat64()	{	return vFloat64;	}
	orsIObject *getObject()	{ return vObject;	};
	orsIProperty *getProperty()	{ return vProperty;	};
	orsPropertyListener *getPropListener()	{ return vPropListener;	};
};


struct orsPropertyRecord	{
private:
	std::vector<orsVariant *> variants;
public:
	~orsPropertyRecord();

	unsigned int size()	{ return variants.size();	}
	orsVariant *GetVariant( int i )	{ return variants[i]; };
	void AddVariant( orsVariant *var )	{ variants.push_back( var ); };
};


//typedef orsVariant  orsPropertyRecord;

typedef std::map<stdString, orsPropertyRecord *> orsPropertyRecordMap;

class orsXProperty : public orsIProperty, public orsObjectBase
{
public:
	orsXProperty();
	virtual ~orsXProperty();

	virtual void copy( const orsIProperty *pProperty );
	
	// ֻ�����Լ��е�
	virtual void update( const orsIProperty *pProperty ) ;

	///////////////////////  ���ӿ� //////////////////////
	virtual bool getAttr(const orsChar * name,	bool &value) const;

	virtual bool getAttr(const orsChar * name,	ors_int16 & value) const;
	virtual bool getAttr(const orsChar * name,	ors_int32 &value) const;
	virtual bool getAttr(const orsChar * name,	ors_float64 &value) const;

	// ���ؿ���
	virtual bool getAttr(const orsChar * name,	ors_string &value) const;
	virtual bool getAttr(const orsChar * name,	string &value) const;
	virtual bool getAttr(const orsChar * name,	orsVectorBase &value ) const;
	virtual bool getAttr(const orsChar * name,	orsMatrixBase &value ) const;

	// �����ڲ�ָ��
	virtual bool getAttr(const orsChar * name,	const ors_byte* &pValue, ors_int32 &nLength) const;
	virtual bool getAttr(const orsChar * name,	ref_ptr<orsIProperty> &value ) const;

	/////////////////////// ���ӿ� ///////////////////////
	virtual bool getAttr(const orsChar * name,	orsArray<ors_int16> &values) const;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_int32> &values) const;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_float64> &values) const;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_int16> &values) const;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_int32> &values) const;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_float64> &values) const;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_string> &values) const;
    //virtual bool getAttr(const orsChar * name,	std::vector<string>  &values) const;
	// �����ڲ�ָ��
	virtual bool getAttr(const orsChar * name,	orsArray<const ors_byte *> &values,orsArray<ors_int32> &vLength) const;
	//virtual bool getAttr(const orsChar * name,	std::vector<const ors_byte *> &values,std::vector<ors_int32> &vLength) const;
	virtual bool getAttr(const orsChar * name,	orsArray<ref_ptr<orsIProperty> > &values) const;
	//virtual bool getAttr(const orsChar * name,	std::vector <ref_ptr<orsIProperty> > &values) const;

	/////////////////////// ��ӿ� ///////////////////////
	virtual bool setAttr(const orsChar * name,	bool value ) ;

	virtual bool setAttr(const orsChar * name,	ors_int16 value, ors_uint32 index  ) ;
	virtual bool setAttr(const orsChar * name,	ors_int32 value, ors_uint32 index  ) ;
	virtual bool setAttr(const orsChar * name,	ors_float64 value, ors_uint32 index  ) ;

	// �ڲ�����
	virtual bool setAttr(const orsChar * name,	const orsChar * value, ors_uint32 index  ) ;
	virtual bool setAttribute(const orsChar * name1, const orsChar * value, ors_uint32 index);
	virtual bool setAttr(const orsChar * name,	const orsVectorBase &value );
	virtual bool setAttr(const orsChar * name,	const orsMatrixBase &value );
	virtual bool setAttr(const orsChar * name,	const ors_byte* pValue, ors_int32 nLength,ors_uint32 index  ) ;

	// �ڲ�����
	virtual bool setAttr(const orsChar * name,	ref_ptr<orsIProperty> value, ors_uint32 index = 0 ) ;

	/////////////////////// ���ӿ� ///////////////////////
	virtual void addAttr(const orsChar * name,	bool value, bool bUnique = false) ;

	virtual void addAttr(const orsChar * name,	ors_int16 value, bool bUnique = false) ;
	virtual void addAttr(const orsChar * name,	ors_int32 value, bool bUnique = false) ;
	virtual void addAttr(const orsChar * name,	ors_float64 value, bool bUnique = false);

	// �ڲ�����
	virtual void addAttr(const orsChar * name,	const orsChar * value, bool bUnique = false) ;
	//�����ʵ�����ԣ��磺<task name="ImageGeoQualityCheck">
	virtual void addAttribute(const orsChar * name,	const orsChar * value, bool bUnique = false) ;
	virtual void addAttr(const orsChar * name,	const orsVectorBase &value, bool bUnique = false );
	virtual void addAttr(const orsChar * name,	const orsMatrixBase &value, bool bUnique = false );
	virtual void addAttr(const orsChar * name,	const ors_byte *pValue, ors_int32 nLength, bool bUnique = false) ;

	// �ڲ�����
	virtual void addAttr(const orsChar * name,	ref_ptr<orsIProperty> value, bool bUnique = false) ;

	// ��չ������ָ��
	virtual void addAttr(const orsChar * name, orsIObject *value, bool bUnique = false );
	virtual bool setAttr(const orsChar * name, orsIObject *value );
	virtual bool getAttr(const orsChar * name, orsIObject *&value)  const;
	
	// ���Լ�����
	virtual void addAttr(const orsChar * name, orsPropertyListener *value, bool bUnique = false );
	virtual bool setAttr(const orsChar * name, orsPropertyListener *value );
	virtual bool getAttr(const orsChar * name, orsPropertyListener *&value)  const;

	//////////////////////////////////////////////////////////////////////////
	// �������Խڵ�
	virtual orsIProperty *findNode( const orsChar *nodePath, orsIProperty *&pParent );

	virtual bool findAttrName(const orsChar * name);//�ж��Ƿ���ڽڵ�name


	virtual bool remove( const orsChar * name );
	virtual bool removeAll();

	virtual orsIProperty *createChild(const orsChar * name, bool bUnique = false);

	//��ѯ�ӿ�
	//���Ը���
	virtual ors_uint32 size() const 
	{
		// return m_records.size();
		return m_attrNames.size();
	}

	//�������ƺ�����
	//ind�����Ե������ţ���ΧΪ0 --- (size()-1);
	virtual void getAttributeInfo(ors_uint32 ind, ors_string &name,orsVariantType &type,
		ors_int32 &valuenum, bool bSort = true )  const;

	virtual orsVariantType getAttrType( const orsChar *attrName ) const;

public:
	// �������������
	std::vector <stdString>	m_attrNames;
	orsPropertyRecordMap m_records;

	//�ӿں궨��
	ORS_OBJECT_IMP1(orsXProperty, orsIProperty, _T("default"), _T("property") )
};


// ȡ�������͵���Чֵ

ors_float64 orsGetDefaultNULL(orsDataTYPE type) ;

// ȡ���͵ĵķ�Χ
ors_float64 orsGetDefaultMin(orsDataTYPE type) ;
ors_float64 orsGetDefaultMax(orsDataTYPE type) ;

//�����������õ����͵��ֽ���sizeof
int orsGetSizeOfType(orsDataTYPE type) ;

#endif

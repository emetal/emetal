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
// Purpose: <Key,Value>属性实现
//
// Author:  GUO Wei, JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsXProperty.h,v $
//
//
// Revision 0.9 date: 2008/11/27 by JIANG Wanshou
// 增加矩阵和矢量
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
	
	// 只更新自己有的
	virtual void update( const orsIProperty *pProperty ) ;

	///////////////////////  单接口 //////////////////////
	virtual bool getAttr(const orsChar * name,	bool &value) const;

	virtual bool getAttr(const orsChar * name,	ors_int16 & value) const;
	virtual bool getAttr(const orsChar * name,	ors_int32 &value) const;
	virtual bool getAttr(const orsChar * name,	ors_float64 &value) const;

	// 传回拷贝
	virtual bool getAttr(const orsChar * name,	ors_string &value) const;
	virtual bool getAttr(const orsChar * name,	string &value) const;
	virtual bool getAttr(const orsChar * name,	orsVectorBase &value ) const;
	virtual bool getAttr(const orsChar * name,	orsMatrixBase &value ) const;

	// 传回内部指针
	virtual bool getAttr(const orsChar * name,	const ors_byte* &pValue, ors_int32 &nLength) const;
	virtual bool getAttr(const orsChar * name,	ref_ptr<orsIProperty> &value ) const;

	/////////////////////// 复接口 ///////////////////////
	virtual bool getAttr(const orsChar * name,	orsArray<ors_int16> &values) const;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_int32> &values) const;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_float64> &values) const;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_int16> &values) const;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_int32> &values) const;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_float64> &values) const;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_string> &values) const;
    //virtual bool getAttr(const orsChar * name,	std::vector<string>  &values) const;
	// 传回内部指针
	virtual bool getAttr(const orsChar * name,	orsArray<const ors_byte *> &values,orsArray<ors_int32> &vLength) const;
	//virtual bool getAttr(const orsChar * name,	std::vector<const ors_byte *> &values,std::vector<ors_int32> &vLength) const;
	virtual bool getAttr(const orsChar * name,	orsArray<ref_ptr<orsIProperty> > &values) const;
	//virtual bool getAttr(const orsChar * name,	std::vector <ref_ptr<orsIProperty> > &values) const;

	/////////////////////// 变接口 ///////////////////////
	virtual bool setAttr(const orsChar * name,	bool value ) ;

	virtual bool setAttr(const orsChar * name,	ors_int16 value, ors_uint32 index  ) ;
	virtual bool setAttr(const orsChar * name,	ors_int32 value, ors_uint32 index  ) ;
	virtual bool setAttr(const orsChar * name,	ors_float64 value, ors_uint32 index  ) ;

	// 内部拷贝
	virtual bool setAttr(const orsChar * name,	const orsChar * value, ors_uint32 index  ) ;
	virtual bool setAttribute(const orsChar * name1, const orsChar * value, ors_uint32 index);
	virtual bool setAttr(const orsChar * name,	const orsVectorBase &value );
	virtual bool setAttr(const orsChar * name,	const orsMatrixBase &value );
	virtual bool setAttr(const orsChar * name,	const ors_byte* pValue, ors_int32 nLength,ors_uint32 index  ) ;

	// 内部引用
	virtual bool setAttr(const orsChar * name,	ref_ptr<orsIProperty> value, ors_uint32 index = 0 ) ;

	/////////////////////// 增接口 ///////////////////////
	virtual void addAttr(const orsChar * name,	bool value, bool bUnique = false) ;

	virtual void addAttr(const orsChar * name,	ors_int16 value, bool bUnique = false) ;
	virtual void addAttr(const orsChar * name,	ors_int32 value, bool bUnique = false) ;
	virtual void addAttr(const orsChar * name,	ors_float64 value, bool bUnique = false);

	// 内部拷贝
	virtual void addAttr(const orsChar * name,	const orsChar * value, bool bUnique = false) ;
	//添加真实的属性，如：<task name="ImageGeoQualityCheck">
	virtual void addAttribute(const orsChar * name,	const orsChar * value, bool bUnique = false) ;
	virtual void addAttr(const orsChar * name,	const orsVectorBase &value, bool bUnique = false );
	virtual void addAttr(const orsChar * name,	const orsMatrixBase &value, bool bUnique = false );
	virtual void addAttr(const orsChar * name,	const ors_byte *pValue, ors_int32 nLength, bool bUnique = false) ;

	// 内部引用
	virtual void addAttr(const orsChar * name,	ref_ptr<orsIProperty> value, bool bUnique = false) ;

	// 扩展，对象指针
	virtual void addAttr(const orsChar * name, orsIObject *value, bool bUnique = false );
	virtual bool setAttr(const orsChar * name, orsIObject *value );
	virtual bool getAttr(const orsChar * name, orsIObject *&value)  const;
	
	// 属性监听器
	virtual void addAttr(const orsChar * name, orsPropertyListener *value, bool bUnique = false );
	virtual bool setAttr(const orsChar * name, orsPropertyListener *value );
	virtual bool getAttr(const orsChar * name, orsPropertyListener *&value)  const;

	//////////////////////////////////////////////////////////////////////////
	// 查找属性节点
	virtual orsIProperty *findNode( const orsChar *nodePath, orsIProperty *&pParent );

	virtual bool findAttrName(const orsChar * name);//判断是否存在节点name


	virtual bool remove( const orsChar * name );
	virtual bool removeAll();

	virtual orsIProperty *createChild(const orsChar * name, bool bUnique = false);

	//轮询接口
	//属性个数
	virtual ors_uint32 size() const 
	{
		// return m_records.size();
		return m_attrNames.size();
	}

	//属性名称和类型
	//ind是属性的索引号，范围为0 --- (size()-1);
	virtual void getAttributeInfo(ors_uint32 ind, ors_string &name,orsVariantType &type,
		ors_int32 &valuenum, bool bSort = true )  const;

	virtual orsVariantType getAttrType( const orsChar *attrName ) const;

public:
	// 不排序的属性名
	std::vector <stdString>	m_attrNames;
	orsPropertyRecordMap m_records;

	//接口宏定义
	ORS_OBJECT_IMP1(orsXProperty, orsIProperty, _T("default"), _T("property") )
};


// 取给点类型的无效值

ors_float64 orsGetDefaultNULL(orsDataTYPE type) ;

// 取类型的的范围
ors_float64 orsGetDefaultMin(orsDataTYPE type) ;
ors_float64 orsGetDefaultMax(orsDataTYPE type) ;

//辅助方法，得到类型的字节数sizeof
int orsGetSizeOfType(orsDataTYPE type) ;

#endif

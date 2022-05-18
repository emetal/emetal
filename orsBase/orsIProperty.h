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
// Purpose: <Key,Value>属性实现
//
// Author: GUO Wei, JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIProperty.h,v $
//
// Revision 0.9 date: 2008/11/27 by JIANG Wanshou
// 增加矩阵和矢量
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

//属性类型
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
		ORS_V_Attribute = 12 //Node真实的属性
};

//
// 定义属性监听器, 用于属性变化通知，设置其它属性的取值范围和默认取值等
//
//	bool OnPropChanged( const char *propName, orsIProperty *propControlList )
//		propName --- 变化的属性名	
//		propControlList	--- 用于编辑的属性控件列表
//	返回：true == 接受, false == 拒绝
//

interface orsIPropControlList;
interface orsIPropControl;

using namespace fastdelegate;
typedef FastDelegate2<orsIPropControlList *, orsIPropControl *, bool> orsPropertyListener;

#define ORS_PROPERTY_LISTNER _T("PropertyListner")

//
//
// OpenRS的orsIProperty 实际上是一个 属性列表（attrs)接口
//
//	
interface orsIProperty : public orsIObject
{
public:
	// 全部复制
	virtual void copy( const orsIProperty *pProperty ) = 0;

	// 只更新自己有的
	virtual void update( const orsIProperty *pProperty ) = 0;

public:		//单接口
	virtual bool getAttr(const orsChar * name,	bool & value) const = 0;

	virtual bool getAttr(const orsChar * name,	ors_int16 & value) const = 0;
	virtual bool getAttr(const orsChar * name,	ors_int32 &value) const = 0;
	virtual bool getAttr(const orsChar * name,	ors_float64 &value) const = 0;

	// 传回拷贝
	virtual bool getAttr(const orsChar * name,	ors_string &value) const = 0;
	virtual bool getAttr(const orsChar * name,	string &value) const = 0;
	virtual bool getAttr(const orsChar * name,	orsVectorBase &value ) const = 0;
	virtual bool getAttr(const orsChar * name,	orsMatrixBase &value ) const = 0;

	// 传回内部指针
	virtual bool getAttr(const orsChar * name,	const ors_byte* &pValue, ors_int32 &nLength) const = 0;
	virtual bool getAttr(const orsChar * name,	ref_ptr<orsIProperty> &value ) const = 0;

public:		//复接口
	virtual bool getAttr(const orsChar * name,	orsArray<ors_int16> &values) const = 0;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_int32> &values) const = 0;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_float64> &values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_int16> &values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_int32> &values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<ors_float64> &values) const = 0;
	virtual bool getAttr(const orsChar * name,	orsArray<ors_string>	&values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<string>  &values) const = 0;

	// 传回内部指针
	virtual bool getAttr(const orsChar * name,	orsArray<const ors_byte *> &values,orsArray<ors_int32> &vLength) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<const ors_byte *> &values,std::vector<ors_int32> &vLength) const = 0;
	virtual bool getAttr(const orsChar * name,	orsArray<ref_ptr<orsIProperty> > &values) const = 0;
	//virtual bool getAttr(const orsChar * name,	std::vector<ref_ptr<orsIProperty> > &values) const = 0;

public:		//变接口
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

public:		//增接口
	virtual void addAttr(const orsChar * name,	bool value, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	ors_int16 value, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	ors_int32 value, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	ors_float64 value, bool bUnique = false)= 0;

	// 内部拷贝
	virtual void addAttr(const orsChar * name,	const orsChar * value, bool bUnique = false) = 0;
	//添加真实的属性，如：<task name="ImageGeoQualityCheck">
	virtual void addAttribute(const orsChar * name,	const orsChar * value, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	const ors_byte *pValue, ors_int32 nLength, bool bUnique = false) = 0;
	virtual void addAttr(const orsChar * name,	const orsVectorBase &value, bool bUnique = false ) = 0;
	virtual void addAttr(const orsChar * name,	const orsMatrixBase &value, bool bUnique = false ) = 0;

	// 内部引用
	virtual void addAttr(const orsChar * name,	ref_ptr<orsIProperty> value, bool bUnique = false) = 0;

public:	
	// 扩展，对象指针
	virtual void addAttr(const orsChar * name, orsIObject *value, bool bUnique = false ) = 0;
	virtual bool setAttr(const orsChar * name, orsIObject *value ) = 0;
 	virtual bool getAttr(const orsChar * name, orsIObject *&value)  const= 0;

	// 属性监听器
	virtual void addAttr(const orsChar * name, orsPropertyListener *value, bool bUnique = false ) = 0;
	virtual bool setAttr(const orsChar * name, orsPropertyListener *value ) = 0;
	virtual bool getAttr(const orsChar * name, orsPropertyListener *&value)  const= 0;

public:
	virtual bool remove( const orsChar * name ) = 0;
	virtual bool removeAll() = 0;

	virtual orsIProperty *createChild( const orsChar * name, bool bUnique = false) = 0;

	// 查找属性节点, nodePath形式如，//ClssHrchy/AllClss
	virtual orsIProperty *findNode( const orsChar *nodePath, orsIProperty *&pParent ) = 0;
	////判断是否存在节点name
	virtual bool findAttrName(const orsChar * name) = 0;

	// 返回属性个数
	virtual ors_uint32 size() const = 0;

	//属性名称和类型，用于序列化
	//ind是属性的索引号，范围为0 --- (size()-1);
	virtual void getAttributeInfo(ors_uint32 ind,ors_string &name,orsVariantType &type,
		ors_int32 &numOfValues, bool bSort = true ) const = 0;

	// 取属性类型
	virtual orsVariantType getAttrType( const orsChar *attrName ) const = 0;

	//接口
	ORS_INTERFACE_DEF(orsIObject, _T("property"))
};

#define ORS_PROPERTY		  _T("ors.property")
#define ORS_PROPERTY_DEFAULT  _T("ors.property.default")


//属性序列化器
interface orsIPropertySerialize : public orsIObject
{
public:
	// 从xml字节流导入属性，info需要事先分配内存
	virtual bool import(const char *bytes, long length, orsIProperty *info) = 0;

	// 导出属性到xml字节流
	// 默认输入参数char* bytes在堆上分配了内存，要由用户清理，运行时要保持一致
	virtual bool outport(char *&bytes, long *length, const orsIProperty *info) = 0;
	//不包含const char *pHead = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";这个头

	virtual bool outport_noUTF_8Head( char *&bytes,long *length, const orsIProperty *info) = 0;

	// 从文件读取，info需要事先分配内存
	virtual bool importFromFile(const char *xmlFileName, orsIProperty *info) = 0;

	// 保存到文件
	virtual bool outportToFile(const char *xmlFileName, const orsIProperty *info, bool bWriteValueTypeAttribute = true) = 0;

	// 释放函数
	virtual bool free(char *&bytes) = 0;

	//接口
	ORS_INTERFACE_DEF(orsIObject, _T("propertySerialize") );
};

#define ORS_PROPERTYSERIALIZE		   _T("ors.propertySerialize")
#define ORS_PROPERTYSERIALIZE_DEFAULT  _T("ors.propertySerialize.xml")

#endif


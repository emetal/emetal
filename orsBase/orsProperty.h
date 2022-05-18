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

#ifndef _ORS_orsProperty_INTERFACE_H
#define _ORS_orsProperty_INTERFACE_H


#include "orsIObject.h"




//////////////////////////////////////////////////////////////////////////




// 对象属性描述
struct orsorsPropertyDesc 
{
	char name[20];			// 名字
	orsorsPropertyType	type;	// 语义类型

	union	{
		struct FILE {
			char defautExt[8];		// 文件缺省后缀名
			char extFilter[256];	// 文件后缀过滤
		} file;

		long strLen;

		struct iVALUE {
			long min;
			long max;
		} iv;

		struct fVALUE {
			double min;
			double max;
		} fv;
	
		char objClass[80];			// 对象类别，接口名
	};

	char desc[80];			// 属性描述, 用于提示
};


//////////////////////////////////////////////////////////////////////////

//属性类型
enum orsVariantType{
	ORS_V_UNKOWN = 0,	// 未知
		ORS_V_U1 = 11,
		ORS_V_I2 = 12,	// 16 bit整数
		ORS_V_I4 = 14,	// 32 bit整数
		ORS_V_R4 = 24,	// 32 bit 浮点
		ORS_V_R8 = 28,	// 64 bit 浮点
		ORS_V_STR = 30,	// 字符串
		ORS_V_STRUCT = 40,	// 结构，在xml中一种特殊的group
		ORS_V_GROUP = 50	// 属性组，一般对应子对象的属性
};



// 属性类型，用于属性设置，流程中orsIExecute的输入/输出约束
enum orsValyeTYPE	{
	ORS_PT_UNKNOWN = 0,
		ORS_PT_NUMBER = 1,	// 数值类型
		ORS_PT_OBJECT = 2,	// 对象, 需要对象类别，
		ORS_PT_STRING = 3,	// 普通字符串
		ORS_PT_FILE = 4,	// 普通文件, 可以Browse
		ORS_PT_PATH = 5		// 目录, 可以Browse
		// 		ORS_PT_IMAGE = 5,		// 影像文件，可以Browse
		// 		ORS_PT_DEM = 6,			// DEM文件，可以Browse
		// 		ORS_PT_sfVECTOR = 7,	// 简单要素类型矢量文件，可以Browse
		// 								// 无法穷尽
};

interface orsIProperty
{
	// 语义类别：数值、字符串、一般文件、影像文件、矢量文件、...
	virtual valueTYPE getValueType() = 0;

	virtual const orsChar *getSemanticType() = 0;

	// 属性描述，可用于输入提示
	virtual const orsChar *getDesc() = 0;

	virtual bool serialize( orsISerializer *serializer ) = 0;

}




template <typename _Type>
class orsNumberorsProperty : orsIProperty {
private:
	_Type m_min;
	_Type m_max;

	_Type m_value;
	
public:
	orsNumberorsProperty()
	{
		m_min = (_Type)0xffffffffffffffff;
		m_max = (_Type)0xefffffffffffffff;

		if( m_min > m_max ) {
			m_min = 0;
			m_max = (_Type)0xffffffffffffffff;
		}
	}

	void setRange(_Type min, _Type max)	
	{ 
		m_min = min;
		m_max = max;
	};

	void getRange( _Type *min, _Type *max)	
	{ 
		*min = m_min;
		*max = m_max;
	};	
	
	virtual bool set( _Type value )	
	{
		if( value < m_min || value > m_max )
			return false;
		
		m_value = value;

		return true;
	}

	virtual _Type get()	
	{
		return m_value;
	}

	virtual serialize( orsISerializer *serializer );

};



template <typename _Type>
class orsVectorProperty : orsIProperty {
private:
	
	orsVector<_Type> m_values;
	
public:
	virtual bool set( const orsVector<_Type> &values )	
	{
		m_values = values;		
		return true;
	}
	
	virtual const orsVector<_Type> &get()	
	{
		return m_values;
	}
	
	virtual serialize( orsISerializer *serializer );
	
};


template <typename _Type>
class orsMatrixProperty : orsIProperty {
private:
	
	orsMatrix<_Type> m_values;
	
public:
	virtual bool set( const orsMatrix<_Type> &values )	
	{
		m_values = values;		
		return true;
	}
	
	virtual const orsMatrix<_Type> &get()	
	{
		return m_values;
	}
	
	virtual serialize( orsISerializer *serializer );
};


class orsStringProperty : orsIProperty {
private:
	ors_int32 m_maxLen;
	orsString m_str;
	
public:
	orsStringProperty()
	{
		m_maxLen = 256;
	}
	
	void setMaxLen( ors_int32 maxLen )	{	m_maxLen = maxLen;	}
	ors_int32 getMaxLen()	{	return m_maxLen;	};

	virtual bool set( const orsChar *str )	
	{
		m_str = str;

		if( str )
			return true;
		else
			return false;
	}

	virtual const orsString &get() const
	{
		return m_str;
	}

	virtual serialize( orsISerializer *serializer );
};
                                  


class orsFileNameProperty : orsIProperty 
{
private:
	ors_int32 m_maxLen;
	orsString m_str;
	orsString m_defaultExt;
	orsString m_extfilter;
	
public:
	orsFileNameProperty()
	{
		m_maxLen = 256;
	}

	ors_int32 getMaxLen()	{	return m_maxLen;	};
	void setMaxLen( ors_int32 maxLen )	{	m_maxLen = maxLen;	}

	void setExtFilter( const orsChar *defaultExt, const orsChar *extFilter );
	
	virtual bool set( const orsChar *str )	
	{
		m_str = str;
		
		if( str )
			return true;
		else
			return false;
	}

	virtual const orsString &get() const
	{
		return m_str;
	}

	virtual serialize( orsISerializer *serializer );
}


// horizontal coordinate System
class orsHcsProperty : orsIProperty 
{
private:
	ors_int32 m_maxLen;
	orsString m_str;
	
public:
	orsFileNameProperty()
	{
		m_maxLen = 256;
	}
	
	ors_int32 getMaxLen()	{	return m_maxLen;	};
	void setMaxLen( ors_int32 maxLen )	{	m_maxLen = maxLen;	}
		
	virtual bool set( const orsChar *str )	
	{
		m_str = str;
		
		if( str )
			return true;
		else
			return false;
	}
	
	virtual const orsString &get() const
	{
		return m_str;
	}
	
	virtual serialize( orsISerializer *serializer );
}




template <typename _PropClass, typename ValueType>
class orsProperty_R: public _PropClass
{
public:
	//-- 使属性类能转换为内部类型成为可能--
	operator ValueType()
	{
		return get();
	}
};


template <typename _PropClass, typename ValueType>
class orsProperty_RW: public _PropClass
{
public:
	//-- 重载''=''号操作符使其能用set成员设定属性值--
	ValueType operator = (const ValueType& value )
	{
		return set( value );
	}
	
	//-- 使属性类能转换为内部类型成为可能--
	operator ValueType()
	{
		return get();
	}
};


//////////////////////////////////////////////////////////////////////////

#ifdef USE_orsProperty
	#define DECLARE_orsProperty(ClassName)\
		typedef ClassName HostClass;\

		class orsPropertyBase {\
		public: inline static void SetPPOwner(HostClass* thispoint){PPOwner=thispoint;}\
		protected:\
			static HostClass* PPOwner;\
		};

	#define BasicorsProperty(TYPE,NAME,REFObj) \
		class TYPE##orsProperty_##NAME :orsPropertyBase{\
		public:\
			inline TYPE operator=(TYPE value){PPOwner->REFObj=value;return value;}\
			inline operator TYPE(){return PPOwner->REFObj;}\
		};\
		TYPE##orsProperty_##NAME NAME;
		//End of BasicorsProperty define.

	#define ComplexProperty(TYPE,NAME) \
		class TYPE##orsProperty_##NAME :orsPropertyBase{\
		public:\
			TYPE operator=(TYPE ac);\
			operator TYPE();\
		};\
		TYPE##orsProperty_##NAME NAME;
		//End of ComplexProperty define.
		//ComplexProperty should use Marco Setor and Getor to define its own get/set behavior

	#define IMPLEMENT_orsProperty(ClassName) \
		typedef ClassName HostClass;
		HostClass* HostClass::orsPropertyBase::PPOwner=NULL;

	#define orsPropertyBASE_INIT orsPropertyBase::SetPPOwner(this); 
	#define Setor(TYPE,NAME) TYPE HostClass::TYPE##orsProperty_##NAME::operator=(TYPE value)
	#define Getor(TYPE,NAME) HostClass::TYPE##orsProperty_##NAME::operator TYPE()

	#define orsProperty(TYPE,NAME,REFObj) BasicorsProperty(TYPE,NAME,REFObj)
	#define orsCProperty(TYPE,NAME) ComplexProperty(TYPE,NAME)

#else
	#define DECLARE_orsProperty(ClassName)
	#define IMPLEMENT_orsProperty(ClassName)
	#define BasicorsProperty(TYPE,NAME,REFObj)
	#define ComplexProperty(TYPE,NAME)
	#define orsPropertyBASE_INIT
#endif

//////////////////////////////////////////////////////////////////////////

#define READ_ONLY 1
#define WRITE_ONLY 2
#define READ_WRITE 3

template <typename Container, typename ValueType, int nPropType>
class orsProperty
{
private:
	Container *m_cObject;  //-- 指向包含属性的类模块 --
	void (Container::*Set)(ValueType value);

	//-- 指向set成员函数的函数指针 --
	ValueType (Container::*Get)();
	//-- 指向get成员函数的函数指针 --

public:
	orsProperty()
	{
		m_cObject = NULL;
		Set = NULL;
		Get = NULL;
	}
	
	//-- 将m_cObject指向包含属性的container类 --
	void setContainer(Container* cObject)
	{
		m_cObject = cObject;
	}

	//-- 设定可改变属性值的set成员函数 --
	void setter( void (Container::*pSet)(ValueType value) )
	{
		if((nPropType == WRITE_ONLY) || (nPropType == READ_WRITE))
			Set = pSet;
		else
			Set = NULL;
	}

	//-- 设定可检索属性值的get成员函数 --
	void getter( ValueType (Container::*pGet)() )
	{
		if((nPropType == READ_ONLY) || (nPropType == READ_WRITE))
			Get = pGet;
		else
			Get = NULL;
	}

	//-- 重载''=''号操作符使其能用set成员设定属性值--
	ValueType operator = (const ValueType& value )
	{
		assert(m_cObject != NULL);
		assert(Set != NULL);

		(m_cObject->*Set)(value);

		return value;
	}

	//-- 使属性类能转换为内部类型成为可能--
	operator ValueType()
	{
		assert(m_cObject != NULL);
		assert(Get != NULL);
		return (m_cObject->*Get)();
	}

};


#endif
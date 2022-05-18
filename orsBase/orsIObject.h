/******************************************************************************
* $Id: orsIObject.h 2008/10/05 9:58 JWS $
*
* Project:  OpenRS
* Purpose:  用于定义可引用计数的对象
*
* Author:   JIANG Wanshou, jws@lmars.whu.edu.cn
******************************************************************************
* Copyright (c) 2008, Liesmars, Wuhan University
* History:
*      
****************************************************************************/

#ifndef _ORS_OBJECT_INTERFACE_H
#define _ORS_OBJECT_INTERFACE_H

#include "orsTypedef.h"
#include "orsMatrix.h"

#include "orsArray.h"

#include "orsString.h"


#include "orsRefPtr.h"
#include "orsMacro.h"

interface orsIProperty;

struct orsIObject
{
	// 用于析构派生类
protected:
	virtual ~orsIObject(){};

public:	
	//////////////////////////////////////////////////////////////////////////
	//下面两个方法，可以通过继承内联的orsObjectBase来实现，不需要自己实现
	//加引用
	virtual void addRef() = 0;
	virtual void release() = 0;

	//////////////////////////////////////////////////////////////////////////
	// 下面4个函数可以通过系统提供的宏ORS_OBJECT_IMP1,ORS_OBJECT_IMP2...来实现
	
	// 类名, 短描述
	virtual ors_string	getClassName()	const	{ return _T("Unknown");	};

	// 层次化对象ID, 区分对象的唯一标记, 实际ID由接口ID和对象ID连接而成
	// 命名规则与eclipse相似，由对象ID的层次名字可以构建插件树
	virtual ors_string	getClassID()	const { return _T("ors"); }

	// 得到detailed description
	virtual ors_string	getClassDesc() const { return _T("empty");}
	
	// 得到实现的接口类名
	virtual orsArray<ors_string> getInterfaceNames() const = 0;

	// 根据接口名取得类指针
	virtual void *queryInterface( const orsChar *className)  const = 0;
	
	///////////////////// 属性操作 ////////////////////////////////////////////
	// 得到属性, 可以用于获取内部数据, 序列化等
	virtual const orsIProperty *getProperty() const	{	return NULL; };

	// 用于从xml恢复对象状态，xml->property->object
	virtual bool initFromProperty( orsIProperty *property ) {	return true;	};

	orsIProperty *getProperty()	
	{
		return const_cast<orsIProperty *> ( (static_cast<const orsIObject&>(*this)).getProperty() );
	}; 


// 	//设置属性
// 	virtual void propertyChangeNotify() {} ;
// 	
// 	virtual bool checkAttr(const orsChar * name,	ors_int16 value, ors_int32 index  ) { return true;	};
// 	virtual bool checkAttr(const orsChar * name,	ors_int32 value, ors_int32 index  ) { return true;	};
// 	virtual bool checkAttr(const orsChar * name,	ors_float64 value, ors_int32 index  )  { return true;	};
// 	virtual bool checkAttr(const orsChar * name,	const orsChar * value, ors_int32 index  )  { return true;	} ;
// 	
// 	// 得到属性约束
// 	virtual ors_string getAttrContrainedDescription( const orsChar * name ) const { return _T("empty");}

};



//用于辅助引用计数实现
//是否应该考虑多线程支持???
class orsObjectBase
{
public:
	orsObjectBase(){m_refCount = 0;}
	virtual ~orsObjectBase(){};
	virtual void internalAddRef() {
		m_refCount++;
	}
	
	virtual void internalRelease() {
		m_refCount--;
		if(m_refCount <= 0) 
			delete this;
	}
protected:
	mutable ors_int32 m_refCount;
};


//
// 对象的描述信息
//
interface orsIObjectDesc
{
public:	// for ref_ptr
	virtual void		addRef() = 0;
	virtual void		release() = 0;

public:
	virtual const orsChar *getName() = 0;
	virtual const orsChar *getID() = 0;
	virtual const orsChar *getDesc() = 0;
	virtual const orsChar *getPluginName() = 0;
	virtual const orsArray<orsString> &getInterfaceNames() const = 0;
};


#include "orsIProperty.h"

#endif

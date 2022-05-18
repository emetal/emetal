#ifndef ORS_PROPERTYS_IMP_H
#define ORS_PROPERTYS_IMP_H

#include <map>
#include <string>

#include "orsBase/orsIPlatform.h"


class ORS_BASE_API orsVariant
{
public:
	orsVariantType type ;

	union {
		ors_int16		i_value ;
		ors_int32		l_value ;
		ors_float64		d_value ;
		ors_byte*		b_value_value;
	};

	orsString s_value ;
	long b_value_length ;

	ref_ptr<orsIProperty> child_value;

public:
	orsVariant();
	orsVariant(orsVariant & var) ;
	~orsVariant();	
};



class ORS_BASE_API orsXProperty : public orsIProperty, public orsObjectBase
{
public:
	orsXProperty();
	virtual ~orsXProperty();
	
	//单接口
	virtual bool getAttribute(ors_string name,	ors_int32 &value);
	virtual void getAttribute(ors_string name,	ors_int16 & value);
	virtual bool getAttribute(ors_string name,	ors_string &value);
	virtual bool getAttribute(ors_string name,	ors_float64 &value);
	virtual bool getAttribute(ors_string name,	ors_byte* &pValue, ors_int32 &nLength);
	virtual bool getAttribute(ors_string name,	ref_ptr<orsIProperty> &value );
	
	//复接口
	virtual bool getAttribute(ors_string name,	orsArray<ors_string>	&value);
	virtual bool getAttribute(ors_string name,	orsArray<ors_byte *> &value,orsArray<ors_int32> &nLength);
	virtual bool getAttribute(ors_string name,	orsArray<ors_float64> &value);
	virtual bool getAttribute(ors_string name,	orsArray<ors_int16> &value);
	virtual bool getAttribute(ors_string name,	orsArray<ors_int32> &value);
	virtual bool getAttribute(ors_string name,	orsArray<ref_ptr<orsIProperty> > &value);

	//变接口
	virtual bool setAttribute(ors_string name,	ors_int32 value, ors_int32 index = 0);
	virtual bool setAttribute(ors_string name,	ors_int16 value, ors_int32 index = 0);
	virtual bool setAttribute(ors_string name,	ors_string &value, ors_int32 index = 0);
	virtual bool setAttribute(ors_string name,	ors_float64 value, ors_int32 index = 0);
	virtual bool setAttribute(ors_string name,	ors_byte* pValue, ors_int32 nLength,ors_int32 index = 0);

	virtual bool setAttribute(ors_string name,	ref_ptr<orsIProperty> value, ors_int32 index = 0);
		
	//增接口
	virtual void addAttribute(ors_string name,	ors_int32 value);
	virtual void addAttribute(ors_string name,	ors_int16 value);
	virtual void addAttribute(ors_string name,	ors_char* value);
	virtual void addAttribute(ors_string name,	ors_string	value);
	virtual void addAttribute(ors_string name,	ors_byte *pValue,ors_int32 nLength);
	virtual void addAttribute(ors_string name,	ors_float64 value);
	virtual void addAttribute(ors_string name,	ref_ptr<orsIProperty> value);
	
	//属性变换通知回调
	virtual void setListener(orsIPropertyListener* listener)
	{
		m_listener = listener;
	}

	//轮询接口
	//属性个数
	virtual ors_uint32 size();
	
	//属性名称和类型
	//ind是属性的索引号，范围为0 --- (size()-1);
	virtual void getAttributeInfo(ors_int32 ind,ors_string &name,orsVariantType &type,
		ors_int32 &valuenum);
	
public:	
	std::map<std::string,orsArray<orsVariant*>* > m_values;
	orsIPropertyListener	*m_listener;

	//接口宏定义
	ORS_OBJECT_IMP1(orsXProperty,orsIProperty, "default")
};

#endif
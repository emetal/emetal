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
	
	//���ӿ�
	virtual bool getAttribute(ors_string name,	ors_int32 &value);
	virtual void getAttribute(ors_string name,	ors_int16 & value);
	virtual bool getAttribute(ors_string name,	ors_string &value);
	virtual bool getAttribute(ors_string name,	ors_float64 &value);
	virtual bool getAttribute(ors_string name,	ors_byte* &pValue, ors_int32 &nLength);
	virtual bool getAttribute(ors_string name,	ref_ptr<orsIProperty> &value );
	
	//���ӿ�
	virtual bool getAttribute(ors_string name,	orsArray<ors_string>	&value);
	virtual bool getAttribute(ors_string name,	orsArray<ors_byte *> &value,orsArray<ors_int32> &nLength);
	virtual bool getAttribute(ors_string name,	orsArray<ors_float64> &value);
	virtual bool getAttribute(ors_string name,	orsArray<ors_int16> &value);
	virtual bool getAttribute(ors_string name,	orsArray<ors_int32> &value);
	virtual bool getAttribute(ors_string name,	orsArray<ref_ptr<orsIProperty> > &value);

	//��ӿ�
	virtual bool setAttribute(ors_string name,	ors_int32 value, ors_int32 index = 0);
	virtual bool setAttribute(ors_string name,	ors_int16 value, ors_int32 index = 0);
	virtual bool setAttribute(ors_string name,	ors_string &value, ors_int32 index = 0);
	virtual bool setAttribute(ors_string name,	ors_float64 value, ors_int32 index = 0);
	virtual bool setAttribute(ors_string name,	ors_byte* pValue, ors_int32 nLength,ors_int32 index = 0);

	virtual bool setAttribute(ors_string name,	ref_ptr<orsIProperty> value, ors_int32 index = 0);
		
	//���ӿ�
	virtual void addAttribute(ors_string name,	ors_int32 value);
	virtual void addAttribute(ors_string name,	ors_int16 value);
	virtual void addAttribute(ors_string name,	ors_char* value);
	virtual void addAttribute(ors_string name,	ors_string	value);
	virtual void addAttribute(ors_string name,	ors_byte *pValue,ors_int32 nLength);
	virtual void addAttribute(ors_string name,	ors_float64 value);
	virtual void addAttribute(ors_string name,	ref_ptr<orsIProperty> value);
	
	//���Ա任֪ͨ�ص�
	virtual void setListener(orsIPropertyListener* listener)
	{
		m_listener = listener;
	}

	//��ѯ�ӿ�
	//���Ը���
	virtual ors_uint32 size();
	
	//�������ƺ�����
	//ind�����Ե������ţ���ΧΪ0 --- (size()-1);
	virtual void getAttributeInfo(ors_int32 ind,ors_string &name,orsVariantType &type,
		ors_int32 &valuenum);
	
public:	
	std::map<std::string,orsArray<orsVariant*>* > m_values;
	orsIPropertyListener	*m_listener;

	//�ӿں궨��
	ORS_OBJECT_IMP1(orsXProperty,orsIProperty, "default")
};

#endif
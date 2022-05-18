#ifndef ORS_PROPERTYS_IMP_H
#define ORS_PROPERTYS_IMP_H

#include <map>
#include <string>

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
		ors_int16		vInt16;
		ors_int32		vInt32;
		ors_float64		vFloat64;
		orsBLOB			vBlob;
		orsIProperty	*vProperty;
	};

// 	long b_value_length ;
// 	orsString				s_value ;
// 	ref_ptr<orsIProperty>	child_value;

public:
	orsVariant();
	orsVariant(orsVariant & var) ;
	~orsVariant();

	void set( ors_int16 v )	{ vInt16 = v;	}
	void set( ors_int32 v )	{ vInt32 = v;	}
	void set( ors_float64 v )	{ vFloat64 = v;	}
	void set( const ors_byte *data, ors_int32 length );
	void set( const orsChar * str );
	void set( ref_ptr<orsIProperty> pProperty );

	const ors_byte *getBlob( ors_int32 &length );
	const orsChar * getString();

	orsIProperty *getProperty()
	{
		return vProperty;
	};

	ors_int16 getInt16()	{	return vInt16;	}
	ors_int32 getInt32()	{	return vInt32;	}
	ors_float64 getFloat64()	{	return vFloat64;	}
};


struct orsPropertyRecord	{
private:
	orsArray<orsVariant *> variants;
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

	///////////////////////  ���ӿ� //////////////////////
	virtual bool getAttribute(const orsChar * name,	ors_int16 & value) const;
	virtual bool getAttribute(const orsChar * name,	ors_int32 &value) const;
	virtual bool getAttribute(const orsChar * name,	ors_float64 &value) const;

	// ���ؿ���
	virtual bool getAttribute(const orsChar * name,	ors_string &value) const;

	// �����ڲ�ָ��
	virtual bool getAttribute(const orsChar * name,	const ors_byte* &pValue, ors_int32 &nLength) const;
	virtual bool getAttribute(const orsChar * name,	ref_ptr<orsIProperty> &value ) const;


	/////////////////////// ���ӿ� ///////////////////////
	virtual bool getAttribute(const orsChar * name,	orsArray<ors_int16> &values) const;
	virtual bool getAttribute(const orsChar * name,	orsArray<ors_int32> &values) const;
	virtual bool getAttribute(const orsChar * name,	orsArray<ors_float64> &values) const;
	virtual bool getAttribute(const orsChar * name,	orsArray<ors_string> &values) const;

	// �����ڲ�ָ��
	virtual bool getAttribute(const orsChar * name,	orsArray<const ors_byte *> &values,orsArray<ors_int32> &vLength) const;
	virtual bool getAttribute(const orsChar * name,	orsArray<ref_ptr<orsIProperty> > &values) const;

	/////////////////////// ��ӿ� ///////////////////////
	virtual bool setAttribute(const orsChar * name,	ors_int16 value, ors_int32 index  ) ;
	virtual bool setAttribute(const orsChar * name,	ors_int32 value, ors_int32 index  ) ;
	virtual bool setAttribute(const orsChar * name,	ors_float64 value, ors_int32 index  ) ;
	virtual bool setAttribute(const orsChar * name,	const orsChar * value, ors_int32 index  ) ;

	// �ڲ�����
	virtual bool setAttribute(const orsChar * name,	const ors_byte* pValue, ors_int32 nLength,ors_int32 index  ) ;

	// �ڲ�����
	virtual bool setAttribute(const orsChar * name,	ref_ptr<orsIProperty> value, ors_int32 index  ) ;

	/////////////////////// ���ӿ� ///////////////////////
	virtual void addAttribute(const orsChar * name,	ors_int16 value) ;
	virtual void addAttribute(const orsChar * name,	ors_int32 value) ;
	virtual void addAttribute(const orsChar * name,	ors_float64 value);
	virtual void addAttribute(const orsChar * name,	const orsChar * value) ;

	// �ڲ�����
	virtual void addAttribute(const orsChar * name,	const ors_byte *pValue, ors_int32 nLength) ;

	// �ڲ�����
	virtual void addAttribute(const orsChar * name,	ref_ptr<orsIProperty> value) ;

	//////////////////////////////////////////////////////////////////////////

	//���Ա任֪ͨ�ص�
	virtual void setListener(orsIPropertyListener* listener)
	{
		m_listener = listener;
	}

	//��ѯ�ӿ�
	//���Ը���
	virtual ors_uint32 size() const {return m_records.size();}

	//�������ƺ�����
	//ind�����Ե������ţ���ΧΪ0 --- (size()-1);
	virtual void getAttributeInfo(ors_int32 ind, ors_string &name,orsVariantType &type,
		ors_int32 &valuenum)  const;

protected:
	void setParent(orsIProperty *parent){m_parent = parent;}
	orsIProperty * getParent(){return m_parent;}

public:
	orsPropertyRecordMap m_records;

	orsIPropertyListener *m_listener;

	orsIProperty *m_parent;

	//�ӿں궨��
	ORS_OBJECT_IMP1(orsXProperty,orsIProperty, "default")
};

#endif

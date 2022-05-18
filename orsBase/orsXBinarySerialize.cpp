/********************************************************************
	created:	2008/09/20
	created:	20:9:2008   18:35
	filename: 	E:\OpenRS\DESKTOP\src\orsBase\orsBinarySerializeImp.cpp
	file path:	E:\OpenRS\DESKTOP\src\orsBase
	file base:	orsBinarySerializeImp
	file ext:	cpp
	author:		Guo Wei
	
	purpose:	二进制属性序列化器

*********************************************************************/
#include "stdafx.h"

#include "orsBase/orsConstDef.h"
#include "orsBase/orsUtil.h"

#include "orsXBinarySerialize.h"
#include "orsXProperty.h"

/************************************************************************\
	此宏用来确定是否数据错误，以致于读写流时内存访问越界, 如果数据错误
	确实发生，则立即返回-1

\************************************************************************/
#define INSURE_BOUND(cursor, step, limit) if((cursor+step)>limit)return (-1);

int operator >> ( orsMessageStream &in_nms, orsIProperty *record)
{
	long pair_num ;

	in_nms >> pair_num ;

	INSURE_BOUND (in_nms.rd_ptr (), sizeof(long), in_nms.wr_ptr ())
	for (int i = 0 ; i < pair_num ; i++) {
		std::string		key;
		char	type;
		long 	value_num;
		in_nms >> key;
		INSURE_BOUND (in_nms.rd_ptr (), sizeof(char), in_nms.wr_ptr ())
		in_nms >> type;
		INSURE_BOUND (in_nms.rd_ptr (), sizeof(long), in_nms.wr_ptr ())
		in_nms >> value_num;
		
		int j = 0 ;
		switch(type) {
		case ORS_V_I2:
			for( j = 0 ; j < value_num ; j++) {
				ors_int16  value;
				INSURE_BOUND (in_nms.rd_ptr (), sizeof(short), in_nms.wr_ptr ())
				in_nms >> value ;
				record->addAttr(key.c_str(),value);
			}
			break;

		case ORS_V_I4:
			for( j=0;j<value_num;j++){
				ors_int32 value;
				INSURE_BOUND (in_nms.rd_ptr (), sizeof(long), in_nms.wr_ptr ())
				in_nms >> value ;
				record->addAttr(key.c_str(),value);
			}
			break;
		case ORS_V_R8:
			for( j=0;j<value_num;j++){
				ors_float64 value;
				INSURE_BOUND (in_nms.rd_ptr (), sizeof(double), in_nms.wr_ptr ())
				in_nms >> value ;
				record->addAttr(key.c_str(),value);
			}
			break;
		case ORS_V_STR:
			for( j=0;j<value_num;j++){
				std::string value;
				in_nms >> value ;
				record->addAttr(key.c_str(),value.c_str());
			}
			break;
		case ORS_V_BLOB:
			for ( j =0 ;j < value_num ; j++){
				INSURE_BOUND (in_nms.rd_ptr (), sizeof(long), in_nms.wr_ptr ())
				ors_int32 b_value_length;
				in_nms >> b_value_length  ;
				ors_byte* b_value_value = new ors_byte[b_value_length];
				INSURE_BOUND (in_nms.rd_ptr (), b_value_length, in_nms.wr_ptr ())
				in_nms.readBlob((char*)b_value_value, b_value_length);
				record->addAttr(key.c_str(),b_value_value,b_value_length);
				delete b_value_value;
			}
			break;
		case ORS_V_CHILD:
			for ( j = 0 ; j < value_num ; j++) {
				ref_ptr<orsIProperty> child = new orsXProperty;

				if (-1 == in_nms >> child.get() ) 
					return -1 ;

				record->addAttr( key.c_str(), child );
			}
			break;
		default:
			assert(0);
			break ;
		}
	}
	return 0 ;
}

int operator << ( orsMessageStream &out_nms, const orsIProperty *record)
{
	long pair_num = record->size();
	out_nms << pair_num;
	
	for(int i=0;i<record->size();i++)
	{
		orsString	key;
		orsVariantType type;
		ors_int32	num;
		record->getAttributeInfo(i,key,type,num);

		out_nms << key;
		out_nms << (char)type;

		switch(type){
		case ORS_V_I2:
			{
				orsArray<ors_float64> values;
				record->getAttr(key.c_str(),values);
				ors_int32 value_num = values.size();
				out_nms << value_num;
				for(int j=0;j<value_num;j++)
					out_nms << (ors_int16)values[j];
			}
			break;
		case ORS_V_I4:
			{
				orsArray<ors_float64> values;
				record->getAttr(key.c_str(),values);
				ors_int32 value_num = values.size();
				out_nms << value_num;
				for(int j=0;j<value_num;j++)
					out_nms << (ors_int32)values[j];
			}
			break;
		case ORS_V_R8:
			{
				orsArray<ors_float64> values;
				record->getAttr(key.c_str(),values);
				ors_int32 value_num = values.size();
				out_nms << value_num;
				for(int j=0;j<value_num;j++)
					out_nms << values[j];
			}
			break;
		case ORS_V_STR:
			{
				orsArray<orsString> values;
				record->getAttr(key.c_str(),values);
				ors_int32 value_num = values.size();
				out_nms << value_num;
				for(int j=0;j<value_num;j++)
					out_nms << values[j];
			}
			break;
		case ORS_V_BLOB:
			{
				orsArray<const ors_byte*> values;
				orsArray<ors_int32> lens;
				record->getAttr(key.c_str(),values,lens);
				ors_int32 value_num = values.size();
				out_nms << value_num;
				for(int j=0;j<value_num;j++){
					out_nms << lens[i]  ;
					out_nms.writeBlob((char*)values[i], lens[i]);
				}
			}
			break;
		case ORS_V_CHILD:
			{
				orsArray<ref_ptr<orsIProperty> > values;

				record->getAttr(key.c_str(),values);

				ors_int32 value_num = values.size();
				out_nms << value_num;

				for(int j=0;j<value_num;j++)
					out_nms << values[j].get();
			}
			break;
		default:
			assert(0);
			break ;
		}
	}
	return 0 ;
}

bool orsXBinarySerialize::import( const char *bytes,long length, orsIProperty *info)
{
	bool retval = false ; 
	orsMessageStream in_nms(bytes,length) ;

	in_nms >> info ;

	retval = true ;
	return retval ;	
}

bool orsXBinarySerialize::outport(char *&bytes,long *length, const orsIProperty *info)
{
	bool retval = false ; 
	long askLen = getLength(info);

	if( *length < askLen)
	{
		delete bytes;
		bytes = new char[askLen];
	}

	*length =  askLen;
	
	if (NULL != bytes)  {
		orsMessageStream out_nms(*length) ;
		out_nms << info;
		if (*length != out_nms.length ()) assert(0) ;
		memcpy (bytes, out_nms.rd_ptr(), *length) ;
	}
	retval = true ;
	return retval ;	
}

long orsXBinarySerialize::getLength( const orsIProperty *info)
{
	long length = 0 ;
	
	length += sizeof(long);
	
	for(int i=0;i<info->size();i++)
	{
		orsString	key;
		orsVariantType type;
		ors_int32 num;
		info->getAttributeInfo(i,key,type,num);
		
		length += sizeof(long) + key.length() + sizeof(char) + sizeof(long) ;
		
		switch(type){
		case ORS_V_I2:
			{
				orsArray<ors_int16> values;
				info->getAttr(key,values);
				length += values.size()*sizeof(ors_int16);
			}
			break;
		case ORS_V_I4:
			{
				orsArray<ors_int32> values;
				info->getAttr(key,values);
				length += values.size()*sizeof(ors_int32);
			}
			break;
		case ORS_V_R8:
			{
				orsArray<ors_float64> values;
				info->getAttr(key,values);
				length += values.size()*sizeof(ors_float64);
			}
			break;
		case ORS_V_STR:
			{
				orsArray<orsString> values;
				info->getAttr(key,values);
				for(int j=0;j<values.size();j++)
				{
					length += values[j].length() + sizeof(ors_int32); 
				}
			}
			break;
		case ORS_V_BLOB:
			{
				orsArray<const ors_byte*> values;
				orsArray<ors_int32> lens;
				info->getAttr(key,values,lens);
				for(int j=0;j<values.size();j++)
				{
					length += lens[j] + sizeof (ors_int32);
				}
			}
			break;
		case ORS_V_CHILD:
			{
				orsArray<ref_ptr<orsIProperty> > childs;
				info->getAttr(key,childs);
				for(int j=0;j<childs.size();j++)
				{
					length += getLength( childs[j].get() );
				}
			}
			break;
		default:
			assert(0);
			break ;
		}
	}
	return length ;
}

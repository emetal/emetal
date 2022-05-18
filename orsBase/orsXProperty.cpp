//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsXProperty.cpp 2008/9/24 8:41 JWS $
//
// Project: OpenRS
//
// Purpose: <Key,Value>属性实现
//
// Author:  GUO Wei, JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsXProperty.cpp,v $
//
// Revision 0.9 date: 2008/11/27 by JIANG Wanshou
// 增加矩阵和矢量
//
// Revision 0.8 date: 2008/9/24 by GUO Wei
// new
//

#include "stdafx.h"

#include "orsBase/orsUtil.h"
#include "orsXProperty.h"

struct blobDATA {
	ors_int32 length;
	ors_byte  data[1];
};

// 内存块
class orsBlob : public orsBLOB
{
private:
	blobDATA *alloc( ors_int32 length );

public:
	orsBlob()	{ pData = NULL; }
	~orsBlob()	{ free( pData );	}

	bool SetData( const ors_byte *data, ors_int32 length );

	bool SetString( const orsChar * str );

	void setVector( const orsVectorBase &value );
	void setMatrix( const orsMatrixBase &value );

	const orsBlob &operator = (const orsBlob &v)
	{
		SetData( v.GetData(), v.GetLength() );
		return *this;
	}

	ors_int32 GetLength()  const
	{
		return ((blobDATA *)pData)->length;
	}

	const ors_byte *GetData() const
	{
		return ((blobDATA *)pData)->data;
	};

	const orsChar * GetString() const
	{
		if(pData == NULL)
			return NULL;
		return (const orsChar *)((blobDATA *)pData)->data;
	};

	bool getVector( orsVectorBase &value ) const;
	bool getMatrix( orsMatrixBase &value ) const;

};

blobDATA *orsBlob::alloc( ors_int32 length )
{
	if( NULL != pData )	{
		blobDATA *blob = (blobDATA *)pData;
		if( blob->length < length )	{
			free( pData );
			pData = NULL;
		}
		else
			return (blobDATA *)pData;
	}

	if( NULL != pData )
		return (blobDATA *)pData;

	pData = malloc( length + sizeof(ors_int32) );
	blobDATA *blob = (blobDATA *)pData;
	blob->length = length;

	return blob;
}


bool orsBlob::SetData( const ors_byte *data, ors_int32 length )
{
	blobDATA *blob = alloc( length );

	if( NULL != blob ) {
		memcpy( blob->data, data, length );
		return true;
	}

	return false;
}

bool orsBlob::SetString( const orsChar * str )
{
	if(str == NULL)
		return false;
	int len = strlen(str);
	blobDATA *blob = alloc( len + 1 );

	if( blob )	{
		memcpy( blob->data, str, len );
		blob->data[len] = 0;
		return true;
	}

	return false;
}

void orsBlob::setVector( const orsVectorBase &v )
{
	int headBytes = 2*sizeof( ors_int32 );
	int dataBytes = v.Rows()*orsGetSizeOfType( v.GetDataType() );

	blobDATA *blob = alloc( headBytes + dataBytes );

	ors_int32 *pInfo = (ors_int32 *)blob->data;

	pInfo[0] = v.GetDataType();	
	pInfo[1] = v.Rows();

	memcpy( blob->data + headBytes, v.Buf(), dataBytes );
}

void orsBlob::setMatrix( const orsMatrixBase &m )
{
	int headBytes = 3*sizeof( ors_int32 );
	int dataBytes = m.Rows()*m.Cols() * orsGetSizeOfType( m.GetDataType() );

	blobDATA *blob = alloc( headBytes + dataBytes );
	ors_int32 *pInfo = (ors_int32 *)blob->data;
	
	pInfo[0] = m.GetDataType();	
	pInfo[1] = m.Rows();
	pInfo[2] = m.Cols();
	
	memcpy( blob->data + headBytes, m.Buf(), dataBytes );
}

bool orsBlob::getVector( orsVectorBase &vectorOut ) const
{
	blobDATA *blob = (blobDATA *)pData;
	ors_int32 *pInfo = (ors_int32 *)blob->data;

	orsDataTYPE type = (orsDataTYPE)pInfo[0];
	ors_int32 nRows =  pInfo[1];

	int headBytes = 2*sizeof( ors_int32 );
	int dataBytes = nRows* orsGetSizeOfType( type );

	vectorOut.SetDataType( type );

	switch( type)	{
	case ORS_DT_FLOAT32:
		{
			orsVector<ors_float32>	*tVector = (orsVector<ors_float32>	*)&vectorOut;
			tVector->Alloc( nRows );
		}
		break;
	case ORS_DT_FLOAT64:
		{
			orsVector<ors_float64>	*tVector = (orsVector<ors_float64>	*)&vectorOut;
			tVector->Alloc( nRows );
		}
		break;
	default:
		assert( false );
	}

	memcpy( vectorOut.Buf(), (BYTE *)blob->data+headBytes, dataBytes );

	return true;
}

bool orsBlob::getMatrix( orsMatrixBase &matrixOut ) const
{
	blobDATA *blob = (blobDATA *)pData;
	ors_int32 *pInfo = (ors_int32 *)blob->data;
	
	orsDataTYPE type = (orsDataTYPE)pInfo[0];
	ors_int32 nRows =  pInfo[1];
	ors_int32 nCols =  pInfo[2];
	
	int headBytes = 3*sizeof( ors_int32 );
	int dataBytes = nRows*nCols*orsGetSizeOfType( type );
	
	matrixOut.SetDataType( type );
	
	switch( type)	{
	case ORS_DT_FLOAT32:
		{
			orsMatrix<ors_float32>	*tMatrix = (orsMatrix<ors_float32>	*)&matrixOut;
			tMatrix->Alloc( nRows, nCols );
		}
		break;
	case ORS_DT_FLOAT64:
		{
			orsMatrix<ors_float64>	*tMatrix = (orsMatrix<ors_float64>	*)&matrixOut;
			tMatrix->Alloc( nRows, nCols );
		}
		break;
	default:
		assert( false );
	}

	memcpy( matrixOut.Buf(), (BYTE *)blob->data+headBytes, dataBytes );

	return true;
}

//////////////////////////////////////////////////////////////////////////

orsVariant::orsVariant()
{
	memset( this, 0, sizeof(orsVariant) );
// 	type = ORS_V_UNKOWN;
// 	vInt32 = 0;
// 	vInt16 = 0;
// 	vFloat64 = 0;
// 	b_value_ptr = NULL;
// 	b_value_length = 0;
// 	pProperty = NULL;
}

orsVariant::orsVariant (orsVariant & var)
{
	type = var.type ;
	switch (var.type) {
	case ORS_V_I2 :
		vInt16 = var.vInt16 ;
		break;
	case ORS_V_I4 :
		vInt32 = var.vInt32 ;
		break ;
	case ORS_V_R8 :
		vFloat64 = var.vFloat64 ;
		break ;
	case ORS_V_STR :
		set( var.getString() );
		break;
	case ORS_V_BLOB :
		{
			ors_int32 len;
			const ors_byte *buf= var.getBlob( len );
			set( buf, len );
		}
		break ;
	case ORS_V_CHILD :
		set( var.getProperty() );
		break ;
	case ORS_V_OBJECT:
		set( (orsIObject *)NULL );

	default:
		assert( false );
	}
}


orsVariant::~orsVariant()
{
	switch( type )	{
	case ORS_V_OBJECT:
// 		if( vObject )
// 			vObject->release();
		break;
	case ORS_V_CHILD :
		if( vProperty )
			vProperty->release();
		break;
	case ORS_V_STR:
	case ORS_V_BLOB:
		//delete (ors_byte*)vBlob.pData;
		free( (ors_byte*)vBlob.pData );
	}
}

void orsVariant::set( ref_ptr<orsIProperty> property )
{
	if( property.get() == vProperty )
		return ;

	if( vProperty )
		vProperty->release();

	vProperty = property.get();

	if( vProperty )
		vProperty->addRef();
}

void orsVariant::set( orsIObject *pObject )
{
	if( vObject == pObject )
		return ;

// 	if( vObject )
// 		vObject->release();
	
	vObject = pObject;

// 	if( vObject )
// 		vObject->addRef();
}


void orsVariant::set( const ors_byte *data, ors_int32 length )
{
	orsBlob *pBlob = (orsBlob *)&vBlob;
	pBlob->SetData( data, length );
}

void orsVariant::set( const orsChar * str )
{
	orsBlob *pBlob = (orsBlob *)&vBlob;
	pBlob->SetString( str );
}


void orsVariant::set( const orsVectorBase &vector )
{
	orsBlob *pBlob = (orsBlob *)&vBlob;
	pBlob->setVector( vector );
}

void orsVariant::set( const orsMatrixBase &matrix )
{
	orsBlob *pBlob = (orsBlob *)&vBlob;
	pBlob->setMatrix( matrix );
}



void orsVariant::getVector( orsVectorBase &vector )
{
	orsBlob *pBlob = (orsBlob *)&vBlob;
	pBlob->getVector( vector );
}

void orsVariant::getMatrix( orsMatrixBase &matrix )
{
	orsBlob *pBlob = (orsBlob *)&vBlob;
	pBlob->getMatrix( matrix );
}


const ors_byte *orsVariant::getBlob( ors_int32 &length )
{
	orsBlob *pBlob = (orsBlob *)&vBlob;

	length = pBlob->GetLength();

	return pBlob->GetData();
}

const orsChar * orsVariant::getString()
{
	orsBlob *pBlob = (orsBlob *)&vBlob;
	return pBlob->GetString();
}

orsPropertyRecord::~orsPropertyRecord()
{
	for( unsigned int i=0; i<size();i++)
	{
		delete GetVariant( i );
	}
}

////////////////////////////////////orsXProperty/////////////////////////////

// 防止属性名带空格
stdString ToRegularName( const orsChar *name1 )
{	
	stdString name = name1;

	orsChar *s = (orsChar *)name.c_str();
	while( *s )	{
		switch( *s ) {
		case ' ':
		case '(':
		case ')':
		case '%':
			*s = '_'; 
		}
		s++; 
	};

	return name;
}



typedef orsPropertyRecordMap::value_type		_recordType;
typedef orsPropertyRecordMap::iterator			_recordIterator;
typedef orsPropertyRecordMap::const_iterator	_recordConstIterator;

orsXProperty::orsXProperty()//:m_listener(NULL)
{
}

orsXProperty::~orsXProperty()
{
	removeAll();
}

bool orsXProperty::removeAll()
{
	_recordIterator iter;
	
	for(iter = m_records.begin();iter != m_records.end();iter++)
	{
		orsPropertyRecord *pRecord = iter->second;
		
		delete pRecord;
	}

	m_records.clear();

	m_attrNames.clear();

	return true;
}


bool orsXProperty::remove( const orsChar * name )
{
	bool bRet = false;

	std::vector<stdString>::iterator it;

	for( it = m_attrNames.begin(); it<m_attrNames.end(); it++ )
	{
		if( 0 == it->compare( name ) ) {
			m_attrNames.erase( it );
			bRet = true;
			break;
		}
	}

	m_records.erase( name);
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////

#define ADD_RECORD0(name1, value)\
if( bUnique ) {\
	if( setAttr( name1, value ) )\
		return ;\
}\
\
stdString name = ToRegularName( name1 );\
\
_recordIterator pointer = m_records.find(name);\
orsPropertyRecord *pRecord = NULL;\
\
if(pointer == m_records.end()){\
	pRecord = new orsPropertyRecord;\
	m_records.insert(_recordType(name, pRecord));\
	\
	m_attrNames.push_back( name );\
} else {\
	pRecord = pointer->second;\
}

//////////////////////////////////////////////////////////////////////////

#define ADD_RECORD1(name1, value)\
	if( bUnique ) {\
	if( setAttr( name1, value, 0 ) )\
	return ;\
	}\
	\
	stdString name = ToRegularName( name1 );\
	\
	_recordIterator pointer = m_records.find(name);\
	orsPropertyRecord *pRecord = NULL;\
	\
	if(pointer == m_records.end()){\
	pRecord = new orsPropertyRecord;\
	m_records.insert(_recordType(name, pRecord));\
	\
	m_attrNames.push_back( name );\
	} else {\
	pRecord = pointer->second;\
}

//////////////////////////////////////////////////////////////////////////

void orsXProperty::addAttr(const orsChar * name1, bool value, bool bUnique )
{
	ADD_RECORD0(name1,value);
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_BOOL;
	var->set( value );
	
	pRecord->AddVariant(var);
}


void orsXProperty::addAttr(const orsChar * name1,ors_int16 value, bool bUnique )
{
	ADD_RECORD1( name1, value);
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_I2;
	var->set( value );
	
	pRecord->AddVariant(var);
}


void orsXProperty::addAttr(const orsChar * name1, ors_int32 value, bool bUnique )
{
	ADD_RECORD1(name1,value);
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_I4;
	var->set( value );
	
	pRecord->AddVariant( var );
}



void orsXProperty::addAttr(const orsChar * name1, ors_float64 value, bool bUnique )
{
	ADD_RECORD1(name1,value);

	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_R8;
	var->set( value );

	pRecord->AddVariant( var );
}

void orsXProperty::addAttr(const orsChar * name1, const orsChar * value, bool bUnique )
{
	ADD_RECORD1(name1,value);

	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_STR;
	var->set( value );

	pRecord->AddVariant(var);
}
void orsXProperty::addAttribute(const orsChar * name1, const orsChar * value, bool bUnique )
{
	ADD_RECORD1(name1,value);

	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_Attribute;
	var->set( value );

	pRecord->AddVariant(var);
}
void orsXProperty::addAttr(const orsChar * name1, const orsVectorBase &value , bool bUnique )
{
	ADD_RECORD0(name1,value);

	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_VECTOR;
	var->set( value );

	pRecord->AddVariant( var );
}

void orsXProperty::addAttr(const orsChar * name1, const orsMatrixBase &value, bool bUnique )
{
	ADD_RECORD0(name1,value);
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_MATRIX;
	var->set( value );
	
	pRecord->AddVariant( var );
	
}

void orsXProperty::addAttr(const orsChar * name1, const ors_byte *pValue, ors_int32 nLength, bool bUnique )
{
	ADD_RECORD0(name1, pValue);

	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_BLOB;
	var->set( pValue, nLength );

	pRecord->AddVariant(var);
}



//////////////////////////////////////////////////////////////////////////

#define GET_RECORD(name1, value)\
stdString name = ToRegularName( name1 );\
\
_recordIterator iter;\
iter = m_records.find(name);\
\
if(iter == m_records.end())\
return false;\
\
orsPropertyRecord* pRecord = iter->second;

//////////////////////////////////////////////////////////////////////////

bool orsXProperty::setAttr(const orsChar * name1, bool value)
{
	GET_RECORD( name1, value);
	
	orsVariant *var = pRecord->GetVariant( 0 );
	
	switch( var->type )
	{
	case ORS_V_BOOL:
		var->set( value );
		break;
	default:
		return false;
	}
		
	return true;
}


bool orsXProperty::setAttr(const orsChar * name1, ors_int32 value, ors_uint32 index)
{
	GET_RECORD( name1, value);
		
	if( index >= pRecord->size())
		return false;
	
	orsVariant *var = pRecord->GetVariant( index );
	
	switch( var->type )
	{
	case ORS_V_I4:
		var->set( value );
		break;
	case ORS_V_I2:
		var->set( (ors_int16)value );
		break;

	default:
		return false;
	}

	return true;
}

bool orsXProperty::setAttr(const orsChar * name1, ors_int16 value, ors_uint32 index)
{
	GET_RECORD( name1, value);
	
	if(index >= pRecord->size())
		return false;
	
	orsVariant *var = pRecord->GetVariant(index);
	
	switch( var->type )
	{
	case ORS_V_I2:
		var->set( value );
		break;
	case ORS_V_I4:
		var->set( (ors_int32)value );
	default:
		return false;
	}

	return true;
}

bool orsXProperty::setAttr(const orsChar * name1, const orsChar * value, ors_uint32 index)
{
	GET_RECORD( name1, value);
	
	if( index >= pRecord->size() )
		return false;
	
	orsVariant *var = pRecord->GetVariant(index);
	
	if(var->type == ORS_V_STR)
		var->set( value );
	else
		return false;

	return true;
}
bool orsXProperty::setAttribute(const orsChar * name1, const orsChar * value, ors_uint32 index)
{
	GET_RECORD( name1, value);

	if( index >= pRecord->size() )
		return false;

	orsVariant *var = pRecord->GetVariant(index);

	if(var->type == ORS_V_Attribute)
		var->set( value );
	else
		return false;

	return true;
}

bool orsXProperty::setAttr(const orsChar * name1, const orsVectorBase &value )
{
	GET_RECORD( name1, value);

	orsVariant *var = pRecord->GetVariant(0);

	var->type = ORS_V_VECTOR;

	var->set( value );
	
	return true;
}

bool orsXProperty::setAttr(const orsChar * name1, const orsMatrixBase &value )
{
	GET_RECORD( name1, value);

	orsVariant *var = pRecord->GetVariant(0);
	
	var->type = ORS_V_MATRIX;
	var->set( value );

	return true;
}

bool orsXProperty::setAttr(const orsChar * name1, ors_float64 value, ors_uint32 index)
{
	GET_RECORD( name1, value);
	
	if(index >= pRecord->size())
		return false;
	
	orsVariant *var = pRecord->GetVariant(index);
	
	if(var->type == ORS_V_I2)
	{
		var->set( (ors_int16)value );
	}
	else if(var->type == ORS_V_I4)
	{
		var->set( (ors_int32)value );
	}
	else if(var->type == ORS_V_R8)
	{
		var->set( value );
	}
	else
		return false;

	return true;
}

bool orsXProperty::setAttr(const orsChar * name1, const ors_byte* pValue, ors_int32 nLength,
								ors_uint32 index)
{
	GET_RECORD( name1, value);
	
	if(index >= pRecord->size())
		return false;
	
	orsVariant *var = pRecord->GetVariant(index);
	
	if( var->type == ORS_V_BLOB )
	{
		var->set( pValue, nLength );
	}
	else
		return false;

	return true;
}

bool orsXProperty::setAttr(const orsChar * name1, ref_ptr<orsIProperty> value, ors_uint32 index )
{
	GET_RECORD( name1, value);
	
	if( index >= pRecord->size() )
		return false;
	
	orsVariant *var = pRecord->GetVariant( index );
	
	if( var->type == ORS_V_CHILD )
	{
		var->set( value );
	}
	else
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////

bool orsXProperty::getAttr( const orsChar * name1, bool & value )  const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;
	
	if (it != m_records.end()) {
		pRecord = it->second ;
		for ( unsigned i = 0 ; i < pRecord->size() ; i++) {
			switch( pRecord->GetVariant(i)->type )
			{
			case ORS_V_BOOL:
				value = pRecord->GetVariant(i)->getBool();
				break;
			case ORS_V_STR:
				{
					if( orsString::findSubStr_i( pRecord->GetVariant(i)->getString(), "true" ) )
						value = true;
					else if( orsString::findSubStr_i( pRecord->GetVariant(i)->getString(), "false" ) )
						value = false;
					else
						return false;
				}
				break;
			}
		}
		
		return true ;
	}
	return false ;
}

bool orsXProperty::getAttr(const orsChar * name1, orsArray<ors_int16> &values) const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;

	if (it != m_records.end()) {
		values.clear();

		pRecord = it->second ;
		for ( unsigned i = 0 ; i < pRecord->size() ; i++) {
			switch( pRecord->GetVariant(i)->type )
			{
			case ORS_V_I2:
				values.push_back( pRecord->GetVariant(i)->getInt16() );
				break;
			case ORS_V_I4:
				values.push_back( pRecord->GetVariant(i)->getInt32() );
				break;
			case ORS_V_STR:
				{
					int value;
					if(pRecord->GetVariant(i)->getString()!=NULL)
					{
						if( 1 == sscanf( pRecord->GetVariant(i)->getString(), "%d", &value ) )
						values.push_back( (ors_int16)value );
					}
				}
				break;
			default:
				return false;
				break;
			}
		}
		return true ;
	}
	return false ;
}


bool orsXProperty::getAttr(const orsChar * name1, orsArray<ors_int32> &values) const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;

	if (it != m_records.end()) {
		values.clear();

		pRecord = it->second ;
		for ( unsigned i = 0 ; i < pRecord->size() ; i++) {
			switch(pRecord->GetVariant(i)->type)
			{
			case ORS_V_I2:
				values.push_back(pRecord->GetVariant(i)->getInt16()) ;
				break;
			case ORS_V_I4:
				values.push_back(pRecord->GetVariant(i)->getInt32()) ;
				break;
			case ORS_V_STR:
				{
					int value;
					if(pRecord->GetVariant(i)->getString()!=NULL)
					{
						if( 1 == sscanf( pRecord->GetVariant(i)->getString(), "%d", &value ) )
						values.push_back( (ors_int32)value );
					}
				}
				break;
			default:
				return false;
				break;
			}
		}
		return true ;
	}
	return false ;
}


bool
orsXProperty::getAttr(const orsChar * name1,orsArray<ors_float64> &values) const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;

	if (it != m_records.end()) {
		values.clear();

		pRecord = it->second ;
		for ( unsigned i = 0 ; i < pRecord->size() ; i++)
		{
			switch( pRecord->GetVariant(i)->type )
			{
			case ORS_V_I2:
				values.push_back( pRecord->GetVariant(i)->getInt16() ) ;
				break;
			case ORS_V_I4:
				values.push_back( pRecord->GetVariant(i)->getInt32() ) ;
				break;
			case ORS_V_R8:
				values.push_back( pRecord->GetVariant(i)->getFloat64() ) ;
				break;
			case ORS_V_STR:
				{
					double value;
					if(pRecord->GetVariant(i)->getString()!=NULL)
					{
						if( 1 == sscanf( pRecord->GetVariant(i)->getString(), "%lf", &value ) )
							values.push_back( (ors_float64)value );
					}
				}
				break;
			}
		}

		return true ;
	}

	return false ;
}
//bool orsXProperty::getAttr(const orsChar * name1, std::vector<ors_int16> &values) const
//{
//	stdString name = ToRegularName( name1 );
//
//	orsPropertyRecord* pRecord;
//	_recordConstIterator it = m_records.find(name) ;
//
//	if (it != m_records.end()) {
//		values.clear();
//
//		pRecord = it->second ;
//		for ( unsigned i = 0 ; i < pRecord->size() ; i++) {
//			switch( pRecord->GetVariant(i)->type )
//			{
//			case ORS_V_I2:
//				values.push_back( pRecord->GetVariant(i)->getInt16() );
//				break;
//			case ORS_V_I4:
//				values.push_back( pRecord->GetVariant(i)->getInt32() );
//				break;
//			case ORS_V_STR:
//				{
//					int value;
//
//					if( 1 == sscanf( pRecord->GetVariant(i)->getString(), "%d", &value ) )
//						values.push_back( (ors_int16)value );
//				}
//				break;
//			default:
//				return false;
//				break;
//			}
//		}
//		return true ;
//	}
//	return false ;
//}
//

//bool orsXProperty::getAttr(const orsChar * name1, std::vector<ors_int32> &values) const
//{
//	stdString name = ToRegularName( name1 );
//
//	orsPropertyRecord* pRecord;
//	_recordConstIterator it = m_records.find(name) ;
//
//	if (it != m_records.end()) {
//		values.clear();
//
//		pRecord = it->second ;
//		for ( unsigned i = 0 ; i < pRecord->size() ; i++) {
//			switch(pRecord->GetVariant(i)->type)
//			{
//			case ORS_V_I2:
//				values.push_back(pRecord->GetVariant(i)->getInt16()) ;
//				break;
//			case ORS_V_I4:
//				values.push_back(pRecord->GetVariant(i)->getInt32()) ;
//				break;
//			case ORS_V_STR:
//				{
//					int value;
//
//					if( 1 == sscanf( pRecord->GetVariant(i)->getString(), "%d", &value ) )
//						values.push_back( (ors_int32)value );
//				}
//				break;
//			default:
//				return false;
//				break;
//			}
//		}
//		return true ;
//	}
//	return false ;
//}
//
//
//bool
//	orsXProperty::getAttr(const orsChar * name1,std::vector<ors_float64> &values) const
//{
//	stdString name = ToRegularName( name1 );
//
//	orsPropertyRecord* pRecord;
//	_recordConstIterator it = m_records.find(name) ;
//
//	if (it != m_records.end()) {
//		values.clear();
//
//		pRecord = it->second ;
//		for ( unsigned i = 0 ; i < pRecord->size() ; i++)
//		{
//			switch( pRecord->GetVariant(i)->type )
//			{
//			case ORS_V_I2:
//				values.push_back( pRecord->GetVariant(i)->getInt16() ) ;
//				break;
//			case ORS_V_I4:
//				values.push_back( pRecord->GetVariant(i)->getInt32() ) ;
//				break;
//			case ORS_V_R8:
//				values.push_back( pRecord->GetVariant(i)->getFloat64() ) ;
//				break;
//			case ORS_V_STR:
//				{
//					double value;
//
//					if( 1 == sscanf( pRecord->GetVariant(i)->getString(), "%lf", &value ) )
//						values.push_back( (ors_float64)value );
//				}
//				break;
//			}
//		}
//
//		return true ;
//	}
//
//	return false ;
//}

bool orsXProperty::getAttr(const orsChar * name1, orsArray<const ors_byte *> &values, orsArray<ors_int32> &vLength) const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord * pRecord;
	_recordConstIterator it = m_records.find(name) ;

	if (it != m_records.end()) {
		values.clear();
		vLength.clear();

		pRecord = it->second ;
		for ( unsigned i = 0 ; i < pRecord->size() ; i++)
		{
			switch( pRecord->GetVariant(i)->type )	{
			case ORS_V_BLOB:
			case ORS_V_VECTOR:
			case ORS_V_MATRIX:
				{
					ors_int32 len;
					values.push_back( pRecord->GetVariant(i)->getBlob( len ) ) ;
					vLength.push_back( len );
				}
				break;
			}			
		}

		return true ;
	}

	return false ;
}
//bool orsXProperty::getAttr(const orsChar * name1, std::vector<const ors_byte *> &values, std::vector<ors_int32> &vLength) const
//{
//	stdString name = ToRegularName( name1 );
//
//	orsPropertyRecord * pRecord;
//	_recordConstIterator it = m_records.find(name) ;
//
//	if (it != m_records.end()) {
//		values.clear();
//		vLength.clear();
//
//		pRecord = it->second ;
//		for ( unsigned i = 0 ; i < pRecord->size() ; i++)
//		{
//			switch( pRecord->GetVariant(i)->type )	{
//			case ORS_V_BLOB:
//			case ORS_V_VECTOR:
//			case ORS_V_MATRIX:
//				{
//					ors_int32 len;
//					values.push_back( pRecord->GetVariant(i)->getBlob( len ) ) ;
//					vLength.push_back( len );
//				}
//				break;
//			}			
//		}
//
//		return true ;
//	}
//
//	return false ;
//}
bool orsXProperty::getAttr(const orsChar * name1, orsArray<ors_string> &values) const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;

	_recordConstIterator it = m_records.find( name ) ;

	if( it != m_records.end() ) {
		values.clear();

		pRecord = it->second ;
		for (int i = 0 ; i < pRecord->size() ; i++)
		{
			switch( pRecord->GetVariant(i)->type ) {
			case ORS_V_STR:
				{
					const orsChar * str = pRecord->GetVariant(i)->getString();
					if(str == NULL){
						ors_string nullstr;
						values.push_back(nullstr);
					}else {
						values.push_back(str);
					}
				}
				break;
			case ORS_V_Attribute:
				{
					const orsChar * str = pRecord->GetVariant(i)->getString();
					if(str == NULL){
						ors_string nullstr;
						values.push_back(nullstr);
					}else {
						values.push_back(str);
					}
				}
				break;
			case ORS_V_BLOB:
				{
					orsVariant* var = pRecord->GetVariant( i );

					ors_int32 len;
					const ors_byte *buf = var->getBlob( len );

					char * temp_str = new char[len+1];
					memcpy( temp_str, buf, len );

					temp_str[len] = '\0';

					values.push_back( temp_str );

					delete temp_str;
				}
			default:
				return false;
			}
		}
		return true ;
	}
	return false ;
}
//bool orsXProperty::getAttr(const orsChar * name1, std::vector<string>  &values) const
//{
//	stdString name = ToRegularName( name1 );
//
//	orsPropertyRecord* pRecord;
//
//	_recordConstIterator it = m_records.find( name ) ;
//
//	if( it != m_records.end() ) {
//		values.clear();
//
//		pRecord = it->second ;
//		for (unsigned i = 0 ; i < pRecord->size() ; i++)
//		{
//			switch( pRecord->GetVariant(i)->type ) {
//			case ORS_V_STR:
//				{
//					const orsChar * str = pRecord->GetVariant(i)->getString();
//					if(str == NULL){
//						string nullstr;
//						values.push_back(nullstr);
//					}else {
//						values.push_back(str);
//					}
//				}
//				break;
//			case ORS_V_Attribute:
//				{
//					const orsChar * str = pRecord->GetVariant(i)->getString();
//					if(str == NULL){
//						string nullstr;
//						values.push_back(nullstr);
//					}else {
//						values.push_back(str);
//					}
//				}
//				break;
//			case ORS_V_BLOB:
//				{
//					orsVariant* var = pRecord->GetVariant( i );
//
//					ors_int32 len;
//					const ors_byte *buf = var->getBlob( len );
//
//					char * temp_str = new char[len+1];
//					memcpy( temp_str, buf, len );
//
//					temp_str[len] = '\0';
//
//					values.push_back( temp_str );
//
//					delete temp_str;
//				}
//			default:
//				return false;
//			}
//		}
//		return true ;
//	}
//	return false ;
//}

bool
orsXProperty::getAttr(const orsChar * name1,orsArray< ref_ptr<orsIProperty> > &values ) const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;

	if (it != m_records.end()) {
		values.clear();

		pRecord = it->second ;
		for (unsigned i = 0 ; i < pRecord->size() ; i++)
		{
			if( ORS_V_CHILD == pRecord->GetVariant(i)->type )
				values.push_back( pRecord->GetVariant(i)->getProperty() );
		}
		return true ;
	}
	return false ;
}
//bool
//	orsXProperty::getAttr(const orsChar * name1,std::vector< ref_ptr<orsIProperty> > &values ) const
//{
//	stdString name = ToRegularName( name1 );
//
//	orsPropertyRecord* pRecord;
//	_recordConstIterator it = m_records.find(name) ;
//
//	if (it != m_records.end()) {
//		values.clear();
//
//		pRecord = it->second ;
//		for (unsigned i = 0 ; i < pRecord->size() ; i++)
//		{
//			if( ORS_V_CHILD == pRecord->GetVariant(i)->type )
//				values.push_back( pRecord->GetVariant(i)->getProperty() );
//		}
//		return true ;
//	}
//	return false ;
//}




/////////////////////////////// 单接口: 复接口的特例 /////////////////////////////////////

bool orsXProperty::getAttr( const orsChar * name1, ors_float64 &value) const
{
	stdString name = ToRegularName( name1 );

	orsArray<ors_float64> v;
	if (getAttr(name.c_str(),v)) {
		if (!v.size()) return false ;
		value = v[0] ;
		return true ;
	}
	return false ;
}


bool orsXProperty::getAttr(const orsChar * name1, ors_int32 &value) const
{
	stdString name = ToRegularName( name1 );

	orsArray<ors_int32> v;

	if( getAttr(name.c_str(),v) ) {
		if( 0 == v.size() )
			return false ;

		value = v[0];

		return true ;
	}
	return false ;
}



bool orsXProperty::getAttr( const orsChar * name1, ors_int16 & value )  const
{
	stdString name = ToRegularName( name1 );

	orsArray<ors_int16> v;
	
	if( getAttr(name.c_str(), v) ) {
		if( 0 == v.size() )
			return false ;
		
		value = v[0] ;

		return true ;
	}
	return false ;
}


bool orsXProperty::getAttr( const orsChar * name1, ors_string &value )  const
{
	//printf("getAttr: %s \n", name1);

	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;

	_recordConstIterator it = m_records.find( name ) ;

	if( it != m_records.end() ) {
		pRecord = it->second ;

		switch( pRecord->GetVariant(0)->type ) {
		case ORS_V_STR:
			{
				const orsChar *str = pRecord->GetVariant(0)->getString();
				if (str != NULL) {
					//printf("getAttr: %s = %s\n", name.c_str(), str);
					value = str;
				}
				//else
					//printf("getAttr: %s = NULL \n", name.c_str());
			}
			break;
		case ORS_V_Attribute:
			{
				const orsChar *str = pRecord->GetVariant(0)->getString();
				if(str != NULL)
					value = str;
			}
			break;
		case ORS_V_BLOB:
			{
				orsVariant* var = pRecord->GetVariant( 0 );

				ors_int32 len;
				const ors_byte *buf = var->getBlob( len );

				char * temp_str = new char[len+1];
				memcpy( temp_str, buf, len );

				temp_str[len] = '\0';

				value = temp_str;

				delete temp_str;
			}
		default:
			return false;
		}

		return true;
	}
	return false ;
}


bool orsXProperty::getAttr( const orsChar * name1, string &value )  const
{
	//printf("getAttr: %s\n", name1);
	stdString name = ToRegularName( name1 );

	//	printf("name: %s\n", name.c_str());

	orsPropertyRecord* pRecord;

	_recordConstIterator it = m_records.find( name ) ;

	if( it != m_records.end() ) {
		pRecord = it->second ;

		switch( pRecord->GetVariant(0)->type ) {
		case ORS_V_STR:
			{
				const orsChar *str = pRecord->GetVariant(0)->getString();
				if(str != NULL)
					value = str;
			}
			break;
		case ORS_V_Attribute:
			{
				const orsChar *str = pRecord->GetVariant(0)->getString();
				if(str != NULL)
					value = str;
			}
			break;
		case ORS_V_BLOB:
			{
				orsVariant* var = pRecord->GetVariant( 0 );

				ors_int32 len;
				const ors_byte *buf = var->getBlob( len );

				char * temp_str = new char[len+1];
				memcpy( temp_str, buf, len );

				temp_str[len] = '\0';

				value = temp_str;

				delete temp_str;
			}
		default:
			return false;
		}

		return true;
	}
	return false ;
}


bool orsXProperty::getAttr(const orsChar * name1, orsVectorBase &vector ) const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;

	_recordConstIterator it = m_records.find( name ) ;

	if( it != m_records.end() ) {
		pRecord = it->second ;

		switch( pRecord->GetVariant(0)->type ) {
		case ORS_V_VECTOR:
			pRecord->GetVariant(0)->getVector( vector );
			return true;
		}
	}

	return false;
}


bool orsXProperty::getAttr(const orsChar * name1, orsMatrixBase &matrix ) const
{
	stdString name = ToRegularName( name1 );

	orsPropertyRecord* pRecord;

	_recordConstIterator it = m_records.find( name ) ;

	if( it != m_records.end() ) {
		pRecord = it->second ;

		switch( pRecord->GetVariant(0)->type ) {
		case ORS_V_MATRIX:
			pRecord->GetVariant(0)->getMatrix( matrix );
			return true;
		}
	}

	return false;
}

bool orsXProperty::getAttr(const orsChar * name1, const ors_byte * &value, ors_int32 &nLength)  const
{
	stdString name = ToRegularName( name1 );

	orsArray<const ors_byte *> vChar;
	orsArray<ors_int32> vLong ;

	if( getAttr( name.c_str(), vChar, vLong) )
	{
		if( 0 == vChar.size() )
			return false ;

		value = vChar[0] ;
		nLength = vLong[0] ;

		return true;
	}
	return false;
}

bool orsXProperty::getAttr(const orsChar * name1, ref_ptr<orsIProperty> &value) const
{
	stdString name = ToRegularName( name1 );

	orsArray< ref_ptr<orsIProperty> > v ;
	if( getAttr(name.c_str(), v) ) {
		if (!v.size()) return false ;
		value = v[0];
		return true ;
	}
	return false ;
}

//////////////////////////////////////////////////////////////////////////

void orsXProperty::addAttr(const orsChar * name1, ref_ptr<orsIProperty> value, bool bUnique )
{
	if( bUnique ) {
		if( setAttr( name1, value, 0 ) )
			return ;
	}

	stdString name = ToRegularName( name1 );
	
	_recordIterator pointer = m_records.find(name);
	
	orsPropertyRecord *pRecord = NULL;
	
	if(pointer == m_records.end()){
		pRecord = new orsPropertyRecord;
		m_records.insert(_recordType(name, pRecord));
		m_attrNames.push_back( name );
	} else
		pRecord = pointer->second;
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_CHILD ;
	var->set( value );
	
	pRecord->AddVariant( var );
}


orsIProperty *orsXProperty::createChild(const orsChar * name1, bool bUnique )
{
	stdString name = ToRegularName( name1 );
	
	_recordIterator pointer = m_records.find(name);

	ref_ptr<orsIProperty> pChild;

	if( pointer == m_records.end() || !bUnique ) {
		pChild = _getPlatform()->createProperty();
		addAttr( name1, pChild );
	} 
	else	{
		getAttr( name1, pChild );
	}

	return pChild.get();
}

void orsXProperty::addAttr(const orsChar * name1, orsIObject *objPtr, bool bUnique )
{
	if( ORS_PTR_CAST( orsIProperty, objPtr ) ) {
		ref_ptr<orsIProperty> property = (orsIProperty *)objPtr;
		addAttr( name1, property, bUnique );
		return ;
	}

	if( bUnique ) {
		if( setAttr( name1, objPtr  ) )
			return ;
	}

	stdString name = ToRegularName( name1 );
	
	_recordIterator pointer = m_records.find(name);
	
	orsPropertyRecord *pRecord = NULL;
	
	if(pointer == m_records.end()){
		pRecord = new orsPropertyRecord;
		m_records.insert(_recordType(name, pRecord));
		m_attrNames.push_back( name );
	} else
		pRecord = pointer->second;
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_OBJECT ;
	var->set( objPtr );
	
	pRecord->AddVariant( var );	
}

bool orsXProperty::setAttr(const orsChar * name1, orsIObject *objPtr )
{
	stdString name = ToRegularName( name1 );
	
	_recordIterator iter;
	
	iter = m_records.find(name);
	
	if( iter == m_records.end() )
		return false;
	
	orsPropertyRecord *pRecord = iter->second;
	
	orsVariant *var = pRecord->GetVariant( 0 );
	
	if( var->type == ORS_V_OBJECT )
	{
		var->set( objPtr );
	}
	else
		return false;
		
	return true;
}



bool
orsXProperty::getAttr(const orsChar * name1, orsIObject *&value ) const
{
	stdString name = ToRegularName( name1 );
	
	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;
	
	if (it != m_records.end()) {
		pRecord = it->second ;
		for (unsigned i = 0 ; i < pRecord->size() ; i++)
		{
			if( ORS_V_OBJECT == pRecord->GetVariant(i)->type )	{
				value = pRecord->GetVariant(i)->getObject();
				break;
			}
		}
		return true ;
	}
	return false ;
}


//////////////////////////////////////////////////////////////////////////

void orsXProperty::addAttr(const orsChar * name1, orsPropertyListener *objPtr, bool bUnique )
{
	if( bUnique ) {
		if( setAttr( name1, objPtr ) )
			return ;
	}

	stdString name = ToRegularName( name1 );
	
	_recordIterator pointer = m_records.find(name);
	
	orsPropertyRecord *pRecord = NULL;
	
	if(pointer == m_records.end()){
		pRecord = new orsPropertyRecord;
		m_records.insert(_recordType(name, pRecord));
		m_attrNames.push_back( name );
	} else
		pRecord = pointer->second;
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_LISTNER ;
	var->set( objPtr );
	
	pRecord->AddVariant( var );	
}

bool orsXProperty::setAttr(const orsChar * name1, orsPropertyListener *objPtr )
{
	stdString name = ToRegularName( name1 );
	
	_recordIterator iter;
	
	iter = m_records.find(name);
	
	if( iter == m_records.end() )
		return false;
	
	orsPropertyRecord *pRecord = iter->second;
	
	// 	if( index >= pRecord->size() )
	// 		return false;
	
	orsVariant *var = pRecord->GetVariant( 0 );
	
	if( var->type == ORS_V_LISTNER )
	{
		var->set( objPtr );
	}
	else
		return false;
	
	return true;
}



bool
orsXProperty::getAttr(const orsChar * name1, orsPropertyListener *&value ) const
{
	stdString name = ToRegularName( name1 );
	
	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;
	
	if (it != m_records.end()) {
		pRecord = it->second ;
		for (unsigned i = 0 ; i < pRecord->size() ; i++)
		{
			if( ORS_V_LISTNER == pRecord->GetVariant(i)->type )	{
				value = pRecord->GetVariant(i)->getPropListener();
				break;
			}
		}
		return true ;
	}
	return false ;
}


//////////////////////////////////////////////////////////////////////////

void orsXProperty::copy( const orsIProperty *pProperty )
{
	if(pProperty == NULL)
		return ;
	int i;
	
	ors_string name;
	orsVariantType type;
	ors_int32 valuenum;

	for( i=0; i<pProperty->size(); i++ )
	{
		pProperty->getAttributeInfo(i, name, type, valuenum);
		
		switch(type){
		case ORS_V_BOOL:
			{
				bool value;
				pProperty->getAttr(name.c_str(), value );
				
				addAttr( name, value );
			}
			break;
		case ORS_V_I2:
			{
				orsArray<ors_int16> values;
				pProperty->getAttr(name.c_str(), values);
				
				for (int j=0;j<values.size();j++)
					addAttr( name, values[j] );
			}
			break;
		case ORS_V_I4:
			{
				orsArray<ors_int32> values;
				
				pProperty->getAttr(name.c_str(), values);
				
				for (int j=0;j<values.size();j++)
					addAttr( name, values[j]);
			}
			break;
		case ORS_V_R8:
			{
				orsArray<ors_float64> values;
				
				pProperty->getAttr(name.c_str(), values);
				
				for (int j=0;j<values.size();j++)
					addAttr( name, values[j] );
			}
			break;
		case ORS_V_STR:
			{
				orsArray<ors_string> values;
				
				pProperty->getAttr(name.c_str(), values);
				
				for (int j=0;j<values.size();j++)
					addAttr( name, values[j].c_str());
			}
			break;
		case ORS_V_BLOB:
			{
				orsArray<const ors_byte *> values;
				orsArray<ors_int32> nLengths;
				
				pProperty->getAttr(name.c_str(), values,nLengths);
				
				for (int j=0;j<values.size();j++)
				{
					addAttr( name, values[j], nLengths[j] );
				}
			}
			break;
		case ORS_V_CHILD:
			{
				orsArray<ref_ptr<orsIProperty> > children;
				pProperty->getAttr(name.c_str(), children );
				
				for (int j=0; j<children.size();j++)
				{
					ref_ptr<orsIProperty> newChild = _getPlatform()->createProperty();

					newChild->copy(  children[j].get() );
					addAttr( name, newChild );
				}
			}
			break;

		case ORS_V_MATRIX:
			{
				orsMatrixD value;
				
				pProperty->getAttr(name.c_str(), value );
				
				addAttr( name, value );
			}
			break;
		case ORS_V_VECTOR:
			{
				orsVectorD value;
				
				pProperty->getAttr(name.c_str(), value );
				
				addAttr( name, value );
			}
			break;
		case ORS_V_LISTNER:
			{
// 				orsPropertyListener *pListner;
// 				
// 				pProperty->getAttr(name.c_str(), pListner );
// 				
// 				addAttr( name, pListner );
			}
			break;
		case ORS_V_OBJECT:
			{
				orsIObject *value = NULL;
				
				pProperty->getAttr(name.c_str(), value );
				
				addAttr( name, value );
			}
		default:
			assert( false ) ;
		}
	}
}


//////////////////////////////////////////////////////////////////////////


// 只更新自己有的
void orsXProperty::update( const orsIProperty *pProperty )
{
	if(pProperty == NULL)
		return ;
	int i;

	ors_string name;
	orsVariantType type;
	ors_int32 valuenum;

	for( i=0; i<size(); i++ )
	{
		getAttributeInfo(i, name, type, valuenum);
		
		switch(type){
		case ORS_V_BOOL:
			{
				bool value;
				
				if( pProperty->getAttr(name.c_str(), value ) )
					setAttr( name, value );
			}
			break;
		case ORS_V_I2:
			{
 				orsArray<ors_int16> values;
 				pProperty->getAttr(name.c_str(), values);

				for (int j=0;j<values.size();j++)
					setAttr( name, values[j], j );
			}
			break;
		case ORS_V_I4:
			{
				orsArray<ors_int32> values;
				
				pProperty->getAttr(name.c_str(), values);
				
				for (int j=0;j<values.size();j++)
					setAttr( name, values[j], j );
			}
			break;
		case ORS_V_R8:
			{
 				orsArray<ors_float64> values;

				pProperty->getAttr(name.c_str(), values);
				
				for (int j=0;j<values.size();j++)
					setAttr( name, values[j], j );
			}
			break;
		case ORS_V_STR:
			{
				orsArray<ors_string> values;

				pProperty->getAttr(name.c_str(), values);
				
				for (int j=0;j<values.size();j++)
					setAttr( name, values[j].c_str(), j );
			}
			break;
 		case ORS_V_BLOB:
 			{
 				orsArray<const ors_byte *> values;
 				orsArray<ors_int32> nLengths;

 				pProperty->getAttr(name.c_str(), values,nLengths);

 				for (int j=0;j<values.size();j++)
 				{
					setAttr( name, values[j], nLengths[j], j );
 				}
 			}
 			break;
 		case ORS_V_CHILD:
			{
				ref_ptr<orsIProperty> theProp;
				ref_ptr<orsIProperty> inProp;

				pProperty->getAttr( name.c_str(), inProp );

				if( inProp.get() )	{
					getAttr( name.c_str(), theProp );
					theProp->update( inProp.get() );
				}
			}
			break;
		case ORS_V_MATRIX:
			{
				orsMatrixD value;
				
				if( pProperty->getAttr(name.c_str(), value ) )
					setAttr( name, value );
			}
			break;
		case ORS_V_VECTOR:
			{
				orsVectorD value;
				
				if( pProperty->getAttr(name.c_str(), value ) )
					setAttr( name, value );
			}
			break;
		case ORS_V_OBJECT:
			{
				orsIObject *pObj;
					
				pProperty->getAttr( name.c_str(), pObj );
							
				setAttr( name, pObj );
			}
			break;
		case ORS_V_LISTNER:
// 			{
// 				orsPropertyListener *pListner;
// 				
// 				pProperty->getAttr(name.c_str(), pListner );
// 				
// 				setAttr( name, pListner );
// 			}
			break;
			
		default:
			assert( false ) ;
		}
	}

}



//////////////////////////////////////////////////////////////////////////
void orsXProperty::getAttributeInfo( ors_uint32 index, ors_string &name, orsVariantType &type,
									ors_int32 &numOfValues, bool bSort ) const
{
	assert( index < size() && index >=0 );

	if( !bSort )	{
		name = m_attrNames[index].c_str();
		
		_recordConstIterator iter = m_records.find( name.c_str() ) ;
		if( iter != m_records.end() ) {
			orsVariant* var = iter->second->GetVariant( 0 );
			for (int i=0;i<iter->second->size();i++)
			{
				orsVariant* vv = iter->second->GetVariant(i);
				int ee;
				ee = 3;
			}
			type = var->type;
			numOfValues = iter->second->size();
		}
		else
			assert( false );
	}
	else	{
		_recordConstIterator iter = m_records.begin();
		 
		for( int i=0; i<index; i++)
			iter ++;
		 
		name = iter->first.c_str();
		
		orsVariant* var = iter->second->GetVariant( 0 );
		
		type = var->type;
		
		numOfValues = iter->second->size();
	}
}


orsVariantType orsXProperty::getAttrType( const orsChar *attrName ) const
{
	_recordConstIterator iter = m_records.find( attrName ) ;
	
	if( iter != m_records.end() ) {
		orsVariant* var = iter->second->GetVariant( 0 );
		return var->type;
	}

	return ORS_V_UNKOWN;
}
bool orsXProperty::findAttrName(const orsChar * name)//判断是否存在节点name
{
	stdString name0 = ToRegularName( name );
	_recordConstIterator it = m_records.find( name ) ;
	if( it != m_records.end() ) return true;
	else return false;
}

#include "orsBase/orsString.h"
orsIProperty *orsXProperty::findNode( const orsChar *nodePath, orsIProperty *&pParent )
{
	assert( nodePath[0] == '/' );	//&& nodePath[1] == '/'  );

	if( nodePath[1] == '/' )
		nodePath += strlen( "//" );
	else
		nodePath += strlen( "/" );

	orsIProperty *pNode = this;

	ref_ptr<orsIProperty> pNodeMatch = NULL;

	do 	{
		orsString node = nodePath;
		
		int pos = node.find( '/' );
		orsString key;

		if( -1 == pos )		// 最后一个了
			key = node;
		else
			key = node.left(pos);
		
		nodePath += pos + 1;

		if( *nodePath == '/' )
			nodePath++;
		
		pParent = pNodeMatch.get();
		pNodeMatch = NULL;

		int i;
		for( i=0; i< pNode->size(); i++ )
		{
			orsString name;
			orsVariantType type;
			ors_int32 numOfValues = 0;
			
			pNode->getAttributeInfo( i, name, type, numOfValues );
			
			if( name == key ) {
				if( ORS_V_CHILD == type )
					pNode->getAttr( name, pNodeMatch );

				break;
			}
		}

		if( NULL == pNodeMatch.get() )
			return NULL;

		pNode = pNodeMatch.get();

		if( -1 == pos || 0 == nodePath[0] )
			break;

	} while (1);

	return pNodeMatch.get();
}
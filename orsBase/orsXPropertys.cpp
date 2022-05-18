/********************************************************************
	created:	2008/09/24
	created:	24:9:2008   0:26
	filename: 	G:\OpenRS\SVN\openRS\desktop\src\orsBase\orsPropertysImp.cpp
	file path:	G:\OpenRS\SVN\openRS\desktop\src\orsBase
	file base:	orsPropertysImp
	file ext:	cpp
	author:		Guo Wei
	
	purpose:	<Key,Value>属性实现
*********************************************************************/

#include "stdafx.h"

#include "orsXPropertys.h"

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
		return (const orsChar *)((blobDATA *)pData)->data;	
	};
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
	int len = strlen(str);
	blobDATA *blob = alloc( len + 1 );
	
	if( blob )	{
		memcpy( blob->data, str, len );
		blob->data[len] = 0;
		return true;
	}

	return false;
}



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
	default:
		assert( false );
	}
}


orsVariant::~orsVariant()
{
	switch( type )	{
	case ORS_V_CHILD :
		vProperty->release();
		break;
	case ORS_V_STR:
	case ORS_V_BLOB:
		delete vBlob.pData;	
	}
}

void orsVariant::set( ref_ptr<orsIProperty> property )
{
	if( vProperty )
		vProperty->release();
		
	vProperty = property.get();
	vProperty->addRef();
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
	for( size_t i=0; i<size();i++) 
	{
		delete GetVariant( i );
	}
}

////////////////////////////////////orsXProperty/////////////////////////////

typedef orsPropertyRecordMap::value_type		_recordType;
typedef orsPropertyRecordMap::iterator			_recordIterator;
typedef orsPropertyRecordMap::const_iterator	_recordConstIterator;

orsXProperty::orsXProperty():m_listener(NULL),m_parent(NULL)
{
}

orsXProperty::~orsXProperty()
{
	_recordIterator iter;

	for(iter = m_records.begin();iter != m_records.end();iter++)
	{
		orsPropertyRecord *pRecord = iter->second;

		delete pRecord;
	}
}


void orsXProperty::addAttribute(const orsChar * name,ors_int16 value)
{
	_recordIterator pointer = m_records.find(name);
	orsPropertyRecord *pRecord = NULL;
	
	if(pointer == m_records.end()){
		pRecord = new orsPropertyRecord;
		m_records.insert(_recordType(name, pRecord));
	} else{ 
		pRecord = pointer->second;
	}
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_I2;
	var->set( value );
	
	pRecord->AddVariant(var);
}

void orsXProperty::addAttribute(const orsChar * name, ors_int32 value)
{
	_recordIterator pointer = m_records.find(name);

	orsPropertyRecord *pRecord = NULL;

	if( pointer == m_records.end() )
	{
		pRecord = new orsPropertyRecord;
		m_records.insert( _recordType( name, pRecord) );
	} 
	else	{ 
		pRecord = pointer->second;
	}

	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_I4;
	var->set( value );

	pRecord->AddVariant( var );
}



void 
orsXProperty::addAttribute(const orsChar * name, ors_float64 value)
{
	_recordIterator pointer = m_records.find(name);
	orsPropertyRecord *pRecord = NULL;
	
	if(pointer == m_records.end()){
		pRecord = new orsPropertyRecord;
		m_records.insert(_recordType(name, pRecord));
	} else
		pRecord = pointer->second;
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;
	
	var->type = ORS_V_R8;
	var->set( value );
	
	pRecord->AddVariant( var );
}

void orsXProperty::addAttribute(const orsChar * name, const orsChar * value)
{
	_recordIterator pointer = m_records.find(name);
	orsPropertyRecord *pRecord = NULL;

	if(pointer == m_records.end()) {
		pRecord = new orsPropertyRecord;
		m_records.insert(_recordType(name, pRecord));
	} else{ 
		pRecord = pointer->second;
	}

	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_STR;
	var->set( value );

	pRecord->AddVariant(var);
}

void orsXProperty::addAttribute(const orsChar * name, const ors_byte *pValue, ors_int32 nLength)
{
	_recordIterator pointer = m_records.find(name);
	orsPropertyRecord *pRecord = NULL;

	if(pointer == m_records.end()){
		pRecord = new orsPropertyRecord;
		m_records.insert(_recordType(name, pRecord));
	} else{ 
		pRecord = pointer->second;
	}

	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_BLOB;
	var->set( pValue, nLength );

	pRecord->AddVariant(var);
}


void orsXProperty::addAttribute(const orsChar * name, ref_ptr<orsIProperty> value)
{
	
	_recordIterator pointer = m_records.find(name);

	orsPropertyRecord *pRecord = NULL;

	if(pointer == m_records.end()){
		pRecord = new orsPropertyRecord;
		m_records.insert(_recordType(name, pRecord));
	} else
		pRecord = pointer->second;
	
	orsVariant	*var = new orsVariant;
	//orsVariant	*var = pRecord;

	var->type = ORS_V_CHILD ;
	var->set( value );
	
	pRecord->AddVariant( var );
}

//////////////////////////////////////////////////////////////////////////

bool orsXProperty::setAttribute(const orsChar * name, ors_int32 value, ors_int32 index)
{
	_recordIterator iter;
	iter = m_records.find(name);

	if(iter == m_records.end())
		return false;
	
	orsPropertyRecord* pRecord = iter->second;
	
	if(index < 0 || index >= pRecord->size())
		return false;
	
	orsVariant *var = pRecord->GetVariant( index );

	if( var->type == ORS_V_I4 )
	{
		var->set( value );
	}else if(var->type == ORS_V_R8)
	{
		var->set((ors_float64)value);
	}else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}

bool orsXProperty::setAttribute(const orsChar * name, ors_int16 value, ors_int32 index)
{
	_recordIterator iter;
	iter = m_records.find(name);

	if(iter == m_records.end())
		return false;
	
	orsPropertyRecord* pRecord = iter->second;
	
	if(index < 0 || index >= pRecord->size())
		return false;
	
	orsVariant *var = pRecord->GetVariant(index);

	if( var->type == ORS_V_I2 )
	{
		var->set( value );
	}
	else if(var->type == ORS_V_I4 )
	{
		var->set( (ors_int32)value );
	}
	else if(var->type == ORS_V_R8 )
	{
		var->set( (ors_float64)value );
	}
	else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}

bool orsXProperty::setAttribute(const orsChar * name, const orsChar * value, ors_int32 index)
{
	_recordIterator iter;

	iter = m_records.find(name);

	if( iter == m_records.end() )
		return false;
	
	orsPropertyRecord* pRecord = iter->second;
	
	if( index < 0 || index >= pRecord->size() )
		return false;
	
	orsVariant *var = pRecord->GetVariant(index);

	if(var->type == ORS_V_STR)
		var->set( value );
	else
		return false;
	
	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}

bool orsXProperty::setAttribute(const orsChar * name, ors_float64 value, ors_int32 index)
{
	_recordIterator iter;
	iter = m_records.find(name);

	if(iter == m_records.end())
		return false;
	
	orsPropertyRecord* pRecord = iter->second;
	
	if(index < 0 || index >= pRecord->size())
		return false;
	
	orsVariant *var = pRecord->GetVariant(index);
	
	if(var->type == ORS_V_R8)
	{
		var->set( value );
	}
	else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}

bool orsXProperty::setAttribute(const orsChar * name, const ors_byte* pValue, ors_int32 nLength,	
								ors_int32 index)
{
	_recordIterator iter;
	iter = m_records.find(name);

	if(iter == m_records.end())
		return false;
	
	orsPropertyRecord *pRecord = iter->second;
	
	if(index < 0 || index >= pRecord->size())
		return false;
	
	orsVariant *var = pRecord->GetVariant(index);

	if( var->type == ORS_V_BLOB )
	{
		var->set( pValue, nLength );
	}
	else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);	

	return true;
}

bool orsXProperty::setAttribute(const orsChar * name, ref_ptr<orsIProperty> value, ors_int32 index)
{
	_recordIterator iter;

	iter = m_records.find(name);

	if( iter == m_records.end() )
		return false;
	
	orsPropertyRecord *pRecord = iter->second;
	
	if( index < 0 || index >= pRecord->size() )
		return false;
	
	orsVariant *var = pRecord->GetVariant( index );

	if( var->type == ORS_V_CHILD )
	{
		var->set( value );
	}
	else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}
	

//////////////////////////////////////////////////////////////////////////

bool orsXProperty::getAttribute(const orsChar * name, orsArray<ors_int16> &values) const
{
	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;
	
	if (it != m_records.end()) {
		pRecord = it->second ;
		for (size_t i = 0 ; i < pRecord->size() ; i++) {
			switch( pRecord->GetVariant(i)->type )
			{
			case ORS_V_I2:
				values.push_back( pRecord->GetVariant(i)->getInt16() );
				break;
// 			case ORS_V_I4:
// 				values.push_back( pRecord->GetVariant(i)->getInt32() );
//				break;
			default:
				return false;
				break;
			}
		}
		return true ;
	}
	return false ;
}


bool orsXProperty::getAttribute(const orsChar * name, orsArray<ors_int32> &values) const
{
	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;
	
	if (it != m_records.end()) {
		pRecord = it->second ;
		for (size_t i = 0 ; i < pRecord->size() ; i++) {
			switch(pRecord->GetVariant(i)->type)
			{
			case ORS_V_I2:
				values.push_back(pRecord->GetVariant(i)->getInt16()) ;
				break;
			case ORS_V_I4:
				values.push_back(pRecord->GetVariant(i)->getInt32()) ;
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
orsXProperty::getAttribute(const orsChar * name,orsArray<ors_float64> &values) const
{
	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;
	
	if (it != m_records.end()) {
		pRecord = it->second ;
		for (size_t i = 0 ; i < pRecord->size() ; i++) 
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
			default:
				return false;
				break;
			}
		}
		return true ;
	}
	return false ;
}

bool orsXProperty::getAttribute(const orsChar * name, orsArray<const ors_byte *> &values, orsArray<ors_int32> &vLength) const
{
	orsPropertyRecord * pRecord;
	_recordConstIterator it = m_records.find(name) ;

	if (it != m_records.end()) {
		pRecord = it->second ;
		for (size_t i = 0 ; i < pRecord->size() ; i++) 
		{
			if( ORS_V_BLOB == pRecord->GetVariant(i)->type ) {
				ors_int32 len;
	
				values.push_back( pRecord->GetVariant(i)->getBlob( len ) ) ;
				vLength.push_back( len );
			}
		}
		return true ;
	}
	return false ;
}

bool orsXProperty::getAttribute(const orsChar * name, orsArray<ors_string> &values) const
{
	orsPropertyRecord* pRecord;
	
	_recordConstIterator it = m_records.find( name ) ;
	
	if( it != m_records.end() ) {
		pRecord = it->second ;
		for (size_t i = 0 ; i < pRecord->size() ; i++) 
		{
			switch( pRecord->GetVariant(i)->type ) {
			case ORS_V_STR:
				values.push_back( pRecord->GetVariant(i)->getString() );
				break;
// 			case ORS_V_BLOB:
// 				{
// 					orsVariant* var = pRecord->GetVariant( i ); 
// 
// 					ors_int32 len;
// 					const ors_byte *buf = var->getBlob( len );
// 
// 					char * temp_str = new char[len+1];
// 					memcpy( temp_str, buf, len );
// 
// 					temp_str[len] = '\0';
// 
// 					values.push_back( temp_str );
// 
// 					delete temp_str;
//				}
			default:
				return false;
			}
		}
		return true ;
	}
	return false ;
}

bool 
orsXProperty::getAttribute(const orsChar * name,orsArray< ref_ptr<orsIProperty> > &values ) const
{
	orsPropertyRecord* pRecord;
	_recordConstIterator it = m_records.find(name) ;

	if (it != m_records.end()) {
		pRecord = it->second ;
		for (size_t i = 0 ; i < pRecord->size() ; i++) 
		{
			if( ORS_V_CHILD == pRecord->GetVariant(i)->type )  
				values.push_back( pRecord->GetVariant(i)->getProperty() );
		}
		return true ;
	}
	return false ;
}



/////////////////////////////// 单接口: 复接口的特例 /////////////////////////////////////

bool orsXProperty::getAttribute( const orsChar * name, ors_float64 &value) const
{
	orsArray<ors_float64> v;
	if (getAttribute(name,v)) {
		if (!v.size()) return false ;
		value = v[0] ;
		return true ;
	}
	return false ;
}


bool orsXProperty::getAttribute(const orsChar * name, ors_int32 &value) const
{
	orsArray<ors_int32> v;

	if( getAttribute(name,v) ) {
		if( 0 == v.size() ) 
			return false ;

		value = (ors_int32)v[0] ;
		return true ;
	}
	return false ;
}

bool orsXProperty::getAttribute(const orsChar * name, ors_int16 & value )  const
{
	orsArray<ors_int16> v;

	if( getAttribute(name,v) ) {
		if( 0 == v.size() ) 
			return false ;

		value = (ors_int16)v[0] ;
		return true ;
	}
	return false ;
}


bool orsXProperty::getAttribute( const orsChar * name, ors_string &value )  const
{
	orsPropertyRecord* pRecord;
	
	_recordConstIterator it = m_records.find( name ) ;
	
	if( it != m_records.end() ) {
		pRecord = it->second ;

		switch( pRecord->GetVariant(0)->type ) {
		case ORS_V_STR:
			value = pRecord->GetVariant(0)->getString();
			break;
// 		case ORS_V_BLOB:
// 			{
// 				orsVariant* var = pRecord->GetVariant( 0 ); 
// 				
// 				ors_int32 len;
// 				const ors_byte *buf = var->getBlob( len );
// 				
// 				char * temp_str = new char[len+1];
// 				memcpy( temp_str, buf, len );
// 				
// 				temp_str[len] = '\0';
// 				
// 				value = temp_str;
// 				
// 				delete temp_str;
//			}
		default:
			return false;
		}

		return true;
	}
	return false ;
}


bool orsXProperty::getAttribute(const orsChar * name, const ors_byte * &value, ors_int32 &nLength)  const
{
	orsArray<const ors_byte *> vChar;
	orsArray<ors_int32> vLong ;

	if( getAttribute( name, vChar, vLong) )
	{
		if( 0 == vChar.size() ) 
			return false ;

		value = vChar[0] ;
		nLength = vLong[0] ;

		return true;
	} 
	return false;
}

bool orsXProperty::getAttribute(const orsChar * name, ref_ptr<orsIProperty> &value) const
{
	orsArray< ref_ptr<orsIProperty> > v ;
	if( getAttribute(name, v) ) {
		if (!v.size()) return false ;
		value = v[0];
		return true ;
	}
	return false ;
}

//////////////////////////////////////////////////////////////////////////


void orsXProperty::getAttributeInfo( ors_int32 index, ors_string &name, orsVariantType &type,
									ors_int32 &numOfValues ) const
{
	assert( index < size() && index >=0 );
	
	_recordConstIterator iter = m_records.begin();
	
	for( int i=0; i<index; i++)
		iter++;
		
	name = iter->first.c_str();
	
	orsVariant* var = iter->second->GetVariant( 0 );
	
	type = var->type;

	numOfValues = iter->second->size();
}
/********************************************************************
	created:	2008/09/24
	created:	24:9:2008   0:26
	filename: 	G:\OpenRS\SVN\openRS\desktop\src\orsBase\orsPropertysImp.cpp
	file path:	G:\OpenRS\SVN\openRS\desktop\src\orsBase
	file base:	orsPropertysImp
	file ext:	cpp
	author:		Guo Wei
	
	purpose:	<Key,Value> Ù–‘ µœ÷
*********************************************************************/

#include "stdafx.h"

#include "orsXPropertys.h"

typedef std::map<std::string,orsArray<orsVariant*>*> CNetObjectMap;
typedef std::map<std::string,orsArray<orsVariant*>*>::value_type mapElement ;
typedef std::map<std::string,orsArray<orsVariant*>*>::iterator mapItr;

orsVariant::orsVariant(){
	type = ORS_V_UNKOWN;
	l_value = 0;
	i_value = 0;
	d_value = 0;
	b_value_value = NULL;
	b_value_length = 0;
	child_value = NULL;
}

orsVariant::orsVariant (orsVariant & var) {
	
	type = var.type ;
	switch (var.type) {
	case ORS_V_I2 :
		i_value = var.i_value ; 
		break;
	case ORS_V_I4 :
		l_value = var.l_value ;
		break ;
	case ORS_V_R8 :
		d_value = var.d_value ;
		break ;
	case ORS_V_BSTR :
		s_value = var.s_value ;
		break ;
	case ORS_V_BLOB :
		b_value_length = var.b_value_length ;
		b_value_value = new ors_byte [b_value_length] ;
		memcpy (b_value_value, var.b_value_value, b_value_length) ;
		break ;
	case ORS_V_RECORD :
		child_value = var.child_value;	
		break ;
	default:
		break ;
	}
}


orsVariant::~orsVariant(){
	if(ORS_V_BLOB == type && b_value_value != NULL) {
		delete [] b_value_value ;
	}
}

////////////////////////////////////orsXProperty/////////////////////////////
orsXProperty::orsXProperty():m_listener(NULL){
	
}

orsXProperty::~orsXProperty(){
	CNetObjectMap::iterator iter;
	for(iter = m_values.begin();iter != m_values.end();iter++){
		orsArray<orsVariant*>* var = iter->second;
		for(size_t i=0;i<var->size();i++){
			delete (*var)[i];
		}
		delete var;
	}
}

void 
orsXProperty::addAttribute(ors_string name,ors_int32 value){
	CNetObjectMap::iterator pointer = m_values.find(name.c_str());
	orsArray< orsVariant*> *attr = NULL;
	if(pointer == m_values.end()){
		attr = new orsArray<orsVariant*>;
		m_values.insert(CNetObjectMap::value_type(name.c_str(),attr));
	} else{ 
		attr = pointer->second;
	}
	orsVariant* var = new orsVariant;
	var->type = ORS_V_I4;
	var->l_value= value ;
	attr->push_back(var);
}

void 
orsXProperty::addAttribute(ors_string name,ors_int16 value)
{
	CNetObjectMap::iterator pointer = m_values.find(name.c_str());
	orsArray< orsVariant*> *attr = NULL;
	if(pointer == m_values.end()){
		attr = new orsArray<orsVariant*>;
		m_values.insert(CNetObjectMap::value_type(name.c_str(),attr));
	} else{ 
		attr = pointer->second;
	}
	orsVariant* var = new orsVariant;
	var->type = ORS_V_I2;
	var->i_value = value ;
	attr->push_back(var);
}

void 
orsXProperty::addAttribute(ors_string name,ors_char* value)
{
	addAttribute(name,ors_string(value));
}

void 
orsXProperty::addAttribute(ors_string name,ors_string value){
	CNetObjectMap::iterator pointer = m_values.find(name.c_str());
	orsArray< orsVariant*> *attr = NULL;
	if(pointer == m_values.end()){
		attr = new orsArray< orsVariant*>;
		m_values.insert(CNetObjectMap::value_type(name.c_str(),attr));
	} else{ 
		attr = pointer->second;
	}
	orsVariant* var = new orsVariant;
	var->type = ORS_V_BSTR;
	var->s_value = value ;
	attr->push_back(var);
}

void 
orsXProperty::addAttribute(ors_string name,ors_byte *pValue,ors_int32 nLength){
	CNetObjectMap::iterator pointer = m_values.find(name.c_str());
	orsArray< orsVariant*> *attr = NULL;
	if(pointer == m_values.end()){
		attr = new orsArray< orsVariant*>;
		m_values.insert(CNetObjectMap::value_type(name.c_str(),attr));
	} else{ 
		attr = pointer->second;
	}
	orsVariant* var = new orsVariant;
	var->type = ORS_V_BLOB;
	var->b_value_value = new ors_byte [nLength] ;
	memcpy (var->b_value_value, pValue, nLength) ;
	var->b_value_length = nLength ;
	attr->push_back(var);
}

void 
orsXProperty::addAttribute(ors_string name,ors_float64 value){	
	CNetObjectMap::iterator pointer = m_values.find(name.c_str());
	orsArray< orsVariant*> *attr = NULL;
	if(pointer == m_values.end()){
		attr = new orsArray< orsVariant*>;
		m_values.insert(CNetObjectMap::value_type(name.c_str(),attr));
	} else{ 
		attr = pointer->second;
	}
	orsVariant* var = new orsVariant;
	var->type = ORS_V_R8;
	var->d_value = value ;
	attr->push_back(var);
}

void 
orsXProperty::addAttribute(ors_string name,ref_ptr<orsIProperty> value){
	CNetObjectMap::iterator pointer = m_values.find(name.c_str());
	orsArray< orsVariant*> *attr = NULL;
	if(pointer == m_values.end()){
		attr = new orsArray<orsVariant*>;
		m_values.insert(CNetObjectMap::value_type(name.c_str(),attr));
	} else{ 
		attr = pointer->second;
	}
	orsVariant* var = new orsVariant;
	var->type = ORS_V_RECORD ;
	var->child_value = value;
	attr->push_back(var);
}

bool 
orsXProperty::getAttribute(ors_string name,ors_string &value) 
{
	orsArray<orsString> v;
	if (getAttribute(name,v)){
		if (!v.size()) return false ;
		value = v[0] ;
		return true ;
	}
	return false ;
}

bool 
orsXProperty::getAttribute(ors_string name,ors_byte * &value,ors_int32 &nLength){
	orsArray<ors_byte *> vChar ;
	orsArray<ors_int32> vLong ;
	if (getAttribute(name,vChar,vLong)){
		if (!vChar.size()) return false ;
		value = vChar[0] ;
		nLength = vLong[0] ;
		return true;
	} 
	return false;
}

bool 
orsXProperty::getAttribute(ors_string name,ors_float64 &value){
	orsArray<ors_float64> v;
	if (getAttribute(name,v)) {
		if (!v.size()) return false ;
		value = v[0] ;
		return true ;
	}
	return false ;
}


bool 
orsXProperty::getAttribute(ors_string name,ors_int32 &value){
	orsArray<double> v;
	if (getAttribute(name,v)) {
		if (!v.size()) return false ;
		value = (ors_int32)v[0] ;
		return true ;
	}
	return false ;
}


void orsXProperty::getAttribute(ors_string name,ors_int16 & value)
{
	ors_int32 valtemp = 0;
	getAttribute(name,valtemp);
	value =(ors_int16)valtemp;
}

bool 
orsXProperty::getAttribute(ors_string name,orsArray<ors_string> &value)
{
	orsArray<orsVariant*>* vData;	
	CNetObjectMap::iterator it = m_values.find(name.c_str()) ;

	if (it != m_values.end()) {
		vData = it->second ;
		for (size_t i = 0 ; i < vData->size() ; i++) {
			if ((*vData)[i]->type == ORS_V_BSTR){
				value.push_back((*vData)[i]->s_value) ;
			}else if((*vData)[i]->type == ORS_V_BLOB){
				orsVariant* var = (*vData)[i]; 
				char * temp_str = new char[var->b_value_length+1];
				memcpy(temp_str,var->b_value_value,var->b_value_length);
				temp_str[var->b_value_length] = '\0';
				value.push_back(temp_str);
				delete temp_str;
			}else 
				return false;
		}
		return true ;
	}
	return false ;
}

bool 
orsXProperty::getAttribute(ors_string name,orsArray<ors_byte *> &value,orsArray<ors_int32> &nLength){
	orsArray<orsVariant*>* vData;
	CNetObjectMap::iterator it = m_values.find(name.c_str()) ;

	if (it != m_values.end()) {
		vData = it->second ;
		for (size_t i = 0 ; i < vData->size() ; i++) {
			if ((*vData)[i]->type != ORS_V_BLOB) return false ;
			value.push_back((*vData)[i]->b_value_value) ;
			nLength.push_back((*vData)[i]->b_value_length) ;
		}
		return true ;
	}
	return false ;
}

bool 
orsXProperty::getAttribute(ors_string name,orsArray<ors_float64> &value){
	orsArray<orsVariant*>* vData;
	CNetObjectMap::iterator it = m_values.find(name.c_str()) ;

	if (it != m_values.end()) {
		vData = it->second ;
		for (size_t i = 0 ; i < vData->size() ; i++) {
			switch((*vData)[i]->type)
			{
			case ORS_V_R8:
				value.push_back((*vData)[i]->d_value) ;
				break;
			case ORS_V_I2:
				value.push_back((*vData)[i]->i_value) ;
				break;
			case ORS_V_I4:
				value.push_back((*vData)[i]->l_value) ;
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

bool orsXProperty::getAttribute(ors_string name,	orsArray<ors_int16> &value)
{
	orsArray<orsVariant*>* vData;
	CNetObjectMap::iterator it = m_values.find(name.c_str()) ;

	if (it != m_values.end()) {
		vData = it->second ;
		for (size_t i = 0 ; i < vData->size() ; i++) {
			switch((*vData)[i]->type)
			{
			case ORS_V_I2:
				value.push_back((*vData)[i]->i_value) ;
				break;
			case ORS_V_I4:
				value.push_back((*vData)[i]->l_value) ;
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

bool orsXProperty::getAttribute(ors_string name,	orsArray<ors_int32> &value)
{
	orsArray<orsVariant*>* vData;
	CNetObjectMap::iterator it = m_values.find(name.c_str()) ;

	if (it != m_values.end()) {
		vData = it->second ;
		for (size_t i = 0 ; i < vData->size() ; i++) {
			switch((*vData)[i]->type)
			{
			case ORS_V_I4:
				value.push_back((*vData)[i]->l_value) ;
				break;
			case ORS_V_I2:
				value.push_back((*vData)[i]->i_value) ;
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
orsXProperty::getAttribute(ors_string name,ref_ptr<orsIProperty> &value){
	orsArray< ref_ptr<orsIProperty> > v ;
	if (getAttribute(name,v)) {
		if (!v.size()) return false ;
		value = v[0];
		return true ;
	}
	return false ;
}

bool 
orsXProperty::getAttribute(ors_string name,orsArray< ref_ptr<orsIProperty> > &value){
	orsArray<orsVariant*>* vData;
	CNetObjectMap::iterator it = m_values.find(name.c_str()) ;

	if (it != m_values.end()) {
		vData = it->second ;
		for (size_t i = 0 ; i < vData->size() ; i++) {
			if ((*vData)[i]->type != ORS_V_RECORD) return false ;
			value.push_back((*vData)[i]->child_value) ;
		}
		return true ;
	}
	return false ;
}

ors_uint32 orsXProperty::size()
{
	return m_values.size();
}	

void orsXProperty::getAttributeInfo(ors_int32 ind,ors_string &name,orsVariantType &type,
		ors_int32 &valuenum)
{
	assert(ind < size() && ind >=0);

	std::map<std::string,orsArray<orsVariant*>* >::iterator iter;
	iter = m_values.begin();

	for(int i=0;i<ind;i++)
		iter++;
	name = iter->first.c_str();
	orsVariant* var = (*iter->second)[0];
	type = var->type;
	valuenum = iter->second->size();
}

bool orsXProperty::setAttribute(ors_string name,	ors_int32 value, ors_int32 index)
{
	std::map<std::string,orsArray<orsVariant*>* >::iterator iter;
	iter = m_values.find(name.c_str());

	if(iter == m_values.end())
		return false;
	
	orsArray<orsVariant*>* vars = iter->second;
	
	if(index < 0 || index >= vars->size())
		return false;
	
	orsVariant *var = (*vars)[index];
	if(var->type == ORS_V_I2)
	{
		var->i_value = value;
	}else if(var->type == ORS_V_I4)
	{
		var->l_value = value;
	}else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}

bool orsXProperty::setAttribute(ors_string name,	ors_int16 value, ors_int32 index)
{
	std::map<std::string,orsArray<orsVariant*>* >::iterator iter;
	iter = m_values.find(name.c_str());

	if(iter == m_values.end())
		return false;
	
	orsArray<orsVariant*>* vars = iter->second;
	
	if(index < 0 || index >= vars->size())
		return false;
	
	orsVariant *var = (*vars)[index];
	if(var->type == ORS_V_I2)
	{
		var->i_value = value;
	}else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}

bool orsXProperty::setAttribute(ors_string name,	ors_string &value, ors_int32 index)
{
	std::map<std::string,orsArray<orsVariant*>* >::iterator iter;
	iter = m_values.find(name.c_str());

	if(iter == m_values.end())
		return false;
	
	orsArray<orsVariant*>* vars = iter->second;
	
	if(index < 0 || index >= vars->size())
		return false;
	
	orsVariant *var = (*vars)[index];
	if(var->type == ORS_V_BSTR)
	{
		var->s_value = value;
	}else
		return false;
	
	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}

bool orsXProperty::setAttribute(ors_string name,	ors_float64 value, ors_int32 index)
{
	std::map<std::string,orsArray<orsVariant*>* >::iterator iter;
	iter = m_values.find(name.c_str());

	if(iter == m_values.end())
		return false;
	
	orsArray<orsVariant*>* vars = iter->second;
	
	if(index < 0 || index >= vars->size())
		return false;
	
	orsVariant *var = (*vars)[index];
	if(var->type == ORS_V_I2)
	{
		var->i_value = value;
	}else if(var->type == ORS_V_I4)
	{
		var->l_value = value;
	}else if(var->type == ORS_V_R8)
	{
		var->d_value = value;
	}else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}

bool orsXProperty::setAttribute(ors_string name,	ors_byte* pValue, ors_int32 nLength,
								   ors_int32 index)
{
	std::map<std::string,orsArray<orsVariant*>* >::iterator iter;
	iter = m_values.find(name.c_str());

	if(iter == m_values.end())
		return false;
	
	orsArray<orsVariant*>* vars = iter->second;
	
	if(index < 0 || index >= vars->size())
		return false;
	
	orsVariant *var = (*vars)[index];
	if(var->type == ORS_V_BLOB)
	{
		if(var->b_value_value != NULL)
			delete var->b_value_value;
		if(nLength == 0)
		{
			var->b_value_value = NULL;
			var->b_value_length = nLength;
		}else
		{
			var->b_value_value = new ors_byte[nLength];
			memcpy(var->b_value_value,pValue,nLength);
			var->b_value_length = nLength;
		}
	}else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);	

	return true;
}

bool orsXProperty::setAttribute(ors_string name,	ref_ptr<orsIProperty> value, ors_int32 index)
{
	std::map<std::string,orsArray<orsVariant*>* >::iterator iter;
	iter = m_values.find(name.c_str());

	if(iter == m_values.end())
		return false;
	
	orsArray<orsVariant*>* vars = iter->second;
	
	if(index < 0 || index >= vars->size())
		return false;
	
	orsVariant *var = (*vars)[index];
	if(var->type == ORS_V_RECORD)
	{
		var->child_value = value;
	}else
		return false;

	if(m_listener != NULL)
		m_listener->propertyChange(name,index);

	return true;
}
	

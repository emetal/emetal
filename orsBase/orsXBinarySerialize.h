#ifndef ORS_BINARY_SERIALIZE_IMP_H
#define ORS_BINARY_SERIALIZE_IMP_H
#include <string>

#include "orsBase/orsIProperty.h"

#include "orsBase/orsIPlatform.h"

#include "orsMessageStream.h"

class ORS_BASE_API orsXBinarySerialize :
		public orsIPropertySerialize, public orsObjectBase
{
public:
	virtual bool import( const char *bytes,long length, orsIProperty *info);
	virtual bool outport( char *&bytes, long *length, const orsIProperty *info);

public:
	long getLength( const orsIProperty *info);

	//接口宏定义
	ORS_OBJECT_IMP1(orsXBinarySerialize, orsIPropertySerialize, _T("binary"), _T("BinarySerializer" ))
};

#endif

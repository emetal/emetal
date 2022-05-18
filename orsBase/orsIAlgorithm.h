#ifndef _ORS_ALGORITHM_HEADER
#define _ORS_ALGORITHM_HEADER

#include "orsIObject.h"
#include "orsIProperty.h"


//
// 算法对象接口，目前只起到分类的作用
//
interface orsIAlgorithm: public orsIObject
{
public:
	ORS_INTERFACE_DEF( orsIObject, _T("algorithm" ))
};

#endif

#ifndef _ORS_ALGORITHM_HEADER
#define _ORS_ALGORITHM_HEADER

#include "orsIObject.h"
#include "orsIProperty.h"


//
// �㷨����ӿڣ�Ŀǰֻ�𵽷��������
//
interface orsIAlgorithm: public orsIObject
{
public:
	ORS_INTERFACE_DEF( orsIObject, _T("algorithm" ))
};

#endif

#ifndef OPENRS_COMPLEX_H
#define OPENRS_COMPLEX_H

#include <math.h>

#include "orsBase/orsTypedef.h"

#include <complex>

#define orsComplex std::complex

typedef orsComplex<ors_int16>	ors_cint16;
typedef orsComplex<ors_int32>	ors_cint32;
typedef orsComplex<ors_float32>	ors_cfloat32;
typedef orsComplex<ors_float64>	ors_cfloat64;

namespace ORS 
{
	typedef ors_cint16		cint16;
	typedef ors_cint32		cint32;
	typedef ors_cfloat32	cfloat32;
	typedef ors_cfloat64	cfloat64;
	
} // end namespace ORS


#endif
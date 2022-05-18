#ifndef ORS_CONSTANTS_H
#define ORS_CONSTANTS_H

#ifndef M_PI
#  define M_PI             ((ors_float64)3.141592653589793238462643)
#endif
#ifndef TWO_PI
#  define TWO_PI           ((ors_float64)(2.0 * M_PI))
#endif
#ifndef DEG_PER_RAD
#  define DEG_PER_RAD      ((ors_float64)(180.0/M_PI))
#endif
#ifndef SEC_PER_RAD
#  define SEC_PER_RAD      ((ors_float64)206264.8062471)
#endif
#ifndef RAD_PER_DEG
#  define RAD_PER_DEG      ((ors_float64)(M_PI/180.0))
#endif
#ifndef MTRS_PER_FT
#  define MTRS_PER_FT      ((ors_float64)0.3048)
#endif
#ifndef FT_PER_MTRS
#  define FT_PER_MTRS      ((ors_float64)3.280839895013122)
#endif
#ifndef FT_PER_MILE
#  define FT_PER_MILE      ((ors_float64)5280.0)
#endif
#ifndef SQRT_TWO_PI
#  define SQRT_TWO_PI      ((ors_float64)2.50662827463100050242)
#endif
#ifndef SQRT_TWO_PI_DIV2
#  define SQRT_TWO_PI_DIV2 ((ors_float64)7.07106781186547524401E-1)
#endif

#endif
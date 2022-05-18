#ifndef _ORS_BASE_TYPEDEF_H
#define _ORS_BASE_TYPEDEF_H

#include <stdlib.h>
#include <math.h>

// #include "orsString.h"
// #include "orsArray.h"
//#include "orsComplex.h"

#ifdef WIN32
#include <tchar.h>
#else
	#if defined(_UNICODE) || defined(UNICODE)
		#include <wchar.h>
		#define _T(x) L##x
	#else
		#define _T(x) x
	#endif

    // ignore case
	#define stricmp strcasecmp

	#if defined(_UNICODE) || defined(UNICODE)
		#define _tcschr     wcschr
		#define _tcsstr     wcsstr

		// 实际的字符数，不是字节数
		#define _tcsclen    wcslen
		#define _tcsicmp	wcsicmp
		#define _tcscmp		wcscmp
	#else
		#define _tcschr     strchr
		#define _tcsstr     strstr

		// 实际的字符数，不是字节数
		#define _tcsclen    strlen
		#define _tcsicmp	stricmp
		#define _tcscmp		strcmp
	#endif
#endif

#if defined(_UNICODE) || defined(UNICODE)
	typedef wchar_t				orsChar;

	// 字符数，是字节数的一半
	#define	orsStrLen			wcslen
#else
	typedef char				orsChar;

	// 按英文计算的字符数，中文算两个，就是实际的字节数
	#define	orsStrLen			strlen
#endif


#if defined(WIN64) || defined(_WIN64)
	#define ORS_PLATFORM_64
	#define ORS_PLATFORM_WINDOWS
#elif defined(WIN32) || defined(_WIN32)
	#define ORS_PLATFORM_32
	#define ORS_PLATFORM_WINDOWS
#elif defined(LINUX64) || defined(_LINUX64)
	#define ORS_PLATFORM_64
	#define ORS_PLATFORM_LINUX
#elif defined(LINUX32) || defined(_LINUX32)
	#define ORS_PLATFORM_32
	#define ORS_PLATFORM_LINUX
#else
	#error "Platform not Specified!"
#endif


#ifndef ORS_PLATFORM_WINDOWS
	typedef unsigned int UINT;
	typedef unsigned char BYTE;
	#define stricmp strcasecmp
#endif

#include <float.h>

#define ors_max(a,b)    (((a) > (b)) ? (a) : (b))
#define ors_min(a,b)    (((a) < (b)) ? (a) : (b))


/* Basic types */
typedef  bool			ors_bool;

typedef  char           ors_int8;
typedef  unsigned char  ors_uint8;
typedef  unsigned char  ors_byte;
typedef  unsigned int	ors_uint;

typedef  short			ors_int16;
typedef  unsigned short ors_uint16;

typedef  float          ors_float32;
typedef  double         ors_float64;


#if defined(ORS_PLATFORM_WINDOWS )
	typedef  long             ors_int32;
	typedef  unsigned long    ors_uint32;
	typedef  __int64          ors_int64;
	typedef  unsigned __int64	ors_uint64;
#elif defined(ORS_PLATFORM_LINUX)
	#ifdef ORS_PLATFORM_32
		typedef  int             ors_int32;
		typedef  unsigned int    ors_uint32;
		typedef  long long       ors_int64;
		typedef  unsigned long long   ors_uint64;
	#else
		typedef  int             ors_int32;
		typedef  unsigned int    ors_uint32;
		typedef  long            ors_int64;
		typedef  unsigned long   ors_uint64;
	#endif
#endif



// 改为ors_uint64在linux下会出错
#ifdef ORS_PLATFORM_32
	typedef ors_int32	ors_ptr2int;
	typedef ors_int32	ors_registerInt;
#else
	#ifdef UBUNTU64
		typedef ors_uint64	ors_ptr2int;
		typedef ors_uint64	ors_registerInt;
	#else
		typedef ors_int64	ors_ptr2int;
		typedef ors_int64	ors_registerInt;
	#endif
#endif

//////////////////////////////////////////////////////////////////////////
//由于C语言不支持对象，所以库中的对象为了调用其功能，转成void*类型的指针
typedef void*		orsHandle;
typedef void*		orsHDC;
typedef void*		orsWnd;


typedef long orsID;

#ifndef interface
#define interface struct
#endif

//////////////////////////////////////////////////////////////////////////
#define ORS_EVENT_OBJECT_DESTRUCTING_ID     1
#define ORS_EVENT_CONNECTION_DISCONNECT_ID	2
#define ORS_EVENT_CONNECTION_CONNECT_ID		3

#ifndef orsZERO
#define orsZERO 1e-6
#endif
#ifndef NULL
#define NULL 0
#endif

#define ORS_DBL_NAN    ((ors_float64)-1.0/DBL_EPSILON)
#define ORS_FLT_NAN    ((ors_float32)-1.0/FLT_EPSILON)
#define ORS_LONG_NAN   ((ors_int32)0x80000000)
#define ORS_INT_NAN    ((ors_int32)0x80000000)
#define ORS_ULONG_NAN  ((ors_uint32)0x0)
#define ORS_UINT_NAN   ((ors_uint32)0x0)
#define ORS_SSHORT_NAN ((ors_int16)0x8000)
#define ORS_USHORT_NAN ((ors_uint16)0)

#define ORS_DEFAULT_MIN_PIX_UCHAR ((ors_byte)1)
#define ORS_DEFAULT_MAX_PIX_UCHAR ((ors_byte)255)
#define ORS_DEFAULT_MIN_PIX_CHAR ((ors_byte)-128)
#define ORS_DEFAULT_MAX_PIX_CHAR ((ors_byte)127)
#define ORS_DEFAULT_MIN_PIX_UINT8 ((ors_byte)1)
#define ORS_DEFAULT_MAX_PIX_UINT8 ((ors_byte)255)
#define ORS_DEFAULT_MIN_PIX_INT8 ((ors_int8)0x81)
#define ORS_DEFAULT_MAX_PIX_INT8 ((ors_int8)0x7F)
#define ORS_DEFAULT_MIN_PIX_INT16 ((ors_int16)0x8001)
#define ORS_DEFAULT_MAX_PIX_INT16 ((ors_int16)0x7FFF)
#define ORS_DEFAULT_MIN_PIX_UINT16 ((ors_uint16)1)
#define ORS_DEFAULT_MAX_PIX_UINT16 ((ors_uint16)0xFFFF)
#define ORS_DEFAULT_MIN_PIX_INT32 ((ors_int32)0x80000001)
#define ORS_DEFAULT_MAX_PIX_INT32 ((ors_int32)0x7FFFFFFF)
#define ORS_DEFAULT_MIN_PIX_UINT32 ((ors_uint32)1)
#define ORS_DEFAULT_MAX_PIX_UINT32 ((ors_uint32)0xFFFFFFFF)

#define ORS_DEFAULT_MIN_PIX_UINT11 ((ors_uint16)1)
#define ORS_DEFAULT_MAX_PIX_UINT11 ((ors_uint16)0x07FF)

#define ORS_DEFAULT_MIN_PIX_FLOAT ((ors_float32)((-1.0/FLT_EPSILON) + 1))
#define ORS_DEFAULT_MAX_PIX_FLOAT  ((ors_float32)((1.0/FLT_EPSILON)))
#define ORS_DEFAULT_MIN_PIX_NORM_FLOAT ((ors_float32)((2*FLT_EPSILON)))
#define ORS_DEFAULT_MAX_PIX_NORM_FLOAT ((ors_float32)1.0)
#define ORS_DEFAULT_MIN_PIX_DOUBLE ((ors_float64)((-1.0/DBL_EPSILON) + 1))
#define ORS_DEFAULT_MAX_PIX_DOUBLE ((ors_float64)((1.0/DBL_EPSILON)))
#define ORS_DEFAULT_MIN_PIX_NORM_DOUBLE ((ors_float64)((2*DBL_EPSILON)))
#define ORS_DEFAULT_MAX_PIX_NORM_DOUBLE ((ors_float64)(1.0))

/*! Pixel data types */
// 与GDAL对应，但存在不支持hdf有符号8bits整数的问题
// 修改：增加ORS_DT_INT8，假定是有符号8bits整数整数时，最小值为负数，即读写的时候利用最小值进行转换
//		
enum orsDataTYPE
{
	ORS_DT_UnKNOWN		=  0,
	ORS_DT_BYTE		=  1,	/*! 8 bit unsigned INTeger        */
	//ORS_DT_INT8		=  2,	/*! 8 bit signed INTeger          */
	ORS_DT_UINT16		=  2,	/*! 16 bit unsigned INTeger       */
	ORS_DT_INT16		=  3,	/*! 16 bit signed INTeger         */
	ORS_DT_UINT32		=  4,	/*! 32 bit unsigned INTeger       */
	ORS_DT_INT32		=  5,	/*! 32 bit signed INTeger         */
	ORS_DT_FLOAT32		=  6,	/*! 32 bit FLOATing poINT         */
	ORS_DT_FLOAT64		=  7,	/*! 64 bit FLOATing poINT         */
	ORS_DT_CINT16		=  8,	/*! Complex INT16  */
	ORS_DT_CINT32		=  9,	/*! Complex INT32       */
	ORS_DT_CFLOAT32		=  10,	/*! Complex FLOAT32     */
	ORS_DT_CFLOAT64		=  11,	/*! Complex FLOAT64        */
	ORS_DT_INT8		=  12,	/*! 8 bit signed INTeger， gdal不支持，需要特殊处理  */
	ORS_DT_COUNT	= 13		/* maximum type # + 1 */
};


//////////////////////////////////////////////////////////////////////////


struct orsPOINT2Df
{
	float x;
	float y;
};


struct orsPOINT2Di
{
	ors_int32 x;
	ors_int32 y;
	
	orsPOINT2Di(){	x = y = 0;	};
	orsPOINT2Di( ors_int32 xIn, ors_int32 yIn)	: x(xIn),y(yIn){};
};


struct orsPOINT3Df
{
	float X;
	float Y;
	float Z;
};
struct orsLINE2Df
{
	orsPOINT2Df startPoint;
	orsPOINT2Df endPoint;
};
//////////////////////////////////////////////////////////////////////////

struct orsPOINT2D
{
	union {double x; double samp; double u; double lon;};
	union {double y; double line; double v; double lat;};

	orsPOINT2D(){	x=y=0;	};
	orsPOINT2D(double x0, double y0){x=x0; y=y0; }

	/*!
    * METHOD: length()
    * Returns the RSS of the components.
    */
   double length() const { return sqrt(x*x + y*y); }

   //***
   // OPERATORS: +, -, +=, -=
   // Point add/subtract with other point:
   //***
   orsPOINT2D operator+(const orsPOINT2D& p) const
      { return orsPOINT2D(x+p.x, y+p.y); }
   orsPOINT2D operator-(const orsPOINT2D& p) const
      { return orsPOINT2D(x-p.x, y-p.y); }
   const orsPOINT2D& operator+=(const orsPOINT2D& p)
      { x += p.x; y += p.y; return *this; }
   const orsPOINT2D& operator-=(const orsPOINT2D& p)
      { x -= p.x; y -= p.y; return *this; }

   //***
   // OPERATORS: *, /
   // Scale point components by scalar:
   //***
   orsPOINT2D operator*(const double& d) const
      { return orsPOINT2D(d*x, d*y); }
   orsPOINT2D operator/(const double& d) const
      { return orsPOINT2D(x/d, y/d); }
};


struct orsPOINT3D
{
	union {double X; double lon;};
	union {double Y; double lat;};
	union {double Z; double h;};

// 说明: 结构不需要写赋值运算符，orsPOINT3D point应为const orsPOINT3D &point
// 	inline orsPOINT3D& operator=(orsPOINT3D point)	//
// 	{
// 		X = point.X;
// 		Y = point.Y;
// 		X = point.Z;
// 		return *this;
// 	}

	orsPOINT3D(){X=0; Y=0; Z=0.0;}
	orsPOINT3D(double x, double y, double z){X=x; Y=y; Z=z; }

	bool operator == (const orsPOINT3D &point)
	{
		return (point.X==X && point.Y==Y && point.Z==Z );
	}

	bool operator != (const orsPOINT3D &point)
	{
		return (  point.X  !=X  ||  point.Y  != Y || point.Z != Z );
	}

   /*!
    * METHOD: length()
    * Returns the RSS of the components.
    */
   double length() const { return sqrt(X*X + Y*Y + Z*Z); }
   double length2() const { return X*X + Y*Y + Z*Z; }

  //***
   // OPERATORS: +, -, +=, -=
   // Point add/subtract with other point:
   //***
   orsPOINT3D operator+(const orsPOINT3D& p) const
      { return orsPOINT3D(X+p.X, Y+p.Y, Z+p.Z); }
   orsPOINT3D operator-(const orsPOINT3D& p) const
      { return orsPOINT3D(X-p.X, Y-p.Y, Z-p.Z); }
   const orsPOINT3D& operator+=(const orsPOINT3D& p)
      { X += p.X; Y += p.Y; Z += p.Z; return *this; }
   const orsPOINT3D& operator-=(const orsPOINT3D& p)
      { X -= p.X; Y -= p.Y; Z -= p.Z; return *this; }

   //***
   // OPERATORS: *, /
   // Scale point components by scalar:
   //***
   orsPOINT3D operator*(const double& d) const
      { return orsPOINT3D(d*X, d*Y, d*Z); }
   orsPOINT3D operator/(const double& d) const
      { return orsPOINT3D(X/d, Y/d, Z/d); }
  void operator /=(double value)
      {
         X /= value;
         Y /= value;
         Z /= value;
      }
   void operator *=(double value) 
      {
         X *= value;
         Y *= value;
         Z *= value;
      }

   double operator *(const orsPOINT3D& src)const
   {
      return (X*src.X + Y*src.Y + Z*src.Z);
   }

   inline const orsPOINT3D operator ^ (const orsPOINT3D& rhs) const
   {
      return orsPOINT3D(Y*rhs.Z-Z*rhs.Y, Z*rhs.X-X*rhs.Z, X*rhs.Y-Y*rhs.X);
   }

   friend const orsPOINT3D operator *( const double &scale, const orsPOINT3D& rhs)
   {
	   return rhs*scale;
   }

};


//typedef orsPOINT3D orsPOINT;

struct orsSIZE	{
	ors_int32 cx;
	ors_int32 cy;
	
	orsSIZE()	:cx(0), cy(0){};
	orsSIZE( ors_int32 cx1, ors_int32 cy1)	:cx(cx1), cy(cy1){};
};


//
// 关于宽度、高度的定义与MFC一致, 即xmax, ymax比实际的大1
//

class orsRect_i
{
public:
	ors_int32	m_xmin;
	ors_int32	m_ymin;
	ors_int32	m_xmax;
	ors_int32	m_ymax;

	orsRect_i():m_xmin(0), m_ymin(0), m_xmax(0), m_ymax(0) {};

	// 容易出错, 换成左上角和右下角点
	orsRect_i( orsPOINT2Di &tl, orsPOINT2Di &br ) : m_xmin(tl.x), m_ymin(tl.y), m_xmax(br.x), m_ymax(br.y){};
	orsRect_i(const orsRect_i& w): m_xmin(w.m_xmin), m_ymin(w.m_ymin), m_xmax(w.m_xmax), m_ymax(w.m_ymax){};
	orsRect_i(int xmin, int ymin, int xmax, int ymax): m_xmin(xmin), m_ymin(ymin), m_xmax(xmax), m_ymax(ymax){};


	//~orsRect_i()	{;};

	ors_int32 height() const	{return m_ymax-m_ymin;}	// 与MFC一致
	ors_int32 width() const		{return m_xmax-m_xmin;}	// 与MFC一致

	orsRect_i& operator = (const orsRect_i &rect)
	{
		if (this != &rect)
		{
			m_ymin=rect.m_ymin;m_ymax=rect.m_ymax;m_xmin=rect.m_xmin;m_xmax=rect.m_xmax;
		}
		return *this;
	}

	bool operator == ( const orsRect_i &rect) const
	{
		return (m_ymin==rect.m_ymin&&m_ymax==rect.m_ymax &&m_xmin==rect.m_xmin && m_xmax==rect.m_xmax) ?  true : false;
	}

	bool operator != ( const orsRect_i &rect) const
	{
		return (m_ymin==rect.m_ymin&&m_ymax==rect.m_ymax &&m_xmin==rect.m_xmin && m_xmax==rect.m_xmax) ?   false : true;
	}

	bool isIntersect( const orsRect_i& w) const
	{
		int xmax_min = ors_max( w.m_xmin, m_xmin );
		int xmin_max = ors_min( w.m_xmax, m_xmax );

		int ymax_min = ors_max( w.m_ymin, m_ymin );
		int ymin_max = ors_min( w.m_ymax, m_ymax );

		return (( xmax_min < xmin_max) && (ymax_min < ymin_max) );
	}

	// 裁剪当前矩形并返回
	orsRect_i clipToRect( orsRect_i& w )
	{

		m_xmin = ors_max( w.m_xmin, m_xmin );
		m_xmax = ors_min( w.m_xmax, m_xmax );

		m_ymin = ors_max( w.m_ymin, m_ymin );
		m_ymax = ors_min( w.m_ymax, m_ymax );

		if( width() <= 0 || height() <= 0 ) {
			m_xmin = m_xmax = 0;
			m_ymin = m_ymax = 0;

			return orsRect_i();
		}

		return *this;
	}
	orsRect_i extendToRect( orsRect_i& w )
	{

		m_xmin = ors_min( w.m_xmin, m_xmin );
		m_xmax = ors_max( w.m_xmax, m_xmax );

		m_ymin = ors_min( w.m_ymin, m_ymin );
		m_ymax = ors_max( w.m_ymax, m_ymax );

		if( width() <= 0 || height() <= 0 ) {
			m_xmin = m_xmax = 0;
			m_ymin = m_ymax = 0;

			return orsRect_i();
		}

		return *this;
	}
	bool isEmpty() const
	{
		if(m_ymax == m_ymin && m_xmax == m_xmin)
			return true;
		else
			return false;
	}

	void Inflate( int dx, int dy )
	{
		m_xmin -= dx;	m_xmax += dx;
		m_ymin -= dy;	m_ymax += dy;
	}

	void Deflate( int dx, int dy )
	{
		m_xmin += dx;	m_xmax -= dx;
		m_ymin += dy;	m_ymax -= dy;
	}
	
	void OffsetRect( int dx, int dy )
	{
		m_xmin += dx;	m_xmax += dx;
		m_ymin += dy;	m_ymax += dy;
	}

	bool PtInRect(int px, int py)
	{
		if (px >= m_xmin && px <= m_xmax-1 && py >= m_ymin && py <= m_ymax -1 )
		{
			return true;
		}
		else
			return false;
	}

	bool PtInRect(double px, double py)
	{
		if (px >= m_xmin && px <= m_xmax-1 && py >= m_ymin && py <= m_ymax -1 )
		{
			return true;
		}
		else
			return false;
	}

	void SetRect(int xLeft, int yTop, int xRight, int yBottom)
	{
		m_xmin = xLeft;
		m_xmax = xRight;
		m_ymin = yTop;
		m_ymax = yBottom;
	}

};
class orsRect_d
{
public:
	double	m_xmin;
	double	m_ymin;
	double	m_xmax;
	double	m_ymax;

	orsRect_d():m_xmin(0), m_ymin(0), m_xmax(0), m_ymax(0) {};

	// 容易出错, 换成左上角和右下角点
	orsRect_d( orsPOINT2Di &tl, orsPOINT2Di &br ) : m_xmin(tl.x), m_ymin(tl.y), m_xmax(br.x), m_ymax(br.y){};
	orsRect_d(const orsRect_d& w): m_xmin(w.m_xmin), m_ymin(w.m_ymin), m_xmax(w.m_xmax), m_ymax(w.m_ymax){};
	orsRect_d(double xmin, double ymin, double xmax, double ymax): m_xmin(xmin), m_ymin(ymin), m_xmax(xmax), m_ymax(ymax){};


	//~orsRect_i()	{;};

	double height() const	{return m_ymax-m_ymin;}	// 与MFC一致
	double width() const		{return m_xmax-m_xmin;}	// 与MFC一致

	orsRect_d& operator = (const orsRect_d &rect)
	{
		if (this != &rect)
		{
			m_ymin=rect.m_ymin;m_ymax=rect.m_ymax;m_xmin=rect.m_xmin;m_xmax=rect.m_xmax;
		}
		return *this;
	}

	bool operator == ( orsRect_d &rect) const
	{
		return (m_ymin==rect.m_ymin&&m_ymax==rect.m_ymax &&m_xmin==rect.m_xmin && m_xmax==rect.m_xmax) ?  true : false;
	}

	bool operator != ( orsRect_d &rect) const
	{
		return (m_ymin==rect.m_ymin&&m_ymax==rect.m_ymax &&m_xmin==rect.m_xmin && m_xmax==rect.m_xmax) ?   false : true;
	}

	bool isIntersect( orsRect_d& w) const
	{
		double xmax_min = ors_max( w.m_xmin, m_xmin );
		double xmin_max = ors_min( w.m_xmax, m_xmax );

		double ymax_min = ors_max( w.m_ymin, m_ymin );
		double ymin_max = ors_min( w.m_ymax, m_ymax );

		return (( xmax_min < xmin_max) && (ymax_min < ymin_max) );
	}

	// 裁剪当前矩形并返回
	orsRect_d clipToRect( orsRect_d& w )
	{

		m_xmin = ors_max( w.m_xmin, m_xmin );
		m_xmax = ors_min( w.m_xmax, m_xmax );

		m_ymin = ors_max( w.m_ymin, m_ymin );
		m_ymax = ors_min( w.m_ymax, m_ymax );

		if( width() <= 0 || height() <= 0 ) {
			m_xmin = m_xmax = 0;
			m_ymin = m_ymax = 0;

			return orsRect_d();
		}

		return *this;
	}

	bool isEmpty() const
	{
		if(m_ymax == m_ymin && m_xmax == m_xmin)
			return true;
		else
			return false;
	}

	void Inflate( double dx, double dy )
	{
		m_xmin -= dx;	m_xmax += dx;
		m_ymin -= dy;	m_ymax += dy;
	}

	void Deflate( double dx, double dy )
	{
		m_xmin += dx;	m_xmax -= dx;
		m_ymin += dy;	m_ymax -= dy;
	}

	void OffsetRect( double dx, double dy )
	{
		m_xmin += dx;	m_xmax += dx;
		m_ymin += dy;	m_ymax += dy;
	}

	bool PtInRect(int px, int py)
	{
		if (px >= m_xmin && px <= m_xmax-1 && py >= m_ymin && py <= m_ymax -1 )
		{
			return true;
		}
		else
			return false;
	}

	bool PtInRect(double px, double py)
	{
		if (px >= m_xmin && px <= m_xmax-1 && py >= m_ymin && py <= m_ymax -1 )
		{
			return true;
		}
		else
			return false;
	}

	void SetRect(double xLeft, double yTop, double xRight, double yBottom)
	{
		m_xmin = xLeft;
		m_xmax = xRight;
		m_ymin = yTop;
		m_ymax = yBottom;
	}

};
////重载加号操作
orsRect_i operator+ (const orsRect_i& rect1, const orsRect_i& rect2);


// 预定义变形模式
enum orsWarpMODE	{
	ORS_wmNONE = 0,				// 不做变形
	ORS_wmTRANSLATION,			// 平移				"ors.image.source.warp.translation"
	ORS_wmRIGID,				// 刚性				"ors.image.source.warp.rigid"
	ORS_wmSIMILARITY,			// 相似变换			"ors.image.source.warp.similarity"
	ORS_wmAFFINE,				// 仿射变换			"ors.image.source.warp.affine"
	ORS_wmPROJECTIVE,			// 二维DLT			"ors.image.source.warp.projective"
	ORS_wmBILINEAR,				// 双线性			"ors.image.source.warp.bilinear"
	ORS_wmQuadPOLY				// 二次多项式		"ors.image.source.warp.quadpoly"
}	;


// 预定义采样模式
enum orsResampleMODE	{
	ORS_rsmNEAREST = 0,	// 最邻近	"ors.image.resample.neareast"
	ORS_rsmBILINEAR,	// 双线性	"ors.image.resample.bilinear"
	ORS_rsmBICUBIC,		// 双三次	"ors.image.resample.bicubic",4点，双三次
	ORS_rsmBICUBIC_6P,			// 6点, 双三次，
	ORS_rsmTruncted_SINC_6P,	// 6点截断sinc
	ORS_rsmKNAB_6P,			// 6点Knab
	ORS_rsmRaised_COS_6P,		// 6点升余弦
	ORS_rsmBSPLINE 			// UNSER B样条
	//ORS_rsmBSPLINE	// B样条	"ors.image.resample.bspline"
};


namespace ORS
{
	// Basic types
	typedef ors_byte        byte;
	typedef ors_byte        uint8;

	typedef ors_int16       int16;
	typedef ors_uint16      uint16;

	typedef ors_uint		uint;

	typedef ors_int32       int32;
	typedef ors_uint32      uint32;

	typedef ors_int64       int64;
	typedef ors_uint64      uint64;

	typedef ors_float32     float32;
	typedef ors_float64     float64;

	typedef orsPOINT2D		POINT2D;
	typedef	orsPOINT3D		POINT3D;
	typedef orsPOINT2Df		POINT2Df;
	typedef	orsPOINT3Df		POINT3Df;
	typedef orsPOINT2Di		POINT2Di;
	typedef orsSIZE			SIZE;

} // end namespace ORS

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef SWIG
	#ifdef ORS_PLATFORM_WINDOWS
		#define ORS_EXPORT __declspec(dllexport)
		#define ORS_IMPORT __declspec(dllimport)
		#define ORS_HIDDEN
	#else
		#define ORS_EXPORT __attribute__ ((visibility ("default")))
		#define ORS_HIDDEN __attribute__ ((visibility ("hidden")))
		#define ORS_IMPORT
	#endif
#else
	#define ORS_HIDDEN
	#define ORS_EXPORT
	#define ORS_IMPORT
#endif

#ifdef ORSBASE_EXPORTS
#define ORS_BASE_API					ORS_EXPORT
#else
#define ORS_BASE_API					ORS_IMPORT
#endif


typedef enum orsLogLEVEL{
		ORS_LOG_DEBUG,
		ORS_LOG_INFO,
		ORS_LOG_WARNING,
		ORS_LOG_ERROR,
		ORS_LOG_FATAL
}orsLogLEVEL;


#endif





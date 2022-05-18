#ifndef _ORS_LOG_SERVICE_H_
#define _ORS_LOG_SERVICE_H_


#include "orsBase/orsIService.h"

#include "orsBase/orsIPlatform.h"


#define  ORS_MAX_LOG_ITEM_SIZE  2048


// 日志插件对象接口
interface orsILogger: public orsIObject
{
public:
	// 任务名称和日志文件，只需最开始设置一次
	virtual void setTaskName( const orsChar *taskName ) = 0;

	// 如果设置了文件名，即为文件方式，否则为控制台方式或自定义方式
	virtual void setLogFile( const orsChar *logFile, bool bAppend = true ) = 0;

	// 设置日志样式
	virtual void setLayout(const orsChar *layout, const char *conversionPattern = NULL ) = 0;

public:
	//致命错误信息输出
	virtual void fatal( const orsChar *  msg ) = 0;
	
	//错误信息输出
	virtual void error( const orsChar *  msg )  = 0;
	
	//警告信息输出
	virtual void warn( const orsChar *  msg )  = 0;
	
	//一般信息输出
	virtual void info( const orsChar *  msg ) = 0;
	
	//调试信息输出
	virtual void debug( const orsChar *  msg ) = 0;
		
	//接口名称
	ORS_INTERFACE_DEF(orsIObject, _T("logger"))
};



//日志服务
interface orsILogService : public orsIService
{
public:
	// 任务名称和日志文件，只需最开始设置一次
	virtual void setTaskName(const orsChar *taskName, const orsChar *logFile = NULL ) = 0;

public:
	//致命错误信息输出
	virtual void fatal(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 ) = 0;

	//错误信息输出
	virtual void error(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 )  = 0;

	//警告信息输出
	virtual void warn(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 )  = 0;

	//一般信息输出
	virtual void info(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 ) = 0;

	//调试信息输出
	virtual void debug(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 ) = 0;

	// 是否允许交互式消息框
	virtual void enableInteractive( bool bEnable = true ) = 0;

	//接口名称
	ORS_INTERFACE_DEF(orsIService, _T("log"))
};

#define ORS_SERVICE_LOG _T("ors.service.log")


// 获取日志服务的宏定义
orsILogService *getLogService();

#define ORS_GET_LOG_SERVICE_IMPL()	\
static orsILogService *s_logService = NULL;\
orsILogService *getLogService()\
{\
	if( NULL != s_logService )\
	return s_logService;\
	\
	s_logService =\
	ORS_PTR_CAST( orsILogService, getPlatform()->getService( ORS_SERVICE_LOG) );\
	\
	return s_logService;\
}


//////////////////////////////////// from glog ///////////////////// 
#ifndef CHECK_OPTION_IMPL

// Option checker macros. In contrast to glog, this function does not abort the
// program, but simply returns false on failure.
#define CHECK_OPTION_IMPL(expr) \
  orsCheckOptionImpl(__FILE__, __LINE__, (expr), #expr)
#define CHECK_OPTION(expr)                                     \
  if (!orsCheckOptionImpl(__FILE__, __LINE__, (expr), #expr)) { \
    return false;                                              \
  }

#define CHECK_OPTION_OP(name, op, val1, val2)                              \
  if (!orsCheckOptionOpImpl(__FILE__, __LINE__, (val1 op val2), val1, val2, \
                           #val1, #val2, #op)) {                           \
    return false;                                                          \
  }


#define CHECK_OPTION_EQ(val1, val2) CHECK_OPTION_OP(_EQ, ==, val1, val2)
#define CHECK_OPTION_NE(val1, val2) CHECK_OPTION_OP(_NE, !=, val1, val2)
#define CHECK_OPTION_LE(val1, val2) CHECK_OPTION_OP(_LE, <=, val1, val2)
#define CHECK_OPTION_LT(val1, val2) CHECK_OPTION_OP(_LT, <, val1, val2)
#define CHECK_OPTION_GE(val1, val2) CHECK_OPTION_OP(_GE, >=, val1, val2)
#define CHECK_OPTION_GT(val1, val2) CHECK_OPTION_OP(_GT, >, val1, val2)


template <typename T1, typename T2>
bool orsCheckOptionOpImpl(const char* file, const int line, const bool result,
	const T1& val1, const T2& val2, const char* val1_str,
	const char* val2_str, const char* op_str) {
	if (result) {
		return true;
	}
	else {
		getPlatform()->logPrint(ORS_LOG_ERROR, "[%s:%d] Check failed: %s %s %s (%s vs. %s)",
			orsString::getPureFileName(file), line,
			val1_str, op_str, val2_str, std::to_string(val1).c_str(), std::to_string(val2).c_str());

		return false;
	}
}

#endif

#endif

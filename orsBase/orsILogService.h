#ifndef _ORS_LOG_SERVICE_H_
#define _ORS_LOG_SERVICE_H_


#include "orsBase/orsIService.h"

#include "orsBase/orsIPlatform.h"


#define  ORS_MAX_LOG_ITEM_SIZE  2048


// ��־�������ӿ�
interface orsILogger: public orsIObject
{
public:
	// �������ƺ���־�ļ���ֻ���ʼ����һ��
	virtual void setTaskName( const orsChar *taskName ) = 0;

	// ����������ļ�������Ϊ�ļ���ʽ������Ϊ����̨��ʽ���Զ��巽ʽ
	virtual void setLogFile( const orsChar *logFile, bool bAppend = true ) = 0;

	// ������־��ʽ
	virtual void setLayout(const orsChar *layout, const char *conversionPattern = NULL ) = 0;

public:
	//����������Ϣ���
	virtual void fatal( const orsChar *  msg ) = 0;
	
	//������Ϣ���
	virtual void error( const orsChar *  msg )  = 0;
	
	//������Ϣ���
	virtual void warn( const orsChar *  msg )  = 0;
	
	//һ����Ϣ���
	virtual void info( const orsChar *  msg ) = 0;
	
	//������Ϣ���
	virtual void debug( const orsChar *  msg ) = 0;
		
	//�ӿ�����
	ORS_INTERFACE_DEF(orsIObject, _T("logger"))
};



//��־����
interface orsILogService : public orsIService
{
public:
	// �������ƺ���־�ļ���ֻ���ʼ����һ��
	virtual void setTaskName(const orsChar *taskName, const orsChar *logFile = NULL ) = 0;

public:
	//����������Ϣ���
	virtual void fatal(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 ) = 0;

	//������Ϣ���
	virtual void error(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 )  = 0;

	//������Ϣ���
	virtual void warn(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 )  = 0;

	//һ����Ϣ���
	virtual void info(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 ) = 0;

	//������Ϣ���
	virtual void debug(const orsChar *  strModule,const orsChar *  msg,const orsChar *  file = NULL, int row = 0 ) = 0;

	// �Ƿ�������ʽ��Ϣ��
	virtual void enableInteractive( bool bEnable = true ) = 0;

	//�ӿ�����
	ORS_INTERFACE_DEF(orsIService, _T("log"))
};

#define ORS_SERVICE_LOG _T("ors.service.log")


// ��ȡ��־����ĺ궨��
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

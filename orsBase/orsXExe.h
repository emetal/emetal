#ifndef ORS_X_EXE_H_H
#define ORS_X_EXE_H_H

#include <stdarg.h>
#include "orsBase/orsIExe.h"
#include "orsBase/orsIPlatform.h"

#include <stdarg.h>

class orsXProcessMsg :
		public orsIProcessMsg, public orsObjectBase
{
public:
	//进度，在0.0-1.0之间
	bool process(double status);

	//消息打印
	int logPrint(orsLogLEVEL loglevel, const orsChar *fmt, ...);

	//接口宏定义
	ORS_OBJECT_IMP1(orsXProcessMsg, orsIProcessMsg, _T("cmd"), _T("cmd") )
};

		
class orsXProcessMsg_multiStage : public orsIProcessMsg_multiStage, public orsObjectBase
{
	ref_ptr<orsIProcessMsg> m_preMsgr;

	double m_progressOffset;
	double m_progressRatio;

	int		m_curProgress;

public:

	//消息打印
	int logPrint(orsLogLEVEL loglevel, const orsChar *fmt, ...)
	{
		assert( NULL != m_preMsgr.get() );

		orsChar buf[ORS_MAX_LOG_ITEM_SIZE];
		
		int count;
		va_list args;
		va_start(args,fmt);
		count = vsprintf(buf,fmt,args);
	
		va_end(args);

		return m_preMsgr->logPrint( loglevel, buf );
	}

	//进度，在0.0-1.0之间
	bool process(double status)
	{
		assert( NULL != m_preMsgr.get() );

		double progress = m_progressOffset + m_progressRatio*status;

		// 每次进度增量达到0.1，响应一次
		if( (int)(1000*progress) > m_curProgress ){
			m_curProgress = int(1000*progress);
			return m_preMsgr->process( 0.001*m_curProgress );
		}
	
		return true;
	}		
	
	void setProcessMsgr( orsIProcessMsg *pMsgr )
	{
		m_preMsgr = pMsgr;
	}
	
	void setStage( double progressOffset, double progressRatio )
	{
		m_progressOffset = progressOffset;
		m_progressRatio = progressRatio;

		m_curProgress = 1000*progressOffset;
	}

public:	
	//接口宏定义
	ORS_OBJECT_IMP2(orsXProcessMsg_multiStage, orsIProcessMsg_multiStage, orsIProcessMsg, _T("default"), _T("default multi stage warper") )
};
		

#endif

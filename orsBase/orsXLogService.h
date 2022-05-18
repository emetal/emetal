//*************************************************************************
//
// Copyright (C) 2008-2028, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsXLogService.h 2008/09/20 18:35 JWS $
//
// Project: OpenRS 
//
// Purpose: Logging Service with log4cxx
//
// Author: JIANG Wanshou, GUO Wei, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsXLogService.h,v $
//
// Revision 1.2 date: 2009/12/6 by JIANG Wanshou
// add configure file, improved getLogger by Module
// 
// Revision 1.1 date: 2009/7/6 by GUO Wei
// add logPrint 
//
// Revision 1.0 date: 2008/09/20 by JIANG Wanshou
// new
//

#ifndef OPENRS_LOG_SERVICE_IMP_H
#define OPENRS_LOG_SERVICE_IMP_H

#include "orsBase/orsIPlatform.h"

#include "orsBase/orsILogService.h"

#include <vector>


struct  log4AppenderINFO
{
	orsString appenerID;
	orsString logFileName;
	bool	  bAppend;
	orsString layout;
	orsString conversionPattern;
};


//��־����
class orsXLogService : public orsILogService, public orsObjectBase
{
private:
//	ref_ptr<orsIGuiLogExt>	m_guiLogger;
	
	// �Ƿ�����Ի���
	bool m_bInteractive;

	std::vector <log4AppenderINFO>		m_vAppendInfos;
	std::vector <ref_ptr<orsILogger> >	m_vAppenders;

	FILE		*m_logFile;
	orsString	m_taskName;

	orsLogLEVEL m_logLevel;

public:
	orsXLogService() 
	{ 
		m_bInteractive = false; 
		
		m_logFile = NULL;
	};

	//����
	virtual bool startup(orsIPlatform *platform);
 
	//�ر�ʱ������
	virtual void shutdown();

	//�Ƿ������ɹ�
	virtual bool isok();

public:
	// �������ƣ�ֻ���ʼ����һ��
	virtual void setTaskName(const orsChar *taskName, const orsChar *logFile = NULL );

	//����������Ϣ���
	virtual void fatal( const orsChar * strModule, const orsChar * msg, const orsChar * file = NULL, int row = 0 );

	//������Ϣ���
	virtual void error( const orsChar * strModule, const orsChar * msg, const orsChar * file = NULL, int row = 0 );

	//������Ϣ���
	virtual void warn(const orsChar * strModule, const orsChar * msg, const orsChar * file = NULL, int row = 0 );

	//һ����Ϣ���
	virtual void info(const orsChar * strModule, const orsChar * msg, const orsChar * file = NULL, int row = 0 );

	//������Ϣ���
	virtual void debug(const orsChar * strModule, const orsChar * msg, const orsChar * file = NULL, int row = 0 );

	virtual void enableInteractive( bool bEnable = true ) {	m_bInteractive = bEnable; };

private:

	ORS_OBJECT_IMP2(orsXLogService,orsILogService, orsIService, "stdprint", "stdprint")

};


#define ORS_SERVICE_LOG_DEFAULT "ors.service.log.log4cxx"


#endif

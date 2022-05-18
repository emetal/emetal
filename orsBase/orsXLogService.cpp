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


#include "stdafx.h"

#include "orsCommon.h"

#include "orsXLogService.h"

bool orsXLogService::startup(orsIPlatform *platform)
{
	//加载log4cxx的配置文件，这里使用了属性文件
	std::string exdir = getExeDir();

	std::string log4cxx_profile = exdir + "/log4cxx.properties";

	//////////////////////////////////////////////////////////////////////////

	m_logLevel = ORS_LOG_DEBUG;
		
	FILE *fp = fopen( log4cxx_profile.c_str(), "r" );
	if( NULL != fp )	{
		orsChar buf[256];
		
		orsArray<orsString>	vAppenders;

		while( !feof(fp) ) {
			fgets( buf, 256, fp );

			// 起始日志级别
			if( strstr( buf, _T("log4j.rootLogger" ) ) ) {
				if( strstr( buf, _T("ALL" ) ) )
					m_logLevel = ORS_LOG_DEBUG;
				else if( strstr( buf, _T("TRACE" ) ) )
					m_logLevel = ORS_LOG_DEBUG;
				else if( strstr( buf, _T("DEBUG" ) ) )
					m_logLevel = ORS_LOG_DEBUG;
				else if( strstr( buf, _T("INFO" ) ) )
					m_logLevel = ORS_LOG_INFO;
				else if( strstr( buf, _T("WARN" ) ) )
					m_logLevel = ORS_LOG_WARNING;
				else if( strstr( buf, _T("ERROR" ) ) )
					m_logLevel = ORS_LOG_ERROR;
				else if( strstr( buf, _T("FATAL" ) ) )
					m_logLevel = ORS_LOG_FATAL;

				//////////////////////////////////////////////////////////////////////////
				// 寻找appender
				orsChar *pAppender = (orsChar *)orsString::findSubStr_i( buf, "," );
				
				orsChar *s=pAppender;

				// 替换,为空格
				while( *s )	{
					if( ',' == *s )	
						*s = ' ';
					s++;
				}

				// 扫描appender
				char buf1[80];

				do 
				{
					if( sscanf( pAppender, "%s", buf1 ) > 0  )	{
						int len= strlen(buf1);
						if( len > 0 )
						{
							orsString appender = "log4j.appender.";
							appender = appender + buf1;
							vAppenders.push_back(appender);
							
							log4AppenderINFO info;
							m_vAppendInfos.push_back(info);
						}
						
						pAppender = strstr( pAppender, buf1 ) + strlen(buf1);						
					}
					else 
						break;

				} while (true);
			}
			else {
				char buf1[80];

				int i;
				for( i=0; i<vAppenders.size(); i++)
				{
					if( strstr( buf, vAppenders[i].c_str() ) ) 
					{
						if( '=' == buf[vAppenders[i].length()] ) {
							sscanf( buf+ vAppenders[i].length()+1, "%s", buf1 );
							m_vAppendInfos[i].appenerID = buf1;
						}
						else if( strstr( buf, "File=" ) )	{
							const orsChar *pFileStr = strstr( buf, "File=");
							sscanf( pFileStr+strlen("File="), "%s", buf1 );
							m_vAppendInfos[i].logFileName = buf1;
						}
						else if( strstr( buf, "Append=" ) )	{
							if( strstr( buf, "true" ) )	
								m_vAppendInfos[i].bAppend = true;
							else
								m_vAppendInfos[i].bAppend = false;
						}
						else if( strstr( buf, "layout=" ) )	{
							const orsChar *layout = strstr( buf, "layout=");
							sscanf( layout+strlen("layout="), "%s", buf1 );
							m_vAppendInfos[i].layout = buf1;
						}
						else if( strstr( buf, "layout.ConversionPattern=" ) )	{
							orsChar *pattern = (orsChar *)strstr( buf, "layout.ConversionPattern=");

							pattern = pattern + +strlen("layout.ConversionPattern=");

							int len=strlen(pattern);

							if( '\r' == pattern[len-1] )
								pattern[len-1] = 0;
							else if( '\n' == pattern[len-1] )
								pattern[len-1] = 0;

							m_vAppendInfos[i].conversionPattern = pattern;
						}
					}
				}
			}
		};

		fclose(fp);
	}
	else	{
		printf("Can not open %s \n", log4cxx_profile.c_str() );
	}

	return true;
}



void orsXLogService::shutdown()
{
	// 插件的中对象不能等到插件卸载后再析构
	m_vAppenders.clear();
}

bool orsXLogService::isok()
{
	return true;
}


// 任务名称，只需最开始设置一次
void orsXLogService::setTaskName(const orsChar* taskName, const orsChar* logFile)
{
	m_taskName = taskName;

	if (m_taskName.isEmpty())
		return;

	//创建日志对象

	if (0 == m_vAppenders.size()) {
		int i;
		for (i = 0; i < m_vAppendInfos.size(); i++)
		{
			ref_ptr<orsILogger> logger = ORS_PTR_CAST(orsILogger, _getPlatform()->createObject(m_vAppendInfos[i].appenerID));

			if (NULL != logger.get()) {
				logger->setLayout(m_vAppendInfos[i].layout, m_vAppendInfos[i].conversionPattern);
				m_vAppenders.push_back(logger);
			}
		}
	}

	///////////////////////////////////
	int i;
	for (i = 0; i < m_vAppenders.size(); i++)
	{
		if (NULL != m_vAppenders[i].get()) {
			m_vAppenders[i]->setTaskName(taskName);

			if (!m_vAppendInfos[i].logFileName.isEmpty()) {
				if (NULL != logFile)
					m_vAppenders[i]->setLogFile(logFile);
				else
					m_vAppenders[i]->setLogFile(m_vAppendInfos[i].logFileName, m_vAppendInfos[i].bAppend);
			}
		}
	}
}


// 总是log
void orsXLogService::fatal(const char * strModule, const char * msg,
						const char * file, int row)
{
	if ( m_vAppenders.size() > 0 && !m_taskName.isEmpty() ) {
		int i;
		for( i=0; i<m_vAppenders.size(); i++)
			m_vAppenders[i]->fatal( msg );
	}
	else {
		if (NULL != file)
			printf("fatal: %s: %s row%d\n", msg, file, row);
		else
			printf("fatal: %s\n", msg);

		fflush(stdout);
	}

#ifdef _WIN32
	if( m_bInteractive )	{
		char buf[256];
		
		if( NULL != file )
			sprintf( buf, "fatal: %s: %s row%d\n", msg, file, row );	
		else 
		sprintf( buf, "fatal: %s\n", msg);	

		MessageBox( NULL, msg, "OpenRS", MB_ICONERROR );
	}
#endif

}

// 总是log
void orsXLogService::error(const char * strModule, const char * msg,
						const char * file, int row)
{
	if (m_vAppenders.size() > 0 && !m_taskName.isEmpty()) {
		int i;
		for( i=0; i<m_vAppenders.size(); i++)
			m_vAppenders[i]->error( msg );
	}
	else {
		if (NULL != file)
			printf("error: %s: %s row%d\n", msg, file, row);
		else
			printf("error: %s\n", msg);

		fflush(stdout);
	}

#ifdef _WIN32
	if( m_bInteractive )	{
		char buf[256];
		
		if( NULL != file )
			sprintf( buf, "error: %s: %s row%d\n", msg, file, row );	
		else 
			sprintf( buf, "error: %s\n", msg);	
		
		MessageBox( NULL, msg, "OpenRS", MB_ICONERROR );
	}
#endif
}

void orsXLogService::warn(const char * strModule, const char * msg,
						   const char * file, int row)
{
	if (m_logLevel <= ORS_LOG_WARNING) {
		if (m_vAppenders.size() > 0 && !m_taskName.isEmpty()) {
			int i;
			for (i = 0; i < m_vAppenders.size(); i++)
				m_vAppenders[i]->warn(msg);
		}
		else {
			if (NULL != file)
				printf("warning: %s:%s row%d\n", msg, file, row);
			else
				printf("warning: %s\n", msg);

			fflush(stdout);
		}
	}
}



void orsXLogService::info(const char * strModule, const char * msg,
					   const char * file, int row)
{
	if (m_logLevel <= ORS_LOG_INFO ) {
		if (m_vAppenders.size() > 0 && !m_taskName.isEmpty()) {
			int i;
			for (i = 0; i < m_vAppenders.size(); i++)
				m_vAppenders[i]->info(msg);
		}
		else {
#ifdef ORS_PLATFORM_WINDOWS
			if (orsString::findSubStr_i(msg, "process") && strchr(msg, '%')) {
				printf("info: %s\r", msg);
				fflush(stdout);
				return;
			}
#else
			if (orsString::findSubStr_i(msg, "process") && strchr(msg, '%')) {
				printf("info: %s\r", msg);
				fflush(stdout);
				return;
			}
#endif

			if (NULL != file)
				printf("info: %s: %s row%d\n", msg, file, row);
			else
				printf("info: %s\n", msg);

			fflush(stdout);
		}
	}
}

void	orsXLogService::debug(const char * strModule, const char * msg,
						const char * file, int row)
{
	if (m_logLevel <= ORS_LOG_DEBUG) {
		if (m_vAppenders.size() > 0 && !m_taskName.isEmpty()) {
			// 系统注销中
			if (NULL != orsString::findSubStr_i(msg, _T("orsXRegisterService: shutdown"))) {
				m_vAppenders.clear();
				m_vAppendInfos.clear();
			}
			else {
				int i;
				for (i = 0; i < m_vAppenders.size(); i++)
					m_vAppenders[i]->debug(msg);
			}
		}
		else {
			if (NULL != file)
				printf("debug: %s: %s row%d\n", msg, file, row);
			else
				printf("debug: %s\n", msg);
		}
	}
}
//*************************************************************************
//
// Copyright (C) 2009, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsIExe.h 2009/07/18 9 : 38 JWS $
//
// Project: OpenRS
//
// Purpose: 用于粗粒度算法包装，是并行处理的基础
//
// Author:  Guowei, guowei98032@gmail.com
//			JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIExe.h,v $
//
// Revision 1.2 date: 2009/09/30 by JIANG Wanshou
//		定义输入、内部参数、输出、配置GUI 三类接口
//
// Revision 1.1 date: 2009/06/10 by JIANG Wanshou
//		orsIExecute-> orsISimpleExe
//					->orsIParallelExe
//					->orsIGuiExe
//
// Revision 1.0 date: 2008/07/18 by JIANG Wanshou
// new
//

#ifndef _ORS_EXE_INTERFACE_H
#define _ORS_EXE_INTERFACE_H

#include "orsBase/orsIObject.h"
#include "orsBase/orsIProperty.h"
#include "orsBase/orsArray.h"
#include "orsBase/orsILogService.h"

#include "orsBase/orsIExtension.h"
#include "orsBase/orsUtil.h"

#include "orsBase/orsIDataSource.h"

//////////////////////////////////////////////////////////////////////////
//
// 粗粒度算法的处理消息接口，如进度条，错误信息等
//
interface orsIProcessMsg : public orsIObject
{
public:
	//进度，在0.0-1.0之间, 返回零表示 取消任务
	virtual bool process( double status) = 0;

	//消息打印
	virtual int logPrint(orsLogLEVEL loglevel, const orsChar *fmt, ...) = 0;

	//接口名称
	ORS_INTERFACE_DEF( orsIObject, _T("process") )
};


#define ORS_PROCESSMSG_CMD	_T("ors.process.cmd")


//////////////////////////////////////////////////////////////////////////
// 多阶段处理消息，必须先设置最终的消息处理器

interface orsIProcessMsg_multiStage : public orsIProcessMsg
{
public:
	virtual void setProcessMsgr( orsIProcessMsg *pMsgr ) = 0;
	virtual void setStage( double progressOffset, double progressRatio ) = 0;
	
	//接口名称
	ORS_INTERFACE_DEF( orsIProcessMsg, _T("multiStage") )
};


#define ORS_PROCESSMSG_MULTISTAGE	_T("ors.process.multiStage.default")

//////////////////////////////////////////////////////////////////////////

// 可执行对象接口
interface orsIExecute : public orsIObject
{
public:
	// 输入文件
	virtual orsIProperty *getInputFileNames() = 0;

	// 内部参数
	virtual orsIProperty *getParameterArgs() = 0;

	// 输出文件
	virtual orsIProperty *getOutputFileNames() = 0;

	virtual bool setArguments( orsIProperty *inputFileNames, orsIProperty *parameterArgs, orsIProperty *outputFileNames ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// 序列化接口，在orsIObject已定义
	// virtual const orsIProperty *getProperty() const = 0;

	//输入参数信息
	// virtual bool initFromProperty( orsIProperty *property ) = 0;

	////////////////////////////////////////////////////////////
	// 取自定义算法配置界面的对象ID. 默认没有自定义界面
	//
	// 注意：不在算法内部直接实现该配置界面，应把配置界面作为一个扩展在GUI插件里单独实现。
	//       否则影响算法插件的移植性
	virtual const orsChar *getConfigDlg() { return NULL; };

	ORS_INTERFACE_DEF( orsIObject, _T("execute") );
};


interface orsIExeConfigDlg: public orsIExtension
{
public:
	virtual bool config( orsIExecute *pExeObj ) = 0;

	ORS_INTERFACE_DEF( orsIExtension, _T("exeConfigDlg") );
};



//////////////////////////////////////////////////////////////////////////

//非并行算法接口
interface orsISimpleExe: public orsIExecute
{
public:
	//计算得到输出参数
	virtual ref_ptr<orsIProperty> execute(orsIProcessMsg *process) = 0;

	ORS_INTERFACE_DEF( orsIExecute, _T("simple") );
};

// 
// interface orsILicenseExe: public orsISimpleExe
// {
// public:
// 	virtual bool executeAuthrize(enum orsServiceTYPE servertype) = 0;
// 	ORS_INTERFACE_DEF( orsIExecute, _T("License") );
// };


//
// 带界面的人工交互程序，目前只能在本地执行
//	 本程序调用外部.exe程序，无进度显示
//
interface orsIGuiExe: public orsIExecute
{
public:
	//计算得到输出参数
	virtual ref_ptr<orsIProperty> execute() = 0;

	ORS_INTERFACE_DEF( orsIExecute, _T("gui" ) );
};


//
// 分布式并行处理程序
//
interface orsIParallelExe : public orsIExecute
{
public:
	// 如果 nTasks = 0, 算法自己决定任务数，否则按照给定的任务数运行
	virtual orsArray<ref_ptr<orsIProperty> > getTasks( int nTasks = 0 ) = 0;

	//通过Job信息和Task信息进行计算得到输出信息,taskOutput在外部分配内存<map>
	virtual ref_ptr<orsIProperty> taskExecute(ref_ptr<orsIProperty> taskInput,orsIProcessMsg *process) = 0;

	//进行子任务合并<reduce>
	virtual ref_ptr<orsIProperty> taskCombine(orsArray<ref_ptr<orsIProperty> > taskInputs,orsIProcessMsg *process) = 0;

	ORS_INTERFACE_DEF( orsIExecute, _T( "parallel" ) );
};



//
// 多层次分布式并行处理程序
//
interface orsIParallelExe_L : public orsIExecute
{
public:
	// 
	virtual ors_int32 getLevels( ors_int32 *totalTasks, int nTasks = 0 ) = 0;

	// 如果 nTasks = 0, 算法自己决定任务数，否则按照给定的任务数运行
	virtual orsArray<ref_ptr<orsIProperty> > getTasks( int iLevel ) = 0;
	
	//通过Job信息和Task信息进行计算得到输出信息,taskOutput在外部分配内存<map>
	virtual ref_ptr<orsIProperty> taskExecute( int iLevel, ref_ptr<orsIProperty> taskInput,orsIProcessMsg *process) = 0;
	
	//进行子任务合并<reduce>
	virtual ref_ptr<orsIProperty> taskCombine(int iLevel, orsArray<ref_ptr<orsIProperty> > taskInputs,orsIProcessMsg *process) = 0;
	
	ORS_INTERFACE_DEF( orsIExecute, _T( "parallel_l" ) );
};



#define INPUT_FILE_NAMES _T("InputFileNames")
#define OUTPUT_FILE_NAMES _T("OutputFileNames")

template <typename orsIExeInteface>
class orsIExeHelper: public orsIExeInteface
{
protected:
	orsIExeHelper( bool bForRegister )
	{
		if( !bForRegister ) {
			m_jobArguments = getPlatform()->createProperty();

			m_inputFileNames = getPlatform()->createProperty();
			m_parameterArgs = getPlatform()->createProperty();
			m_outputFileNames = getPlatform()->createProperty();

			m_jobArguments->addAttr( INPUT_FILE_NAMES, m_inputFileNames );
			m_jobArguments->addAttr( OUTPUT_FILE_NAMES, m_outputFileNames);
			m_jobArguments->addAttr( PARAMETER_ARGS, m_parameterArgs);
		}
	}

public:
	virtual orsIProperty *getInputFileNames()
	{
		return m_inputFileNames.get();
	}

	// 内部参数
	virtual orsIProperty *getParameterArgs()
	{
		return m_parameterArgs.get();
	}

	// 输出文件
	virtual orsIProperty *getOutputFileNames()
	{
		return m_outputFileNames.get();
	}

	virtual const orsIProperty *getProperty() const
	{
		return m_jobArguments.get();
	}

	//输入参数信息
	virtual bool initFromProperty( orsIProperty *property )
	{
		ref_ptr<orsIProperty> inputFileNames;
		ref_ptr<orsIProperty> parameterArgs;
		ref_ptr<orsIProperty> outputFileNames;

		property->getAttr(INPUT_FILE_NAMES, inputFileNames );
		property->getAttr( PARAMETER_ARGS, parameterArgs );
		property->getAttr(OUTPUT_FILE_NAMES, outputFileNames );

		// 兼容旧的调用方式
		if( NULL == inputFileNames.get() )
			inputFileNames = property;
		if( NULL == parameterArgs.get() )
			parameterArgs =  property;
		if( NULL == outputFileNames.get() )
			outputFileNames = property;

		// 自动更新内部属性
		if( inputFileNames.get() != m_inputFileNames.get() )
			m_inputFileNames->update( inputFileNames.get() );

		if( outputFileNames.get() != m_outputFileNames.get() )
			m_outputFileNames->update( outputFileNames.get() );

		if( parameterArgs.get() != m_parameterArgs.get() )
			m_parameterArgs->update( parameterArgs.get() );

		return this->setArguments( inputFileNames.get(), parameterArgs.get(), outputFileNames.get() );
	}

protected:
	ref_ptr<orsIProperty> m_jobArguments;
	ref_ptr<orsIProperty> m_inputFileNames;
	ref_ptr<orsIProperty> m_parameterArgs;
	ref_ptr<orsIProperty> m_outputFileNames;

};


// 并行进程数
#define ORS_PARALLEL_NUM_PROCESSES	_T("Num Of Processes")
#define ORS_MPI_CMD		_T("MPI_Command")

#define ORS_WebSERVICE	_T("Web Service")

#endif

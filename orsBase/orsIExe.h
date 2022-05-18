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
// Purpose: ���ڴ������㷨��װ���ǲ��д���Ļ���
//
// Author:  Guowei, guowei98032@gmail.com
//			JIANG Wanshou, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsIExe.h,v $
//
// Revision 1.2 date: 2009/09/30 by JIANG Wanshou
//		�������롢�ڲ����������������GUI ����ӿ�
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
// �������㷨�Ĵ�����Ϣ�ӿڣ����������������Ϣ��
//
interface orsIProcessMsg : public orsIObject
{
public:
	//���ȣ���0.0-1.0֮��, �������ʾ ȡ������
	virtual bool process( double status) = 0;

	//��Ϣ��ӡ
	virtual int logPrint(orsLogLEVEL loglevel, const orsChar *fmt, ...) = 0;

	//�ӿ�����
	ORS_INTERFACE_DEF( orsIObject, _T("process") )
};


#define ORS_PROCESSMSG_CMD	_T("ors.process.cmd")


//////////////////////////////////////////////////////////////////////////
// ��׶δ�����Ϣ���������������յ���Ϣ������

interface orsIProcessMsg_multiStage : public orsIProcessMsg
{
public:
	virtual void setProcessMsgr( orsIProcessMsg *pMsgr ) = 0;
	virtual void setStage( double progressOffset, double progressRatio ) = 0;
	
	//�ӿ�����
	ORS_INTERFACE_DEF( orsIProcessMsg, _T("multiStage") )
};


#define ORS_PROCESSMSG_MULTISTAGE	_T("ors.process.multiStage.default")

//////////////////////////////////////////////////////////////////////////

// ��ִ�ж���ӿ�
interface orsIExecute : public orsIObject
{
public:
	// �����ļ�
	virtual orsIProperty *getInputFileNames() = 0;

	// �ڲ�����
	virtual orsIProperty *getParameterArgs() = 0;

	// ����ļ�
	virtual orsIProperty *getOutputFileNames() = 0;

	virtual bool setArguments( orsIProperty *inputFileNames, orsIProperty *parameterArgs, orsIProperty *outputFileNames ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// ���л��ӿڣ���orsIObject�Ѷ���
	// virtual const orsIProperty *getProperty() const = 0;

	//���������Ϣ
	// virtual bool initFromProperty( orsIProperty *property ) = 0;

	////////////////////////////////////////////////////////////
	// ȡ�Զ����㷨���ý���Ķ���ID. Ĭ��û���Զ������
	//
	// ע�⣺�����㷨�ڲ�ֱ��ʵ�ָ����ý��棬Ӧ�����ý�����Ϊһ����չ��GUI����ﵥ��ʵ�֡�
	//       ����Ӱ���㷨�������ֲ��
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

//�ǲ����㷨�ӿ�
interface orsISimpleExe: public orsIExecute
{
public:
	//����õ��������
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
// ��������˹���������Ŀǰֻ���ڱ���ִ��
//	 ����������ⲿ.exe�����޽�����ʾ
//
interface orsIGuiExe: public orsIExecute
{
public:
	//����õ��������
	virtual ref_ptr<orsIProperty> execute() = 0;

	ORS_INTERFACE_DEF( orsIExecute, _T("gui" ) );
};


//
// �ֲ�ʽ���д������
//
interface orsIParallelExe : public orsIExecute
{
public:
	// ��� nTasks = 0, �㷨�Լ������������������ո���������������
	virtual orsArray<ref_ptr<orsIProperty> > getTasks( int nTasks = 0 ) = 0;

	//ͨ��Job��Ϣ��Task��Ϣ���м���õ������Ϣ,taskOutput���ⲿ�����ڴ�<map>
	virtual ref_ptr<orsIProperty> taskExecute(ref_ptr<orsIProperty> taskInput,orsIProcessMsg *process) = 0;

	//����������ϲ�<reduce>
	virtual ref_ptr<orsIProperty> taskCombine(orsArray<ref_ptr<orsIProperty> > taskInputs,orsIProcessMsg *process) = 0;

	ORS_INTERFACE_DEF( orsIExecute, _T( "parallel" ) );
};



//
// ���ηֲ�ʽ���д������
//
interface orsIParallelExe_L : public orsIExecute
{
public:
	// 
	virtual ors_int32 getLevels( ors_int32 *totalTasks, int nTasks = 0 ) = 0;

	// ��� nTasks = 0, �㷨�Լ������������������ո���������������
	virtual orsArray<ref_ptr<orsIProperty> > getTasks( int iLevel ) = 0;
	
	//ͨ��Job��Ϣ��Task��Ϣ���м���õ������Ϣ,taskOutput���ⲿ�����ڴ�<map>
	virtual ref_ptr<orsIProperty> taskExecute( int iLevel, ref_ptr<orsIProperty> taskInput,orsIProcessMsg *process) = 0;
	
	//����������ϲ�<reduce>
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

	// �ڲ�����
	virtual orsIProperty *getParameterArgs()
	{
		return m_parameterArgs.get();
	}

	// ����ļ�
	virtual orsIProperty *getOutputFileNames()
	{
		return m_outputFileNames.get();
	}

	virtual const orsIProperty *getProperty() const
	{
		return m_jobArguments.get();
	}

	//���������Ϣ
	virtual bool initFromProperty( orsIProperty *property )
	{
		ref_ptr<orsIProperty> inputFileNames;
		ref_ptr<orsIProperty> parameterArgs;
		ref_ptr<orsIProperty> outputFileNames;

		property->getAttr(INPUT_FILE_NAMES, inputFileNames );
		property->getAttr( PARAMETER_ARGS, parameterArgs );
		property->getAttr(OUTPUT_FILE_NAMES, outputFileNames );

		// ���ݾɵĵ��÷�ʽ
		if( NULL == inputFileNames.get() )
			inputFileNames = property;
		if( NULL == parameterArgs.get() )
			parameterArgs =  property;
		if( NULL == outputFileNames.get() )
			outputFileNames = property;

		// �Զ������ڲ�����
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


// ���н�����
#define ORS_PARALLEL_NUM_PROCESSES	_T("Num Of Processes")
#define ORS_MPI_CMD		_T("MPI_Command")

#define ORS_WebSERVICE	_T("Web Service")

#endif

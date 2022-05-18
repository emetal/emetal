///////////////////////////////////////////////////////////
//  orsIDataSource.h
//  Implementation of the Interface orsIDataSource
//  Created on:      26-九月-2008 10:21:13
//  Original author: jws
///////////////////////////////////////////////////////////

#if !defined(EA_626192F5_EB0D_408b_B0FB_57AC2E8F45A4__INCLUDED_)
#define EA_626192F5_EB0D_408b_B0FB_57AC2E8F45A4__INCLUDED_

#include "orsBase/orsIPlatform.h"
#include "orsBase/orsIConnectableObject.h"
#include "orsBase/orsFileFormat.h"

// 定义数据链节点的输入输出属性组
#define ORS_SOURCES_INPUT	_T("InputSources")
#define ORS_SOURCES_OUTPUT	_T("OutputSources")
#define PARAMETER_ARGS _T("ParameterArgs")

interface orsIDataSource : public orsIConnectableObject
{
public:
	virtual const orsChar *getFilePath() const = 0;

public:
    ORS_INTERFACE_DEF( orsIConnectableObject, _T("dataSource") );
};

#endif // !defined(EA_626192F5_EB0D_408b_B0FB_57AC2E8F45A4__INCLUDED_)

#ifndef _ORS_REGISTER_SERVICE_H_
#define _ORS_REGISTER_SERVICE_H_

#include "orsBase/orsIService.h"

#include "orsBase/orsIPlugin.h"


// #if defined kyLINUX64
// #include <vector>
// using namespace std;
// #define orsArray std::vector
// #endif



class orsPluginInfo
{
public:
	orsString	pluginName;
	orsString	pluginVersion;
	orsString	pluginID;
	orsString	pluginProvider;
};


interface orsIPluginManager : public orsIObject
{
public:
	//加入插件，如果isForced开关被打开，将强制更新插件XML树
	virtual bool addPlugin(const orsChar * pluginName, bool isForced = false) = 0;

	//移除插件，如果isForced开关被打开，将强制更新插件XML树
	virtual bool removePlugin(const orsChar * pluginName, bool isForced = false) = 0;

	//scan文件夹内部所有的插件
	virtual bool scanDir(const orsChar * dirName, bool isForced = false) = 0;

	//得到插件相关信息：（插件版本号，主版本号）
	virtual std::vector<orsPluginInfo> &getPluginInfos() = 0;
	virtual bool getPluginInfo(const orsChar *pluginName, orsPluginInfo &info) = 0;

	ORS_INTERFACE_DEF(orsIObject, _T("pluginManager"))
};


// 对象注册服务
interface orsIRegisterService : public orsIService
{
public:
	//注册插件
	virtual bool registerObject( ORS_OBJECT_CREATOR_FUN objCreator ) = 0;

	//加入插件，如果isForced开关被打开，将强制更新插件XML树
	virtual bool addPlugin(const orsChar * pluginName,bool isForced = false) = 0;

	//移除插件，如果isForced开关被打开，将强制更新插件XML树
	virtual bool removePlugin(const orsChar * pluginName,bool isForced = false) = 0;

	// 通过对象名称得到描述
	virtual ref_ptr<orsIObjectDesc> getObjectDescByID(const orsChar * objID ) = 0;

	// 得到所有的对象描述
	virtual orsArray<ref_ptr<orsIObjectDesc> > getAllObjectDescs() =0 ;

	// 得到兼容某个接口的算法描述
	//!!!!!注意，interfaceClass是interface的类名称，而不是接口的名字
	virtual orsArray<ref_ptr<orsIObjectDesc> > getObjectDescsByInterface( const orsChar * interfaceName) = 0;

	// 创建对象实例
	virtual orsIObject *createObject( const orsChar *objectID ) = 0;

	//接口名称
	ORS_INTERFACE_DEF( orsIService, _T("register"))
};



#endif

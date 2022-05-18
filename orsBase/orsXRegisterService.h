/********************************************************************
	created:	2008/09/20
	created:	20:9:2008   17:32
	filename: 	e:\OpenRS\DESKTOP\src\orsBase\orsIRegisterServiceImp.h
	file path:	e:\OpenRS\DESKTOP\src\orsBase
	file base:	orsIRegisterServiceImp
	file ext:	h
	author:		Guo Wei

	purpose:
*********************************************************************/
#ifndef ORS_REGISTER_SERVICE_IMP_H
#define ORS_REGISTER_SERVICE_IMP_H



#include "orsBase/orsIPlatform.h"
#include "orsBase/orsIRegisterService.h"

#include "orsPluginDLL.h"


//对象描述
class orsXObjectDesc: public orsIObjectDesc, public orsObjectBase
{
public:
	virtual void addRef()			{internalAddRef();}
	virtual void release()			{internalRelease();}

public:
	virtual const orsChar *getName()	{return m_name;}
	virtual const orsChar *getID()		{return m_id;}
	virtual const orsChar *getDesc()	{return m_desc;}
	virtual const orsChar *getPluginName()	{return m_pluginName;}

	virtual const orsArray<orsString>	&getInterfaceNames() const {	return m_interfaceNames;	}

public:
	orsString				m_name;			// 对象名字
	orsString				m_id;			// 对象ID, 区分对象的唯一标记
	orsString				m_desc;			// 对象描述
	orsString				m_pluginName;		// 对象所在的插件（dll）文件名

	ref_ptr<orsIProperty>	m_propertys;		// 对象属性
	orsArray<orsString>		m_interfaceNames;	// 对象接口名字
};


typedef std::map<stdString , ref_ptr<orsIObjectDesc> >	orsObjDescMap;


//算法注册服务
class orsXRegisterService : public orsIRegisterService,
								public orsIPluginManager,
								public orsObjectBase
{
public:
	orsXRegisterService();
	virtual ~orsXRegisterService();

	//服务接口
	//启动时被调用
	virtual bool startup(orsIPlatform *platform);

	//关闭时被调用
	virtual void shutdown();

	//是否启动成功
	virtual bool isok();

	//注册插件，如果之前有，则将替换之前的构造函数
	//注意，该接口可被系统内部调用，而不一定非经过插件调用
	//在系统启动时，系统内置对象将加入到注册服务中，如属性序列化器
	bool registerObject(ORS_OBJECT_CREATOR_FUN objCreator);

	//加入插件，如果isForced开关被打开，将强制更新插件XML树
	bool addPlugin(const orsChar * pluginName,bool isForced = false);

	//移除插件，如果isForced开关被打开，将强制更新插件XML树
	bool removePlugin(const orsChar * pluginName,bool isForced = false);

	//得到插件相关信息：（插件版本号，主版本号）
	std::vector<orsPluginInfo> &getPluginInfos();

	bool getPluginInfo(const orsChar *pluginName,orsPluginInfo &info);

	//scan文件夹内部所有的插件
	bool scanDir(const orsChar * dirName,bool isForced = false);

	//得到插件目录
	orsString getPluginDir();

	//得到插件树文件名称
	orsString getPluginTreeFileName();


	// 通过对象名称得到描述
	virtual ref_ptr<orsIObjectDesc> getObjectDescByID(const orsChar * objID );

	// 得到所有的对象描述
	virtual orsArray<ref_ptr<orsIObjectDesc> > getAllObjectDescs();

	// 得到兼容某个接口的算法描述
	//!!!!!注意，interfaceClass是interface的类名称，而不是接口的名字
	virtual orsArray<ref_ptr<orsIObjectDesc> > getObjectDescsByInterface( const orsChar * interfaceName);

	// 创建对象实例
	virtual orsIObject *createObject( const orsChar *objectID );

protected:
	//判断插件是否在插件XML树上存在
	bool pluginExistInTree(const orsChar * pluginName);

	//读取XML插件树
	void writeXML();

	//写XML插件树
	void readXML();

	//得到插件节点
	ref_ptr<orsIProperty> findPluginNode(const orsChar * pluginName);

	//序列化方法，当该方法被调用时间，shutdown时会保存
	void serializePlugin( orsIProperty *pluginTree, orsPluginDLL *plugin);

	//在XML树上移除一个插件描述
	void unSerializePlugin( const orsChar * pluginName );

	//序列化插件内的算法对象(被serializePlugin调用)
	void serializeObject( orsIProperty *pluginNode, orsIObject *obj);

	//通过插件名称得到插件的绝对路径
	orsString getPluginPathByName(const orsChar * pluginPath);

	//通过绝对路径得到插件名
	orsString getPluginNameByPath(const orsChar * pathName);

	//设置保存标志，当shutup时，真正save
	void setSaveFlag(){m_needSave = true;}

	//从xml节点内抽取对象描述，并加入到描述Map中
	void updateDescFromPluginProperty( const orsIProperty *node);

private:
	//如果XML产生变化，将需要重新保存m_keywordList
	bool	m_needSave;

	// 插件树
	ref_ptr<orsIProperty>	m_objTree;

	// XML序列化器
	ref_ptr<orsIPropertySerialize> m_serializer;

	// 当前日志
	ref_ptr<orsILogService> getLogService();

private:
	// 已加载的插件(DLL)中抽取的object创建者 (对象名，创建函数指针)
	orsObjCreatorMap	m_creatorMap;

	//对象描述Map(对象名，对象描述)
	orsObjDescMap m_objDescMap;

	// 当前在加载的插件,用于探测插件内部有哪些object
	ref_ptr<orsPluginDLL>  m_runningPlugin;

	// 已加载的插件实体（dll）map
	orsPluginDLLMap m_loadedPlugins;

	// 注册插件的相关信息
	std::vector<orsPluginInfo> m_registeredPluginInfos;

private:

	//系统平台
	orsIPlatform *m_platform;

public:

	ORS_OBJECT_DEF_NORMAL(orsIRegisterService,"default", "Register Service")

	ORS_BEGIN_VTABLE_MAP(orsXRegisterService)
		ORS_INTERFACE_ENTRY2(orsIObject,orsIRegisterService)
		ORS_INTERFACE_ENTRY(orsIRegisterService)
		ORS_INTERFACE_ENTRY(orsIPluginManager)
		ORS_INTERFACE_ENTRY(orsIService)
	ORS_END_VTABLE_MAP
};

#endif

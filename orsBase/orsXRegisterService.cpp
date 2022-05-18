/********************************************************************
	created:	2008/09/20
	created:	20:9:2008   17:31
	filename: 	E:\OpenRS\DESKTOP\src\orsBase\orsIRegisterServiceImp.cpp
	file path:	E:\OpenRS\DESKTOP\src\orsBase
	file base:	orsIRegisterServiceImp
	file ext:	cpp
	author:		Guo Wei

	modifications:
				2008，9，21， 增加日志，by jws

	purpose:
				实现插件的登记服务

*********************************************************************/
#include "stdafx.h"

#include "orsBase/orsConstDef.h"
#include "orsBase/orsUtil.h"
#include "orsBase/orsILastErrorService.h"


#include "orsXRegisterService.h"

#include "orsXXMLSerialize.h"

#include "orsXProperty.h"

#include "orsBase/orsTypedef.h"

#include "orsBase/orsString.h"

//插件树序列化用的标签

#define ORS_Property					"Property"

#define ORS_OBJECT_ID					"ID"

#define ORS_OBJECT_NAME					"Name"

#define ORS_OBJECT_DESC					"Desc"

#define ORS_OBJECT_PARENT_INTERACE		"Interface"

#define ORS_OBJECT_PROPERTYS			"Property"

#define ORS_PLUGIN_TREE_FIRST_START_UP	"First_start_up"

#define ORS_PLUGIN						"Plugin"

#define ORS_PLUGIN_NAME					"Name"

#define ORS_OBJECT						"Object"

#define ORS_PLUGIN_VERSION				"Version"

#define ORS_PLUGIN_PROVIDER				"Provider"

#define ORS_PLUGIN_ID					"ID"


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

bool g_lazyLoad = true;

void orsXRegisterService::readXML()
{
	//1)加载XML插件树
	stdString  filename = getPluginDir().c_str();
	filename += "/";
	filename += getPluginTreeFileName().c_str();

	FILE *fp;
	if( g_lazyLoad )
		fp = fopen(filename.c_str(), "rb" );
	else
		fp = NULL;

	if( fp != NULL )
	{
		//得到文件大小
		fseek(fp,0,SEEK_END);
		long file_len = ftell(fp);

		fseek(fp,0,SEEK_SET);

		//读取信息
		char *buf = new char[file_len+10];
		memset(buf,0,file_len+10);
		fread(buf,file_len,sizeof(char),fp);

//		printf(buf);

		//导入信息
		ref_ptr<orsIPropertySerialize> serializer = new orsXXMLSerialize;
		ref_ptr<orsIProperty> rootP = new orsXProperty;

		serializer->import( buf, file_len+10, rootP.get() );
		rootP->getAttr( "OpenRS_Plugin_System", m_objTree);

		delete buf;
		fclose(fp);
	}
}

orsXRegisterService::orsXRegisterService()
{
	m_runningPlugin = NULL;
	m_objTree = new orsXProperty;
	m_needSave = false;
}

orsXRegisterService::~orsXRegisterService()
{

}

ors_string orsXRegisterService::getPluginTreeFileName()
{
#ifdef _WIN32
	return "plugintree.xml";
#elif WIN32
	return "plugintree.xml";
#elif WIN64
	return "plugintree.xml";
#elif _WIN64
	return "plugintree.xml";
#else
	return "plugintree_linux.xml";
#endif
}

void orsXRegisterService::writeXML()
{
	//1)加载XML插件树
	stdString  filename = getPluginDir().c_str();
	filename += "/";
	filename += getPluginTreeFileName().c_str();

	FILE *fp = fopen(filename.c_str(),"wb");
	if(fp != NULL)
	{
		//导出信息
		char *buf = NULL;
		long len = 0;
		ref_ptr<orsIPropertySerialize> serializer = new orsXXMLSerialize;
		ref_ptr<orsIProperty> rootP = new orsXProperty;

		rootP->addAttr( "OpenRS_Plugin_System", m_objTree );

		serializer->outport(buf, &len, rootP.get() );

		fwrite(buf,len,sizeof(char),fp);

		if( NULL != buf )
			delete buf;

		fclose(fp);
	}
}

void orsXRegisterService::updateDescFromPluginProperty(const orsIProperty *pluginProperty)
{
	orsString	pluginName;
	orsString	pluginVersion;
	orsString	pluginID;
	orsString	pluginProvider;

	orsArray<ref_ptr<orsIProperty> > objNodes;

	pluginProperty->getAttr(ORS_PLUGIN_NAME, pluginName);
	pluginProperty->getAttr(ORS_PLUGIN_VERSION, pluginVersion);
	pluginProperty->getAttr(ORS_OBJECT, objNodes );
	pluginProperty->getAttr(ORS_PLUGIN_PROVIDER, pluginProvider);
	pluginProperty->getAttr(ORS_PLUGIN_ID, pluginID );

	//将插件信息放入插件信息队列中
	{
		orsPluginInfo pluginInfo;
		pluginInfo.pluginName = pluginName;
		pluginInfo.pluginVersion = pluginVersion;
		pluginInfo.pluginProvider = pluginProvider;
		pluginInfo.pluginID = pluginID;

		// 删除原信息
		std::vector<orsPluginInfo>::iterator iter;

		int n = m_registeredPluginInfos.size();

		for (iter = m_registeredPluginInfos.begin(); iter != m_registeredPluginInfos.end(); iter++)
		{
			if (iter->pluginName == pluginName)
			{
				iter = m_registeredPluginInfos.erase(iter);
				break;
			}
		}

		// 插入信息
#if  defined(_MSC_VER) && _MSC_VER < 1800
		m_registeredPluginInfos.push_back(pluginInfo);
#else
		m_registeredPluginInfos.push_back(std::move(pluginInfo) );
#endif
	}

	int n = objNodes.size();
	for(int j=0;j<n;j++)
	{
		orsIProperty *objNode = objNodes[j].get();
		ref_ptr<orsXObjectDesc> objDesc = new orsXObjectDesc;
		
		objNode->getAttr(ORS_OBJECT_NAME, objDesc->m_name);
		objNode->getAttr(ORS_OBJECT_ID, objDesc->m_id );
		objNode->getAttr(ORS_OBJECT_DESC, objDesc->m_desc );
		objNode->getAttr(ORS_OBJECT_PARENT_INTERACE, objDesc->m_interfaceNames );

		//取消对象属性序列化存储
		//objNode->getAttr(ORS_OBJECT_PROPERTYS,objDesc->m_propertys);
		objDesc->m_pluginName = pluginName;

		if( !objDesc->m_id.isEmpty() )	{
			m_objDescMap.erase(	objDesc->m_id.c_str() );
			m_objDescMap.insert(
				orsObjDescMap::value_type(
				objDesc->m_id.c_str(), objDesc.get()) );
		}
		else	{
			assert( !objDesc->m_id.isEmpty() );
		}
	}
}

ref_ptr<orsILogService> orsXRegisterService::getLogService()
{
	return _getPlatform()->getLogService();
}

bool orsXRegisterService::startup(orsIPlatform *platform)
{
	getLogService()->debug( "orsBase", "orsXRegisterService: startup" );

	//////////////////////////////////////////////////////////////////////////

	//保存平台指针
	m_platform = platform;

	//加载XML插件树
	readXML();

	//判断是否是第一次加载
	ors_string isfirst = "false";

	m_objTree->getAttr(ORS_PLUGIN_TREE_FIRST_START_UP,isfirst);
	if(isfirst == "false"){

		getLogService()->debug( "orsBase", "Scan Plugins" );

		//扫描目录
		scanDir( getPluginDir().c_str(), false );

		//加入第一次加载标示
		m_objTree->addAttr(ORS_PLUGIN_TREE_FIRST_START_UP,"true");

		//需要保存
		setSaveFlag();
	}

	//生成对象描述Map
	orsArray<ref_ptr<orsIProperty> > pluginNodes;
	m_objTree->getAttr( ORS_PLUGIN, pluginNodes);

	int nPluginNodes = pluginNodes.size();

	//printf("nPluginNodes = %d\n", nPluginNodes);
	for(unsigned i=0;i< nPluginNodes;i++)
	{
		updateDescFromPluginProperty( pluginNodes[i].get() );
	}


	return true;
}


void orsXRegisterService::shutdown()
{
	//if(m_needSave)
	//	writeXML();

	getLogService()->debug( "orsBase", "orsXRegisterService: shutdown" );

	m_creatorMap.clear();

	m_objTree = NULL;

	m_runningPlugin = NULL;

	m_objDescMap.clear();

	m_loadedPlugins.clear();

	m_serializer = NULL;

}

bool orsXRegisterService::isok()
{
	return true;
}

//
// 利用对象创建器注册对象,
//
//

extern const orsChar *g_curModuleName;

bool orsXRegisterService::registerObject(ORS_OBJECT_CREATOR_FUN objCreator )
{
	// 创建临时对象
	ref_ptr<orsIObject> obj = objCreator(true);

	if( NULL == obj.get() ) {
 		m_platform->logPrint( ORS_LOG_WARNING, "Failed Creating Object!");
 		m_platform->getLastErrorService()->setErrorInfo(0, "Failed Creating Object!");
		return false;
	}

	// 取对象ID
	ors_string objName = obj->getClassName();
	ors_string objID = obj->getClassID();
	if( objID.isEmpty() )
		return false;

	// 对象创建map
	m_creatorMap.erase( objID.c_str() );
	m_creatorMap.insert( orsObjCreatorMap::value_type( objID.c_str(), objCreator ) );

	if( NULL == m_runningPlugin.get() && NULL != g_curModuleName )
	{
		orsObjDescMap::iterator iter;

		// 是否已经注册过
		iter = m_objDescMap.find( objID.c_str() );

		// 注册module中的对象
		if( iter == m_objDescMap.end() )
		{
			// 内部对象注册，所以在对象描述树上没有找到
			ref_ptr<orsXObjectDesc> objDesc = new orsXObjectDesc;
			
			// 对象名，由接口名和实现名构成
			objDesc->m_name = objName;
			objDesc->m_id = objID;
			objDesc->m_desc= obj->getClassDesc();
			//if(m_runningPlugin != NULL)
			
			objDesc->m_pluginName = g_curModuleName;
			
			// 接口名
			objDesc->m_interfaceNames = obj->getInterfaceNames();
			objDesc->m_propertys = obj->getProperty();
			
			m_objDescMap.insert(
				orsObjDescMap::value_type( objID.c_str(), objDesc.get() ) );
		}
	}

	if( m_runningPlugin != NULL ) {
		m_runningPlugin->m_objectCreators.erase( objID.c_str());

		//printf("m_runningPlugin: erase obj\n");

 		m_runningPlugin->m_objectCreators.insert(
 			std::map<stdString , ORS_OBJECT_CREATOR_FUN>::value_type( objID.c_str(), objCreator) );
	}

	return true;
}


orsIObject *orsXRegisterService::createObject(const orsChar * objectID)
{
	orsIObject* obj = NULL;

	if (NULL == objectID) 
		return NULL;

	orsObjCreatorMap::iterator iter = m_creatorMap.find( objectID );

	if(iter == m_creatorMap.end()){
		//如果没有找到创建者
		//判断是否有对象的描述
		orsObjDescMap::iterator iter;

		iter = m_objDescMap.find( objectID );
		//如果没有描述，返回NULL
		if(iter == m_objDescMap.end()) {
			orsString msg = "Fail to create object: ";
			msg = msg + objectID;

			getLogService()->error( "orsBase", msg );

			return NULL;
		}
		else{
			ref_ptr<orsIObjectDesc> description = iter->second;
			ors_string pluginName = description->getPluginName();

			if( !addPlugin( pluginName.c_str(), false ) ) {
				orsString msg = "Fail to create object: ";
				msg = msg + objectID;
				getLogService()->error( "orsBase", msg );

				return NULL;
			}

			//再次找到创建者，OK
			orsObjCreatorMap::iterator iter2 = m_creatorMap.find( objectID );
			if(iter2 == m_creatorMap.end())
				return NULL;
			ORS_OBJECT_CREATOR_FUN creator = iter2->second;
			obj = creator(false);
		}
	}else {
		//如果找到创建者，OK
		ORS_OBJECT_CREATOR_FUN creator = iter->second;
		obj = creator(false);
	}

	return obj;
}

ors_string orsXRegisterService::getPluginDir()
{
	stdString  dir = getExeDir() + "/" + "plugins";
	return dir.c_str();
}

ors_string orsXRegisterService::getPluginNameByPath(const orsChar * pluginPath)
{
	stdString  pluginName;
	stdString  pluginDir = getPluginDir().c_str();
	stdString  pluginPathSTL = pluginPath;
	for(unsigned i=pluginDir.length()+1;i<pluginPathSTL.size();i++)
	{
		pluginName += pluginPathSTL[i];
	}

	return pluginName.c_str();
}

ors_string orsXRegisterService::getPluginPathByName(const orsChar * pluginName)
{
	stdString  pluginDir = getPluginDir().c_str();
	stdString  name = pluginDir + "/" + pluginName;
	return name.c_str();
}

bool orsXRegisterService::addPlugin(const orsChar * pluginName, bool isForced)
{
	//得到插件的路径名
	orsString pathName = getPluginPathByName(pluginName);

	//判断是否插件被加载过
	std::map<stdString ,ref_ptr<orsPluginDLL> >::iterator iter =
		m_loadedPlugins.find(pluginName);

	if(iter != m_loadedPlugins.end()){
		return false;
	}

	// 用于在release版防止崩溃，在debug中为定位错误，最好不要捕获异常
#ifndef _DEBUG
	try
#endif	
	{
		//创建插件
		ref_ptr<orsPluginDLL> plugin = new orsPluginDLL( pluginName, pathName.c_str() );
		
		//设置该插件为当前插件
		m_runningPlugin = plugin;
		
		// 加载插件
		if( plugin->load(m_platform) )
		{

			//放入到插件实体map中
			m_loadedPlugins.erase( pluginName );
			m_loadedPlugins.insert( orsPluginDLLMap::value_type( pluginName, m_runningPlugin) );
			
			//更新XML插件树信息
			if( isForced ){
				//如果是强制加载
				//如果之前插件树已经存在该节点，则删除
				if(pluginExistInTree( pluginName) )
					unSerializePlugin(pluginName);
				
				serializePlugin( m_objTree.get(), plugin.get() );
			}else{
				//如果是非强制加载
				//如果之前插件树已经存在该节点，则不用管
				//如果没有存在，加载
				if( !pluginExistInTree(pluginName) )
					serializePlugin( m_objTree.get(), plugin.get() );
			}

			return true;
		}
		else	{
			orsString msg = "Fail to add plugin: ";
			msg = msg + pluginName;
			
			getLogService()->warn( "orsBase", msg );
			
			return false;
		}
	}
#ifndef _DEBUG
	catch ( ... )
	{
		orsString msg = "Exception during adding plugin: ";
		msg = msg + pluginName;

		getLogService()->error( "orsBase", msg );
	}
#endif

	return true;
}

//目前不提供动态卸载方法
bool orsXRegisterService::removePlugin(const orsChar * pluginName,bool isForced)
{
	m_platform->getLastErrorService()->setErrorInfo(0,"remove plugin isn't support");
	return false;
}


bool orsXRegisterService::scanDir(const orsChar * dirName,bool isForced)
{
	ors_string dir = getPluginDir();
	
	//printf("scan dir :%s\n",dir.c_str());

	//找到目录下所有的dll
	orsArray<orsString> allDlls;
	
	findAllFiles(dir,PLUGIN_EXTENSION, allDlls );

	//加载插件
	for(unsigned i=0;i<allDlls.size();i++)
	{
		ors_string& pathName = allDlls[i];

		if( orsString::findSubStr_i( pathName, _T(".svn") ) )
			continue;

		ors_string pluginName = getPluginNameByPath( pathName.c_str() );

		if (pluginName.c_str() != orsString::findSubStr_i(pluginName, _T("ors")))
			continue;

		const  char* name = pluginName.c_str();
		addPlugin( pluginName.c_str(), isForced);
		//printf("plugin dll load :%s\n",pluginName.c_str());
	}

	return true;
}

ref_ptr<orsIObjectDesc>  orsXRegisterService::getObjectDescByID(const orsChar *objId )
{
	orsObjDescMap::iterator iter;
	iter = m_objDescMap.find( objId );
	if(iter != m_objDescMap.end())
		return iter->second;
	else
		return NULL;
}

orsArray<ref_ptr<orsIObjectDesc> > orsXRegisterService::getAllObjectDescs()
{
	orsArray<ref_ptr<orsIObjectDesc> > descriptions;
	orsObjDescMap::iterator iter;

	for(iter = m_objDescMap.begin();iter != m_objDescMap.end();iter++)
	{
		descriptions.push_back(iter->second);
	}
	return descriptions;
}

orsArray<ref_ptr<orsIObjectDesc> > orsXRegisterService::getObjectDescsByInterface( const orsChar *interfaceName)
{
	orsArray<ref_ptr<orsIObjectDesc> > descriptions;

	orsObjDescMap::iterator iter;
	for(iter = m_objDescMap.begin(); iter != m_objDescMap.end();iter++)
	{
		orsIObjectDesc *pDesc = iter->second.get();

		orsArray<ors_string> interfaceNames = pDesc->getInterfaceNames();
		for(unsigned i=0;i<interfaceNames.size();i++)
		{
			ors_string &name = interfaceNames[i];
			if(name == interfaceName){
				descriptions.push_back( pDesc );
				break;
			}
		}
	}
	return descriptions;
}

void orsXRegisterService::serializePlugin( orsIProperty *pluginTree, orsPluginDLL *plugin)
{
	stdString  msg = "Serialize Plugin : " + plugin->m_pluginName;

	getLogService()->debug( "orsBase", msg.c_str() );

	//插件本身信息
	const orsChar * name = plugin->m_pluginName.c_str();
	const orsObjCreatorMap &objectCreators = plugin->m_objectCreators;

	//序列化名字
	ref_ptr<orsIProperty> pluginNode = new orsXProperty;
	pluginNode->addAttr( ORS_PLUGIN_NAME, name );
	pluginNode->addAttr(ORS_PLUGIN_ID,plugin->m_plugin->getID());

	//插件版本号,ID,提供者
	pluginNode->addAttr( ORS_PLUGIN_VERSION,plugin->m_plugin->getVersion());
	pluginNode->addAttr(ORS_PLUGIN_PROVIDER,plugin->m_plugin->getProvider());

	//序列化objects
	for(orsObjCreatorMap::const_iterator iter = objectCreators.begin(); iter != objectCreators.end(); iter++)
	{
		ORS_OBJECT_CREATOR_FUN creator = iter->second;

		ref_ptr<orsIObject> obj = creator(true);

		serializeObject( pluginNode.get(), obj.get() );
	}

	//加入插件
	pluginTree->addAttr( ORS_PLUGIN, pluginNode );

	//更新对象描述表
	updateDescFromPluginProperty( pluginNode.get() );

	//设置保存标签
	setSaveFlag();
}

void orsXRegisterService::unSerializePlugin(const orsChar * pluginName)
{
	orsString msg = "unSerialize Plugin : ";
	msg += pluginName;

	getLogService()->debug( "orsBase", msg.c_str() );

	//找到该插件
	std::map<stdString ,ref_ptr<orsPluginDLL> >::iterator iter;
	iter = m_loadedPlugins.find( pluginName );
	if(iter == m_loadedPlugins.end())
		return;

	ref_ptr<orsPluginDLL> plugin = iter->second;

	//去除插件实体map中插件
	m_loadedPlugins.erase( pluginName );

	//去除该插件的对象工厂
	for(orsObjCreatorMap::iterator iter2 = plugin->m_objectCreators.begin();
		iter2 != plugin->m_objectCreators.end();iter2++)
	{
		stdString  name = iter2->first;
		m_creatorMap.erase(name);
		m_objDescMap.erase(name);
	}
}

void orsXRegisterService::serializeObject( orsIProperty *pluginNode, orsIObject *obj)
{
	ref_ptr<orsIProperty> objNode = new orsXProperty;
	assert(objNode != NULL);

	// 序列化对象信息
	objNode->addAttr( ORS_OBJECT_NAME, obj->getClassName().c_str() );
	objNode->addAttr(ORS_OBJECT_ID, obj->getClassID().c_str() ); 
	objNode->addAttr( ORS_OBJECT_DESC, obj->getClassDesc().c_str() );

	orsArray<ors_string> interfaces = obj->getInterfaceNames();
	for(unsigned i=0;i<interfaces.size(); i++)
	{
		objNode->addAttr(ORS_OBJECT_PARENT_INTERACE, interfaces[i].c_str() );
	}

	//ref_ptr<orsIProperty> ps = obj->getProperty();

	//gw修改2008.11.11,不实现属性序列化
	//if( obj->getProperty() != NULL )
	//	objNode->addAttr(ORS_OBJECT_PROPERTYS,obj->getProperty());

	pluginNode->addAttr( ORS_OBJECT, objNode);

	//需要重新保存
	m_needSave = true;
}

bool orsXRegisterService::pluginExistInTree(const orsChar * pluginName)
{
	orsArray<ref_ptr<orsIProperty> > pluginNodes;

	m_objTree->getAttr(ORS_PLUGIN,pluginNodes);

	for(unsigned i=0;i<pluginNodes.size();i++)
	{
		ref_ptr<orsIProperty> pluginNode = pluginNodes[i];

		orsString name;

		pluginNode->getAttr(ORS_PLUGIN_NAME,name);

		if(name == pluginName)
			return true;
	}

	return false;
}

std::vector<orsPluginInfo> &orsXRegisterService::getPluginInfos()
{
	return m_registeredPluginInfos;
}

bool orsXRegisterService::getPluginInfo(const orsChar *pluginName,orsPluginInfo &info)
{
	for ( unsigned i=0;i<m_registeredPluginInfos.size();i++)
	{
		orsPluginInfo &_info = m_registeredPluginInfos[i];
		if(_info.pluginName == pluginName)
		{
			info = _info;
			return true;
		}
	}

	return false;
}

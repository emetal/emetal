/********************************************************************
	created:	2008/09/20
	created:	20:9:2008   17:31
	filename: 	E:\OpenRS\DESKTOP\src\orsBase\orsIRegisterServiceImp.cpp
	file path:	E:\OpenRS\DESKTOP\src\orsBase
	file base:	orsIRegisterServiceImp
	file ext:	cpp
	author:		Guo Wei

	modifications:
				2008��9��21�� ������־��by jws

	purpose:
				ʵ�ֲ���ĵǼǷ���

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

//��������л��õı�ǩ

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
	//1)����XML�����
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
		//�õ��ļ���С
		fseek(fp,0,SEEK_END);
		long file_len = ftell(fp);

		fseek(fp,0,SEEK_SET);

		//��ȡ��Ϣ
		char *buf = new char[file_len+10];
		memset(buf,0,file_len+10);
		fread(buf,file_len,sizeof(char),fp);

//		printf(buf);

		//������Ϣ
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
	//1)����XML�����
	stdString  filename = getPluginDir().c_str();
	filename += "/";
	filename += getPluginTreeFileName().c_str();

	FILE *fp = fopen(filename.c_str(),"wb");
	if(fp != NULL)
	{
		//������Ϣ
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

	//�������Ϣ��������Ϣ������
	{
		orsPluginInfo pluginInfo;
		pluginInfo.pluginName = pluginName;
		pluginInfo.pluginVersion = pluginVersion;
		pluginInfo.pluginProvider = pluginProvider;
		pluginInfo.pluginID = pluginID;

		// ɾ��ԭ��Ϣ
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

		// ������Ϣ
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

		//ȡ�������������л��洢
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

	//����ƽָ̨��
	m_platform = platform;

	//����XML�����
	readXML();

	//�ж��Ƿ��ǵ�һ�μ���
	ors_string isfirst = "false";

	m_objTree->getAttr(ORS_PLUGIN_TREE_FIRST_START_UP,isfirst);
	if(isfirst == "false"){

		getLogService()->debug( "orsBase", "Scan Plugins" );

		//ɨ��Ŀ¼
		scanDir( getPluginDir().c_str(), false );

		//�����һ�μ��ر�ʾ
		m_objTree->addAttr(ORS_PLUGIN_TREE_FIRST_START_UP,"true");

		//��Ҫ����
		setSaveFlag();
	}

	//���ɶ�������Map
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
// ���ö��󴴽���ע�����,
//
//

extern const orsChar *g_curModuleName;

bool orsXRegisterService::registerObject(ORS_OBJECT_CREATOR_FUN objCreator )
{
	// ������ʱ����
	ref_ptr<orsIObject> obj = objCreator(true);

	if( NULL == obj.get() ) {
 		m_platform->logPrint( ORS_LOG_WARNING, "Failed Creating Object!");
 		m_platform->getLastErrorService()->setErrorInfo(0, "Failed Creating Object!");
		return false;
	}

	// ȡ����ID
	ors_string objName = obj->getClassName();
	ors_string objID = obj->getClassID();
	if( objID.isEmpty() )
		return false;

	// ���󴴽�map
	m_creatorMap.erase( objID.c_str() );
	m_creatorMap.insert( orsObjCreatorMap::value_type( objID.c_str(), objCreator ) );

	if( NULL == m_runningPlugin.get() && NULL != g_curModuleName )
	{
		orsObjDescMap::iterator iter;

		// �Ƿ��Ѿ�ע���
		iter = m_objDescMap.find( objID.c_str() );

		// ע��module�еĶ���
		if( iter == m_objDescMap.end() )
		{
			// �ڲ�����ע�ᣬ�����ڶ�����������û���ҵ�
			ref_ptr<orsXObjectDesc> objDesc = new orsXObjectDesc;
			
			// ���������ɽӿ�����ʵ��������
			objDesc->m_name = objName;
			objDesc->m_id = objID;
			objDesc->m_desc= obj->getClassDesc();
			//if(m_runningPlugin != NULL)
			
			objDesc->m_pluginName = g_curModuleName;
			
			// �ӿ���
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
		//���û���ҵ�������
		//�ж��Ƿ��ж��������
		orsObjDescMap::iterator iter;

		iter = m_objDescMap.find( objectID );
		//���û������������NULL
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

			//�ٴ��ҵ������ߣ�OK
			orsObjCreatorMap::iterator iter2 = m_creatorMap.find( objectID );
			if(iter2 == m_creatorMap.end())
				return NULL;
			ORS_OBJECT_CREATOR_FUN creator = iter2->second;
			obj = creator(false);
		}
	}else {
		//����ҵ������ߣ�OK
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
	//�õ������·����
	orsString pathName = getPluginPathByName(pluginName);

	//�ж��Ƿ��������ع�
	std::map<stdString ,ref_ptr<orsPluginDLL> >::iterator iter =
		m_loadedPlugins.find(pluginName);

	if(iter != m_loadedPlugins.end()){
		return false;
	}

	// ������release���ֹ��������debug��Ϊ��λ������ò�Ҫ�����쳣
#ifndef _DEBUG
	try
#endif	
	{
		//�������
		ref_ptr<orsPluginDLL> plugin = new orsPluginDLL( pluginName, pathName.c_str() );
		
		//���øò��Ϊ��ǰ���
		m_runningPlugin = plugin;
		
		// ���ز��
		if( plugin->load(m_platform) )
		{

			//���뵽���ʵ��map��
			m_loadedPlugins.erase( pluginName );
			m_loadedPlugins.insert( orsPluginDLLMap::value_type( pluginName, m_runningPlugin) );
			
			//����XML�������Ϣ
			if( isForced ){
				//�����ǿ�Ƽ���
				//���֮ǰ������Ѿ����ڸýڵ㣬��ɾ��
				if(pluginExistInTree( pluginName) )
					unSerializePlugin(pluginName);
				
				serializePlugin( m_objTree.get(), plugin.get() );
			}else{
				//����Ƿ�ǿ�Ƽ���
				//���֮ǰ������Ѿ����ڸýڵ㣬���ù�
				//���û�д��ڣ�����
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

//Ŀǰ���ṩ��̬ж�ط���
bool orsXRegisterService::removePlugin(const orsChar * pluginName,bool isForced)
{
	m_platform->getLastErrorService()->setErrorInfo(0,"remove plugin isn't support");
	return false;
}


bool orsXRegisterService::scanDir(const orsChar * dirName,bool isForced)
{
	ors_string dir = getPluginDir();
	
	//printf("scan dir :%s\n",dir.c_str());

	//�ҵ�Ŀ¼�����е�dll
	orsArray<orsString> allDlls;
	
	findAllFiles(dir,PLUGIN_EXTENSION, allDlls );

	//���ز��
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

	//���������Ϣ
	const orsChar * name = plugin->m_pluginName.c_str();
	const orsObjCreatorMap &objectCreators = plugin->m_objectCreators;

	//���л�����
	ref_ptr<orsIProperty> pluginNode = new orsXProperty;
	pluginNode->addAttr( ORS_PLUGIN_NAME, name );
	pluginNode->addAttr(ORS_PLUGIN_ID,plugin->m_plugin->getID());

	//����汾��,ID,�ṩ��
	pluginNode->addAttr( ORS_PLUGIN_VERSION,plugin->m_plugin->getVersion());
	pluginNode->addAttr(ORS_PLUGIN_PROVIDER,plugin->m_plugin->getProvider());

	//���л�objects
	for(orsObjCreatorMap::const_iterator iter = objectCreators.begin(); iter != objectCreators.end(); iter++)
	{
		ORS_OBJECT_CREATOR_FUN creator = iter->second;

		ref_ptr<orsIObject> obj = creator(true);

		serializeObject( pluginNode.get(), obj.get() );
	}

	//������
	pluginTree->addAttr( ORS_PLUGIN, pluginNode );

	//���¶���������
	updateDescFromPluginProperty( pluginNode.get() );

	//���ñ����ǩ
	setSaveFlag();
}

void orsXRegisterService::unSerializePlugin(const orsChar * pluginName)
{
	orsString msg = "unSerialize Plugin : ";
	msg += pluginName;

	getLogService()->debug( "orsBase", msg.c_str() );

	//�ҵ��ò��
	std::map<stdString ,ref_ptr<orsPluginDLL> >::iterator iter;
	iter = m_loadedPlugins.find( pluginName );
	if(iter == m_loadedPlugins.end())
		return;

	ref_ptr<orsPluginDLL> plugin = iter->second;

	//ȥ�����ʵ��map�в��
	m_loadedPlugins.erase( pluginName );

	//ȥ���ò���Ķ��󹤳�
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

	// ���л�������Ϣ
	objNode->addAttr( ORS_OBJECT_NAME, obj->getClassName().c_str() );
	objNode->addAttr(ORS_OBJECT_ID, obj->getClassID().c_str() ); 
	objNode->addAttr( ORS_OBJECT_DESC, obj->getClassDesc().c_str() );

	orsArray<ors_string> interfaces = obj->getInterfaceNames();
	for(unsigned i=0;i<interfaces.size(); i++)
	{
		objNode->addAttr(ORS_OBJECT_PARENT_INTERACE, interfaces[i].c_str() );
	}

	//ref_ptr<orsIProperty> ps = obj->getProperty();

	//gw�޸�2008.11.11,��ʵ���������л�
	//if( obj->getProperty() != NULL )
	//	objNode->addAttr(ORS_OBJECT_PROPERTYS,obj->getProperty());

	pluginNode->addAttr( ORS_OBJECT, objNode);

	//��Ҫ���±���
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

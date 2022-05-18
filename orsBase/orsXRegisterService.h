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


//��������
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
	orsString				m_name;			// ��������
	orsString				m_id;			// ����ID, ���ֶ����Ψһ���
	orsString				m_desc;			// ��������
	orsString				m_pluginName;		// �������ڵĲ����dll���ļ���

	ref_ptr<orsIProperty>	m_propertys;		// ��������
	orsArray<orsString>		m_interfaceNames;	// ����ӿ�����
};


typedef std::map<stdString , ref_ptr<orsIObjectDesc> >	orsObjDescMap;


//�㷨ע�����
class orsXRegisterService : public orsIRegisterService,
								public orsIPluginManager,
								public orsObjectBase
{
public:
	orsXRegisterService();
	virtual ~orsXRegisterService();

	//����ӿ�
	//����ʱ������
	virtual bool startup(orsIPlatform *platform);

	//�ر�ʱ������
	virtual void shutdown();

	//�Ƿ������ɹ�
	virtual bool isok();

	//ע���������֮ǰ�У����滻֮ǰ�Ĺ��캯��
	//ע�⣬�ýӿڿɱ�ϵͳ�ڲ����ã�����һ���Ǿ����������
	//��ϵͳ����ʱ��ϵͳ���ö��󽫼��뵽ע������У����������л���
	bool registerObject(ORS_OBJECT_CREATOR_FUN objCreator);

	//�����������isForced���ر��򿪣���ǿ�Ƹ��²��XML��
	bool addPlugin(const orsChar * pluginName,bool isForced = false);

	//�Ƴ���������isForced���ر��򿪣���ǿ�Ƹ��²��XML��
	bool removePlugin(const orsChar * pluginName,bool isForced = false);

	//�õ���������Ϣ��������汾�ţ����汾�ţ�
	std::vector<orsPluginInfo> &getPluginInfos();

	bool getPluginInfo(const orsChar *pluginName,orsPluginInfo &info);

	//scan�ļ����ڲ����еĲ��
	bool scanDir(const orsChar * dirName,bool isForced = false);

	//�õ����Ŀ¼
	orsString getPluginDir();

	//�õ�������ļ�����
	orsString getPluginTreeFileName();


	// ͨ���������Ƶõ�����
	virtual ref_ptr<orsIObjectDesc> getObjectDescByID(const orsChar * objID );

	// �õ����еĶ�������
	virtual orsArray<ref_ptr<orsIObjectDesc> > getAllObjectDescs();

	// �õ�����ĳ���ӿڵ��㷨����
	//!!!!!ע�⣬interfaceClass��interface�������ƣ������ǽӿڵ�����
	virtual orsArray<ref_ptr<orsIObjectDesc> > getObjectDescsByInterface( const orsChar * interfaceName);

	// ��������ʵ��
	virtual orsIObject *createObject( const orsChar *objectID );

protected:
	//�жϲ���Ƿ��ڲ��XML���ϴ���
	bool pluginExistInTree(const orsChar * pluginName);

	//��ȡXML�����
	void writeXML();

	//дXML�����
	void readXML();

	//�õ�����ڵ�
	ref_ptr<orsIProperty> findPluginNode(const orsChar * pluginName);

	//���л����������÷���������ʱ�䣬shutdownʱ�ᱣ��
	void serializePlugin( orsIProperty *pluginTree, orsPluginDLL *plugin);

	//��XML�����Ƴ�һ���������
	void unSerializePlugin( const orsChar * pluginName );

	//���л�����ڵ��㷨����(��serializePlugin����)
	void serializeObject( orsIProperty *pluginNode, orsIObject *obj);

	//ͨ��������Ƶõ�����ľ���·��
	orsString getPluginPathByName(const orsChar * pluginPath);

	//ͨ������·���õ������
	orsString getPluginNameByPath(const orsChar * pathName);

	//���ñ����־����shutupʱ������save
	void setSaveFlag(){m_needSave = true;}

	//��xml�ڵ��ڳ�ȡ���������������뵽����Map��
	void updateDescFromPluginProperty( const orsIProperty *node);

private:
	//���XML�����仯������Ҫ���±���m_keywordList
	bool	m_needSave;

	// �����
	ref_ptr<orsIProperty>	m_objTree;

	// XML���л���
	ref_ptr<orsIPropertySerialize> m_serializer;

	// ��ǰ��־
	ref_ptr<orsILogService> getLogService();

private:
	// �Ѽ��صĲ��(DLL)�г�ȡ��object������ (����������������ָ��)
	orsObjCreatorMap	m_creatorMap;

	//��������Map(����������������)
	orsObjDescMap m_objDescMap;

	// ��ǰ�ڼ��صĲ��,����̽�����ڲ�����Щobject
	ref_ptr<orsPluginDLL>  m_runningPlugin;

	// �Ѽ��صĲ��ʵ�壨dll��map
	orsPluginDLLMap m_loadedPlugins;

	// ע�����������Ϣ
	std::vector<orsPluginInfo> m_registeredPluginInfos;

private:

	//ϵͳƽ̨
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

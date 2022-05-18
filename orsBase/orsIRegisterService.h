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
	//�����������isForced���ر��򿪣���ǿ�Ƹ��²��XML��
	virtual bool addPlugin(const orsChar * pluginName, bool isForced = false) = 0;

	//�Ƴ���������isForced���ر��򿪣���ǿ�Ƹ��²��XML��
	virtual bool removePlugin(const orsChar * pluginName, bool isForced = false) = 0;

	//scan�ļ����ڲ����еĲ��
	virtual bool scanDir(const orsChar * dirName, bool isForced = false) = 0;

	//�õ���������Ϣ��������汾�ţ����汾�ţ�
	virtual std::vector<orsPluginInfo> &getPluginInfos() = 0;
	virtual bool getPluginInfo(const orsChar *pluginName, orsPluginInfo &info) = 0;

	ORS_INTERFACE_DEF(orsIObject, _T("pluginManager"))
};


// ����ע�����
interface orsIRegisterService : public orsIService
{
public:
	//ע����
	virtual bool registerObject( ORS_OBJECT_CREATOR_FUN objCreator ) = 0;

	//�����������isForced���ر��򿪣���ǿ�Ƹ��²��XML��
	virtual bool addPlugin(const orsChar * pluginName,bool isForced = false) = 0;

	//�Ƴ���������isForced���ر��򿪣���ǿ�Ƹ��²��XML��
	virtual bool removePlugin(const orsChar * pluginName,bool isForced = false) = 0;

	// ͨ���������Ƶõ�����
	virtual ref_ptr<orsIObjectDesc> getObjectDescByID(const orsChar * objID ) = 0;

	// �õ����еĶ�������
	virtual orsArray<ref_ptr<orsIObjectDesc> > getAllObjectDescs() =0 ;

	// �õ�����ĳ���ӿڵ��㷨����
	//!!!!!ע�⣬interfaceClass��interface�������ƣ������ǽӿڵ�����
	virtual orsArray<ref_ptr<orsIObjectDesc> > getObjectDescsByInterface( const orsChar * interfaceName) = 0;

	// ��������ʵ��
	virtual orsIObject *createObject( const orsChar *objectID ) = 0;

	//�ӿ�����
	ORS_INTERFACE_DEF( orsIService, _T("register"))
};



#endif

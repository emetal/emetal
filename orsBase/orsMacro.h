#ifndef OPENRS_MARO_H
#define OPENRS_MARO_H

#include "orsTypedef.h"
#include "orsString.h"

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

//
// 接口名由"父接口名" + "." + "本接口名" 构成
//
// 对象都是继承自orsIObject, 如果不重复定义纯虚函数，则会有编译的模糊性问题
//
#define ORS_INTERFACE_DEF(parentInteface, interfaceID) public:\
	virtual void addRef() = 0;\
	virtual void release() = 0;\
	virtual ors_string	getClassID() const { return ors_string(parentInteface::getClassID() + _T(".") + interfaceID);}\
	virtual void* queryInterface(const orsChar *className) const = 0;\
	virtual orsArray<ors_string> getInterfaceNames() const = 0;
	//virtual ors_string	getClassName(){ return ors_string(interfaceName);}

//宏定义，为了实现orsIObject的接口，开发者需要自己实现类的头文件中加入
//ORS_OBJECT_IMP0 : 自己的类直接从orsIObject继承而来，或
//ORS_OBJECT_IMP1 : 自己的类从一个主接口继承而来，或
//ORS_OBJECT_IMP2 : 自己的类不仅实现了一个主接口，还实现了1个副接口，或
//ORS_OBJECT_IMP3 : 自己的类不仅实现了一个主接口，还实现了2个副接口
//一般而言，ORS_OBJECT_IMP1被使用，其他三个宏一般不使用

struct ORS_INTERFACE_INTMAP_ENTRY
{
	const orsChar*	name;
	unsigned long dw;
	void* flag;
};

#define ORS_VTABLE_PACKING	0x10000000
#define ORS_ITL_PACKING 8
#define ORS_VTABLE_PTR(base, derived)	((ors_ptr2int)(static_cast<base*>((derived*)ORS_ITL_PACKING))-ORS_ITL_PACKING)
#define ORS_VTABLE_PTR2(base,base2, derived)	((ors_ptr2int)((base*)(base2*)(derived*)ORS_ITL_PACKING)-ORS_ITL_PACKING)

#define ORS_BEGIN_VTABLE_MAP(derived) public: \
	typedef derived _Derived;\
	static ORS_INTERFACE_INTMAP_ENTRY* _GetEntries() { \
		static const ORS_INTERFACE_INTMAP_ENTRY _entries[] = {

#define ORS_INTERFACE_ENTRY(base) { _T(#base), ORS_VTABLE_PTR(base,_Derived),(void*)1},
#define ORS_INTERFACE_ENTRY2(base, base2) { _T(#base), ORS_VTABLE_PTR2(base,base2,_Derived),(void*)1},

#define ORS_END_VTABLE_MAP {0,0,0}}; return (ORS_INTERFACE_INTMAP_ENTRY*)_entries; } \
	virtual void* queryInterface(const orsChar * className) const \
	{ \
		int i = 0; \
		while (_GetEntries()[i].flag != NULL) {\
			if ( _tcsicmp(_GetEntries()[i].name, className ) == 0) {\
				return (void*)((ors_ptr2int)this + _GetEntries()[i].dw); \
				} i++;\
		} \
		return NULL;\
	}\
	virtual orsArray<ors_string> getInterfaceNames() const \
	{\
		\
		int i = 0; \
		orsArray<orsString> names; \
		while (_GetEntries()[i].name != NULL) { \
			names.push_back(_GetEntries()[i++].name); \
		} \
		return names; \
	}

//
// 对象名由"主接口名" + "." + "具体对象名" 构成
//
#define ORS_OBJECT_DEF_NORMAL(primaryInteface, objID, objName) public: \
	virtual void addRef() {internalAddRef();} \
	virtual void release(){internalRelease();} \
	virtual ors_string	getClassID() const {return ors_string(primaryInteface::getClassID() + _T(".") + objID);}\
	virtual ors_string	getClassName() const {return ors_string(objName);}

#define ORS_OBJECT_DEF_NORMAL_NO_Primary(objID, objName) public: \
	virtual void addRef(){internalAddRef();} \
	virtual void release(){internalRelease();} \
	virtual ors_string	getClassID() const {return ors_string(orsIObject::getClassID() + _T(".") + objID);}\
	virtual ors_string	getClassName() const {return ors_string(objName);}

#define ORS_OBJECT_IMP0(selftype, objID ,objName) \
	ORS_OBJECT_DEF_NORMAL_NO_Primary( objID, objName) \
	ORS_BEGIN_VTABLE_MAP(selftype) \
		ORS_INTERFACE_ENTRY(orsIObject) \
	ORS_END_VTABLE_MAP

#define ORS_OBJECT_IMP1(selftype,primaryInteface, objID, objName) \
	ORS_OBJECT_DEF_NORMAL(primaryInteface, objID,objName) \
	ORS_BEGIN_VTABLE_MAP(selftype) \
		ORS_INTERFACE_ENTRY(orsIObject) \
		ORS_INTERFACE_ENTRY(primaryInteface) \
	ORS_END_VTABLE_MAP

#define ORS_OBJECT_IMP2(selftype,primaryInteface,interface2, objID, objName) \
	ORS_OBJECT_DEF_NORMAL(primaryInteface, objID,objName) \
	ORS_BEGIN_VTABLE_MAP(selftype) \
		ORS_INTERFACE_ENTRY(orsIObject) \
		ORS_INTERFACE_ENTRY(primaryInteface) \
		ORS_INTERFACE_ENTRY(interface2) \
	ORS_END_VTABLE_MAP

#define ORS_OBJECT_IMP3(selftype,primaryInteface,interface2,interface3, objID, objName) \
	ORS_OBJECT_DEF_NORMAL(primaryInteface, objID, objName) \
	ORS_BEGIN_VTABLE_MAP(selftype) \
		ORS_INTERFACE_ENTRY(orsIObject) \
		ORS_INTERFACE_ENTRY(primaryInteface) \
		ORS_INTERFACE_ENTRY(interface2) \
		ORS_INTERFACE_ENTRY(interface3) \
	ORS_END_VTABLE_MAP

#define ORS_OBJECT_IMP4(selftype,primaryInteface,interface2,interface3, interface4, objID, objName) \
	ORS_OBJECT_DEF_NORMAL(primaryInteface, objID, objName) \
	ORS_BEGIN_VTABLE_MAP(selftype) \
	ORS_INTERFACE_ENTRY(orsIObject) \
	ORS_INTERFACE_ENTRY(primaryInteface) \
	ORS_INTERFACE_ENTRY(interface2) \
	ORS_INTERFACE_ENTRY(interface3) \
	ORS_INTERFACE_ENTRY(interface4) \
	ORS_END_VTABLE_MAP

#define ORS_PTR_CAST(T,p) 	      ((p != NULL)? (T*)((p)->queryInterface(#T)) : 0)

//通过宏，来注册插件对象，每个dll都有一个全局instance，自动生成两个函数
//1）得到插件对象函数
//2）得到插件编译期间的版本号，以用作兼容性判断

interface orsIPlatform;

#define ORS_REGISTER_PLUGIN(pluginclass) \
	static pluginclass g_pluginInstance; \
	static orsIPlatform *g_platformInstance;\
	extern "C" PLUGIN_API orsIPlugin *orsGetPluginInstance(orsIPlatform *platformInstance)\
    { 	 g_platformInstance = platformInstance; return &g_pluginInstance;}\
	extern "C" PLUGIN_API const orsChar *orsGetPlatformMajorVersion(){return ORS_PLATFORM_VERSION;}\
	orsIPlatform* getPlatform()	{return g_platformInstance;}


#endif

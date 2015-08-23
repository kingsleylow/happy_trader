#include "global.hpp"

namespace HlpStruct {
	GlobalStorage::GlobalStorage()
	{

	}

	GlobalStorage::~GlobalStorage()
	{

	}

	void GlobalStorage::putGlobal(char const* name, CppUtils::Value const& value)
	{
		LOCK_FOR_SCOPE(*this);
		globalStringMap_m[ name ] = value; 
	}
		
	void GlobalStorage::putGlobal(CppUtils::String const& name, CppUtils::Value const& value)
	{
		LOCK_FOR_SCOPE(*this);
		globalStringMap_m[ name ] = value; 
	}

	void GlobalStorage::putGlobal(int const id, CppUtils::Value const& value)
	{
		LOCK_FOR_SCOPE(*this);
		globalIntMap_m[id] = value;
	}  
	

	CppUtils::Value const& GlobalStorage::getGlobal(char const* name) const
	{
		LOCK_FOR_SCOPE(*this);
		CppUtils::String sname(name);
		map<CppUtils::String, CppUtils::Value>::const_iterator it = globalStringMap_m.find(sname);

		if (it==globalStringMap_m.end())
			THROW(CppUtils::OperationFailed, "exc_GlobalEntryNotFound", "ctx_getGlobalEntry", name);

		return it->second;
	}

	CppUtils::Value const& GlobalStorage::getGlobal(CppUtils::String const& name) const
	{
		return getGlobal(name.c_str());
	}

	CppUtils::Value const& GlobalStorage::getGlobal(int const id) const
	{
		LOCK_FOR_SCOPE(*this);
		map<int, CppUtils::Value>::const_iterator it = globalIntMap_m.find(id);

		if (it==globalIntMap_m.end())
			THROW(CppUtils::OperationFailed, "exc_GlobalEntryNotFound", "ctx_getGlobalEntry", (long)id);

		return it->second;
	}

	CppUtils::Value & GlobalStorage::getGlobal(char const* name) 
	{
		LOCK_FOR_SCOPE(*this);
		CppUtils::String sname(name);
		map<CppUtils::String, CppUtils::Value>::iterator it = globalStringMap_m.find(sname);

		if (it==globalStringMap_m.end()) {
			return globalStringMap_m[ sname ];
		}
		else 
			return it->second;
	}

	CppUtils::Value & GlobalStorage::getGlobal(int const id) 
	{
		LOCK_FOR_SCOPE(*this);
		map<int, CppUtils::Value>::iterator it = globalIntMap_m.find(id);

		if (it==globalIntMap_m.end()) {
			return globalIntMap_m[ id ];
		}
		else 
			return it->second;
	}
		
	void GlobalStorage::deleteGlobal(char const* name)
	{
		LOCK_FOR_SCOPE(*this);
		CppUtils::String sname(name);
		map<CppUtils::String, CppUtils::Value>::iterator it = globalStringMap_m.find(sname);

		if (it==globalStringMap_m.end())
			THROW(CppUtils::OperationFailed, "exc_GlobalEntryNotFound", "ctx_deleteGlobalEntry", name);

		globalStringMap_m.erase(it);
	}

	void GlobalStorage::deleteGlobal(int const id)
	{
		LOCK_FOR_SCOPE(*this);
		map<int, CppUtils::Value>::iterator it = globalIntMap_m.find(id);

		if (it==globalIntMap_m.end())
			THROW(CppUtils::OperationFailed, "exc_GlobalEntryNotFound", "ctx_deleteGlobalEntry", (long)id);

		globalIntMap_m.erase(it);
		
	}

	bool GlobalStorage::isGlobalExist(char const* name)
	{

		LOCK_FOR_SCOPE(*this);
		CppUtils::String sname(name);
		map<CppUtils::String, CppUtils::Value>::iterator it = globalStringMap_m.find(sname);

		if (it==globalStringMap_m.end())
			return false;

		return true;

	}

	bool GlobalStorage::isGlobalExist(int const id)
	{
		LOCK_FOR_SCOPE(*this);

		map<int, CppUtils::Value>::iterator it = globalIntMap_m.find(id);

		if (it==globalIntMap_m.end())
			return false;

		return true;
	}

};
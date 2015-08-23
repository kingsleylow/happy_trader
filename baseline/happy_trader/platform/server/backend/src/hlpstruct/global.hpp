#ifndef _HLPSTRUCT_GLOBAL_INCLUDED
#define _HLPSTRUCT_GLOBAL_INCLUDED

#include "hlpstructdefs.hpp"

namespace HlpStruct {

	/**
	 helper class providing global storage over calls
	*/

	class HLPSTRUCT_EXP GlobalStorage: private CppUtils::Mutex
	{
	public:
		
		// ctor & dtor
		GlobalStorage();

		~GlobalStorage();

		void putGlobal(char const* name, CppUtils::Value const& value);

		void putGlobal(CppUtils::String const& name, CppUtils::Value const& value);
		
		void putGlobal(int const id, CppUtils::Value const& value);

		CppUtils::Value const& getGlobal(char const* name) const;

		CppUtils::Value const& getGlobal(CppUtils::String const& name) const;

		CppUtils::Value const& getGlobal(int const id) const;

		CppUtils::Value & getGlobal(char const* name) ;

		CppUtils::Value & getGlobal(int const id) ;
		
		void deleteGlobal(char const* name);

		void deleteGlobal(int const id);

		bool isGlobalExist(char const* name);

		bool isGlobalExist(int const id);
	private:

		// data storage for thread <onDataArrived> functions
		map<int, CppUtils::Value> globalIntMap_m;

		map<CppUtils::String, CppUtils::Value> globalStringMap_m;

	};
};

#endif
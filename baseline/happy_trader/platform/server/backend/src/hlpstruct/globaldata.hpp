#ifndef _HLPSTRUCT_GLOBALDATA_INCLUDED
#define _HLPSTRUCT_GLOBALDATA_INCLUDED

#include "hlpstructdefs.hpp"


namespace HlpStruct {
	
	
	/**
	 * Helper class to serve as globla data storage
	 * Expect to work in MT enviroment
	 */

	template<class CONTEXT>
	class TradeGlobalStorage;
																	
	template<class CONTEXT>
	class StorageProxy
	{
	public:
		StorageProxy(TradeGlobalStorage<CONTEXT>& base):
			base_m(base)
		 {
			 base_m.lock();
		 }

		~StorageProxy()
		{
			base_m.unlock();
		}

		inline CONTEXT& get(CppUtils::String const& id) 
		{
			 return base_m.getDataRef(id);
		}

		inline CONTEXT const& get(CppUtils::String const& id) const
		{
			 return (CONTEXT const&)base_m.getDataRef(id);
		}

		inline CONTEXT& get() 
		{
			 return base_m.getGlobalDataRef();
		}

		inline CONTEXT const& get() const
		{
			 return (CONTEXT const&)base_m.getGlobalDataRef();
		}


	private:

		TradeGlobalStorage<CONTEXT>& base_m;
	};


	template<class CONTEXT>
	class TradeGlobalStorage: public CppUtils::Mutex {
		friend class StorageProxy<CONTEXT>;
	public:
		TradeGlobalStorage()
		{
		}

		~TradeGlobalStorage()
		{
		}

		StorageProxy<CONTEXT> data()
		{
			 return StorageProxy<CONTEXT>(*this);
		}

	private:

		inline CONTEXT& getDataRef(CppUtils::String const& id)
		{
			return data_m[ id ];
		}

		inline CONTEXT& getGlobalDataRef()
		{
			static CppUtils::String const& global_id = "{11D3C9FF-E05C-4f05-9965-AB2EC4BEE21E}_global_id";
			return data_m[ global_id ];
		}
		
		map<CppUtils::String, CONTEXT> data_m;

	};
};

#endif
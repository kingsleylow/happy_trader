#ifndef _MTQUEUE_HANDLERS_INCLUDED
#define _MTQUEUE_HANDLERS_INCLUDED

#include "mtqueuedefs.hpp"
#include "helperstruct.hpp"
#include <stack>
	using namespace std;


#define HNDLRSTOR "HNDLSTOR"

namespace MtQueue {


	// --------------------
	template<class T>
	class HandlerStorage: public CppUtils::Mutex
	{
		
		public:

		HandlerStorage(char const* name):
				name_m(name)
		{
			LOG(MSG_DEBUG, HNDLRSTOR, "Created handler storage [" << name_m << "]");
		}

		~HandlerStorage()
		{
		
			clearAll();
			LOG(MSG_DEBUG, HNDLRSTOR, "Destroyed handler storage [" << name_m << "]");
		}

		void clearAll()
		{
			LOCK_FOR_SCOPE(*this);
			for(int i = 0; i < handlers_m.size(); i++) {
				if (handlers_m[i].isValid())
					handlers_m[i].clear();
			}

			handlers_m.clear();
			static const stack<int> dummy;
			availableHandlers_m = dummy;

			LOG(MSG_DEBUG, HNDLRSTOR, "Cleared all items in the handler storage [" << name_m << "]");
		}

		

		T HandlerStorage::getItem(int const idx)
		{
				LOCK_FOR_SCOPE(*this)

				if (idx < 0 || idx >= handlers_m.size())
					THROW(CppUtils::OperationFailed, "exc_IndexOutOfrange", "ctx_getItem", idx );

				T& hndlref = handlers_m[idx];

				if (!hndlref.isValid())
					THROW(CppUtils::OperationFailed, "exc_HandlerAlreadyRemoved", "ctx_getItem", idx );

				return hndlref;
		}
		
		// deletes handlers NULL the list
		void freeItem(int const idx)
		{
			LOCK_FOR_SCOPE(*this)

			if (idx < 0 || idx >= handlers_m.size())
				THROW(CppUtils::OperationFailed, "exc_IndexOutOfrange", "ctx_freeItem", idx );

			T& hndlref = handlers_m[idx];

			if (!hndlref.isValid())
				THROW(CppUtils::OperationFailed, "exc_HandlerAlreadyRemoved", "ctx_freeItem", idx );

			// invaidates
			hndlref.clear();
			
			// store for future use
			availableHandlers_m.push(idx);


		}

		// retursn new handler number
		int addItem(HandlerBase const& hndRef)
		{
			LOCK_FOR_SCOPE(*this)
			
			if (!hndRef.isValid())
				THROW(CppUtils::OperationFailed, "exc_NewHandlerNotValid", "ctx_addNewHandler", "" );

			if (availableHandlers_m.empty()) {
				handlers_m.push_back((T&)hndRef);
				int idx = handlers_m.size()-1;

				handlers_m[idx].setId(idx);
				return idx;

			} else {
				int idx = availableHandlers_m.top();

				if (idx < 0 || idx >= handlers_m.size())
					THROW(CppUtils::OperationFailed, "exc_IndexOutOfrange", "ctx_addNewHandler", idx );

				if (handlers_m[idx].isValid())
					THROW(CppUtils::OperationFailed, "exc_PrevHandlerNotRemoved", "ctx_addNewHandler", idx );

				// assign another object
				handlers_m[idx] = (T&)hndRef;
				handlers_m[idx].setId(idx);
				availableHandlers_m.pop();

				return idx;
			}
		}
			

		private:

		
			// list of all handlers
			vector<T> handlers_m;
			
			// available handlers
			stack<int> availableHandlers_m;

			char const* name_m;
	};

}; // end of ns


#endif
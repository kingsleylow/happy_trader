
#include "brokerstruct.hpp"
#include "helperfun.hpp"


namespace BrkLib {
		BrokerContext_String::BrokerContext_String()
		{
		}

		BrokerContext_String::~BrokerContext_String()
		{
		}

		BrokerContext_String::BrokerContext* BrokerContext_String::clone() const
		{
			BrokerContext_String* ptr = new BrokerContext_String();
			ptr->strData_m = strData_m;

			return ptr;
		}; 

		void BrokerContext_String::deallocate()
		{
			delete this;
		};


		

		// -----------------------------
		

};

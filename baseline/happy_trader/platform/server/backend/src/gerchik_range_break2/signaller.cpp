#include "signaller.hpp"
#include "orderdescr.hpp"

namespace AlgLib {

	Signaller::Signaller():
			opIssuePrice_m(-1),
			volume_m(-1),
			orderIsDelayedLocally_m(false),
			orderType_m(BrkLib::OP_DUMMY)
	{
	}

	Signaller::Signaller(CppUtils::String const& strdata)
	{
			fromString(strdata);
	}  

	void Signaller::clear()
	{
		symbol_m = "";
		opIssuePrice_m = -1;
		volume_m = -1;
		orderIsDelayedLocally_m = false;
		orderMethod_m = "";
		orderPlace_m = "";
		additionalOrderInfo_m = "";
		orderUid_m.invalidate();
		orderType_m = BrkLib::OP_DUMMY;
		description_m = "";
	}

	void Signaller::fromString(CppUtils::String const& strdata)
	{
		// parse that data
		// SYMBOL=
		// INSTALL_PRICE=
			
		try {
			CppUtils::StringList tokens;
			CppUtils::tokenize(strdata, tokens, "\n:");

			CppUtils::String var_name, var_value;

			int idx = 0;
			while(idx < tokens.size()) {
				var_name = tokens[idx];
				var_value= tokens[idx+1];


				if (var_name == "SYMBOL") {
					symbol_m = var_value;
				}
				else if (var_name == "ISSUE_PRICE") {
					opIssuePrice_m = atof(var_value.c_str());
				}
				else if (var_name == "ORDER_METHOD") {
					orderMethod_m = var_value;
				}
				else if (var_name == "ORDER_PLACE") {
					orderPlace_m = var_value;
				}
				else if (var_name == "VOLUME") {
					volume_m = atof(var_value.c_str());
				}
				else if (var_name == "REGISTER_LOCALLY") {
					CppUtils::String var_value_u(var_value);
					CppUtils::toupper(var_value_u);
					orderIsDelayedLocally_m = (var_value_u=="TRUE");
				}
				else if (var_name == "ADD_ORDER_INFO") {
					additionalOrderInfo_m = var_value;
				}
				else if (var_name == "UID") {
					orderUid_m.fromString(var_value);
				}
				else if (var_name == "ORDER_TYPE") {
					orderType_m = (BrkLib::OrderType)atoi(var_value.c_str());
				}
				else if (var_name == "DESCRIPTION") {
					description_m = var_value;
				}

				
				idx  = idx + 2;
			}
		}
		catch(...)
		{
			THROW(CppUtils::OperationFailed, "exc_CannotParseSignallerStructure", "ctx_parseFromString", strdata);
		}
	}

};


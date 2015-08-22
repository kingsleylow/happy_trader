#include "signaller.hpp"

namespace AlgLib {

	Signaller::Signaller():
			opSLInstallPrice_LongTrade_m(-1),
			opTPInstallPrice_LongTrade_m(-1),
			opSLInstallPrice_ShortTrade_m(-1),
			opTPInstallPrice_ShortTrade_m(-1),

			opBothSideLongInstallPrice_m(-1),
			opBothSideShortInstallPrice_m(-1),
			volume_m(-1)
	{
	}

	Signaller::Signaller(CppUtils::String const& strdata)
	{
			fromString(strdata);
	}  

	void Signaller::fromString(CppUtils::String const& strdata)
	{
		// parse that data
		// SYMBOL=
		// BOTH_SIDE_LONG_INSTALL_PRICE
		// BOTH_SIDE_SHORT_INSTALL_PRICE
		// VOLUME
		// TP_INSTALL_PRICE_LONG_TRADE
		// TP_INSTALL_PRICE_SHORT_TRADE
		// SL_INSTALL_PRICE_LONG_TRADE
		// SL_INSTALL_PRICE_SHORT_TRADE
		
		CppUtils::StringList tokens;
		CppUtils::tokenize(strdata, tokens, "\n=");

		CppUtils::String var_name, var_value;

		int idx = 0;
		while(idx < tokens.size()) {
			var_name = tokens[idx];
			var_value= tokens[idx+1];


			if (var_name == "SYMBOL") {
				symbol_m = var_value;
			}
			else if (var_name == "BOTH_SIDE_LONG_INSTALL_PRICE") {
				opBothSideLongInstallPrice_m = atof(var_value.c_str());
			}
			else if (var_name == "BOTH_SIDE_SHORT_INSTALL_PRICE") {
				opBothSideShortInstallPrice_m = atof(var_value.c_str());
			}
			else if (var_name == "VOLUME") {
				volume_m = atof(var_value.c_str());
			}
			else if (var_name == "TP_INSTALL_PRICE_LONG_TRADE") {
				opTPInstallPrice_LongTrade_m = atof(var_value.c_str());
			}
			else if (var_name == "TP_INSTALL_PRICE_SHORT_TRADE") {
				opTPInstallPrice_ShortTrade_m = atof(var_value.c_str());
			}
			else if (var_name == "SL_INSTALL_PRICE_LONG_TRADE") {
				opSLInstallPrice_LongTrade_m = atof(var_value.c_str());
			}
			else if (var_name == "SL_INSTALL_PRICE_SHORT_TRADE") {
				opSLInstallPrice_ShortTrade_m = atof(var_value.c_str());
			}

			idx  = idx + 2;
		}
	}
};


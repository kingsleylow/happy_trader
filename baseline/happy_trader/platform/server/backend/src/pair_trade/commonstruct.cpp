#include "commonstruct.hpp"

namespace AlgLib {

	MultuSymbolRequestEntryList fromJsonString(CppUtils::String const& value)
	{
		MultuSymbolRequestEntryList result;

		/*
		 {

				"RequestList" : [
            {
               "Symbol" : "GAZR-6.12_FT",
               "Volume" : 1,
               "Direction" : "SHORT"
            },
            {
               "Symbol" : "LKOH-6.12_FT",
               "Volume" : 1,
               "Direction" : "LONG"
            }
         ]
				}

		*/
		try {
			stringstream stream;
			stream << value;

			json::Object elemRootFile;
			json::Reader::Read(elemRootFile, stream);

			json::Array& dataList = elemRootFile["RequestList"];

			
			for( json::Array::const_iterator it = dataList.Begin(); it != dataList.End(); it++) {
				json::Object const& reqObject = *it;

				json::String const& symbol = reqObject["Symbol"];
				json::Number const& volume = reqObject["Volume"];
				json::String const& direction = reqObject["Direction"];

				MultuSymbolRequestEntry e;
				if (direction.Value() == "SHORT")
					e.direction_m = BrkLib::TD_SHORT;
				else if (direction.Value() == "LONG")
					e.direction_m = BrkLib::TD_LONG;

				e.volume_m = volume.Value();
				e.symbol_m = symbol.Value();

				result.push_back( e );
			}

		}
		catch (json::Reader::ScanException& e_scan)
		{
			THROW(CppUtils::OperationFailed, "exc_jsonScanException", "ctx_fromJsonString", 
				"Reason: " << e_scan.what() << " Doc offset: " << e_scan.m_locError.m_nDocOffset << 
				" Line offset: " << e_scan.m_locError.m_nLineOffset <<
				" Line: " << e_scan.m_locError.m_nLine);
		}
		catch (json::Reader::ParseException& e)
		{
			THROW(CppUtils::OperationFailed, "exc_jsonParseException", "ctx_fromJsonString", e.what());
		}
		catch (std::runtime_error& e)
		{
			THROW(CppUtils::OperationFailed, "exc_jsonException", "ctx_fromJsonString", e.what());
		}
		


		return result;
	}

} // end of ns
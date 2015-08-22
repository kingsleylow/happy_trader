#ifndef _BACKEND_INQUEUE_UTILS_INCLUDED
#define _BACKEND_INQUEUE_UTILS_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "../brklib/brklib.hpp"


namespace Inqueue {
	class SimulHistoryContext;

	void requestRunExternalPackageHelper(
		CppUtils::String const& package_name,
		bool const do_not_wait_response,
		bool &result,
		CppUtils::String& returned_log
	);

	bool loadDataFromMasterHelper(
			HlpStruct::ProviderSymbolList const& providerSymbolList,
			CppUtils::String const& dataProfile,
			bool const keepCacheAlive,
			double const& beginTime = -1,
			double const& endTime = -1
	);

	void serializeSimulationProfile( BrkLib::SimulationProfileData const& simProfile, HlpStruct::XmlParameter &matadata);

	void deserializeSimulationProfile( BrkLib::SimulationProfileData& simProfile, HlpStruct::XmlParameter const& matadata);

	void prepareSimulationTicksHelper(
		Inqueue::SimulHistoryContext &simhistcontext, 
		BrkLib::BrokerBase::ProviderSymbolCookieMap &providerSymbolCookieMap,
		BrkLib::SimulationProfileData const& simulationProfileStorage,
		double const& startSimulationTime,
		double const& endSimulationTime,
		CppUtils::String const& idString
	);

	void initProviderSimulationListFromParameters(
		BrkLib::SimulationProfileData const& simulationProfileStorage,
		CppUtils::DoubleList& provSymbSimulationSpreadList, 
		HlpStruct::ProviderSymbolList& provSymbSimulationList
	);
	



};

#endif
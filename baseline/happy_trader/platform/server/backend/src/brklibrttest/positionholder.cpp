#include "positionholder.hpp"
#include "implement.hpp"
#include "rtbroker.hpp"

namespace BrkLib {
PositionHolder::PositionHolder(OperationProcessorCallBack& processorCallback, BrokerEngine& brokerEngine, PositionHolderCallback& callback): 
	processorCallback_m(processorCallback),
	callback_m(callback),
	brokerEngine_m(brokerEngine),
	onlyOnePositionPerSymbol_m(false)
{

}

PositionHolder::~PositionHolder()
{
}

void PositionHolder::initialize(bool const onlyOnePositionPerSymbol)
{
	onlyOnePositionPerSymbol_m = onlyOnePositionPerSymbol;
}

CppUtils::Uid PositionHolder::createPosition(
	CppUtils::String const& provider, 
	CppUtils::String const& symbol, 
	TradeDirection const direction, 
	double const& tradeTime, 
	double const& tradePrice, 
	double const& volume,
	BrokerSession* brkSess,
	OrderOperation const& oper
)
{
	//LOCK_FOR_SCOPE(*this)

	PositionForSymbol position;
	PositionEntry entry(tradeTime, tradePrice, volume);

	position.opens_m.push_back(entry);
	position.imbalanceVolume_m = volume;
	position.direction_m = direction;
	position.brkSession_m = brkSess;

	callback_m.onPositionUpdate(provider, symbol, position );

	// insert	new
	openPositions_m[provider][symbol].positionsMap_m[position.positionUid_m] = position;

	OperationStructureExecute ox(provider, symbol, tradeTime, tradePrice, volume, brkSess, oper.affectedPositionId_m, direction, PTA_Create);
	processorCallback_m.onExecutionProcedure(ox, oper);

	return position.positionUid_m;

}

void PositionHolder::increasePosition(
	PositionForSymbol& cur_pos,
	CppUtils::String const& provider, 
	CppUtils::String const& symbol, 
	CppUtils::Uid const& positionId,  
	double const& tradeTime, 
	double const& tradePrice, 
	double const& volume,
	BrokerSession* brkSess,
	OrderOperation const& oper,
	TradeDirection const& direction
)
{
	//LOCK_FOR_SCOPE(*this);


	//PositionsDataMap& openPosMap = resolveOpenPositionMap(provider, symbol); 
	PositionEntry entry(tradeTime, tradePrice, volume);

	//PositionForSymbol& cur_pos = resolvePositionInMap(openPosMap, positionId);
	cur_pos.opens_m.push_back(entry);
	cur_pos.imbalanceVolume_m += volume;
	cur_pos.brkSession_m = brkSess;

	callback_m.onPositionUpdate(provider, symbol, cur_pos );

	OperationStructureExecute ox(provider, symbol, tradeTime, tradePrice, volume, brkSess, oper.affectedPositionId_m, direction, PTA_Increase);
	processorCallback_m.onExecutionProcedure(ox, oper);

	

}

void PositionHolder::decreasePosition(
	PositionsDataMap& openPosMap,
	PositionForSymbol& cur_pos,
	CppUtils::String const& provider, 
	CppUtils::String const& symbol,  
	CppUtils::Uid const& positionId, 
	double const& tradeTime, 
	double const& tradePrice, 
	double const& volume,
	BrokerSession* brkSess,
	OrderOperation const& oper,
	TradeDirection const& direction
)
{
	//LOCK_FOR_SCOPE(*this);


	//PositionsDataMap& openPosMap = resolveOpenPositionMap(provider, symbol); 


	//PositionForSymbol& cur_pos = resolvePositionInMap(openPosMap, positionId);
	double prev_imbalance = cur_pos.imbalanceVolume_m;
	double new_imbalance = prev_imbalance - volume;
	
	Math::DoubleLongHolder<4> prev_imbalance_l(prev_imbalance);
	Math::DoubleLongHolder<4> new_imbalance_l(new_imbalance);

	// 
	
	if (new_imbalance_l.getLongInternalValue() == 0) {
		// closing position
	

		PositionEntry entry(tradeTime, tradePrice, volume);
		cur_pos.applyDecreaseOperation(brkSess, entry, 0, true );


		historyPositions_m[provider][symbol].positionsMap_m[cur_pos.positionUid_m] = cur_pos;
		

		callback_m.onPositionUpdate(provider, symbol, cur_pos );

		// remove later
		removePositionInMap(provider, symbol, openPosMap, positionId);

		OperationStructureExecute ox(provider, symbol, tradeTime, tradePrice, volume, brkSess, oper.affectedPositionId_m, direction, PTA_Decrease);
		processorCallback_m.onExecutionProcedure(ox, oper);

	
	}
	else if (new_imbalance_l.getLongInternalValue() > 0) {
		PositionEntry entry(tradeTime, tradePrice, volume);
		
		cur_pos.applyDecreaseOperation(brkSess, entry, new_imbalance, false );
		callback_m.onPositionUpdate(provider, symbol, cur_pos );

		OperationStructureExecute ox(provider, symbol, tradeTime, tradePrice, volume, brkSess, oper.affectedPositionId_m, direction, PTA_Decrease);
		processorCallback_m.onExecutionProcedure(ox, oper);

	}
	else {
		// new imbalance less then zero

		//1) - close our position
	
		PositionEntry entry(tradeTime, tradePrice, volume);
	
		cur_pos.applyDecreaseOperation(brkSess, entry, 0, true );
		historyPositions_m[provider][symbol].positionsMap_m[cur_pos.positionUid_m] = cur_pos;

		callback_m.onPositionUpdate(provider, symbol, cur_pos );

		
		BrkLib::TradeDirection td = cur_pos.direction_m;

		
		// remove from list
		removePositionInMap(provider, symbol, openPosMap, positionId);

		
		// we decrease (close )position - thus if we have long and decrease actually this is short
		new_imbalance = abs(new_imbalance);
		if (td == TD_SHORT) {
			OperationStructureExecute ox(provider, symbol, tradeTime, tradePrice, prev_imbalance, brkSess, oper.affectedPositionId_m, TD_SHORT, PTA_Decrease);
			processorCallback_m.onExecutionProcedure(ox, oper);

			callback_m.onRequireBuyOperation(provider, symbol, tradePrice, new_imbalance, tradeTime, *brkSess, oper );
		}
		else if (td == TD_LONG) {
			OperationStructureExecute ox(provider, symbol, tradeTime, tradePrice, prev_imbalance, brkSess, oper.affectedPositionId_m, TD_LONG, PTA_Decrease);
			processorCallback_m.onExecutionProcedure(ox, oper);

			callback_m.onRequireSellOperation(provider, symbol, tradePrice, new_imbalance, tradeTime, *brkSess, oper );
		}

		
		// 

		//THROW_BRK(getBrokerEngine().getBroker().getName().c_str(),  "exc_InvalidVolumeOnClosingPosition", "ctx_decreasePosition", CppUtils::double2String(new_imbalance));
	}

}

void PositionHolder::goLong(
		CppUtils::String const& provider, 
		CppUtils::String const& symbol, 
		double const& tradeTime, 
		double const& tradePrice, 
		double const& volume,
		CppUtils::Uid& positionId,
		BrokerSession* brkSess,
		OrderOperation const& oper
)
{

	checkValidity(tradeTime,tradePrice,volume);
	{
		LOCK_FOR_SCOPE(*this);

		
		PositionsDataMap *posDataMap = NULL;
		PositionForSymbol* posForSymbol = NULL;

		resolveOpenPositionMap(provider, symbol, &posDataMap);

		if (posDataMap) {
			resolvePositionInMap(	*posDataMap, positionId, &posForSymbol );
		}

		if (posForSymbol == NULL) {
			// create new	as not exist
	
			positionId = createPosition(provider, symbol, TD_LONG, tradeTime, tradePrice, volume, brkSess, oper );

			
		}
		else {
			// position exists, so need to decrease or increase
			if (posForSymbol->direction_m == TD_LONG) {
			
				increasePosition(*posForSymbol, provider, symbol, positionId, tradeTime, tradePrice, volume, brkSess, oper, TD_LONG );

				
			}
			else if (posForSymbol->direction_m == TD_SHORT)	{
		
				decreasePosition(*posDataMap,*posForSymbol,	provider, symbol, positionId, tradeTime, tradePrice, volume, brkSess, oper, TD_LONG );


			}
			else													   
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidDirectionOfExistingPosition", "ctx_goLong", BrkLib::tradeDirection2String(posForSymbol->direction_m));

		}
	}
}

void PositionHolder::goShort(
	 CppUtils::String const& provider, 
	 CppUtils::String const& symbol, 
	 double const& tradeTime, 
	 double const& tradePrice, 
	 double const& volume,
	 CppUtils::Uid& positionId, 
	 BrokerSession* brkSess,
	 OrderOperation const& oper
)
{
	checkValidity(tradeTime,tradePrice,volume);
	{
		LOCK_FOR_SCOPE(*this);
	
		PositionsDataMap *posDataMap = NULL;
		PositionForSymbol* posForSymbol = NULL;

		resolveOpenPositionMap(provider, symbol, &posDataMap);

		if (posDataMap) {
			resolvePositionInMap(	*posDataMap, positionId, &posForSymbol );
		}

		if (posForSymbol == NULL) {
			// create new	as not exist
	
			positionId = createPosition(provider, symbol, TD_SHORT, tradeTime, tradePrice, volume, brkSess, oper );

			
		}
		else {
			// position exists, so need to decrease or increase
			if (posForSymbol->direction_m == TD_SHORT) {
				increasePosition(*posForSymbol, provider, symbol, positionId, tradeTime, tradePrice, volume, brkSess, oper, TD_SHORT );
			}
			else if (posForSymbol->direction_m == TD_LONG) {
				decreasePosition(*posDataMap, *posForSymbol,	provider, symbol, positionId, tradeTime, tradePrice, volume, brkSess, oper,TD_SHORT );
			}
			else
				THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidDirectionOfExistingPosition", "ctx_goLong", BrkLib::tradeDirection2String(posForSymbol->direction_m));

		}
	}
}

void PositionHolder::queryOpenPositions(PositionList& posList) const
{
	LOCK_FOR_SCOPE(*this);
	queryPositions(posList, openPositions_m, STATE_OPEN ); 
}


void PositionHolder::queryHistoryPositions( PositionList& posList) const
{
	LOCK_FOR_SCOPE(*this);
	queryPositions(posList, historyPositions_m, STATE_HISTORY ); 

}


/**
* Helpers
*/

void PositionHolder::queryPositions(PositionList& posList, PositionHolderMap const& posHolderMap, PosState const posState) const
{
		
		for(PositionHolderMap::const_iterator i0 = posHolderMap.begin(); 
			i0 !=	 posHolderMap.end(); i0++) {

				for(map<CppUtils::String, PositionsDataMap >::const_iterator i1 = i0->second.begin(); 
					i1 !=	 i0->second.end(); i1++) {

						PositionsDataMap const& posMap = i1->second;

						for( map<CppUtils::Uid, PositionForSymbol>::const_iterator it = posMap.positionsMap_m.begin(); it != posMap.positionsMap_m.end(); it++)
						{
							PositionForSymbol const& p = it->second;
							Position pos;
							pos.brokerPositionID_m = it->first.toString();
							pos.timeClose_m = p.getCloseTime();
							pos.timeOpen_m = p.getOpenTime();

							pos.avrClosePrice_m = p.getAvrClosePrice();
							pos.avrOpenPrice_m = p.getAvrOpenPrice();
							pos.volumeImbalance_m = p.imbalanceVolume_m;
							pos.volume_m = p.getFullOpenVolume();
							pos.direction_m = p.direction_m;
							pos.provider_m = i0->first;
							pos.symbol_m =  i1->first;
							pos.posState_m = posState;

							posList.push_back( pos );
						}

				}
		}
}

void PositionHolder::resolveOpenPositionMap(CppUtils::String const& provider, CppUtils::String const& symbol, PositionsDataMap **posDataMap)
{

	// find in open positions
	*posDataMap = NULL;
	map< CppUtils::String, map<CppUtils::String, PositionsDataMap > >::iterator it1 = openPositions_m.find(provider);
	if (it1 == openPositions_m.end())
		return;

	map<CppUtils::String, PositionsDataMap >::iterator it2 = it1->second.find(symbol);
	if (it2 == it1->second.end())
		return;

	*posDataMap = &it2->second;
}

PositionsDataMap& PositionHolder::resolveOpenPositionMap(CppUtils::String const& provider, CppUtils::String const& symbol)
{
	// find in open positions
	map< CppUtils::String, map<CppUtils::String, PositionsDataMap > >::iterator it1 = openPositions_m.find(provider);
	if (it1 == openPositions_m.end())
		THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_ProviderNotValid", "ctx_resolvePositionMap", provider);

	map<CppUtils::String, PositionsDataMap >::iterator it2 = it1->second.find(symbol);
	if (it2 == it1->second.end())
		THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_ProviderNotValid", "ctx_resolvePositionMap", symbol);

	return it2->second;
}

void PositionHolder::resolvePositionInMap(PositionsDataMap& posMap, CppUtils::Uid const& positionId, PositionForSymbol** posForSymbol)
{

	*posForSymbol = NULL;

	map<CppUtils::Uid, PositionForSymbol>::iterator it;
	if (onlyOnePositionPerSymbol_m)	{
		// positionId is ignored
		it = posMap.positionsMap_m.begin();
		if (it == posMap.positionsMap_m.end())
			return;
	}
	else {
		if (!positionId.isValid())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidPositionID", "ctx_checkExistPositionInMap", "");
		it = posMap.positionsMap_m.find(positionId);
		if (it == posMap.positionsMap_m.end())
			return;
	}

	*posForSymbol = &it->second;

}


PositionForSymbol& PositionHolder::resolvePositionInMap(PositionsDataMap& posMap, CppUtils::Uid const& positionId)
{
	map<CppUtils::Uid, PositionForSymbol>::iterator it;
	if (onlyOnePositionPerSymbol_m)	{
		// positionId is ignored
		it = posMap.positionsMap_m.begin();
		if (it == posMap.positionsMap_m.end())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_CannotFindPosition_SinglePosMode", "ctx_resolveOpenPosition", "");
	}
	else {
		if (!positionId.isValid())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidPositionID", "ctx_resolveOpenPositions", "");
		it = posMap.positionsMap_m.find(positionId);
		if (it == posMap.positionsMap_m.end())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_CannotFindPosition", "ctx_resolveOpenPositions", positionId.toString());
	}

	return it->second;
}

void PositionHolder::removePositionInMap(CppUtils::String const& provider, CppUtils::String const& symbol, PositionsDataMap& posMap, CppUtils::Uid const& positionId)
{
	map<CppUtils::Uid, PositionForSymbol>::iterator it;
	if (onlyOnePositionPerSymbol_m)	{

		if (posMap.positionsMap_m.size() == 1) {
			posMap.positionsMap_m.clear();
		}
		else
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_CannotFindPosition_SinglePosMode", "ctx_removePositionInMap", "");
	
	}
	else {
		it = posMap.positionsMap_m.find(positionId);
		if (it == posMap.positionsMap_m.end())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_CannotFindPosition", "ctx_removePositionInMap", positionId.toString());

		posMap.positionsMap_m.erase(it);
	}

	if (posMap.positionsMap_m.size() == 0) {

		// empty, clear upper level
		// find in open positions
		map< CppUtils::String, map<CppUtils::String, PositionsDataMap > >::iterator it1 = openPositions_m.find(provider);
		if (it1 == openPositions_m.end())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_ProviderNotValid", "ctx_removePositionInMap", provider);

		map<CppUtils::String, PositionsDataMap >::iterator it2 = it1->second.find(symbol);
		if (it2 == it1->second.end())
			THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_ProviderNotValid", "ctx_removePositionInMap", symbol);

		it1->second.erase(symbol);

		if (it1->second.size() == 0)
			openPositions_m.erase(provider);

	}

}




void PositionHolder::checkValidity(
																	 double const& tradeTime, 
																	 double const& tradePrice, 
																	 double const& volume
																	 ) const
{
	if (tradeTime <=0 || tradePrice <= 0 || volume <= 0)
		THROW_BRK(getBrokerEngine().getBroker().getName().c_str(), "exc_InvalidTradingData", "ctx_checkValidity", "");
}



}; // end of ns
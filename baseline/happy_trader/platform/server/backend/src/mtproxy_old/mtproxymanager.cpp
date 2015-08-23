#include "mtproxymanager.hpp"

#define MTPROXYMANAGER "MTPROXYMANAGER"

namespace MtProxy {

	

	MtProxyManager::MtProxyManager(char const* serverpipe, char const* globaFileName):
		CppUtils::PipeServer( ),
		cmdCounter_m(0),
		globalFileName_m(globaFileName)
	{
		start(false, 1, serverpipe);
		LOG(MSG_INFO, MTPROXYMANAGER, "Proxy manager created and started pipe listener on: " << serverpipe);
	}

  
	MtProxyManager::~MtProxyManager()
	{
		LOG(MSG_INFO, MTPROXYMANAGER, "Proxy manager destroying");
	}
	
	void MtProxyManager::getPendingCommand(CppUtils::String& command, bool& empty)
	{
		LOCK_FOR_SCOPE(pendingCommandsMtx_m);

		empty = pendingCommands_m.empty();
		if (!empty) {
			
			command = pendingCommands_m.front();
			//pendingCommands_m.pop();

			// inc counter
			cmdCounter_m++;
			LOG(MSG_DEBUG, MTPROXYMANAGER, "Extracted command from queue: \"" << command << "\"" << " the queue size: " << (long)pendingCommands_m.size() << " counter: " << cmdCounter_m);


		}
		else 
			empty = true;

	}

	int MtProxyManager::getPendingCounterNumber() const
	{
		LOCK_FOR_SCOPE(pendingCommandsMtx_m);

		return cmdCounter_m;
	}

	void MtProxyManager::deletePendingCommand(bool& empty)
	{
		LOCK_FOR_SCOPE(pendingCommandsMtx_m);

		empty = pendingCommands_m.empty();
		if (!empty) {
			pendingCommands_m.pop();
		}  

		LOG(MSG_DEBUG, MTPROXYMANAGER, "Deleted pending command, reseting counter");

		// reset the counter
		cmdCounter_m = 0;
		
	}

	void MtProxyManager::pushPendingCommand(CppUtils::String& command)
	{
			LOCK_FOR_SCOPE(pendingCommandsMtx_m);
			pendingCommands_m.push( command );	

			// reset the counter
			cmdCounter_m = 0;
			LOG(MSG_INFO, MTPROXYMANAGER, "Pushed command to the queue: \"" << command << "\"" << " the queue size: " << (long)pendingCommands_m.size());

	}

	
	void MtProxyManager::pushCommandResult(char const* remotepipe, char const* cmd)
	{
		CppUtils::String data(cmd != NULL ? cmd : "");
				
		// open pip
		CppUtils::PipeClient client;
		bool result = client.connect(remotepipe);
		if (!result)
				THROW(CppUtils::OperationFailed, "exc_CannotConnectPipe", "ctx_pushCommandResult", cmd );

		CppUtils::MemoryChunk request, response;
		request.fromString(data);
		result = client.makePipeTransaction(request, response);
		if (!result) 
				THROW(CppUtils::OperationFailed, "exc_CannotMakePipeTransaction", "ctx_pushCommandResult", cmd );
		
		if (response.toString()!="OK")
			THROW(CppUtils::OperationFailed, "exc_PipeTransactionReturnedInvalidResponse", "ctx_pushCommandResult", cmd );
	}
  
		
	
	void MtProxyManager::onClientRequestArrived(int const instId, CppUtils::MemoryChunk const& request, CppUtils::MemoryChunk& response)
	{
		try {
			CppUtils::String request_s = request.toString();
			LOG(MSG_INFO, MTPROXYMANAGER, "Received request from remote client: \"" << request_s << "\"");
			if (request_s.size() > 1) {
				 
				CppUtils::String rCommand = request_s.substr(1);
				if (request_s[0]=='I') {
					// expect immediate execution
					// I...

					// parse string
					CppUtils::StringList commandList;
					CppUtils::parseCommaSeparated(',', rCommand, commandList);
  
					if (commandList.size() > 0) {
						// what command we have
						if (commandList[ 0 ] == CMD_IMMEDIATE_SYMBOL_METAINFO) {
							// get symbol metainfo
							BrkLib::SymbolInfo info = getSymbolMetaInfo(commandList[ 1 ]);

							CppUtils::String response_s = commandList[ 0 ] + "," +
								CppUtils::double2String(info.pointValue_m) + "," +
								CppUtils::long2String(info.spread_m ) + "," +
								CppUtils::long2String(info.signDigits_m);

							response.fromString(response_s);
													
						}
						else if (commandList[ 0 ] == CMD_IMMEDIATE_POSITION_LIST) {
							// get position list

							// lock this for a while
							LOCK_FOR_SCOPE(metaInfoMtx_m);  
							
							// comand name and the amount of positions
							CppUtils::String response_s = commandList[ 0 ] + "," + CppUtils::long2String(positions_m.size()) + ",";

							CppUtils::String schunk;
							for(BrkLib::MT4PositionMap::iterator it = positions_m.begin(); it != positions_m.end(); it++) {
								BrkLib::Mt4Position &position = it->second;

								schunk = CppUtils::double2String(position.closePrice_m) + "," +
									CppUtils::double2String(position.closeTime_m) + "," +
									CppUtils::double2String(position.comission_m) + "," +
									CppUtils::double2String(position.executePrice_m) + "," +
									CppUtils::double2String(position.executeTime_m) + "," +
									CppUtils::double2String(position.installPrice_m) + "," +
									CppUtils::double2String(position.installTime_m) + "," +
									CppUtils::double2String(position.lots_m) + "," +
									CppUtils::long2String(position.mt4Type_m) + "," +
									CppUtils::double2String(position.orderPureProfit_m) + "," +
									CppUtils::double2String(position.orderPureSwap_m) + "," +
									position.posId_m + "," +
									position.symbol_m + "," +
									CppUtils::long2String(position.orderType_m) + "," +
									CppUtils::long2String(position.isHistory_m) + "," +
									position.externalId_m +"," +
									position.comment_m +"," +
									CppUtils::double2String(position.orderSlPrice_m) + "," +
									CppUtils::double2String(position.orderTpPrice_m) + "," +
									CppUtils::double2String(position.orderExpiration_m) + ",";

								response_s += schunk;
								
							};

							// response
							response.fromString(response_s);

						}
						else if (commandList[ 0 ] == CMD_ECHO) {
							// just echo
							response = request;
						}

					}
				
					
				}
				else if (request_s[0]=='D') {
					// usual delayed execution
					// D<>,<>,<>,...<>,
					// lock as many threads can do
					pushPendingCommand(rCommand);
				
					// OK response
					response.fromString("OK");
				}
			}
			
			
		}
		catch(CppUtils::Exception &e) {
			LOG(MSG_ERROR, MTPROXYMANAGER, "Exception on pipe data retrival handler: " << e.message());
			response.fromString("ERROR");
		}
		catch(...) {
			LOG(MSG_ERROR, MTPROXYMANAGER, "Unknown exception on pipe data retreival handler ");
			response.fromString("ERROR");
		}
		
		// default
		if (response.length()==0)
			response.fromString("NA");
		//LOG(MSG_DEBUG, MTPROXYMANAGER, "Sent	request to remote client: \"" << response.toString() << "\"");
				
	};

	void MtProxyManager::pushSymbolMetaInfo(char const* symbol, double const pointvalue, int const digit, int const spread)
	{
		LOCK_FOR_SCOPE(metaInfoMtx_m);

		if (symbol != NULL && strlen(symbol)>0) 
			metaInfo_m[ symbol ] = BrkLib::SymbolInfo(pointvalue, spread, digit);

		
	}

	void MtProxyManager::pushPositionMetaInfo(BrkLib::Mt4Position const position)
	{
		LOCK_FOR_SCOPE(metaInfoMtx_m);
		positions_m[ position.posId_m ] = position;

		// also need to make here position management so we could generate events
	} 

	BrkLib::SymbolInfo MtProxyManager::getSymbolMetaInfo(CppUtils::String const& symbol) const
	{
		LOCK_FOR_SCOPE(metaInfoMtx_m);
		map<CppUtils::String, BrkLib::SymbolInfo >::const_iterator it = metaInfo_m.find(symbol);

		if (it!=metaInfo_m.end()) {
			return it->second;
		}

		static const BrkLib::SymbolInfo dummy;
		return dummy;
	}


	void MtProxyManager::setGlobal(CppUtils::String const& id, CppUtils::String const& value)
	{
		LOCK_FOR_SCOPE(globalMtx_m);
		
		CppUtils::MemoryChunk buffer;
		CppUtils::Value global;
		
		CppUtils::readContentFromFile2(globalFileName_m, buffer);
		global.fromBlob(CppUtils::Value::VT_StringMap, buffer.data());

		global.getAsStringMap()[ id ] = value;
		global.toBlob(buffer);

		CppUtils::saveContentInFile2(globalFileName_m, buffer);

	}

	CppUtils::String MtProxyManager::getGlobal(CppUtils::String const& id) const
	{
		LOCK_FOR_SCOPE(globalMtx_m);

		CppUtils::MemoryChunk buffer;
		CppUtils::Value global;

		CppUtils::readContentFromFile2(globalFileName_m, buffer);
		global.fromBlob(CppUtils::Value::VT_StringMap, buffer.data());

		map<CppUtils::String, CppUtils::String>::const_iterator it = global.getAsStringMap().find(id);
		if (it != global.getAsStringMap().end())
			return it->second;

		return "";
	}

	void MtProxyManager::removeGlobal(CppUtils::String const& id)
	{
		LOCK_FOR_SCOPE(globalMtx_m);

		CppUtils::MemoryChunk buffer;
		CppUtils::Value global;
		
		CppUtils::readContentFromFile2(globalFileName_m, buffer);
		global.fromBlob(CppUtils::Value::VT_StringMap, buffer.data());


		map<CppUtils::String, CppUtils::String>::iterator it = global.getAsStringMap().find(id);

		if (it != global.getAsStringMap().end())
			global.getAsStringMap().erase(id);

		global.toBlob(buffer);
		CppUtils::saveContentInFile2(globalFileName_m, buffer);
	}

	void MtProxyManager::removeAllGlobals()
	{
		LOCK_FOR_SCOPE(globalMtx_m);
		CppUtils::removeFile(globalFileName_m);
		
	}


	/**
		Helpers
	*/
	
	
}; // end of namespace
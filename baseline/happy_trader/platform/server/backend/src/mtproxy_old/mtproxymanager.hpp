#ifndef _MTPROXY_MTPROXYMANAGER_INCLUDED
#define _MTPROXY_MTPROXYMANAGER_INCLUDED

#include "mtproxydefs.hpp"
#include <queue>
#include "../brklibmetatrader/mtcommondefinitions.h"

namespace MtProxy {
	// base class for iner-pipe communications to make MT4 communication
	

	class MtProxyManager: private CppUtils::PipeServer {
	public:

		// ctor & dtor
		MtProxyManager(char const* serverpipe, char const* globaFileName);

		~MtProxyManager();

		// returns currently pending command , but not deleting this from the queue
		void getPendingCommand(CppUtils::String& command, bool& empty);

		// return pending command access counter
		int getPendingCounterNumber() const;

		// delete pending comand from the queue
		void deletePendingCommand(bool& empty);

		// push command
		void pushPendingCommand(CppUtils::String& command);

		// pushes command execution result in string form
		// that will directly go to connected remote pipe
		void pushCommandResult(char const* remotepipe, char const* cmd);
	

		// derived function processing input request from the client  -e.i. from MT happy trader broker proxy
		// shall generally push the client call to the queue
		// excepting some commands taht may have immediate response
		virtual void onClientRequestArrived(int const instId, CppUtils::MemoryChunk const& request, CppUtils::MemoryChunk& response);

		// called from MT 
		// upon start - just to store info
		void pushSymbolMetaInfo(char const* symbol, double const pointvalue, int const digit, int const spread);

		// as this is expected only one client
		// pushes info on one position
		void pushPositionMetaInfo(BrkLib::Mt4Position const position);
			
		// reference count
		static int refCnt_m;

		// management of globals
		void setGlobal(CppUtils::String const& id, CppUtils::String const& value);

		CppUtils::String getGlobal(CppUtils::String const& id) const;

		void removeGlobal(CppUtils::String const& id);

		void removeAllGlobals();

		
	private:

		
		// get stored metainfo
		BrkLib::SymbolInfo getSymbolMetaInfo(CppUtils::String const& symbol) const;

	
		// mtx saving pendingCommands_m list	
		CppUtils::Mutex pendingCommandsMtx_m;

		// queue of pending commands
		queue<CppUtils::String> pendingCommands_m;

		// mtx for metainfo and position map
		CppUtils::Mutex metaInfoMtx_m;

		// significant digits for symbol
		map<CppUtils::String, BrkLib::SymbolInfo > metaInfo_m;

		// positions stored
		BrkLib::MT4PositionMap positions_m;

		// mtx preserving global map
		CppUtils::Mutex globalMtx_m;

		// global list
		//map<CppUtils::String, CppUtils::String> global_m;

		CppUtils::String globalFileName_m;


		// pending command counter - how muct time getPendingCommand(...) was called
		int cmdCounter_m;

	};

};

#endif

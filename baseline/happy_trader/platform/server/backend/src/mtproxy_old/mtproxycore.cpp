#include "mtproxycore.hpp"
#include "mtproxymanager.hpp"

#define MTPROXYCORE "MTPROXYCORE"
// globals
MtProxy::MtProxyManager* mngr_g = NULL;

int MtProxy::MtProxyManager::refCnt_m = 0;

extern "C" {

MTPROXY_EXP int PROCCALL  initialize(char const* serverpipe, char const* globaFileName)
{
	int result = 0;
	
	static CppUtils::Mutex mtx_s;
	LOCK_FOR_SCOPE(mtx_s);
	
	if (MtProxy::MtProxyManager::refCnt_m==0 && !mngr_g) {
			try {
				CppUtils::String globalFile;
				if (globaFileName && strlen(globaFileName) > 0) {
					globalFile = globaFileName;
				}
				else {
					globalFile = CppUtils::getStartupDir() + "/ht_brklib_proxy.glb";
				}

				LOG(MSG_INFO, MTPROXYCORE, "Proxy manager will be using the global file name: " << globalFile);
				mngr_g = new MtProxy::MtProxyManager(serverpipe ? serverpipe : "", globalFile.c_str());
				result = 1;
				
			}
			catch(CppUtils::Exception& e) {
				LOG(MSG_WARN, MTPROXYCORE, "Exception on (initialize): " << e.message() );
			}
			catch(...) {
				LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (initialize)"  );
			}
		
	}   
	// inc ref count
	MtProxy::MtProxyManager::refCnt_m++;
	LOG(MSG_INFO, MTPROXYCORE, "Proxy manager of version - "<< VERSION << " - increased reference count: " << MtProxy::MtProxyManager::refCnt_m);

	return  result;
}

MTPROXY_EXP int PROCCALL  deinitialize()
{
	int result = 0;
	
	static CppUtils::Mutex mtx_s;

	if (mngr_g) {
		LOCK_FOR_SCOPE(mtx_s);
		if (mngr_g) {
			try {
				// delete 
				if (--MtProxy::MtProxyManager::refCnt_m == 0) {
					delete mngr_g;
					mngr_g = NULL;
				}

				result = 1;
				LOG(MSG_INFO, MTPROXYCORE, "Proxy manager decreased reference count: " << MtProxy::MtProxyManager::refCnt_m);
			}
			catch(CppUtils::Exception& e) {
				LOG(MSG_WARN, MTPROXYCORE, "Exception on (deinitialize): " << e.message() );
			}
			catch(...) {
				LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (deinitialize)" );
			}
		}	
	}
	
	return  result;

}

MTPROXY_EXP int PROCCALL  getPendingCommand(char* cmd )
{
	int result = -1;

	try {
		CppUtils::String data;
		bool empty = true;
		mngr_g->getPendingCommand(data, empty);

		if (!empty && cmd) {
			//int cmdl = strlen(cmd);
			//if ( cmdl >= data.size()) {
			
				strcpy(cmd, data.c_str() );
				result = 1;
			//}
			//else {
			//	THROW(CppUtils::OperationFailed, "exc_TargetStringIsTooSmall", "ctx_getPendingCommand", CppUtils::long2String(cmdl) + " is less than " + CppUtils::long2String(data.size()) );
			//}
		}
	}
	catch(CppUtils::Exception& e) {
		LOG(MSG_WARN, MTPROXYCORE, "Exception on (getPendingCommand): " << e.message() );
	}
	catch(...) {
		LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (getPendingCommand)" );
	}
	
	return result;
}

MTPROXY_EXP int PROCCALL getPendingCounterNumber()
{
	int result = -1;
	try {
		result = mngr_g->getPendingCounterNumber();
	}
	catch(CppUtils::Exception& e) {
		LOG(MSG_WARN, MTPROXYCORE, "Exception on (getPendingCounterNumber): " << e.message() );
	}
	catch(...) {
		LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (getPendingCounterNumber)" );
	}

	return result;
	
}

MTPROXY_EXP int PROCCALL deletePendingCommand()
{
	int result = -1;
	try {
		bool empty = true;
		mngr_g->deletePendingCommand(empty);

		if (!empty)
			result = 1;
	}
	catch(CppUtils::Exception& e) {
		LOG(MSG_WARN, MTPROXYCORE, "Exception on (deletePendingCommand): " << e.message() );
	}
	catch(...) {
		LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (deletePendingCommand)" );
	}
	
	return result;
}

MTPROXY_EXP int PROCCALL  pushCommandResult(char const* remotetpipe, char const* cmdresult)
{
		int res = -1;
		
		try {
			mngr_g->pushCommandResult(remotetpipe ? remotetpipe : "", cmdresult ? cmdresult : "");
			res = 1;
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_WARN, MTPROXYCORE, "Exception on (pushCommandResult): " << e.message() );
		}
		catch(...) {
			LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (pushCommandResult)" );
		}
	
		return res;
}

MTPROXY_EXP int PROCCALL pushSymbolMetaInfo(
	char const* symbol, 
	double const pointvalue, 
	int const digit, 
	int const spread
)
{
		int res = -1;

		try {
			mngr_g->pushSymbolMetaInfo(symbol ? symbol : "", pointvalue, digit, spread);
			res = 1;
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_WARN, MTPROXYCORE, "Exception on (pushSymbolMetaInfo): " << e.message() );
		}
		catch(...) {
			LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (pushSymbolMetaInfo)" );
		}
	
		return res;

}

MTPROXY_EXP int PROCCALL pushPositionMetaInfo(
	char const* symbol,
	int ticket,
	int ordertype,
	double const installtime,
	double const executetime,
	double const closetime,
	double const installprice,
	double const executeprice,
	double const closeprice,
	double const comission,
	double const lots,
	double const orderpureprofit,
	double const orderpureswap,
	int const isHistory,
	char const* comment,
	double const tpPrice,
	double const slPrice,
	double const orderExpiration
)
{
	int res = -1;

		try {
			BrkLib::Mt4Position mt4position;
			mt4position.posId_m = CppUtils::long2String(ticket);
			mt4position.orderType_m = ordertype;
			mt4position.symbol_m = symbol ? symbol : "";
			mt4position.installTime_m = (installtime > 0 ? installtime : -1);
			mt4position.executeTime_m = (executetime > 0 ? executetime : -1);
			mt4position.closeTime_m = (closetime > 0 ? closetime : -1 );
			mt4position.installPrice_m = (installprice > 0 ? installprice : -1);
			mt4position.executePrice_m = (executeprice > 0 ? executeprice : -1);
			mt4position.closePrice_m = (closeprice > 0 ? closeprice : -1);
			mt4position.comission_m = comission;
			mt4position.lots_m = lots;
			mt4position.orderPureProfit_m = orderpureprofit;
			mt4position.orderPureSwap_m = orderpureswap;
			mt4position.isHistory_m = (isHistory > 0 ? true: false);
			// dummy valid ID
			mt4position.externalId_m = "80F89AC9904A11DC8000005056C00008";

			mt4position.comment_m = comment ? comment : "";
			mt4position.orderSlPrice_m = slPrice;
			mt4position.orderTpPrice_m = tpPrice;

			// TODO expiration time
			mt4position.orderExpiration_m = orderExpiration;

			mngr_g->pushPositionMetaInfo(mt4position);
			res = 1;
		}
		catch(CppUtils::Exception& e) {
			LOG(MSG_WARN, MTPROXYCORE, "Exception on (pushSymbolMetaInfo): " << e.message() );
		}
		catch(...) {
			LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (pushSymbolMetaInfo)" );
		}
	
		return res;
}

MTPROXY_EXP int PROCCALL setGlobal(char const* id, char const* value)
{
	int result = 0;
	try {
			mngr_g->setGlobal(id ? id : "", value ? value : "");
			result = 1;
	}
	catch(CppUtils::Exception& e) {
			LOG(MSG_WARN, MTPROXYCORE, "Exception on (setGlobal): " << e.message() );
	}
	catch(...) {
			LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (setGlobal)" );
	}
	

	return result;
}

MTPROXY_EXP int PROCCALL removeGlobal(char const* id)
{
	int result = 0;
	try {
		mngr_g->removeGlobal( id ? id : "" );
		result = 1;
	}
	catch(CppUtils::Exception& e) {
			LOG(MSG_WARN, MTPROXYCORE, "Exception on (removeGlobal): " << e.message() );
	}
	catch(...) {
			LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (removeGlobal)" );
	}

	return result;
}

MTPROXY_EXP int PROCCALL removeAllGlobals()
{
	int result = 0;
	try {
		mngr_g->removeAllGlobals();
	}
	catch(CppUtils::Exception& e) {
			LOG(MSG_WARN, MTPROXYCORE, "Exception on (removeAllGlobals): " << e.message() );
	}
	catch(...) {
			LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (removeAllGlobals)" );
	}

	return result;
}

MTPROXY_EXP int PROCCALL getGlobal(char const* id, char* value)
{
	int result = 0;
	try {

		CppUtils::String glb = mngr_g->getGlobal(id ? id : "");
		if (glb.size() > 0 && value /*&& strlen(value) >= glb.size()*/ ) {
			strcpy(value, glb.c_str() );
			result = 1;
		}

	}
	catch(CppUtils::Exception& e) {
			LOG(MSG_WARN, MTPROXYCORE, "Exception on (getGlobal): " << e.message() );
	}
	catch(...) {
			LOG(MSG_WARN, MTPROXYCORE, "Unknown exception on (getGlobal)" );
	}
	
	return result;
}

// --------------------------


}

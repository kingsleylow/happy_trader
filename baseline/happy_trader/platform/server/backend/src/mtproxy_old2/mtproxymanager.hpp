#ifndef _MTPROXY_MTPROXYMANAGER_INCLUDED
#define _MTPROXY_MTPROXYMANAGER_INCLUDED

#include "mtproxydefs.hpp"
#include "dataqueue.hpp" 
#include <queue>


#define MTPROXYMANAGER "MTPROXYMANAGER"

// demo restriction number
#define IS_DEMO 1

#ifdef IS_DEMO
#define DEMO_REQUEST_NUMBER 100000
#endif

#define RETURN_MT_DATA_ACCEPTED "OK"
#define HELPER_THREAD_SLEEP_PERIOD_SEC 2
#define HELPER_THREAD_HEARTBEAT_PERIOD_SEC_DEAFULT 5
#define MT_ATTEMPTS_CONNECT 10
#define ATTEMPT_TO_CALLMT4LOGIN 10
#define MAX_MT_CONNECT_TIME_SEC 20

namespace MtProxy {
	// base class for iner-pipe communications to make MT4 communication
	
	struct Settings
	{
		Settings():
			logLevel_m(MSG_DEBUG_DETAIL),
			heartBeatMtproxySec_m(HELPER_THREAD_HEARTBEAT_PERIOD_SEC_DEAFULT),
			mt4attemptstoconnect_m(MT_ATTEMPTS_CONNECT)
		{
		};

		// configuration
		CppUtils::String remotePipe_m;

		CppUtils::String serverPipe_m;

		int logLevel_m;

		CppUtils::String terminalPath_m;

		CppUtils::String instanceName_m;

		CppUtils::Uid terminalUid_m;

		int heartBeatMtproxySec_m;

		// the number of times mt atempts to connect with provided user data
		int mt4attemptstoconnect_m;

		// autoload file with user credentials
		CppUtils::String autoLoadFile_m;
	};

	class MtProxyManager: private CppUtils::PipeServer, public CppUtils::Mutex {
	public:
	
		enum InitFlag{
			IF_CREATE = 0,
			IF_FREE  =1,
			IF_RESOLVE
		};

		// initialize and deinitialize 
		// need to be a real singleton
		// perform ref counting
		static void initialize(int const opFlag, MtProxy::MtProxyManager** ppProxymanager = NULL, int* refCountPar = NULL);

		

		// ctor & dtor
		MtProxyManager();

		~MtProxyManager();

		// startup
		void startup(char const* configXml, int hWnd);
	
		// shutdown
		void shutdown();

		

		// pushes command execution result in string form
		// that will directly go to connected remote pipe
		void pushDataToPipeServer(char const* cmd, CppUtils::String& resp);
	
		// connect to pipe server
		void connectToPipeServer();

		// disconnect from pipe serever
		void disconnectFromPipeServer();

	

		// derived function processing input request from the client  -e.i. from MT happy trader broker proxy
		// shall generally push the client call to the queue
		// excepting some commands taht may have immediate response
		virtual void onClientRequestArrived(int const instId, CppUtils::MemoryChunk const& request, CppUtils::MemoryChunk& response);

		
		// reset dirty flag and set pointer to first queue element
		// return false if no elements
		bool pointFirstElemUserPeriodicalQueue();

		// read status and advise which commands are pending
		void readStatus(int& totalQueueSize) const;

		// update current users with access time
		void updateUsersWithAccessTime(CppUtils::String const& user);

		// update all users with access time
		void updateAllUsersWithAccessTime();
		
		
		

		void getTerminalUid(CppUtils::Uid& termUid) const;

		// the only public method to work with gthe queue
		void waitForUserPeriodicalQueueData(int const tout_msec, GeneralCommand& data, bool& data_found, bool& data_available, bool& is_last);

		void isConnectedToPipeServer(bool &connectedToPipeServer ) const
		{
			connectedToPipeServer = connectedToPipeServer_m;
		}

		void getConnectAttemptNumber(int& connectattemptnumber)
		{
			connectattemptnumber = settings_m.mt4attemptstoconnect_m;
		}

		// thread function
		void run();

		void callConnectForm(char const* user, char const* password, char const* server);


		// command data functions
		// push data
		//void pushCommandData( Command const& data);
		
		// 
		//void waitForCommandData(int const tout_msec, Command& data, bool& data_found, bool& data_available, bool& is_last);

		//void clearCommandQueue();
	private:

		class checkForRemovals_front_if {
		public:
			checkForRemovals_front_if()
			{
			};

			bool operator()(GeneralCommand const& user) const
			{
				if (user.lifetime() == 0) {
					LOG(MSG_DEBUG, MTPROXYMANAGER, "Removing element: [" << user.toString()<< "] because life time is 0");
					
					return true;
				}

				return false;
			}
		};

		// helper predicate to remove some queue elements
		class checkForRemovals_remove_if
		{
		public:
			

			checkForRemovals_remove_if():
				cur_time_m((int)CppUtils::getTime())
			{
			};

			bool operator()(BaseQueueElement& elem)
			{
				if (elem.lifetime() <= 0)
					return false;

				if (elem.lastAccessTime() <=0)
					return false;

				int cur_livetime = cur_time_m - elem.lastAccessTime();
				if (cur_livetime > elem.lifetime()) {

					LOG(MSG_DEBUG, MTPROXYMANAGER, "Removing element: [" << elem.toString()<< "] because life time detected is: " << cur_livetime);
					return true;
				}



				return false;
			}

		private:
			int cur_time_m;
		};

		// send hearbeat to HT layer every HELPER_THREAD_HEARTBEAT_PERIOD_SEC seconds
		// will pass current load
		void sendHeartBeatToHT();

		// update all data
		void initUserPeriodicalQueue(GeneralCommandList const& userInfoList);
		
		// cose MT instnace
		void closeInstance();

		// clear data
		void clearUserPeriodicalQueue();

		// get data
		GeneralCommandList readUserPeriodicalQueue() const;

		void readXMLConfiguration(char const* configXml);

		HWND resolveUpperMtWindow(HWND const chartHwnd) const;

		// if autoload is true -  an attempt to load users on startup
		void autoLoadStartUpFile();

	private:



		CppUtils::Event dataInPolingQueueAvailable_m;

		// pipe client
		CppUtils::PipeClient client_m;

		DataQueue<GeneralCommand> dataQueue_m;

		//DataQueue<Command> commandQueue_m;

		
		CppUtils::Thread* thread_m;

			// event whether thread started
		CppUtils::Event threadStarted_m;

		
		// signal to shutdown
		CppUtils::Event shutDownThread_m;

		bool idle_m;

		atomic<bool> connectedToPipeServer_m;

		
		Settings settings_m;
	
		// main MT window handle
		HWND hWnd_m;

		HWND upperMtWindow_m;

		int pid_m;
#ifdef IS_DEMO	
		int requestCounter_m;
#endif

		// we may hang on reconnect 
		TimeSpan connectSpan_m;

	public:
		
		static HMODULE moduleHandle_m;
	};

	

};

#endif

#ifndef _MTQUEUE_HELPERSTRUCT_INCLUDED
#define _MTQUEUE_HELPERSTRUCT_INCLUDED

#include "mtqueuedefs.hpp"



#define MTQUEUE_VERSION "2.0.4"
#define CONTROL_THREAD_SLEEP_PERIOD_SEC 1
#define QUEUE_MAX_CAPACITY 100000
#define MAX_WORK_THREADS 5
#define MAX_WORK_THREAD_WAIT_SEC 20
#define MAX_REPEAT_ATTEMPTS 10

#define WININET_USER_AGENT "Mozilla/5.0 (iPad; U; CPU OS 3_2_1 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Mobile/7B405"




namespace MtQueue {
	
	struct SimpleDataEntry {
		SimpleDataEntry()
		{
		};

		CppUtils::String data_m;

		CppUtils::String url_m;

		CppUtils::StringMap props_m;
	};

	struct FileDataEntry {
		FileDataEntry()
		{
		};

		FileDataEntry(CppUtils::String const& fileName, CppUtils::String const& fileData, CppUtils::String const& url):
			fileName_m(fileName),
			fileData_m(fileData),
			url_m( url )
		{
			check();
		}

		FileDataEntry(char const* fileName, char const* fileData, char const* url):
				fileName_m(fileName ? fileName : "dummy.dat"), 
				fileData_m(fileData ? fileData : ""), 
				url_m(url ? url : "")
		{
			check();
		}	

		~FileDataEntry()
		{
		}

		void check()
		{
			if (url_m.size() <=0)
				THROW(CppUtils::OperationFailed, "exc_InvalidURL", "ctx_FileDataEntryCtor", "");

			if (fileName_m.size() <=0)
				THROW(CppUtils::OperationFailed, "exc_InvalidFileName", "ctx_FileDataEntryCtor", "");

		}


		CppUtils::String fileName_m;
		
		CppUtils::String fileData_m;

		CppUtils::String url_m;

	};

	struct DataToSend
	{

		enum LifyTimeFlag
		{
			LTF_Dummy = 0x00,
			LTF_RemoveImmediately = 0x01,
			LTF_RemoveAfterFailAttempts = 0x02,
			LTF_DoNotRemove = 0x03
			
		};

		DataToSend():
			lifeTimeFlag_m(LTF_Dummy), 
			failCounter_m(-1), 
			maxFails_m(-1)
		{
				eventId_m = ++eventIdCounter_m; 
		}

		
		
		FileDataEntry& data()
		{
			return data_m;
		}

		FileDataEntry const& data() const
		{
			return (FileDataEntry const&)data_m;
		}

		inline int getFailNumber() const
		{
			return failCounter_m;
		}
		

		inline int getRepeatAttempts() const
		{
			return repeatAttempts_m;
		}

		bool checkFailCounterTriggers()
		{
			if (failCounter_m >= 0 && maxFails_m >=0) {

				if (++failCounter_m >= maxFails_m)
					return true;
			}

			return false;
		}
		
		inline LifyTimeFlag getLifeTimeFlag() const
		{
			return lifeTimeFlag_m;
		}

		bool isValid() const
		{
			return lifeTimeFlag_m != DataToSend::LTF_Dummy;
		}

		// repeat attempts
		void initialize(FileDataEntry const& data, LifyTimeFlag const timeflag, int repeatAttempts  )
		{
			if (timeflag == DataToSend::LTF_Dummy ) 
				THROW(CppUtils::OperationFailed,"exc_InvalidLifetimeFlag", "ctx_initialize", "");

			if (repeatAttempts > MAX_REPEAT_ATTEMPTS || repeatAttempts <= 0 ) 
				THROW(CppUtils::OperationFailed,"exc_InvalidRepeatATtempts", "ctx_initialize", "");



			lifeTimeFlag_m = timeflag;
			failCounter_m = -1;
			maxFails_m = -1;
			data_m = data;
			repeatAttempts_m = repeatAttempts;

		}

		void initializeWithMaxFailCounter(FileDataEntry const& data, int const maxFailCounter, int repeatAttempts  )
		{
			if (maxFailCounter <0 )
				THROW(CppUtils::OperationFailed,"exc_InvalidMaxFailNumber", "ctx_initializeWithMaxFailCounter", maxFailCounter);

			
			if (repeatAttempts > MAX_REPEAT_ATTEMPTS || repeatAttempts <= 0 ) 
				THROW(CppUtils::OperationFailed,"exc_InvalidRepeatATtempts", "ctx_initialize", "");

			lifeTimeFlag_m = LTF_RemoveAfterFailAttempts;
			failCounter_m = 0;
			maxFails_m = maxFailCounter;
			data_m = data;
			repeatAttempts_m = repeatAttempts;
		}

		inline long getEventId() const
		{
			return eventId_m;
		}

	private:

		FileDataEntry data_m;
		LifyTimeFlag lifeTimeFlag_m;
		int failCounter_m;
		int maxFails_m;
		int eventId_m;
		int repeatAttempts_m; // this is just IMMEDAITE repeating attampts in case of failure
		static atomic_long eventIdCounter_m;

		
	};


	// ---------------------------
	// helper class incapsulating working thread

	class MtQueueManager;

	class WorkingThread
	{
	public:

		WorkingThread();

		WorkingThread(CppUtils::Thread* thread, MtQueueManager* base);

		~WorkingThread();
		

		CppUtils::Thread* thread_m;

		CppUtils::Event threadStarted_m;

		long tid_m;

		MtQueueManager* base_m;


	};
	

};

#endif
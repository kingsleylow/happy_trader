#include "helperstruct.hpp"
#include "mtqueuemanager.hpp"

namespace MtQueue {

		atomic_long DataToSend::eventIdCounter_m;

		WorkingThread::WorkingThread():
			thread_m(NULL), tid_m(-1), base_m(NULL)
		{
		};

		WorkingThread::WorkingThread(CppUtils::Thread* thread, MtQueueManager* base):
			thread_m(thread), tid_m(-1), base_m(base)
		{
		};

		WorkingThread::~WorkingThread()
		{
			if (thread_m) {
				if (!thread_m->waitForThread(MAX_WORK_THREAD_WAIT_SEC)) {
					THROW(CppUtils::OperationFailed, "exc_CannotStopWorkingThread", "ctx_WorkingThreadDtor", "");
				}

				delete thread_m;
				thread_m = NULL;
			}
		}



}; // end of ns
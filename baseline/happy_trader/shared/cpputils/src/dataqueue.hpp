#ifndef _CPPUTILSS_DATAQUEUE_INCLUDED
#define _CPPUTILSS_DATAQUEUE_INCLUDED

#include "cpputilsdefs.hpp"
#include <queue>
#include <forward_list>

#define DATAQUEUE "DATAQUEUE"
namespace CppUtils {

	
	// class to provide prioritized queue in multithread enviroment
	// the main thing that some items to be removed after processing whereas other items are not removed, 
	// depending on the special flag set 

	// also prioritized data to be put in the front of the queue



	// add capability to recover form temp storage

	template<class T>
	class DataQueue: private CppUtils::Mutex
	{
	public:
		DataQueue()
		{
			it_m = dataqueue_m.begin();
		}

		virtual ~DataQueue()
		{
		}

		size_t size() const
		{
			LOCK_FOR_SCOPE(*this);

			return dataqueue_m.size();
		}
	
		// put data according priority- may happen in other thread
		void put(T const& data)
		{
			LOCK_FOR_SCOPE(*this);
			if (dataqueue_m.empty()) {
				dataqueue_m.push_back(data);

				it_m = dataqueue_m.begin();
				signalAvailableQueueDataEvent();
			}
			else {
				dataqueue_m.push_back(data);
			}
			
			// save if needed
			saveToCache();

		}

		// put list of data
		

		// add data
		void put(vector<T> const& data_list)
		{
			LOCK_FOR_SCOPE(*this);
			
			if (!data_list.empty()) {
				// init

				if (dataqueue_m.empty()) {
					// add and signal cause queue was empty
					pushVector(data_list);
					it_m = dataqueue_m.begin();

					signalAvailableQueueDataEvent();
				}
				else {
					// NO need to notify
					pushVector(data_list);
				}

				// save if needed
				saveToCache();
			}


		}


		
		void getPendingData(vector<T>& result) const
		{
			LOCK_FOR_SCOPE(*this);
		
			result.clear();
			result.assign(dataqueue_m.begin(), dataqueue_m.end());

		}

		void clear()
		{
			// this is an workaround - need to use intermideate queue
			LOCK_FOR_SCOPE(*this);
			
			if (!dataqueue_m.empty()) {
				clearData();
				
				// save if needed
				saveToCache();

				it_m = dataqueue_m.begin();

				resetAvailableQueueDataEvent();
			}

		}
		
		// get prioritized front element
		// if element not persistent - remove this
		// if no more elements - returns dummy element. Must be Event allowing waiting for elemenst to be available
		// return a copy cause multithread enviroment

		template<class PredicateIfRemove>
		void front(int const tout_msec, T& out_data, bool& found, bool& available, bool& is_last, PredicateIfRemove& predicate)
		{
			
			found = false;
			available = false;

			// if we got the lats element but will go to the next cycle need to raise this flag
			is_last = false;
		
			if (dataAvailable_m.lock(tout_msec))	{
				LOCK_FOR_SCOPE(*this);

				available = true;
				
				if (!dataqueue_m.empty()) {
					// copy
					//out_data = dataqueue_m.front();

					if (it_m == dataqueue_m.end())
						// must not happen
						THROW(CppUtils::OperationFailed, "exc_ConsistencyException", "ctx_front", "");
					
					out_data = *it_m;
				
					found = true;

					if (predicate(out_data)) {
						// remove
						//dataqueue_m.pop_front();
						it_m = dataqueue_m.erase(it_m); // this advances to the next element

						// save if needed
						saveToCache();

					} else {
						it_m++;
					}

					if (it_m == dataqueue_m.end())
							it_m = dataqueue_m.begin();
					

					// check if we are empty
					if (dataqueue_m.empty()) {
						
						it_m = dataqueue_m.begin();

						// if empty send message
						resetAvailableQueueDataEvent();

						is_last =true;
					}

				}
			}


		
			
		};

		

		// process through all elements to check if smth needs to be removed
		// obtain the number of removed elements
		template <class Predicate>
		int checkForRemovals(Predicate& predicate)
		{
			//
			LOCK_FOR_SCOPE(*this);

			int old_size = dataqueue_m.size();
			if (old_size == 0)
				return 0;

			list<T>::iterator it = dataqueue_m.begin();
			while(it != dataqueue_m.end()) {
				if (predicate(*it)) {
				
					bool removeCurrent = (it_m == it);
					it = dataqueue_m.erase(it);

					if (removeCurrent)
						it_m = it;
					
				}
				else {
					it++;
				}
			}

			if (it_m == dataqueue_m.end())
					it_m = dataqueue_m.begin();

			int removed_cnt = old_size -  dataqueue_m.size();
			if (dataqueue_m.empty()) {
						// if empty send message

						it_m = dataqueue_m.begin();

						resetAvailableQueueDataEvent();
					
			}

			return removed_cnt;
		}

		// helper to change some attributes
		// while we are mutexed
		template<typename FUNC>
		void iterateThroughElementsSingleElement(FUNC& func)
		{
				// it can be lambda or predicate class
				LOCK_FOR_SCOPE(*this);
				for(list<T>::iterator it = dataqueue_m.begin(); it != dataqueue_m.end(); it++) {
					func( *it );
				}
		}

		template<typename FUNC>
		void iterateThroughElementsAllElements(FUNC& func)
		{
				// it can be lambda or predicate class
				LOCK_FOR_SCOPE(*this);
				func(dataqueue_m.begin(), dataqueue_m.end());
			
		}

		// to be derived, should be public
		virtual void recoverFromCache()
		{
		}
		

	protected:

		
		inline void pushVector(vector<T> const& data_list)
		{
			//if (!data_list.empty())
			dataqueue_m.insert(dataqueue_m.end(), data_list.begin(), data_list.end());
		
		}

		inline void clearData()
		{
			//if (!dataqueue_m.empty())
			dataqueue_m.clear();
		}

		void resetAvailableQueueDataEvent()
		{
			if (dataAvailable_m.lock(0)) {
					
					dataAvailable_m.clearEvent();

					LOG(MSG_DEBUG, DATAQUEUE, "Data available - reset state");
			}
		}

		void signalAvailableQueueDataEvent()
		{
			if (!dataAvailable_m.lock(0)) {
					dataAvailable_m.signalEvent();

					LOG(MSG_DEBUG, DATAQUEUE, "Data available - signalled state");
			}

			
		}

	
		// cache specific functions
		// implemented in derived
		virtual void saveToCache()
		{
		}

		list<T> dataqueue_m; // want t have size

		typename list<T>::iterator it_m;

		CppUtils::Event dataAvailable_m;

		
	};


	

};

#endif
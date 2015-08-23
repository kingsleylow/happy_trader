#ifndef _BACKEND_INQUEUE_MATHINTERFACE_INCLUDED
#define _BACKEND_INQUEUE_MATHINTERFACE_INCLUDED

#include "mathdefs.hpp"
#include "../inqueue/inqueue.hpp"


namespace Math {

	// class series provider for math engine
	class MultiSymbolCacheSeriesProvider
	{
		public:

			
			void initialize(HlpStruct::MultiSymbolCache& multSymbolCache, CppUtils::String const& symbol)
			{
				multSymbolCacheP_m = &multSymbolCache;
				symbol_m = symbol;
			}

			// called from derived
			inline void setDataOffset(int const count)
			{
			
				if (!multSymbolCacheP_m->setOffsetFromLast(count))
					THROW(CppUtils::OperationFailed, "msg_InvalidBufferSize", "ctx_setDataOffset", (long)count );
			}

			inline long getCacheSize() const
			{
				return multSymbolCacheP_m->size();
			}
		
			inline bool isPtrValid()
			{
				return multSymbolCacheP_m->isPtrValid();
			}

			inline void getNextData()
			{
				multSymbolCacheP_m->next();
			}

			inline void getPrevData()
			{
				multSymbolCacheP_m->prev();
			}

			inline HlpStruct::PriceData const& getCurrentData()
			{
				return multSymbolCacheP_m->get(symbol_m);
			}

			inline HlpStruct::PriceData const& getBackwardOffsetData(long const offset) const
			{
				return multSymbolCacheP_m->getOffsetFromLastData(symbol_m, offset);
			}

			inline HlpStruct::PriceData const& getUncloseddata()
			{
				THROW(CppUtils::OperationFailed, "msg_InvalidCall", "ctx_getUncloseddata", "" );
			}

			virtual int returnNecessaryCacheSize() const = 0;

		protected:
			
			MultiSymbolCacheSeriesProvider():
				multSymbolCacheP_m(NULL)
			{
			}

		private:
			HlpStruct::MultiSymbolCache* multSymbolCacheP_m;

			CppUtils::String symbol_m;

			
	};

	// -------------------------------------------------

	// this class works with vector of values
	class ArbitrarySeriesProvider
	{
		public:

			
			// append to the series to maximum
			static void appendToPriceList(deque<HlpStruct::PriceData> &valuesList,  HlpStruct::PriceData const& pdata, int const max_s)
			{
				if (valuesList.size() >= max_s ) 
					valuesList.pop_front();
				
				valuesList.push_back(pdata);
			}

			void initialize(deque<HlpStruct::PriceData> &valuesList)
			{
				valuesListP_m = &valuesList;
			}

			// called from derived
			inline void setDataOffset(int const count)
			{
				if (count >= valuesListP_m->size())
					THROW(CppUtils::OperationFailed, "msg_BufferSizeLessThanCount", "ctx_ArbitrarySeriesSetDataOffset", (long)count );

				idx_m = valuesListP_m->size() - count - 1;
			}

			inline long getCacheSize() const
			{
				return valuesListP_m->size();
			}
		
			inline bool isPtrValid()
			{
				return idx_m >=0 && idx_m < valuesListP_m->size();
			}

			inline void getNextData()
			{
				idx_m++;
			}

			inline void getPrevData()
			{
				idx_m--;
			}

			inline HlpStruct::PriceData const& getCurrentData()
			{
				return valuesListP_m->at( idx_m );
			}

			inline HlpStruct::PriceData const& getBackwardOffsetData(long const offset)
			{
				if (offset >= valuesListP_m->size())
					THROW(CppUtils::OperationFailed, "msg_InvalidCall", "ctx_ArbitrarySeriesGetBackwardOffsetData", offset );

				long idx = valuesListP_m->size() - offset - 1;
				return valuesListP_m->at(idx);
			}

			inline HlpStruct::PriceData const& getUncloseddata()
			{
				THROW(CppUtils::OperationFailed, "msg_InvalidCall", "ctx_ArbitrarySeriesUnclosedDataAccess", "" );
			}

			virtual int returnNecessaryCacheSize() const = 0;

		protected:
			
			ArbitrarySeriesProvider():
				valuesListP_m(NULL),
				idx_m(0)
			{
			}

		private:
			deque<HlpStruct::PriceData>* valuesListP_m;

			long idx_m;
	};

	
	// ---------------------------------------------

	// this class privides prices series values
	class PriceSeriesProvider
	{
		public:
			
			void initialize(Inqueue::ContextAccessor& contextaccessor, int const symbol_cookie)
			{
				contextAccessor_m = &contextaccessor;
				cookie_m = symbol_cookie;
			}

			// called from derived
			inline void setDataOffset(int const count)
			{
				ptr_m  = contextAccessor_m->dataOffsetFromLast(cookie_m, count);
			}

			inline long getCacheSize() const
			{
				return contextAccessor_m->getSize(cookie_m);
			}
		
			inline bool isPtrValid()
			{
				return ptr_m.isValid();
			}

			inline void getNextData()
			{
				contextAccessor_m->dataNext(cookie_m, ptr_m);
			}

			inline void getPrevData()
			{
				contextAccessor_m->dataPrev(cookie_m, ptr_m);
			}

			inline HlpStruct::PriceData const& getCurrentData()
			{
				return contextAccessor_m->getCurrentPriceData(cookie_m, ptr_m);
			}

			inline HlpStruct::PriceData const& getBackwardOffsetData(long const offset)
			{
				return contextAccessor_m->getBackwardPriceData(cookie_m, ptr_m, offset );
			}

			inline  HlpStruct::PriceData const& getUncloseddata()
			{
				return contextAccessor_m->getUnclosedCandle(cookie_m);
			}

			virtual int returnNecessaryCacheSize() const = 0;
			
						
	protected:
			PriceSeriesProvider()
			{
			}
					
	private:
			Inqueue::ContextAccessor* contextAccessor_m;

			int cookie_m;

			Inqueue::DataPtr ptr_m;

			
	};

}; // end of namepsace

#endif

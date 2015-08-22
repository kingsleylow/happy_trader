#ifndef _BACKEND_INQUEUE_HISTCONTEXT_INCLUDED
#define _BACKEND_INQUEUE_HISTCONTEXT_INCLUDED

#include "inqueuedefs.hpp"
#include "commontypes.hpp"
#include "contextaccessor.hpp"
#include "aholder.hpp"

#define BUFFER_NOOP_SIZE 2048
#define BUFFER_STREAM_FILE_SIZE 256
#define METATADA_FILE "metadata.xml"
#define SYMBOL_LIST_FILE "symbollist.xml"
#define TICKER_STREAM_FILE "tickerstream.g2"

namespace Inqueue {

	class SimulationCollector;

	#pragma pack(push, 1)
	struct FileHeader
	{
		FileHeader()
		{
			memset(this, 0, sizeof(FileHeader));
		}

		FileHeader(CppUtils::String const& provider, CppUtils::String const& symbol)
		{
			memset(this, 0, sizeof(FileHeader));
			setProvider(provider);
			setSymbol(symbol);
		}

		FileHeader(CppUtils::String const& provider, CppUtils::String const& symbol, double const& timeBegin, double const& timeEnd)
		{
			memset(this, 0, sizeof(FileHeader));
			setProvider(provider);
			setSymbol(symbol);

			timeBegin_m = timeBegin;
			timeEnd_m = timeEnd;
		}

		void setSymbol(char const* symbol) {
			symbol_m[0] = '\0';
			if (strlen(symbol)<=MAX_SYMB_LENGTH) {
				strcpy(symbol_m, symbol);
			}
			else {
				strncpy(symbol_m, symbol, MAX_SYMB_LENGTH);
			}
				
		}

		void setProvider(char const* provider) {
			provider_m[0] = '\0';
			if (strlen(provider_m)<=MAX_SYMB_LENGTH) {
				strcpy(provider_m, provider);
			}
			else {
				strncpy(provider_m, provider, MAX_SYMB_LENGTH);
			}
		}

		void setSymbol(CppUtils::String const& symbol) {
			setSymbol(symbol.c_str());
		}

		void setProvider(CppUtils::String const& provider) {
			setProvider(provider.c_str());
		}

	

		char symbol_m[MAX_SYMB_LENGTH + 1];
		char provider_m[MAX_SYMB_LENGTH + 1];
		double timeBegin_m;
		double timeEnd_m;
	};
	#pragma pack (pop)

	
	// helper structure holding information about cached file
	class CacheFileContext {
	
	public:

		
		CacheFileContext();

		bool isInitialized() const;
		// delayed append
		void append_data(CppUtils::Byte const* buffer, size_t bsize);
		
		// reset buffer and write to file
		void flush();
		
		// internally opens file and returns handler
		void openFile();

		void closeFile();
		
		void clearFile();

		void setPosToBegin();
		
		void setPosFromBegin(long offset);
		
		void setPosToEnd();

		void setPosFromEnd(long offset);

		void setPosToBeginRewriteContent(CppUtils::String const& provider, CppUtils::String const& symbol);
		
		long readData(CppUtils::Byte* buffer, size_t buflength);

		void readHeader(FileHeader& header);
		
		long getFileSize();

		
		//HANDLE getOpenHandle();

		inline double const& getBeginTime() const
		{
			return beginTime_m;
		}

		inline double const& getEndTime() const
		{
			return endTime_m;
		}

		void resetBeginEndTime()
		{
			beginTime_m = -1.0;
			endTime_m = -1.0;
		}

		inline void setupBeginTime(double const& curtime) {
			if (beginTime_m < 0) {
				beginTime_m = curtime;
			} else {
				if (curtime < beginTime_m) 
					beginTime_m = curtime;
			}
		}

		inline void setupEndTime(double const& curtime) {
			if (endTime_m < 0) {
				endTime_m = curtime;
			}
			else {
				if (endTime_m < curtime)
					endTime_m = curtime;
			}
		}


		inline CppUtils::String &getProvider()
		{
			return provider_m;
		}

		inline CppUtils::String const& getProvider() const
		{
			return provider_m;
		}

		inline CppUtils::String &getSymbol()
		{
			return symbol_m;
		}

		inline CppUtils::String const& getSymbol() const
		{
			return symbol_m;
		}

		inline CppUtils::String const& getFilename() const
		{
			return fileName_m;
		}

		inline CppUtils::String& getFilename()
		{
			return fileName_m;
		}

		inline bool& getRemoveFlag()
		{
			return	isRemove_m;
		}

		inline bool const& getRemoveFlag() const
		{
			return	isRemove_m;
		}

	public:

		// -----------------------
		
		
		
		
		//long filePos_m;

		// whetehr we will delete cache on exit
	
				
	private:

		bool isRemove_m;

		CppUtils::String provider_m;

		CppUtils::String symbol_m;

		CppUtils::String fileName_m;

		HANDLE file_m;

		// start and end time of the cached data
		double beginTime_m;

		double endTime_m;


		// for delayed writing
		CppUtils::Byte buffer_m[BUFFER_NOOP_SIZE];

		long bufferPos_m;
	
	};

	/** Global singleton class responsible for holding all the history information
		for all the sybols that client passes to the server
	*/
	class INQUEUE_BCK_EXP HistoryContext: 
		public CppUtils::RefCounted, 
		private CppUtils::SharedResource
	{
	
	
	public:
		
		static CppUtils::String const &getRootObjectName ();

		HistoryContext(char const* storagePrefix);

		virtual ~HistoryContext();
		
		// delegates RT data
		void passRtData(HlpStruct::RtData const& rtdata, bool& insideExistingTime);

		// set the file poinetr to 0 to history cache file
		void clearHistoryCache(CppUtils::String const& provider, CppUtils::String const& symbol);

		// clear all
		void clearHistoryCache();

		// pass history chunk 
		// in case of data collision just substitute old data with new ones
		void passHistoryChunk(HlpStruct::RtDataList const& rtlist);

		// just pass a single entry
		void passHistoryEntry(HlpStruct::RtData const& rtdata);

		// just the same as the previous but re-use bufferRtData
		void passHistoryEntry(HlpStruct::RtData const& rtdata, HlpStruct::RtBufferData& bufferRtData);

		void flushAll();

    void returnCachingPeriod(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			double& btime, 
			double& etime
		);

		void setReuseCacheFlag(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol,
			bool const isReuseCache
		);

		// rerurns this flag
		bool getReuseCacheFlag(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol
		);


		// this is global version
		void setReuseCacheFlag(bool const isReuseCache);

		inline void setSimulationTimeOffset(double const& toffset)
		{
			READ_LOCK_FOR_SCOPE(*this);

			simulationTimeOffset_m = toffset;
		}

		
		

		
		// return context
		void getCachedContext(vector<CacheFileContext> &cachedContext) const;

		// performs shutdown
		void shutdown();

		CppUtils::String const& getBaseDirectory() const;

		CppUtils::String const& getMetadataPath() const;

		CppUtils::String const& getStoragePrefix() const;

		// static version to get this path
		static CppUtils::String getMetadataPath(CppUtils::String const& storagePrefix);

		


	
// function reads data from the file to memory cache
template <class T>
void readDataToMemoryCache(
			T& et,	
			AggregationHolderList& alist,
			double const& beginData, 
			double const& endData
)
{
	READ_LOCK_FOR_SCOPE(*this);

	map<CppUtils::String, CacheFileContext>::iterator it = cacheDescriptor_m.find(
		CppUtils::createValidFileName(alist.getProvider())+CppUtils::createValidFileName(alist.getSymbol())
	);
	
	
	if (it!=cacheDescriptor_m.end() ) {
			CacheFileContext &cacheFileCtx = it->second;

						

			// clear up what we alreday had in the cache just for consistency
			alist.clearOnlyPriceData();

			// need to clear aggregated data related to the cookie
			et.cleanupAggregatedData(alist.getCookie());
		
			// read history file
			HlpStruct::RtData rtdata;
			int rres = 0;

			// set provider and symbol in advance
			rtdata.setProvider(alist.getProvider().c_str());
			rtdata.setSymbol(alist.getSymbol().c_str());
			
		
			HlpStruct::RtBufferData bufferdata;

			cacheFileCtx.flush();
			cacheFileCtx.openFile();
			
			cacheFileCtx.setPosToBegin();

			//CppUtils::positionFromBegin(cacheFileCtx.getOpenHandle(), 0, false);
			while((rres = cacheFileCtx.readData((CppUtils::Byte*)&bufferdata, sizeof(HlpStruct::RtBufferData))) > 0) {

				assert(bufferdata.getTime() > 0);

				if (beginData > 0 && beginData > bufferdata.getTime() )
					continue;

				if (endData > 0 && endData < bufferdata.getTime())
					continue;

				// in simulation mode
				if (simulationTimeOffset_m > 0 && simulationTimeOffset_m < bufferdata.getTime() )
					break;
				
				HlpStruct::bufferToRtData(bufferdata, rtdata);

				//CppUtils::String times = CppUtils::getGmtTime(rtdata.getTime());
				et.aggregatePassedData(rtdata, alist.getMultFactor(), alist.getAggregationPeriod(), alist);
				
        
			} // end loop

			cacheFileCtx.closeFile();

			LOG( MSG_DEBUG, "HISTCONTEXT", "Updated memory cache - symbol \"" << alist.getSymbol() << "\" provider \"" << alist.getProvider() << 
					"\" read file: \"" << cacheFileCtx.getFilename() << "\" new memory cached period is: "  <<
					CppUtils::getGmtTime(alist.getBeginDate()) << " - " << CppUtils::getGmtTime(alist.getEndDate()));
		
			
		
	}

}

	
		
private:

		
		void createNewCacheEntry(
			CacheFileContext& cacheContext,
			CppUtils::String const& provider_valid,
			CppUtils::String const& symbol_valid,
			CppUtils::String const& provider,
			CppUtils::String const& symbol
		);

		
		

	private:

		void readAllDatFiles(CppUtils::StringList& flist, CppUtils::String const& baseDir);

		void initialize();

		// this is a special function create good file path
		CppUtils::String getPathPiece(CppUtils::String const& symbolValid);
	private:

		CppUtils::String startupDir_m;

		CppUtils::String tmpDir_m;

		CppUtils::String metadataPath_m;

		// chache files descriptor
		map<CppUtils::String, CacheFileContext> cacheDescriptor_m;

	
		// storage prefix
		CppUtils::String storagePrefix_m;

		// when starting simulating when set this shows up to what time frame alows reading
		double simulationTimeOffset_m;

	
	
	};

	class INQUEUE_BCK_EXP SimulHistoryContext: public HistoryContext
	{
	public:

    
		struct SimulationEntry
		{
			SimulationEntry():
				created_m(-1)
			{
			}

			CppUtils::Uid uid_m;
			CppUtils::String id_m;
			double created_m;
		};

		static CppUtils::String const &getRootObjectName ();
		
		static CppUtils::String getSymbolListPath(CppUtils::String const& storagePrefix);

		static CppUtils::String createStoragePrefixFromUid(CppUtils::Uid const& uid);

		static void resolveAllSimulationEntries(vector<SimulationEntry>& entryList);

				
		// ctor & dtor
		SimulHistoryContext(CppUtils::Uid const& uid);

		virtual ~SimulHistoryContext();

		// this functions stores all ticks in TICKER_STREAM_FILE
		// single thread
		void storeRtDataSeparately(HlpStruct::RtData const& rtdata);

		CppUtils::String const& getTickerStreamPath() const;

		void initStreamFile();

		void closeStreamFile();

		// this just read from the stream file and no warries about how to read this stuff - just reead as it is
		long readAllData(
			HlpStruct::ProviderSymbolList const& provSymbSimulationList,
			SimulationCollector& collector);
		
	private:

		static CppUtils::String resolveBasePath();

		// can't use
		SimulHistoryContext(char const* storagePrefix);

		CppUtils::String tickerStreamPath_m;

		HANDLE file_m;

		CppUtils::Byte streamBuffer_m[BUFFER_STREAM_FILE_SIZE*sizeof(HlpStruct::RtData)];

		int streamBufferOffset_m;


	};

};

#endif

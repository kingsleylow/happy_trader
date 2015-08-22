#include "histcontext.hpp"
#include <time.h>
#include "enginethread.hpp"
#include "comproc.hpp"


#define HISTCONTEXT "HISTCONTEXT"
#define FILE_DATA_EXT ".DAT"



namespace Inqueue {


// ------------------------------------------------------------
CacheFileContext::CacheFileContext(): 
		file_m ( NULL ),
		beginTime_m( -1.0 ),
		endTime_m (-1.0 ),
		isRemove_m ( true ),
		bufferPos_m(0)
	{
	};  

	bool CacheFileContext::isInitialized() const
	{
		return (fileName_m.size() > 0);
	}

	// delayed append
	void CacheFileContext::append_data(CppUtils::Byte const* buffer, size_t bsize) 
	{
		if ((bufferPos_m + bsize) >BUFFER_NOOP_SIZE ) {
			flush();
		}

		
		memcpy(buffer_m+bufferPos_m, buffer, bsize);
		bufferPos_m += bsize;
		
	}


	// reset buffer and write to file
	void CacheFileContext::flush()
	{
		if (bufferPos_m <=0)
			return;

		openFile();

		// need to write header info with date time
		FileHeader hdr(provider_m, symbol_m, getBeginTime(), getEndTime());
		CppUtils::positionFromBegin(file_m,0, false); // do not rewrite

		if(CppUtils::writeToFile(file_m, (CppUtils::Byte const*)&hdr, sizeof(FileHeader)) <=0) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotWriteFile", "ctx_flush", CppUtils::getOSError()+"-" +fileName_m );
		}


		// to the end of file
		CppUtils::positionFromEnd(file_m,0, false);

		if(CppUtils::writeToFile(file_m, buffer_m, bufferPos_m) <=0) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotWriteFile", "ctx_flush", CppUtils::getOSError()+"-" +fileName_m );
		}

		bufferPos_m = 0;
		closeFile();
	};

	
	// internally opens file and returns handler
	void CacheFileContext::openFile() 
	{
		if (file_m != NULL)
			THROW(CppUtils::OperationFailed, "exc_fileAlreadyOpen", "ctx_openFile", fileName_m );

		file_m = CppUtils::openFile(fileName_m, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS);

	
		if (file_m==NULL || file_m==INVALID_HANDLE_VALUE)
			THROW(CppUtils::OperationFailed, "exc_CannotOpenFile", "ctx_openFile", CppUtils::getOSError()+"-" +fileName_m );	


	
	}

	void CacheFileContext::closeFile()
	{
		

		bool res = CppUtils::closeFile(file_m);
		if (!res)
			THROW(CppUtils::OperationFailed, "exc_CannotCloseFile", "ctx_closeFile", CppUtils::getOSError()+"-" +fileName_m );	

	

		file_m = NULL;
	}

	void CacheFileContext::clearFile()
	{
		openFile();
		if (!CppUtils::positionFromBegin( file_m, sizeof(FileHeader), true )) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotSetPosFromBeginErasingFile", "ctx_clearFile", CppUtils::getOSError()+"-" +fileName_m );
		}
		closeFile();
	}

	void CacheFileContext::setPosToBeginRewriteContent(CppUtils::String const& provider, CppUtils::String const& symbol)
	{
		if (!CppUtils::positionFromBegin( file_m, 0, true )) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotSetPosFromBeginErasingFile", "ctx_setPosToBeginRewriteContent", CppUtils::getOSError()+"-" +fileName_m );
		}

		FileHeader hdr(provider, symbol, -1, -1);
		if(CppUtils::writeToFile(file_m, (CppUtils::Byte const*)&hdr, sizeof(FileHeader)) <=0) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotWriteFile", "ctx_setPosToBeginRewriteContent", CppUtils::getOSError()+"-" +fileName_m );
		}
	}



	void CacheFileContext::setPosToBegin()
	{
		if (!CppUtils::positionFromBegin(file_m, sizeof(FileHeader), false)) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotSetPosToBegin", "ctx_setPosToBegin", CppUtils::getOSError()+"-" +fileName_m );
		}

	}

	void CacheFileContext::setPosFromBegin(long offset)
	{
		if (!CppUtils::positionFromBegin(file_m, offset + sizeof(FileHeader), false)) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotSetFilePos", "ñtx_SetPosFromBegin", CppUtils::getOSError()+"-" +fileName_m );
		}

	}

	void CacheFileContext::setPosToEnd()
	{
		if (!CppUtils::positionFromEnd(file_m, 0, false)) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotSetPosToEnd", "ctx_setPosToEnd", CppUtils::getOSError()+"-" +fileName_m );
		}
	}

	void CacheFileContext::setPosFromEnd(long offset)
	{
		if (!CppUtils::positionFromEnd(file_m, offset, false)) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotSetPosFromEnd", "ctx_setPosFromEnd", CppUtils::getOSError()+"-" +fileName_m );
		}
	}

	long CacheFileContext::readData(CppUtils::Byte* buffer, size_t buflength)
	{
		long res = CppUtils::readFromFile(file_m, buffer, buflength);
		if (res < 0) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotReadFile", "ctx_ReadData", CppUtils::getOSError()+"-" +fileName_m );
		}

		return res;

	}
	void CacheFileContext::readHeader(FileHeader& header)
	{
		if (!CppUtils::positionFromBegin(file_m, 0, false)) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotSetFilePos", "ñtx_readHeader", CppUtils::getOSError()+"-" +fileName_m );
		}
		
		long res = CppUtils::readFromFile(file_m, ( CppUtils::Byte* )&header, sizeof(FileHeader));
		if (res != sizeof(FileHeader)) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotReadFile", "tx_readHeader", CppUtils::getOSError()+"-" +fileName_m );
		}

	}


	long CacheFileContext::getFileSize()
	{
		long filesize = CppUtils::getFileSize(file_m);
		if (filesize <0) {
			closeFile();
			THROW(CppUtils::OperationFailed, "exc_CannotReturnFileSize", "ctx_GetFileSize", CppUtils::getOSError()+"-" +fileName_m );
		}

		return filesize;

	}


	



// ------------------------------------------------------------

CppUtils::String const & HistoryContext::getRootObjectName () {
		static CppUtils::String string_s("hist_context");
		return string_s;
}

HistoryContext::HistoryContext(char const* storagePrefix):
	storagePrefix_m(storagePrefix ? storagePrefix : "default"),
	simulationTimeOffset_m(-1)
{
	
	initialize();
}  

HistoryContext::~HistoryContext()
{
	// clear the remaining cache
	// write lock when write data
	WRITE_LOCK_FOR_SCOPE(*this);
		
	for(map<CppUtils::String, CacheFileContext>::iterator it=cacheDescriptor_m.begin(); it != cacheDescriptor_m.end();it++)
	{
		CacheFileContext &ctx = it->second;
		if (ctx.isInitialized()) {
			
		
			ctx.flush();

			if (ctx.getRemoveFlag()) {
				CppUtils::removeFile(ctx.getFilename());
				LOG( MSG_DEBUG, HISTCONTEXT, "Removed cache file " << ctx.getFilename() );
			} else {
				LOG( MSG_DEBUG, HISTCONTEXT, "Left intact cache file " << ctx.getFilename() );
			}

			//ctx.file_m = NULL;
			ctx.getFilename() = "";

			
		}
	};

	// do not force
	CppUtils::removeDir(tmpDir_m);
}
		
void HistoryContext::readAllDatFiles(CppUtils::StringList& flist, CppUtils::String const& baseDir)
{

	CppUtils::StringList l_fileName;
	CppUtils::StringList l_dirName;
	CppUtils::getAllFileMatches(baseDir + "\\*.*", l_fileName, l_dirName);

	for(int i = 0; i < l_dirName.size(); i++) {
		CppUtils::String const& dir_i = l_dirName[i];

		CppUtils::String newBaseDir = baseDir + "\\" + dir_i;
		readAllDatFiles(flist, newBaseDir);

	}
	
	for(int i  = 0; i < l_fileName.size(); i++) {
		CppUtils::String file_i = baseDir + "\\" + l_fileName[i];
		CppUtils::String file_iu(file_i);
		CppUtils::toupper(file_iu);

		if (file_iu.find(FILE_DATA_EXT) != CppUtils::String::npos) {
			flist.push_back(baseDir + "\\" + l_fileName[i]);
			//LOG( MSG_INFO, HISTCONTEXT, "Added file to the list: " << (baseDir + "\\" + l_fileName[i]));
		}
	}


}

void HistoryContext::initialize()
{
	LOG( MSG_INFO, HISTCONTEXT, "Initializing history context...");


	// deterime execution directory
	CppUtils::String str;
	if( !CppUtils::getEnvironmentVar( "%HT_SERVER_DIR%", str ) || str.empty() || !CppUtils::directoryExists( str ) ) {
		THROW(CppUtils::OperationFailed, "exc_InvalidServerRoot", "ctx_HistoryContextBot", "HT_SERVER_DIR is not set" );					
	}

	startupDir_m = str + "\\sdata\\";

	// create temporary directory
	tmpDir_m = startupDir_m + CppUtils::createValidFileName(Inqueue::InQueueController::resolveSessionName()) + "\\hstctx\\" + storagePrefix_m;
	metadataPath_m = tmpDir_m + "\\" + METATADA_FILE;
	CppUtils::makeDir(tmpDir_m);
	

	// simulation time offset
	simulationTimeOffset_m = -1;

	//
	
	CppUtils::StringList l_fileName;
	readAllDatFiles(l_fileName, tmpDir_m);

	
	for(int i  = 0; i < l_fileName.size(); i++) {
		CppUtils::String const& fullFilePath = l_fileName[ i ];

		//LOG( MSG_INFO, HISTCONTEXT, "open file: \"" << fullFilePath << "\"");

		// open file and read header
		FileHandle f = CppUtils::openFile(fullFilePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS);
	
		if (f==NULL || f==INVALID_HANDLE_VALUE)
			THROW(CppUtils::OperationFailed, "exc_CannotOpenFile", "ctx_initialize", CppUtils::getOSError()+"-" +fullFilePath );	

		FileHeader fheader;
		long res = CppUtils::readFromFile(f, ( CppUtils::Byte* )&fheader, sizeof(FileHeader));
		if (res != sizeof(FileHeader)) {
			CppUtils::closeFile( f );
			THROW(CppUtils::OperationFailed, "exc_CannotReadFile", "ctx_initialize", CppUtils::getOSError()+"-" +fullFilePath );
		}
		
		CppUtils::closeFile( f );

		// now ready to initialize
		setReuseCacheFlag(fheader.provider_m, fheader.symbol_m, false );

		
	}



	// initialize this
	LOG( MSG_INFO, HISTCONTEXT, "Successfully initialized, history context directory is \"" << tmpDir_m << "\"");
}

CppUtils::String HistoryContext::getPathPiece(CppUtils::String const& symbolValid)
{
	CppUtils::String out;

	if (symbolValid.size() >=1) {
		out.append(symbolValid.substr(0,1));

		if (symbolValid.size() >=2) {
			out.append(symbolValid.substr(1,1));

		}
	}

	return out;
}


void HistoryContext::passRtData(HlpStruct::RtData const& rtdata, bool& insideExistingTime)
{
	{
		WRITE_LOCK_FOR_SCOPE(*this);

		insideExistingTime = false;
		
		CppUtils::String provider_valid = CppUtils::createValidFileName(rtdata.getProvider());
		CppUtils::String symbol_valid = CppUtils::createValidFileName(rtdata.getSymbol());
		CppUtils::String key = provider_valid+symbol_valid;
  
		CacheFileContext& cacheContext = cacheDescriptor_m[key];
				
		if (!cacheContext.isInitialized()) {
			// not initialized
			createNewCacheEntry(cacheContext, provider_valid, symbol_valid, rtdata.getProvider(), rtdata.getSymbol());
			LOG( MSG_DEBUG, HISTCONTEXT, "Created new cache file via RT retreival: " << cacheContext.getFilename());

		}

	
		cacheContext.setupBeginTime(rtdata.getTime());
		
		if (cacheContext.getEndTime() > 0 && rtdata.getTime() <= cacheContext.getEndTime()) {
			
			insideExistingTime = true;
			// !!!!
			//return;
		}
		
		HlpStruct::RtBufferData bufferRtData;
		HlpStruct::bufferFromRtData(rtdata, bufferRtData);

		cacheContext.append_data((CppUtils::Byte const*)&bufferRtData, sizeof(HlpStruct::RtBufferData));
	
		cacheContext.setupEndTime(rtdata.getTime());
		
	}
	

}

void HistoryContext::clearHistoryCache()
{
	// write lock when write data
	WRITE_LOCK_FOR_SCOPE(*this);

	for(map<CppUtils::String, CacheFileContext>::iterator it = cacheDescriptor_m.begin(); it != cacheDescriptor_m.end(); it++) {
		CacheFileContext& cacheContext = it->second;

		if (cacheContext.isInitialized()) {
			
			CppUtils::removeFile(cacheContext.getFilename());
		
			//ctx.file_m = NULL;
			cacheContext.getFilename() = "";

			LOG( MSG_DEBUG, HISTCONTEXT, "Removed cache file " << cacheContext.getFilename() );
		}
		
	}

	cacheDescriptor_m.clear();

}
void HistoryContext::clearHistoryCache(CppUtils::String const& provider, CppUtils::String const& symbol)
{
	// write lock when write data
	WRITE_LOCK_FOR_SCOPE(*this);

	map<CppUtils::String, CacheFileContext>::iterator it = cacheDescriptor_m.find(
		CppUtils::createValidFileName(provider)+CppUtils::createValidFileName(symbol)
	);
	

	if (it == cacheDescriptor_m.end()) {
		// ignoring , not found
		return;
	}
	else {
		CacheFileContext& cacheContext = it->second;

		cacheContext.clearFile();
		cacheContext.resetBeginEndTime();

		LOG( MSG_DEBUG, HISTCONTEXT, "Cleared previous cache file: " << cacheContext.getFilename());
	}
	
}

void HistoryContext::passHistoryChunk(HlpStruct::RtDataList const& rtlist)
{
	// write lock when write data
	WRITE_LOCK_FOR_SCOPE(*this);
	
	HlpStruct::RtBufferData bufferRtData;
	
	for(int i = 0; i < rtlist.size(); i++) {
		HlpStruct::RtData const& rtdata = rtlist[ i ];

		passHistoryEntry( rtlist[ i ], bufferRtData);
				
	} // end loop

	// flush everything
	flushAll();

	LOG( MSG_INFO, HISTCONTEXT, "History chunk passed of length: " << rtlist.size() );
}
		

void HistoryContext::passHistoryEntry(HlpStruct::RtData const& rtdata)
{
		
	// write lock when write data
	WRITE_LOCK_FOR_SCOPE(*this);

	HlpStruct::RtBufferData bufferRtData;
	passHistoryEntry(rtdata, bufferRtData);

}


void HistoryContext::passHistoryEntry(HlpStruct::RtData const& rtdata, HlpStruct::RtBufferData& bufferRtData)
{
	// write lock when write data
	WRITE_LOCK_FOR_SCOPE(*this);

	CppUtils::String provider_valid = CppUtils::createValidFileName(rtdata.getProvider());
	CppUtils::String symbol_valid = CppUtils::createValidFileName(rtdata.getSymbol());
	CppUtils::String key = provider_valid+symbol_valid;

	
	CacheFileContext& cacheContext = cacheDescriptor_m[key];
			
	if (!cacheContext.isInitialized()) {
		// not initialized
		createNewCacheEntry(cacheContext, provider_valid, symbol_valid, rtdata.getProvider(), rtdata.getSymbol());
		LOG( MSG_DEBUG, HISTCONTEXT, "Created new cache file via HST retreival: " << cacheContext.getFilename());

	}


	cacheContext.setupBeginTime(rtdata.getTime());

	// TODO !!! must be revised
	if (cacheContext.getEndTime() > 0 && rtdata.getTime() <= cacheContext.getEndTime())
		return;
	
	
	HlpStruct::bufferFromRtData(rtdata, bufferRtData);

	cacheContext.append_data( (CppUtils::Byte const*)&bufferRtData, sizeof(HlpStruct::RtBufferData) );
	cacheContext.setupEndTime(rtdata.getTime());
}



void HistoryContext::flushAll()
{
	// write lock when write data
	WRITE_LOCK_FOR_SCOPE(*this);

	// flush everything
	for(map<CppUtils::String, CacheFileContext>::iterator it=cacheDescriptor_m.begin(); it != cacheDescriptor_m.end();it++)	{
		
		CacheFileContext &ctx = it->second;
		if (ctx.isInitialized()) {
			ctx.flush();
		}
	}
}



void HistoryContext::shutdown()
{
	
}

CppUtils::String const& HistoryContext::getBaseDirectory() const
{
	return tmpDir_m;
}

CppUtils::String const& HistoryContext::getMetadataPath() const
{
	return metadataPath_m;
}



CppUtils::String const& HistoryContext::getStoragePrefix() const
{
	return storagePrefix_m;
}

CppUtils::String HistoryContext::getMetadataPath(CppUtils::String const& storagePrefix)
{
	CppUtils::String str;
	if( !CppUtils::getEnvironmentVar( "%HT_SERVER_DIR%", str ) || str.empty() || !CppUtils::directoryExists( str ) ) {
		THROW(CppUtils::OperationFailed, "exc_InvalidServerRoot", "ctx_getMetadataPath", "HT_SERVER_DIR is not set" );					
	}

	// create temporary directory
	str.append("\\sdata\\");
	str.append(CppUtils::createValidFileName(Inqueue::InQueueController::resolveSessionName()));
	str.append("\\hstctx\\simulation\\");
	str.append(storagePrefix);
	str.append("\\");
	str.append(METATADA_FILE);
	return str;
}


void HistoryContext::createNewCacheEntry(
	CacheFileContext& cacheContext,
	CppUtils::String const& provider_valid,
	CppUtils::String const& symbol_valid,
	CppUtils::String const& provider,
	CppUtils::String const& symbol
	)
{
	
	
	cacheContext.resetBeginEndTime();

	cacheContext.getProvider() = provider;
	cacheContext.getSymbol() = symbol;
	
	//CppUtils::String basedir = tmpDir_m + "\\" + provider_valid;
	CppUtils::String basedir = tmpDir_m + "\\" + provider_valid + "\\" + getPathPiece(symbol_valid);
	
	CppUtils::makeDir( basedir );

	// before we add a file name to the special list

	// '.' - is a delimiter between provider and symbol
	cacheContext.getFilename() = basedir + "\\"+ symbol_valid +FILE_DATA_EXT;
		
	// open file or re-write this
	cacheContext.openFile();

	// if the file is empty
	long filesize = cacheContext.getFileSize() - sizeof(FileHeader);
	
	//
	if (filesize >= (long)sizeof(HlpStruct::RtBufferData)  ) {

		// read header
			
		// determine begin and end dates of cached data
		cacheContext.setPosToBegin();

		FileHeader header;
		cacheContext.readHeader(header);

		cacheContext.setupBeginTime(header.timeBegin_m);
		cacheContext.setupEndTime(header.timeEnd_m);
		
		// set at the end of file
		cacheContext.setPosToEnd();
		
		LOG( MSG_DEBUG, HISTCONTEXT, "When using already cached content: \"" << cacheContext.getFilename() << "\" found data from: "
			<< CppUtils::getGmtTime2(cacheContext.getBeginTime()) << " to: " << CppUtils::getGmtTime2(cacheContext.getEndTime()));

		
	}
	else {
		
		
		// rewrite
		cacheContext.setPosToBeginRewriteContent( provider, symbol);
	
	}

	cacheContext.closeFile();
	
		
}

void HistoryContext::returnCachingPeriod(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol, 
			double& btime, 
			double& etime
)
{
	READ_LOCK_FOR_SCOPE(*this);

	
	map<CppUtils::String, CacheFileContext>::const_iterator it = cacheDescriptor_m.find(
		CppUtils::createValidFileName(provider)+CppUtils::createValidFileName(symbol)
	);
	if ( it == cacheDescriptor_m.end()) {
		THROW(CppUtils::OperationFailed, "exc_CannotFindSymbolInCache", "ctx_GetRtPeriod", provider + " - " + symbol );	
	}

	CacheFileContext const& context = it->second; 

	btime = context.getBeginTime();
	etime = context.getEndTime();


}

void HistoryContext::setReuseCacheFlag(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol,
			bool const isReuseCache
		)
{
	// write lock when write data
	WRITE_LOCK_FOR_SCOPE(*this);

	CppUtils::String provider_valid = CppUtils::createValidFileName(provider);
	CppUtils::String symbol_valid = CppUtils::createValidFileName(symbol);
	CppUtils::String key = provider_valid+symbol_valid;
		
	CacheFileContext& cacheContext = cacheDescriptor_m[key];
				
	if (!cacheContext.isInitialized()) {
	
		createNewCacheEntry(cacheContext, provider_valid, symbol_valid, provider, symbol);
		LOG( MSG_DEBUG, HISTCONTEXT, "Created new cache entry via set reuse flag : " << cacheContext.getFilename());
	}


	
	// set reuse flag
	cacheContext.getRemoveFlag() = !isReuseCache;
	LOG( MSG_DEBUG, HISTCONTEXT, "For a single cache entry: " << cacheContext.getFilename() << " is remove flag is: " << (cacheContext.getRemoveFlag()? "TRUE":"FALSE"));


}

bool HistoryContext::getReuseCacheFlag(
			CppUtils::String const& provider, 
			CppUtils::String const& symbol
		)
{
	READ_LOCK_FOR_SCOPE(*this);

	
	map<CppUtils::String, CacheFileContext>::const_iterator it = cacheDescriptor_m.find(
		CppUtils::createValidFileName(provider)+CppUtils::createValidFileName(symbol)
	);
	if ( it == cacheDescriptor_m.end()) {
		return false;
	}

	CacheFileContext const& context = it->second; 
	return (!context.getRemoveFlag());
}

void HistoryContext::setReuseCacheFlag(bool const isReuseCache)
{
	WRITE_LOCK_FOR_SCOPE(*this); 

	for(map<CppUtils::String, CacheFileContext>::iterator it=cacheDescriptor_m.begin(); it != cacheDescriptor_m.end();it++)	{
		
		CacheFileContext &ctx = it->second;
		if (ctx.isInitialized()) {
			ctx.getRemoveFlag() = !isReuseCache;
			LOG( MSG_DEBUG, HISTCONTEXT, "For a cache entry: " << ctx.getFilename() << " is remove flag is: " << (ctx.getRemoveFlag()? "TRUE":"FALSE"));
		}
	}

}


void HistoryContext::getCachedContext(vector<CacheFileContext> &cachedContext) const
{
	READ_LOCK_FOR_SCOPE(*this);
	cachedContext.clear();

		
	for(map<CppUtils::String, CacheFileContext>::const_iterator it = cacheDescriptor_m.begin();	it != cacheDescriptor_m.end(); it++) 	{
		cachedContext.push_back( it->second );
	}

	
}




// -----------------------------------------

CppUtils::String const & SimulHistoryContext::getRootObjectName () {
		static CppUtils::String string_s("sim_hist_context");
		return string_s;
}

CppUtils::String SimulHistoryContext::resolveBasePath()
{
	CppUtils::String basedir;
	if( !CppUtils::getEnvironmentVar( "%HT_SERVER_DIR%", basedir ) || basedir.empty() || !CppUtils::directoryExists( basedir ) ) {
		THROW(CppUtils::OperationFailed, "exc_InvalidServerRoot", "ctx_ResolveBasePath", "HT_SERVER_DIR is not set" );					
	}

	basedir.append("\\sdata\\");
	basedir.append(CppUtils::createValidFileName(Inqueue::InQueueController::resolveSessionName()));
	basedir.append("\\hstctx\\");

	return basedir;
}


SimulHistoryContext::SimulHistoryContext(CppUtils::Uid const& uid):
HistoryContext(createStoragePrefixFromUid(uid).c_str()),
file_m(NULL),
streamBufferOffset_m(0)
{
	
	// create temporary directory
	tickerStreamPath_m.append(resolveBasePath());
	
	tickerStreamPath_m.append(getStoragePrefix());
	tickerStreamPath_m.append("\\");
	tickerStreamPath_m.append(TICKER_STREAM_FILE);

	
	
}

SimulHistoryContext::~SimulHistoryContext()
{
	closeStreamFile();
}

CppUtils::String SimulHistoryContext::createStoragePrefixFromUid(CppUtils::Uid const& uid)
{
	return (CppUtils::String("simulation\\") + uid.toString());
}

CppUtils::String SimulHistoryContext::getSymbolListPath(CppUtils::String const& storagePrefix)
{
	
	CppUtils::String str;

	// create temporary directory
	str.append(resolveBasePath());
	str.append(storagePrefix);
	str.append("\\");
	str.append(SYMBOL_LIST_FILE);
	return str;
}


void SimulHistoryContext::resolveAllSimulationEntries(vector<SimulationEntry>& entryList)
{
	// badse dir

	entryList.clear();

	CppUtils::String basedir;
	

	// create temporary directory
	basedir.append(resolveBasePath());
	basedir.append("simulation\\");

	if (!CppUtils::directoryExists(basedir))
		return;


	CppUtils::StringList l_fileName;
	CppUtils::StringList l_dirName;
	CppUtils::getAllFileMatches(basedir + "\\*.*", l_fileName, l_dirName);

	for(int i = 0; i < l_dirName.size(); i++) {
		SimulationEntry e;
		e.uid_m.fromString(l_dirName[i]);

		if (e.uid_m.isValid()) {
			CppUtils::String fpath = basedir + l_dirName[i] + "\\" + SYMBOL_LIST_FILE;
			CppUtils::String content;

			CppUtils::readContentFromFile2(fpath, content);
			if (content.size() ==0)
				THROW(CppUtils::OperationFailed, "exc_SymbolListContent", "ctx_readAllSimulationEntries", fpath);

			HlpStruct::XmlParameter xmlparam;
			HlpStruct::deserializeXmlParameter(xmlparam, content); 
			
			e.created_m = xmlparam.getParameter("created_on").getAsDate();
			e.id_m = xmlparam.getParameter("id_string").getAsString();

			entryList.push_back(e);

		}
	}
}

void SimulHistoryContext::storeRtDataSeparately(HlpStruct::RtData const& rtdata)
	{
		if (file_m == NULL) {
			THROW(CppUtils::OperationFailed, "exc_FileNotOpened", "ctx_storeRtDataSeparately", "" );
		}

	if ((streamBufferOffset_m + sizeof(HlpStruct::RtData)) > BUFFER_STREAM_FILE_SIZE*sizeof(HlpStruct::RtData)) {
			long written = CppUtils::writeToFile(file_m, (CppUtils::Byte const*)streamBuffer_m, streamBufferOffset_m);
			if (written != streamBufferOffset_m)
				THROW(CppUtils::OperationFailed, "exc_CannotWriteFile", "ctx_storeRtDataSeparately", CppUtils::getOSError()+"-" +tickerStreamPath_m );	


			streamBufferOffset_m=0;
		}

		memcpy((CppUtils::Byte*)streamBuffer_m + streamBufferOffset_m, &rtdata, sizeof(HlpStruct::RtData) );
		streamBufferOffset_m += sizeof(HlpStruct::RtData);
		

	}


	CppUtils::String const& SimulHistoryContext::getTickerStreamPath() const
	{
		return tickerStreamPath_m;
	}

	void SimulHistoryContext::initStreamFile()
	{
		if (file_m == NULL) {
			streamBufferOffset_m = 0;
			file_m = CppUtils::openFile(tickerStreamPath_m, GENERIC_WRITE, 0, CREATE_ALWAYS);
	
			if (file_m==NULL || file_m==INVALID_HANDLE_VALUE)
				THROW(CppUtils::OperationFailed, "exc_CannotOpenFile", "ctx_InitStreamFile", CppUtils::getOSError()+"-" +tickerStreamPath_m );	
			
			
		}
	}

	void SimulHistoryContext::closeStreamFile()
	{
		if (file_m != NULL) {

			// flush the remainder
			long written = CppUtils::writeToFile(file_m, (CppUtils::Byte const*)streamBuffer_m, streamBufferOffset_m);
			if (written != streamBufferOffset_m)
				THROW(CppUtils::OperationFailed, "exc_CannotWriteFile", "ctx_closeStreamFile", CppUtils::getOSError()+"-" +tickerStreamPath_m );	
			
			streamBufferOffset_m = 0;

			CppUtils::closeFile(file_m);
			file_m = NULL;
		}
	}

	long SimulHistoryContext::readAllData(
		  HlpStruct::ProviderSymbolList const& provSymbSimulationList,
			SimulationCollector& collector
	)
	{
			if (file_m != NULL) {
				closeStreamFile();
			}

			file_m = CppUtils::openFile(tickerStreamPath_m, GENERIC_READ, 0, OPEN_ALWAYS);
			if (file_m==NULL || file_m==INVALID_HANDLE_VALUE)
				THROW(CppUtils::OperationFailed, "exc_CannotOpenFile", "ctx_readAllData", CppUtils::getOSError()+"-" +tickerStreamPath_m );	

			
			long globally_read=0;
			long filesize = CppUtils::getFileSize(file_m);
			long chunk_read = 0;
			int current_tickers = 0;

			HlpStruct::RtData buffer[BUFFER_STREAM_FILE_SIZE];
			bool conti = false;
			while(true) {
				
				// read as a chunk
				chunk_read = CppUtils::readFromFile(file_m, ( CppUtils::Byte*)buffer, sizeof(HlpStruct::RtData) * BUFFER_STREAM_FILE_SIZE);
				
				// no read at all
				if (chunk_read <= 0)
					break;

				current_tickers = chunk_read / sizeof(HlpStruct::RtData);
				for(int i = 0; i < current_tickers; i++) {

					//LOG( MSG_INFO, "HST___", "--->" << buffer[i].toString() << "is first: " << (globally_read == 0? "true":"false") << " is last: " << (globally_read == filesize?"true":"false"));
					conti = collector.passRtData(buffer[i], globally_read == 0, globally_read == filesize );
					if (!conti) {
						LOG( MSG_INFO, HISTCONTEXT, "Simulation was forcibly stopped" );
						break;
					}

				}

				if (!conti)
					break;

				if (chunk_read < sizeof(HlpStruct::RtData) * BUFFER_STREAM_FILE_SIZE)
					break;

				globally_read += chunk_read; 

			}	

			CppUtils::closeFile(file_m);
			file_m= NULL;

			// number of ticks
			return globally_read/sizeof(HlpStruct::RtData);

	}







}; // end of namespace
		
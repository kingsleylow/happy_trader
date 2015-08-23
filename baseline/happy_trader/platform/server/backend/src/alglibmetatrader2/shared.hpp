#ifndef _ALGLIBMT2_SHARED_MT_INCLUDED
#define _ALGLIBMT2_SHARED_MT_INCLUDED


#include "shared/cpputils/src/cpputils.hpp"
#include "shared_defs.hpp"

#define SHARED "SHARED"

// ---------------
// definitions of MT scripts and paths
//#define PATH_MT4_MQL "\\experts\\scripts\\"
#define PATH_MT4_MQL "\\MQL4\\Scripts\\"
#define PATH_MT4_MQL_EA "\\MQL4\\Experts\\"
#define PATH_MT4_PRESETS "\\MQL4\\Presets\\"
#define PATH_MT4_CONF "config\\"

#define NAME_MT4_MQL_BASE "mtht_export_engine"
#define NAME_MT4_MQL_COMPILED_EXT ".ex4"
#define NAME_MT4_MQL_SOURCE_EXT ".mq4"



#define NAME_MT4_MQL_COMPILED  NAME_MT4_MQL_BASE NAME_MT4_MQL_COMPILED_EXT
#define NAME_MT4_MQL_SOURCE  NAME_MT4_MQL_BASE NAME_MT4_MQL_SOURCE_EXT

#define NAME_MT4_MQL_COMPILED_EA  NAME_MT4_MQL_BASE_EA NAME_MT4_MQL_COMPILED_EXT
#define NAME_MT4_MQL_SOURCE_EA  NAME_MT4_MQL_BASE_EA NAME_MT4_MQL_SOURCE_EXT


#define NAME_MT4_MQL_INIT_CONFIG "htmt_profile.ini"
#define NAME_MT4_MQL_INIT_PARAMS "mtht_export_engine_param.set"
#define NAME_MT4_MQL_INIT_PARAMS_EA "mtht_export_engine_param_ea.set"

#define START_MT4_CMD_PARAMS_PORTABLE "/portable "
#define START_MT4_CMD_PARAMS START_MT4_CMD_PARAMS_PORTABLE PATH_MT4_CONF NAME_MT4_MQL_INIT_CONFIG

#define BASE_NAME_MT4_PROXY_DLL "htmtproxy_bck"

#ifdef _DEBUG
#define SOURCE_NAME_MT4_PROXY_DLL_BASE  BASE_NAME_MT4_PROXY_DLL##"_d.dll" 
#else
#define SOURCE_NAME_MT4_PROXY_DLL_BASE  BASE_NAME_MT4_PROXY_DLL##".dll" 
#endif

#define TARGET_NAME_MT4_PROXY_DLL_BASE  BASE_NAME_MT4_PROXY_DLL##".dll"

// how many periods of heartbeats from mtproxy -> alglibmetatrader must elapse to assume connecyion is dead
//#define CONNECTION_LOST_HEARTBEATS_MULT 3

// now many second we wait for completion time of child mt
#define WAIT_CHILDMT_COMPLETION_SEC 60

// child mt log level
#define CHILD_MT_LOG_LEVEL 0

#define MT_WORK_THREAD_SLEEP_SEC 1
#define MT_WORK_THREAD_FINISH_WAIT_SEC 10
#define MT_WORK_THREAD_START_WAIT_SEC 10

#define REMOVE_OLD_MESSAGE_CHECK_PERIOD_SEC 2

#define MTRUNNER_BASE_DIR "__mtrunner"

// ---------------------


namespace AlgLib {

	template<typename T>
	class ComparatorMt
	{
	public:
		bool operator()(T const& val) const
		{
			THROW(CppUtils::OperationFailed, "exc_NotImplemented", "ctx_Comparator", "" );
		}
	};

	template<>
	class ComparatorMt<CppUtils::String>
	{
		public:
		bool operator()(CppUtils::String const& val) const
		{
			return (CppUtils::toLowerCase(val) == "true" );
		}

		
	};

	template<>
	class ComparatorMt<char*>
	{
		public:
		bool operator()(const char* val) const
		{
			if (val == NULL)
				return false;
			return (_stricmp( val, "true" ) == 0);
		}
	};


	// --------------------------

	class MtCommonHelpers {
	public:

	

		static CppUtils::String createMtParameterContent(
			char const* configXmlFile,
			int const slaveLogLevel,
			char const* slaveTmpDir
		)
		{
				CppUtils::String setFile_dest_content;
				
				//setFile_dest_content.append("config_file_g=").append(configXmlFile).append("\n");
						setFile_dest_content.append("wait_kill_g=").append(CppUtils::long2String(WAIT_CHILDMT_COMPLETION_SEC)).append("\n").
						append("run_script_as_slave_tmpdir_g=").append(slaveTmpDir).append("\n"). // temp file path where we to store results
						append("log_level_g=").append(CppUtils::long2String(slaveLogLevel)).append("\n");
				

				return setFile_dest_content;
		};


		// ---------------------------

		static CppUtils::String createMTIniContent(
				const char* user = NULL,
				const char* pass = NULL,
				const char* server = NULL
			)
		{

			/*
			; open chart and run expert and/or script
			Symbol=EURUSD
			Period=H4
			Script=mtht_export_engine
			ScriptParameters=mtht_export_engine_param.set 
			; 
			ExpertsEnable=true
			ExpertsDllImport=true
			ExpertsDllConfirm=false
			ExpertsExpImport=true
			ExpertsTrades=true
			ExpertsTradesConfirm=false

			Login=1122716049
			Password=1MmWpHl
			Server=MetaQuotes-demo
			*/

			CppUtils::String inifile_cont;

			inifile_cont.append("; Created automatically on ").append(CppUtils::getGmtTime(CppUtils::getTime())).append("\n").
			append("Symbol=EURUSD\n").
			append("Period=H4\n").
			append("Script=").append(NAME_MT4_MQL_BASE).append("\n").
			append("ScriptParameters=").append(NAME_MT4_MQL_INIT_PARAMS).append("\n").
			append(";\n"). 
			append("ExpertsEnable=true\n").
			append("ExpertsDllImport=true\n").
			append("ExpertsDllConfirm=false\n").
			append("ExpertsExpImport=true\n").
			append("ExpertsTrades=true\n").
			append("ExpertsTradesConfirm=false");

			if (!CppUtils::nvl(user) && !CppUtils::nvl(pass) && !CppUtils::nvl(server) ) {
					inifile_cont.append("\n; Login details are provided\n").
					append("Login=").append(user).append("\n").
					append("Password=").append(pass).append("\n").
					append("Server=").append(server);
			}

			return inifile_cont;
		}

	};


	// ---------------------

	struct AutoLoadFileStruct
	{
		struct AutoLoadFileRow
		{
			CppUtils::String user_m;
			CppUtils::String pass_m;
			CppUtils::String srv_m;
			int tout_m;

		};

		vector<AutoLoadFileRow> rows_m;

		static int loadFromFile(CppUtils::String const& path, AutoLoadFileStruct& fstruct)
		{
			fstruct.rows_m.clear();

			CppUtils::String content;
			CppUtils::readContentFromFile2(path, content);
			if (!CppUtils::readContentFromFile2(path, content) || content.size() <= 0)
				return -1;


			CppUtils::StringList rows;
			CppUtils::StringList line;

			static const CppUtils::String file_tokens = "\r\n";
			static const CppUtils::String row_tokens = ",\t";
			static const CppUtils::String trim_tokens = " \t\n\r\"";

			CppUtils::tokenize(content, rows, file_tokens );
			for(int i = 0; i < rows.size(); i++) {
				CppUtils::String & row_i = rows[i];

				// parse row
				CppUtils::trimBoth(row_i);
			
				CppUtils::tokenize(row_i,line, row_tokens );
				if (line.size() != 4)
					THROW(CppUtils::OperationFailed, "exc_InvalidLineFormat", "ctx_autoLoadStartUpFile", row_i );

				// trim
				for(int i = 0; i < line.size(); i++) {
					CppUtils::trimBoth(line[i], trim_tokens.c_str());
				}
			 
				CppUtils::String const& user_i = line[0];
				CppUtils::String const& pass_i = line[1];
				CppUtils::String const& srv_i = line[2];
				int tout_i = atoi(line[3].c_str());
			
				AutoLoadFileStruct::AutoLoadFileRow alrow;
				alrow.pass_m = pass_i;
				alrow.user_m = user_i;
				alrow.srv_m = srv_i;
				alrow.tout_m = tout_i;

				fstruct.rows_m.push_back(alrow);
			}

			return fstruct.rows_m.size();

		};
	};


	//
	// MT specific definitions
#pragma pack(push,1)
struct RateInfo
  {
   unsigned int      ctm;
   double            open;
   double            low;
   double            high;
   double            close;
   double            vol;
  };
#pragma pack(pop)




struct MqlStr
{
   int len;
   char* string;
};




	class MqlStrHelper
	{
	public:
		
		static bool copy(MqlStr* dstMqlStr, char const* srcStr)
		{
			if (dstMqlStr && dstMqlStr->string){
				size_t dstBufSize =  strlen(dstMqlStr->string);
				//LOG(MSG_INFO, "foo:", dstBufSize);
				if (dstBufSize > 0) {

					if (srcStr) {
					
						size_t scrStrLen = strlen(srcStr);
						if (scrStrLen ==0) {
							initEmptyString(dstMqlStr);
							// because empty string
							return true;
						}
						else {
							// copy at max dstBufSize
							strncpy(dstMqlStr->string, srcStr, dstBufSize );
							return (scrStrLen <= dstBufSize);
						}

						
					} else {
						initEmptyString(dstMqlStr);
						// because empty string
						return true;
					}
				
				}

			}

			return false;
		};

		static void forcibleCopy(MqlStr* dstMqlStr, char const* srcStr, int const charsToCopy)
		{
				if (dstMqlStr && dstMqlStr->string ){
					if (charsToCopy >=0 ) {
						strncpy(dstMqlStr->string, srcStr, charsToCopy );
					}
					else {
						initEmptyString(dstMqlStr);
					}
				}
		}


		static CppUtils::String wcharToString(wchar_t const* str )
		{
			if (str == NULL)
				return "";

			int str_length  = wcslen(str);
			if (str_length <= 0)
				return "";


			// we are sure we have only signle byte characters
			CppUtils::String r;
			
			char* buffer = new char[str_length+1];
			//LOG(MSG_DEBUG_DETAIL, "MqlStrHelper", "wcharToString(...) - initialized data length: " <<  str_length);
			
			
			wcstombs( buffer, str, str_length );
			buffer[str_length] = '\0';

			r = buffer;
			delete [] buffer;
			return r;

		}

		static void copyStringToWchar(wchar_t* dst, char const* src)
		{
			if (dst == NULL)
				return;

			if (src==NULL)
				return;

						
			size_t dst_length  = wcslen(dst);
			size_t src_length = strlen(src);

			size_t to_copy = min(src_length, dst_length);
			//LOG(MSG_DEBUG_DETAIL, "MqlStrHelper", "copyStringToWchar(...) - copying string of size: " << src_length << ", to dst of size: " << dst_length);

			mbstowcs(dst, src, to_copy);
		}

		static void copyStringToWchar(wchar_t* dst, char const* src, size_t const maxl)
		{
			if (dst == NULL)
				return;

			if (src==NULL)
				return;

			size_t dst_length  = wcslen(dst);
			size_t src_length = strlen(src);

			size_t to_copy = min(min(src_length, dst_length), maxl );
			mbstowcs(dst, src, to_copy);
		}

		static void copyStringToWcharNoExcept(wchar_t* dst, char const* src)
		{
			// it's not good idea but we need to secure our call
			try {
				copyStringToWchar(dst, src);
			}
			catch(...)
			{
				LOG(MSG_WARN, SHARED, "Unknown exception on copying data: " << (src ? src : ""));
			}
		}

	private:

		static void initEmptyString(MqlStr* dstStr)
		{
			dstStr->len = 0;
			dstStr->string[0] = '\0';
		}
		
	

	};
	

	// ---------------------------------------------

	// with recover serialize option

	class DataQueueSerializable: public CppUtils::DataQueue< GeneralCommand >
	{
		public:

			void initialize(CppUtils::String const& instanceName)
			{
					// create cache file
					cacheFile_m = CppUtils::getTempPath() + "\\" + instanceName + "_tmp_ht_176AC19F2540415C9AD5C91C98D3AB62.dat"; 

					LOG(MSG_DEBUG_DETAIL, SHARED, "Initialized data queue cache file: " << cacheFile_m);
			}
		
			virtual ~DataQueueSerializable()
			{
				saveToCache();
			}
							
			virtual void recoverFromCache()
			{
				if (cacheFile_m.length() <=0)
					THROW(CppUtils::OperationFailed, "exc_NotInitialized", "ctx_recoverFromCache_DataQueueSerializable", "");

				clear();

				if (!CppUtils::fileExists(cacheFile_m)) {
					LOG(MSG_DEBUG, SHARED, "On attempt to read cache file: " << cacheFile_m << " no file found");
					return;
				}

				CppUtils::String data;
				CppUtils::readContentFromFile2(cacheFile_m, data);

				if (data.size() <=0) {
					LOG(MSG_DEBUG, SHARED, "On attempt to read cache file: " << cacheFile_m << " no data found");
					return;
				}

				CppUtils::StringList rows;
				CppUtils::tokenize(data, rows, "\n");

				vector< GeneralCommand > data_list;
				for(int i = 0; i < rows.size(); i++) {
					CppUtils::String const& row_i = rows[i];

					GeneralCommand dataElem;
					dataElem.deserialize(row_i);

					data_list.push_back(dataElem);
				}

			
				
				put(data_list);

				LOG(MSG_DEBUG, SHARED, "Recovered data from file: " << cacheFile_m << ", totals rows read: " << data_list.size());
			}
		
	protected:
			
		virtual void saveToCache()
		{
				if (cacheFile_m.length() <=0)
					THROW(CppUtils::OperationFailed, "exc_NotInitialized", "ctx_saveToCache_DataQueueSerializable", "");

				CppUtils::String data;
				auto func = [&] (list< GeneralCommand >::iterator & beg_it,list< GeneralCommand >::iterator & end_it) {
					for(auto it = beg_it; it != end_it; it++) {
						CppUtils::String row = it->serialize();

						data.append(row).append("\n");
					}
				};

				this->iterateThroughElementsAllElements(func);

				if(!CppUtils::saveContentInFile2(cacheFile_m, data))
					THROW(CppUtils::OperationFailed, "exc_CannotSaveQueueContent", "ctx_saveToCache_DataQueueSerializable", cacheFile_m);

				LOG(MSG_DEBUG_DETAIL, SHARED, "Saved data queue to cache");
			};



	private:
		CppUtils::String cacheFile_m;


	};

}; // end of ns

#endif

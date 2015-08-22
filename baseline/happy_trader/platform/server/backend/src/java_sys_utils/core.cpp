#include "core.hpp"

#include <windows.h>
#include <tchar.h>
#include <process.h>
#include "../../src/inqueue/inqueue.hpp"
#include "imagehlp.h" 





// globals
// -------------------------------



extern "C" {

	// ---------------------------------------

	void JNU_ThrowByName(JNIEnv *env, const char *name, const char *msg)
	{
		jclass cls = env->FindClass( name);
		/* if cls is NULL, an exception has already been thrown */
		if (cls != NULL) {
			env->ThrowNew( cls, msg);
		}
		/* free the local ref */
		env->DeleteLocalRef( cls);
	}




	BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReasion, LPVOID lpReserved) {
		if(dwReasion == DLL_PROCESS_ATTACH) {

		}

		return TRUE;
	};


	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
	{

		return JNI_VERSION_1_6;
	}

	JNIEXPORT void JNICALL  JNI_OnUnload(JavaVM *vm, void *reserved)
	{

	}



	// ----------------------------------------
	// Java implementation

	JNIEXPORT jboolean JNICALL Java_com_fin_httrader_utils_win32_HtSysUtils_dllIsBrokerLib
		(JNIEnv *env, jclass cls, jstring fullPath)
	{
		if (resolveFunction2(env, fullPath) == BRK_LIB)
			return JNI_TRUE;

		return JNI_FALSE;
	}

	JNIEXPORT jboolean JNICALL Java_com_fin_httrader_utils_win32_HtSysUtils_dllIsAlgorithmLib
		(JNIEnv *env, jclass cls, jstring fullPath)
	{

		if (resolveFunction2(env, fullPath) == ALG_LIB)
			return JNI_TRUE;

		return JNI_FALSE;
	}

	JNIEXPORT jstring JNICALL Java_com_fin_httrader_utils_win32_HtSysUtils_returnSystemAnalisysString
  (JNIEnv * env, jclass cls)
	{
		CppUtils::String r ;

		// collect data for 5 seconds
		CppUtils::ResourecUsageStruct res = CppUtils::createFullSystemUsageReport32(3);

		r.append("Total virtual memory used:  ").append(CppUtils::float2String(res.globVirtMemUsageKB)).append(" KB\r\n");
		r.append("Total physical memory used:  ").append(CppUtils::float2String(res.globPhysMemUsageKB)).append(" KB\r\n");
		r.append("\r\n");

		for(auto it = res.cpuUsages.begin(); it != res.cpuUsages.end(); it++) {
			if (it->second.curProcess)
				r.append("* ");
			r.append(it->second.processName).append(", ").append(CppUtils::long2String(it->second.pid)).append(", ");
			r.append(CppUtils::float2String(it->second.virtMemUsageKB, -1, 1)).append(" KB, ");	
			r.append(CppUtils::float2String(it->second.nCpuCopy, -1, 1) ).append("%");
			r.append("\r\n");
		}

		jstring result = env->NewStringUTF(r.c_str());

		return result;
	}


	/**
	Helpers
	*/

	int resolveFunction2(JNIEnv *env, jstring full_path)
	{
		int result = NO_LIB;
		try {
			const char *str_full_path = env->GetStringUTFChars(full_path, NULL);

			if (str_full_path) {
				CppUtils::String fullPath_s = str_full_path;

				CppUtils::StringSet lst;
				static CppUtils::Mutex mtx;
				LOCK_FOR_SCOPE(mtx) {
					ListDLLFunctions(fullPath_s, lst);
				}

				if (lst.find(ENGINE_CREATOR_ALGORITHM_FUNCTION_NAME) != lst.end())
					 result= ALG_LIB;
				else if	 (lst.find(ENGINE_CREATOR_BROKER_FUNCTION_NAME) != lst.end())
					 result= BRK_LIB;

			}

			env->ReleaseStringUTFChars(full_path, str_full_path);
		}
		catch(...)
		{
			JNU_ThrowByName(env, "java/lang/RuntimeException", "Exception on requesting dll type");
		}

		return result;
	}

	int resolveFunction(JNIEnv *env, jstring full_path)
	{
		int result = NO_LIB;
		try {
			const char *str_full_path = env->GetStringUTFChars(full_path, NULL);

			if (str_full_path) {
				CppUtils::String fullPath_s = str_full_path;


				void* h = CppUtils::dynlibOpenEx(fullPath_s, LOAD_WITH_ALTERED_SEARCH_PATH);

				if (h) {

					void* ptr_alg =  CppUtils::dynlibGetSym( h, ENGINE_CREATOR_ALGORITHM_FUNCTION_NAME);
					if (ptr_alg)	
						result= ALG_LIB;

					void* ptr_brk =	 CppUtils::dynlibGetSym( h, ENGINE_CREATOR_BROKER_FUNCTION_NAME);
					if (ptr_brk)	
						result= BRK_LIB;

					CppUtils::dynlibClose(h);

				}
				//else {
				//	LOG(MSG_INFO, "resolveFunction(...)", "Fail load: " << CppUtils::getOSError() << " - " << fullPath_s );
				//}
			}

			env->ReleaseStringUTFChars(full_path, str_full_path);
		}
		catch(...)
		{
			JNU_ThrowByName(env, "java/lang/RuntimeException", "Exception on requesting dll type");
		}

		return result;
	}

	void ListDLLFunctions(CppUtils::String const& sADllName, CppUtils::StringSet& slListOfDllFunctions) { 
		DWORD *dNameRVAs(0);   
		_IMAGE_EXPORT_DIRECTORY *ImageExportDirectory;  
		unsigned long cDirSize;     
		_LOADED_IMAGE LoadedImage;     
		CppUtils::String sName;     
		slListOfDllFunctions.clear();    

		if (MapAndLoad(sADllName.c_str(), NULL, &LoadedImage, TRUE, TRUE))
		{         
			ImageExportDirectory = (_IMAGE_EXPORT_DIRECTORY*)ImageDirectoryEntryToData( LoadedImage.MappedAddress,  false, IMAGE_DIRECTORY_ENTRY_EXPORT, &cDirSize);   

			if (ImageExportDirectory != NULL)         {    
				dNameRVAs = (DWORD *)ImageRvaToVa(LoadedImage.FileHeader,LoadedImage.MappedAddress, ImageExportDirectory->AddressOfNames, NULL);  
				for(size_t i = 0; i < ImageExportDirectory->NumberOfNames; i++) {
					sName = (char *)ImageRvaToVa(LoadedImage.FileHeader,  LoadedImage.MappedAddress, dNameRVAs[i], NULL); 
					slListOfDllFunctions.insert(sName);             
				}         
			}															   
			
			UnMapAndLoad(&LoadedImage);     
		} 
	} 

}; // end of extern "C"




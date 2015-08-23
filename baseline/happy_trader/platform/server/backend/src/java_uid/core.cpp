#include "core.hpp"

#include <windows.h>
#include <tchar.h>
#include <process.h>





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

JNIEXPORT void JNICALL Java_com_fin_httrader_utils_Uid_generateUniqueUid
  (JNIEnv * env, jobject uidObj)
{
	try {
	
		CppUtils::Uid uid;
		uid.generate();

		jlong part1, part2;
	 
		memcpy(&part1, uid.data(), 8 );
		memcpy(&part2, uid.data() + 8, 8 );

		jclass cls = env->FindClass("com/fin/httrader/utils/Uid");

		jmethodID put_jmeth = env->GetMethodID( cls, "setValue", "(JJ)V"); 

		env->CallVoidMethod(uidObj, put_jmeth, part1, part2);
	
	}
	catch(...)
	{
		JNU_ThrowByName(env, "java/lang/RuntimeException", "Exception in UID generation call");
	}

}


}; // end of extern "C"




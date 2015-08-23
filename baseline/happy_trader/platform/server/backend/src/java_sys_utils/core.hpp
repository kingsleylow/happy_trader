#ifndef _JAVA_SYS_UTILS_CORE_INCLUDED
#define _JAVA_SYS_UTILS_CORE_INCLUDED

#include "java_sys_utilsdefs.hpp"
#include "HtSysUtils_C.h"


#define ALG_LIB 1
#define BRK_LIB 2
#define NO_LIB -1

extern "C" {

int resolveFunction(JNIEnv *env, jstring full_path);

int resolveFunction2(JNIEnv *env, jstring full_path);

void ListDLLFunctions(CppUtils::String const& sADllName, CppUtils::StringSet& slListOfDllFunctions);


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved);  

JNIEXPORT void JNICALL  JNI_OnUnload(JavaVM *vm, void *reserved);  



}; // end of C




#endif 
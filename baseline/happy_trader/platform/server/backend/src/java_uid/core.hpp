#ifndef _JAVA_UID_CORE_INCLUDED
#define _JAVA_UID_CORE_INCLUDED

#include "java_uiddefs.hpp"
#include "EUid_C.h"


extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved);  

JNIEXPORT void JNICALL  JNI_OnUnload(JavaVM *vm, void *reserved);  



}; // end of C




#endif 
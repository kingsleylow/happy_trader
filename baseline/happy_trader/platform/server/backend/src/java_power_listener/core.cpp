
#include "core.hpp"
#include "dde.hpp"


#include <windows.h>
#include <tchar.h>
#include <process.h>


#define COREJAVADDE "COREJAVADDE" 

// globals
// -------------------------------


jobject g_jobj = NULL;

DDEJavaWrap* ddeHelper_g = NULL;


  
extern "C" {

// ---------------------------------------

BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReasion, LPVOID lpReserved) {
	if(dwReasion == DLL_PROCESS_ATTACH) {
		DDEJavaWrap::registerWindowClass(hinstDll);
	}

	return TRUE;
}



// ----------------------------------------
// Java implementation


JNIEXPORT jboolean JNICALL Java_com_fin_httrader_utils_win32_HtPowerEventDetector_init(JNIEnv *pEnv, jobject jobj)
{

	if (g_jobj == NULL) {
		g_jobj = pEnv->NewGlobalRef(jobj);
	}
	return true;

}

 


// -----------------------------------

JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_HtPowerEventDetector_destroy(JNIEnv *pEnv, jobject jobj)
{
		
	try {
		if (ddeHelper_g!=NULL) {
			delete ddeHelper_g;
			ddeHelper_g = NULL;
		}
	}
	catch(CppUtils::OperationFailed& e) {
			LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtPowerEventDetector_disconnectFromDDEServer()", "Exception: " + e.message() ); 
	}
	catch(...) {
			LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtPowerEventDetector_disconnectFromDDEServer()", "Unknown exception" ); 
	}

	// global ref
	if(g_jobj) {
		pEnv->DeleteGlobalRef(g_jobj);
		g_jobj = NULL;
	}

			
}
	
// -----------------------------------



JNIEXPORT jboolean JNICALL Java_com_fin_httrader_utils_win32_HtPowerEventDetector_connectToDDEServer
  (JNIEnv *env, jobject, jstring apps, jobject itemlist)
{

	
	CppUtils::DDEItemList g_itemlist;
	CppUtils::String g_apps_s;

	
	bool result = false;
	

	if (env != NULL) {
		const char* apps_c = env->GetStringUTFChars(apps,0);
    
		if (apps_c != NULL) {
			try {
				g_apps_s = apps_c;

				// resolve item list and create our structure
				jclass cls = env->GetObjectClass(itemlist);

				// size method
				jmethodID jsize = env->GetMethodID( cls, "size", "()I"); 
				if (jsize==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidSizeMethod", "ctx_connectDDEServer", "");
				}

				jmethodID elemAt = env->GetMethodID( cls, "elementAt", "(I)Ljava/lang/Object;");
				if (elemAt==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidElementAtMethod", "ctx_connectDDEServer", "");
				}


				// apply method
				jint itemlist_size = env->CallIntMethod(itemlist, jsize);
				for( jint i = 0; i < itemlist_size; i++) {
					// iterate through vector

					
					jobject ddeitem_i = env->CallObjectMethod( itemlist, elemAt, i);
					if (ddeitem_i==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidElementInVector", "ctx_connectDDEServer", i);
					}

					// resolve structure
					/* Get a reference to obj's class */
					jclass cls_ddeitem = env->GetObjectClass(ddeitem_i);

					// 2 methods to access strings
					jmethodID get_item_method = env->GetMethodID( cls_ddeitem, "getItem", "()Ljava/lang/String;"); 
					if (get_item_method==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidGetItemMethod", "ctx_connectDDEServer", i);
					}

					jmethodID get_topic_method = env->GetMethodID( cls_ddeitem, "getTopic", "()Ljava/lang/String;"); 
					if (get_topic_method==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidGetTopicMethod", "ctx_connectDDEServer", i);
					}
					

					jobject item_obj =  env->CallObjectMethod(ddeitem_i, get_item_method);
					if (item_obj==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidGetItemMethodResult", "ctx_connectDDEServer", i);
					}

					jobject topic_obj =  env->CallObjectMethod(ddeitem_i, get_topic_method);
					if (topic_obj==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidGetTopicMethodResult", "ctx_connectDDEServer", i);
					}

					jstring jstr_topic = (jstring)topic_obj;
					jstring jstr_item = (jstring)item_obj;

					
					//
					const char *str_topic = env->GetStringUTFChars(jstr_topic, NULL);
					if (str_topic==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidCantCreateCStringTopic", "ctx_connectDDEServer", i);
					}
  
					const char *str_item = env->GetStringUTFChars(jstr_item, NULL);
					if (str_item==NULL) {
						THROW(CppUtils::OperationFailed, "exc_InvalidCantCreateCStringItem", "ctx_connectDDEServer", i);
					} 

										
					CppUtils::String str_topic_s = str_topic;
					CppUtils::String str_item_s = str_item;

					CppUtils::DDEItem item_to_create(str_topic_s, str_item_s);
					g_itemlist.push_back(item_to_create);
  


					env->ReleaseStringUTFChars(jstr_item, str_item);
					env->ReleaseStringUTFChars(jstr_topic, str_topic);

					


				} // end loop


				
				if (ddeHelper_g==NULL) {
					JavaVM *vmpp = NULL;
					jint res = env->GetJavaVM(&vmpp);
					if (res != 0)
						THROW(CppUtils::OperationFailed, "exc_cannottResolveVM", "ctx_connectDDEServer", "");

					ddeHelper_g = new DDEJavaWrap( g_apps_s, g_itemlist, vmpp );
					result = true;
				}

			
			}
			catch(CppUtils::OperationFailed& e) {
				LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtPowerEventDetector_connectToDDEServer()", "Exception: " + e.message() ); 
			}
			catch(...) {
				LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtPowerEventDetector_connectToDDEServer()", "Unknown exception" ); 
			}
   

  
		} 

		// release string
		env->ReleaseStringUTFChars(apps, apps_c);
		  
	}

	return result;
}


JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_HtPowerEventDetector_disconnectFromDDEServer
  (JNIEnv *, jobject)
{
	try {
		if (ddeHelper_g!=NULL) {
			delete ddeHelper_g;
			ddeHelper_g = NULL;
		}
	}
	catch(CppUtils::OperationFailed& e) {
			LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtPowerEventDetector_disconnectFromDDEServer()", "Exception: " + e.message() ); 
	}
	catch(...) {
			LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtPowerEventDetector_disconnectFromDDEServer()", "Unknown exception" ); 
	}

}


// ----------------------------------------------------

void callback_onDDEDataArrived(CppUtils::DDEItem const& item, CppUtils::String const& value)
{
	if (ddeHelper_g==NULL) 
		return;

	try {
		
		jclass cls = ddeHelper_g->getJVMEnv()->GetObjectClass(g_jobj);
		 jmethodID mid = ddeHelper_g->getJVMEnv()->GetMethodID(cls, "onDDEDataArrived", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

		 if (mid==NULL) {
				THROW(CppUtils::OperationFailed, "exc_InvalidonDDEDataArrived", "ctx_callback_onDDEDataArrived", "");
		 }
		 
		 // call method
		 ddeHelper_g->getJVMEnv()->CallVoidMethod(g_jobj, mid, 
			 ddeHelper_g->getJVMEnv()->NewStringUTF(item.topic_m.c_str()), 
			 ddeHelper_g->getJVMEnv()->NewStringUTF(item.item_m.c_str()), 
			 ddeHelper_g->getJVMEnv()->NewStringUTF(value.c_str()) 
		);

	}
	catch(CppUtils::OperationFailed& e) {
			LOG(MSG_ERROR, "callback_onDDEDataArrived()", "Exception: " + e.message() ); 
	}
	catch(...) {
			LOG(MSG_ERROR, "callback_onDDEDataArrived()", "Unknown exception" ); 
	}

}


}; // end of extern "C"




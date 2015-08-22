
#include "ddeserver.hpp"

#include <windows.h>
#include <tchar.h>
#include <process.h>

#include "core.hpp"


#define COREJAVADDE "COREJAVADDE" 

// globals
// -------------------------------


jobject g_jobj = NULL;
 
DDEJavaServerWrap* ddeHelper_g = NULL;

 

extern "C" {  

// ---------------------------------------

BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReasion, LPVOID lpReserved) {
	if(dwReasion == DLL_PROCESS_ATTACH) {
		DDEJavaServerWrap::registerWindowClass(hinstDll);
	}

	return TRUE;
}



// ----------------------------------------
// Java implementation


JNIEXPORT jboolean JNICALL Java_com_fin_httrader_utils_win32_HtDDEServer_init(JNIEnv *pEnv, jobject jobj)
{

		

	if (g_jobj == NULL) {
		g_jobj = pEnv->NewGlobalRef(jobj);
	}
	return true;

}




// -----------------------------------

JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_HtDDEServer_destroy(JNIEnv *pEnv, jobject jobj)
{
		
	try {
		if (ddeHelper_g!=NULL) {
			delete ddeHelper_g;
			ddeHelper_g = NULL;
		}
	}
	catch(CppUtils::OperationFailed& e) {
			LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtDDEServer_destroy()", "Exception: " + e.message() ); 
	}
	catch(...) {
			LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtDDEServer_destroy()", "Unknown exception" ); 
	}

	// global ref
	if(g_jobj) {
		pEnv->DeleteGlobalRef(g_jobj);
		g_jobj = NULL;
	}

			
}
	
// -----------------------------------

  

JNIEXPORT jboolean JNICALL Java_com_fin_httrader_utils_win32_HtDDEServer_initialize
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
						THROW(CppUtils::OperationFailed, "exc_InvalidCantResolveVM", "ctx_connectDDEServer", "");

					//if (1==1) {
					//	JNIEnv* vmpp2 =  ddeHelper_g->getJVMEnv();
					//}

					ddeHelper_g = new DDEJavaServerWrap( g_apps_s, g_itemlist, vmpp );
					result = true;
				}

			
			}
			catch(CppUtils::OperationFailed& e) {
				LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtDDEServer_initialize()", "Exception: " + e.message() ); 
			}
			catch(...) {
				LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtDDEServer_initialize()", "Unknown exception" ); 
			}
   

  
		} 

		// release string
		env->ReleaseStringUTFChars(apps, apps_c);
		  
	}

	return result;
}


JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_HtDDEServer_uninitialize2
  (JNIEnv *, jobject)
{
	

	try {
		if (ddeHelper_g!=NULL) {
			delete ddeHelper_g;
			ddeHelper_g = NULL;
		}
	} 
	catch(CppUtils::OperationFailed& e) {
			LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtDDEServer_uninitialize2()", "Exception: " + e.message() ); 
	}
	catch(...) {
			LOG(MSG_ERROR, "Java_com_fin_httrader_utils_win32_HtDDEServer_uninitialize2()", "Unknown exception" ); 
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

// ---------------------------------

void callback_onDDEXLTableDataArrived(CppUtils::DDEItem const& item, vector< vector <CppUtils::String> > const& out)
{
	if (ddeHelper_g==NULL) 
		return;

	try {
		JNIEnv* env = ddeHelper_g->getJVMEnv();

		jclass cls = env->GetObjectClass(g_jobj);
		jmethodID mid = env->GetMethodID(cls, "onDDEXLTableDataArrived", "(Ljava/lang/String;Ljava/lang/String;Ljava/util/Vector;)V");

		// create 2 strings
		jstring j_topic = env->NewStringUTF(item.topic_m.c_str());
		jstring j_item = env->NewStringUTF(item.item_m.c_str());

		// create upper vector
		jclass clsvec = env->FindClass("java/util/Vector");
		jmethodID vec_constr = env->GetMethodID(clsvec, "<init>", "()V");
		jobject obj_vec_upper = env->NewObject(clsvec, vec_constr);

		if (obj_vec_upper==NULL)
				THROW(CppUtils::OperationFailed, "exc_CannotCreateVector", "ctx_onDDEXLTableDataArrived", "");

		// resize vector
		jmethodID setSize = env->GetMethodID( clsvec, "setSize", "(I)V");
		

		// apply method
		env->CallVoidMethod(obj_vec_upper, setSize, out.size() );
		

		jmethodID setElementAt = env->GetMethodID(clsvec, "setElementAt", "(Ljava/lang/Object;I)V");

		// assign values
		for(int i = 0; i < out.size(); i++) {
			
			// create lower vector
			jobject obj_vec_inner = env->NewObject(clsvec, vec_constr);
			if (obj_vec_inner==NULL)
				THROW(CppUtils::OperationFailed, "exc_CannotCreateVector", "ctx_onDDEXLTableDataArrived", "");

			// inner vector
      vector <CppUtils::String> const& vector_i = out[i];

			// initialize inner vector - resize
			env->CallVoidMethod(obj_vec_inner, setSize, vector_i.size() );

			// fill with values
			for(int j = 0; j < vector_i.size(); j++) {

				// work with value to see russian
				CppUtils::String const& val = vector_i[j];


				env->CallVoidMethod(obj_vec_inner, setElementAt,  env->NewStringUTF(val.c_str()), j );
			}


			// set up the element
			env->CallVoidMethod(obj_vec_upper, setElementAt, obj_vec_inner, i );

		} 

		// that it - just call our java method
		env->CallVoidMethod(g_jobj, mid, 
			 j_topic, 
			 j_item, 
			 obj_vec_upper
		);

		
	}
	catch(CppUtils::OperationFailed& e) {
			LOG(MSG_ERROR, "callback_onDDEXLTableDataArrived()", "Exception: " + e.message() ); 
	}
	catch(...) {
			LOG(MSG_ERROR, "callback_onDDEXLTableDataArrived()", "Unknown exception" ); 
	}
}




}; // end of extern "C"




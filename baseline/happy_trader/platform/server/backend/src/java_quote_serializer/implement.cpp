#include "implement.hpp"


namespace AlgLib {
extern "C" {

JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_QuoteSerializer_serializeRtDataToBinary
  (JNIEnv *env, jclass cls, jobject rtdata, jbyteArray outbuffer)
{
		
		jclass cls_htrtdata = env->FindClass("com/fin/httrader/utils/hlpstruct/HtRtData");
	
		jint jval_type = env->GetIntField(rtdata, env->GetFieldID(cls_htrtdata, "type_m", "I"));
		
		// provider & symbol
		
		jstring jval_provider = (jstring)env->CallObjectMethod(rtdata, env->GetMethodID( cls_htrtdata, "getProvider", "()Ljava/lang/String;"));
		jstring jval_symbol = (jstring)env->CallObjectMethod(rtdata, env->GetMethodID( cls_htrtdata, "getSymbol", "()Ljava/lang/String;"));

		const char *str_provider = env->GetStringUTFChars(jval_provider, NULL);
		const char *str_symbol = env->GetStringUTFChars(jval_symbol, NULL);
		double ttime = (double)env->GetLongField(rtdata, env->GetFieldID(cls_htrtdata, "time_m", "J") );
		if (ttime > 0)
			ttime /= 1000.0;
		else 
			ttime = -1.0;

		HlpStruct::RtData rt_native_data(
			(CppUtils::Byte)env->GetIntField(rtdata, env->GetFieldID(cls_htrtdata, "aflag_m", "I")), 
			(CppUtils::Byte)jval_type, 
			ttime
		);
		rt_native_data.setProvider(str_provider);
		rt_native_data.setSymbol(str_symbol);


    

		env->ReleaseStringUTFChars(jval_provider, str_provider);
		env->ReleaseStringUTFChars(jval_symbol, str_symbol);

		

		if (jval_type == (jint)HlpStruct::RtData::RT_Type) {
			

			rt_native_data.getRtData().ask_m = env->GetDoubleField(rtdata, env->GetFieldID(cls_htrtdata, "ask_m", "D") );
			rt_native_data.getRtData().bid_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "bid_m", "D") );
			rt_native_data.getRtData().color_m = (CppUtils::Byte)env->GetIntField(rtdata, env->GetFieldID(cls_htrtdata, "color_m", "I"));
			rt_native_data.getRtData().operation_m = (CppUtils::Byte)env->GetIntField(rtdata, env->GetFieldID(cls_htrtdata, "operation_m", "I"));
			rt_native_data.getRtData().volume_m = env->GetDoubleField(rtdata, env->GetFieldID(cls_htrtdata, "volume_m", "D") );
		

		}
		else if (jval_type == (jint)HlpStruct::RtData::HST_Type) {
		

			rt_native_data.getHistData().open_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "open_m", "D") );
			rt_native_data.getHistData().high_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "high_m", "D") );
			rt_native_data.getHistData().low_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "low_m", "D") );
			rt_native_data.getHistData().close_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "close_m", "D") );

			rt_native_data.getHistData().open2_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "open2_m", "D") );
			rt_native_data.getHistData().high2_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "high2_m", "D") );
			rt_native_data.getHistData().low2_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "low2_m", "D") );
			rt_native_data.getHistData().close2_m = env->GetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "close2_m", "D") );

			rt_native_data.getHistData().volume_m = env->GetDoubleField(rtdata, env->GetFieldID(cls_htrtdata, "volume_m", "D") );

		}

	
		CppUtils::MemoryChunk buffer;
		HlpStruct::convertToBinary(rt_native_data, buffer);
		
		env->SetByteArrayRegion(outbuffer, 0, buffer.length(), (jbyte*)buffer.data());
		
	
}

JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_QuoteSerializer_deserializeRtDataFromBinary
  (JNIEnv *env, jclass cls, jobject rtdata, jbyteArray inbuffer)
{
	

	// copy back to the buffer
	HlpStruct::RtData rt_native_data;
	env->GetByteArrayRegion(inbuffer,0,  sizeof(HlpStruct::RtData), (jbyte*)&rt_native_data );

	jclass cls_htrtdata = env->FindClass("com/fin/httrader/utils/hlpstruct/HtRtData");
	
	env->SetIntField(rtdata, env->GetFieldID(cls_htrtdata, "type_m", "I"), rt_native_data.getType());
	env->SetIntField(rtdata, env->GetFieldID(cls_htrtdata, "aflag_m", "I"), rt_native_data.getAFlag());
	
	
	env->SetLongField(rtdata, env->GetFieldID(cls_htrtdata, "time_m", "J") , (jlong)(rt_native_data.getTime() * 1000));

	// provider symbol

	env->CallVoidMethod(rtdata, env->GetMethodID(cls_htrtdata, "setProvider", "(Ljava/lang/String;)V"), env->NewStringUTF(rt_native_data.getProvider()) );
	env->CallVoidMethod(rtdata, env->GetMethodID(cls_htrtdata, "setSymbol", "(Ljava/lang/String;)V"), env->NewStringUTF(rt_native_data.getSymbol()) );



	if (rt_native_data.getType() == HlpStruct::RtData::HST_Type) {

		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "open_m", "D"), rt_native_data.getHistData().open_m );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "high_m", "D"), rt_native_data.getHistData().high_m );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "low_m", "D"), rt_native_data.getHistData().low_m );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "close_m", "D"), rt_native_data.getHistData().close_m );

		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "open2_m", "D"), rt_native_data.getHistData().open2_m );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "high2_m", "D"), rt_native_data.getHistData().high2_m );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "low2_m", "D"), rt_native_data.getHistData().low2_m );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "close2_m", "D"), rt_native_data.getHistData().close2_m );

		env->SetDoubleField(rtdata, env->GetFieldID(cls_htrtdata, "volume_m", "D"), rt_native_data.getHistData().volume_m );

		// clean 
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "bid_m", "D"), -1 );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "ask_m", "D"), -1 );

		env->SetIntField(rtdata, env->GetFieldID(cls_htrtdata, "color_m", "I"), 0);
		env->SetIntField(rtdata, env->GetFieldID(cls_htrtdata, "operation_m", "I"), 0);


	}
	else if (rt_native_data.getType()== HlpStruct::RtData::RT_Type) {
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "bid_m", "D"), rt_native_data.getRtData().bid_m );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "ask_m", "D"), rt_native_data.getRtData().ask_m );

		env->SetIntField(rtdata, env->GetFieldID(cls_htrtdata, "color_m", "I"), rt_native_data.getRtData().color_m);
		env->SetIntField(rtdata, env->GetFieldID(cls_htrtdata, "operation_m", "I"), rt_native_data.getRtData().operation_m);

		env->SetDoubleField(rtdata, env->GetFieldID(cls_htrtdata, "volume_m", "D"), rt_native_data.getRtData().volume_m );

		// clean
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "open_m", "D"), -1 );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "high_m", "D"), -1 );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "low_m", "D"), -1 );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "close_m", "D"), -1 );

		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "open2_m", "D"), -1 );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "high2_m", "D"), -1 );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "low2_m", "D"), -1 );
		env->SetDoubleField(rtdata,  env->GetFieldID(cls_htrtdata, "close2_m", "D"), -1 );


	}
	
}

JNIEXPORT jint JNICALL Java_com_fin_httrader_utils_win32_QuoteSerializer_getHtRtDataSize
  (JNIEnv *, jclass)
{
	return sizeof(HlpStruct::RtData);
}


// ---------------------------------


JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_QuoteSerializer_serializeLevel1ToBinary
  (JNIEnv * env, jclass cls, jobject level1data, jbyteArray outbuffer)
{
	jclass cls_level1data = env->FindClass("com/fin/httrader/utils/hlpstruct/HtLevel1Data");

	jstring jval_provider = (jstring)env->CallObjectMethod(level1data, env->GetMethodID( cls_level1data, "getProvider", "()Ljava/lang/String;"));
	jstring jval_symbol = (jstring)env->CallObjectMethod(level1data, env->GetMethodID( cls_level1data, "getSymbol", "()Ljava/lang/String;"));

	const char *str_provider = env->GetStringUTFChars(jval_provider, NULL);
	const char *str_symbol = env->GetStringUTFChars(jval_symbol, NULL);

	double ttime = (double)env->GetLongField(level1data, env->GetFieldID(cls_level1data, "time_m", "J") );
	if (ttime > 0)
		ttime /= 1000.0;
	else 
		ttime = -1.0;

	HlpStruct::RtLevel1Data level1data_native(
			env->GetDoubleField(level1data, env->GetFieldID(cls_level1data, "best_bid_price_m", "D") ), 
			env->GetDoubleField(level1data, env->GetFieldID(cls_level1data, "best_ask_price_m", "D") ), 
			env->GetDoubleField(level1data, env->GetFieldID(cls_level1data, "best_bid_volume_m", "D") ), 
			env->GetDoubleField(level1data, env->GetFieldID(cls_level1data, "best_ask_volume_m", "D") ), 
			env->GetDoubleField(level1data, env->GetFieldID(cls_level1data, "open_interest_m", "D") ), 
			ttime,
			str_symbol,
			str_provider
		);

	env->ReleaseStringUTFChars(jval_provider, str_provider);
	env->ReleaseStringUTFChars(jval_symbol, str_symbol);

	CppUtils::MemoryChunk buffer;
	HlpStruct::convertToBinary(level1data_native, buffer);
		
	env->SetByteArrayRegion(outbuffer, 0, buffer.length(), (jbyte*)buffer.data());

}


JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_QuoteSerializer_deserializeLevel1FromBinary
  (JNIEnv *env, jclass , jobject level1data, jbyteArray inbuffer)
{
	jclass cls_level1data = env->FindClass("com/fin/httrader/utils/hlpstruct/HtLevel1Data");

	HlpStruct::RtLevel1Data level1_native_data;
	env->GetByteArrayRegion(inbuffer,0,  sizeof(HlpStruct::RtLevel1Data), (jbyte*)&level1_native_data );

	env->SetDoubleField(level1data,  env->GetFieldID(cls_level1data, "best_ask_price_m", "D"), level1_native_data.best_ask_price_m );
	env->SetDoubleField(level1data,  env->GetFieldID(cls_level1data, "best_bid_price_m", "D"), level1_native_data.best_bid_price_m );
	env->SetDoubleField(level1data,  env->GetFieldID(cls_level1data, "best_ask_volume_m", "D"), level1_native_data.best_ask_volume_m );
	env->SetDoubleField(level1data,  env->GetFieldID(cls_level1data, "best_bid_volume_m", "D"), level1_native_data.best_bid_volume_m );
	env->SetDoubleField(level1data,  env->GetFieldID(cls_level1data, "open_interest_m", "D"), level1_native_data.open_interest_m );
	
	env->SetLongField(level1data, env->GetFieldID(cls_level1data, "time_m", "J") , (jlong)(level1_native_data.getTime()*1000));


	env->CallVoidMethod(level1data, env->GetMethodID(cls_level1data, "setProvider", "(Ljava/lang/String;)V"), env->NewStringUTF(level1_native_data.getProvider()) );
	env->CallVoidMethod(level1data, env->GetMethodID(cls_level1data, "setSymbol", "(Ljava/lang/String;)V"), env->NewStringUTF(level1_native_data.getSymbol()) );


}


JNIEXPORT jint JNICALL Java_com_fin_httrader_utils_win32_QuoteSerializer_getLevel1DataSize
  (JNIEnv *, jclass)
{
	return sizeof(HlpStruct::RtLevel1Data);
}

// -------------------
// LEVEL 2
// -------------------

JNIEXPORT jbyteArray JNICALL Java_com_fin_httrader_utils_win32_QuoteSerializer_serializeLevel2ToBinary
  (JNIEnv *env, jclass cls, jobject level2data)
{
	jclass cls_level2data = env->FindClass("com/fin/httrader/utils/hlpstruct/HtLevel2Data");
	jclass cls_level2entry = env->FindClass("com/fin/httrader/utils/hlpstruct/HtLevel2Data$Htlevel2Entry");

	jstring jval_provider = (jstring)env->CallObjectMethod(level2data, env->GetMethodID( cls_level2data, "getProvider", "()Ljava/lang/String;"));
	jstring jval_symbol = (jstring)env->CallObjectMethod(level2data, env->GetMethodID( cls_level2data, "getSymbol", "()Ljava/lang/String;"));
	 
	HlpStruct::RtLevel2Data level2_native_data;
	const char *str_provider = env->GetStringUTFChars(jval_provider, NULL);
	const char *str_symbol = env->GetStringUTFChars(jval_symbol, NULL);

	level2_native_data.setSymbol(str_symbol);
	level2_native_data.setProvider(str_provider);
				   
	// time
	double ttime = (double)env->CallLongMethod(level2data, env->GetMethodID( cls_level2data, "getTime", "()J"));
	if (ttime > 0)
		ttime /= 1000.0;
	else 
		ttime = -1.0;

	level2_native_data.getTime() = ttime;

	// size
	jint level2_size = env->CallIntMethod(level2data, env->GetMethodID( cls_level2data, "getSize", "()I"));
	jint idx_best_bid = env->CallIntMethod(level2data, env->GetMethodID( cls_level2data, "getBestBidIndex", "()I"));
	jint idx_best_ask = env->CallIntMethod(level2data, env->GetMethodID( cls_level2data, "getBestAskIndex", "()I"));
	level2_native_data.init(level2_size, idx_best_bid, idx_best_ask);

	// now get array of data
	
	jobject level2_data_array = env->CallObjectMethod(level2data, env->GetMethodID( cls_level2data, "getData", "()[Lcom/fin/httrader/utils/hlpstruct/HtLevel2Data$Htlevel2Entry;"));

	
	//jobjectArray* data_array_2=	reinterpret_cast<jobjectArray*>(&data_array);
	//jsize data_size = env->GetArrayLength( (jarray)data_array );
	
	// now get array of data
	
	for (int i = 0; i < (int)level2_size; i++) {
		HlpStruct::Level2Entry& entry_i_tofill = level2_native_data.getLevel2Entry(i);

		// java
		
		jobject data_element_i = env->GetObjectArrayElement((jobjectArray)level2_data_array, i); 
		
		// extract elements
		jdouble price_i = env->CallDoubleMethod(	data_element_i, env->GetMethodID( cls_level2entry, "getPrice", "()D"));
		jdouble volume_i = env->CallDoubleMethod(	data_element_i, env->GetMethodID( cls_level2entry, "getVolume", "()D"));
		jint type_i = env->CallIntMethod(	data_element_i, env->GetMethodID( cls_level2entry, "getType", "()I"));
		jboolean isself_i = env->CallBooleanMethod(	data_element_i, env->GetMethodID( cls_level2entry, "isSelf", "()Z")); 

		entry_i_tofill.getVolume() = (float)volume_i;
		entry_i_tofill.getPrice() = (float)price_i;
		entry_i_tofill.getType()= (int)type_i;
		entry_i_tofill.isSelf() = (bool)isself_i;

		
		level2_native_data.getLevel2Entry(i) = entry_i_tofill;

	}
	

	// release string
	env->ReleaseStringUTFChars(jval_provider, str_provider);
	env->ReleaseStringUTFChars(jval_symbol, str_symbol);

	CppUtils::MemoryChunk buffer;
	HlpStruct::convertToBinary(level2_native_data,	buffer );
	
	// return result
	jbyteArray result = env->NewByteArray(buffer.length() );
	env->SetByteArrayRegion (result, (jint)0, (jint)buffer.length() , (jbyte*)buffer.data());
	
		
	return result;

}

JNIEXPORT void JNICALL Java_com_fin_httrader_utils_win32_QuoteSerializer_deserializeLevel2FromBinary
  (JNIEnv *env, jclass cls, jobject level2data, jbyteArray inbuffer)
{
	jclass cls_level2data = env->FindClass("com/fin/httrader/utils/hlpstruct/HtLevel2Data");
	//jclass cls_level2entry = env->FindClass("com/fin/httrader/utils/hlpstruct/HtLevel2Data/Htlevel2Entry");

	jsize data_size = env->GetArrayLength( inbuffer );
	if (data_size > 0) {
		HlpStruct::RtLevel2Data level2_native_data;
		CppUtils::MemoryChunk raw_buffer((int)data_size);
		
		env->GetByteArrayRegion(inbuffer,0,  raw_buffer.length(), (jbyte*)raw_buffer.data() );

		// now recover
		HlpStruct::convertFromBinary(level2_native_data, raw_buffer);

		// now initialize

		env->CallVoidMethod(level2data, env->GetMethodID(cls_level2data, "setProvider", "(Ljava/lang/String;)V"), env->NewStringUTF(level2_native_data.getProvider()) );
		env->CallVoidMethod(level2data, env->GetMethodID(cls_level2data, "setSymbol", "(Ljava/lang/String;)V"), env->NewStringUTF(level2_native_data.getSymbol()) );
		
		jlong	ttime_adopted =	 level2_native_data.getTime() * 1000;
		env->CallVoidMethod(level2data, env->GetMethodID(cls_level2data, "setTime", "(J)V"), ttime_adopted );


		// set size
		env->CallVoidMethod(level2data, env->GetMethodID(cls_level2data, "init", "(III)V"), 
			(jint)level2_native_data.getSize(),
			(jint)level2_native_data.getBestBidIndex(),
			(jint)level2_native_data.getBestAskIndex()
		);

		for(int i = 0; i < level2_native_data.getSize(); i++) {
			HlpStruct::Level2Entry const& e = level2_native_data.getLevel2Entry(i); 
			
			env->CallVoidMethod(level2data, env->GetMethodID(cls_level2data, "setEntry", "(DDZII)V"),
				(jdouble)e.getVolume(),
				(jdouble)e.getPrice(),
				(jboolean)e.isSelf(),
				(jint)e.getType(),
				(jint)i
			);
		}
																					  

	}


}



/**
* Helpers
*/



}; // end C
} // end of namespace

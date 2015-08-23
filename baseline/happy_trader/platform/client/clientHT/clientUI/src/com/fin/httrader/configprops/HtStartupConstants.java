/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.configprops;

import java.util.LinkedHashMap;

/**
 *
 * @author DanilinS
 */
public class HtStartupConstants extends HtBaseProp {

	public HtStartupConstants() {
		type_m = HtBaseProp.ID_GONFIGURE_STARTUP_CONST;

	}
	
	public static final String VERSION = "1.8.8";

	// -------------------------------------
	// static
	public static final String BASE_EXTERNAL_PACKAGES_DIR = "extjars";
	public static final String BASE_EXTERNAL_PACKAGES_TEMPLATES_DIR = "templates";
	public static final String BASE_LOG_DIRECTORY_NAME = "logs";
	public static final String BASE_JAVA_PLUGIN_DIRECTORY_NAME = "jplug";
	public static final String BASE_JAVA_RT_PROVIDERS_DIRECTORY_NAME = "rtproviders";
	public static final String BASE_JAVA_HST_PROVIDERS_DIRECTORY_NAME = "hstprovider";
	public static final String BASE_JAVA_ALERT_PLUGINS_DIRECTORY_NAME = "alertplugins";
	public static final String BASE_LIB_DIRECTORY_NAME = "lib";
	public static final String BASE_CONFIG_DIRECTORY_NAME = "config";
	public static final String BASE_BIN_DIRECTORY_NAME = "bin";
	public static final String TRADE_SERVER_LOGVAR_NAME = "HT_DEBUG_LEVEL";
	public static final int MYSQL_MAX_PARTITIONS = 100;
	public static final String BASE_WEB_START_DIR = "webstart";
	public static final String WEB_START_DESCRIPTOR_NAME = "launch.jnlp";
	public static final String WEB_PLUGIN_DESCRIPTOR_NAME = "web_plugin.txt";
	public static final String FATAL_ERROR_FILE = "fatal.txt";
	public static final String DB_SCHEMA = "HTRADER";
	public static final String AVOID_SEARCH_LIBS_SUBSTR = "Microsoft.VC90".toUpperCase();
	public static final String SERVER_EXECUTABLE_PREFIX="htmain_srv".toUpperCase();
	public static final String DEBUG_POSTFIX = "_d";
	public static final String WEB_PLUGIN_DIR = "web_plugins";
	public static final String MAIN_JAR_FILE = "clientUI.jar";
	public static final String JNDI_CONNECT_POOL_ENTRY_NAME = "jdbc/connPool";
	public static final String JNDI_CONNECT_POOL_LOCATION = "java:/comp/env/" + JNDI_CONNECT_POOL_ENTRY_NAME;
	
	public static final String MYSQL_DB_DRIVER = "com.mysql.jdbc.Driver";
	

	// -------------------------------------

	// --------------------------------------------

	// the libraries to load
	
	
	
  // the number of minutes the translation from denominator currency to report currency is assumed valid
	// exceeding this means the gap in data
	@HtSaveAnnotation
	public int MAX_MINUTES_TRANSLATE_TO_REP_SYMBOL = 120 * 60;

	// the maximum number errors when therad signals it has problems
	//public static final int THREAD_MAX_ERRORS = 5;
	// web server listening port
	@HtSaveAnnotation
	public int HTTP_LISTEN_PORT = 8082;

	@HtSaveAnnotation
	public int COMMON_SERVICE_LISTEN_PORT=8083;

	// this is the period each functor is checked for execution- e.i. 2 seconds
	// each functor can be called with delayes if multiple this value by ecah functor periods
	@HtSaveAnnotation
	public int IDLE_THREAD_SECS_TIMEOUT = 10;

	// HealthFunctor exceution period
	@HtSaveAnnotation
	public int IDLE_THREAD_HEALTH_FTOR_PERIOD = 2; // 20* IDLE_THREAD_SECS_TIMEOUT

	// period of checking queus status
	@HtSaveAnnotation
	public int IDLE_THREAD_SECS_CHECK_QUEUE_STATUS_PERIOD = 10; // 10 * IDLE_THREAD_SECS_TIMEOUT
	
	// winstone servet executes keep alive SQLs
	@HtSaveAnnotation
	public int WINSTONE_EXECUTE_ALIVE_CONNECTION_MINS = 10; // 10 * IDLE_THREAD_SECS_TIMEOUT

	// the number of records to be inserted in DB in a single transaction
	@HtSaveAnnotation
	public int COMMIT_RECORDS_VALUE = 200;


	
	// this is default page size when we make export operation
	@HtSaveAnnotation
	public int SELECT_PAGE_DEFAULT_SIZE = 80000;

	// how much time we are waiting of the response from the trading server
	@HtSaveAnnotation
	public int WAIT_RESPONSE_FROM_TRADING_SERVER_SEC = 40;

	// if PC is busy we repeat connection to CPP server
	@HtSaveAnnotation
	public int MAX_CPP_SERVER_CONNECT_ATTEMPTS = 1000;

	// 10 minutes of web session timeout
	@HtSaveAnnotation
	public int WEB_SESSION_TIMEOUT_MIN = 365 * 24 * 60; // 1 year so far


	// this is the delimiter for log
	@HtSaveAnnotation
	public String LOG_ATTRIBUTE_DELIMITER = "\n";

	// we will issue dummy symbol, to avoid IE close its channel
	@HtSaveAnnotation
	public int RTPRICE_MAN_HEARTBEAT_TIMEOUT_SEC = 10;

	// the maximum number of critical errors
	@HtSaveAnnotation
	public int MAX_CRITICAL_ERRORS_ALLOWED = 200;

	// the maximum number of symbols in LOG and ALERTS grids for large data columns
	@HtSaveAnnotation
	public int MAX_SYMBOLS_IN_LARGE_COLUMN = 300;

	// the maximum number of elemenst in queues
	@HtSaveAnnotation
	public int MAX_ELEMENTS_IN_QUEUE = 10000000;

	// the maximum number of elemenst in queues when QueuesLoadStatus functor begins to warn
	@HtSaveAnnotation
	public int WARN_ELEMENTS_IN_QUEUE = 200000;

	public String XERCES_C_LIB = "xerces-c_3_1";
	
	public String[] JNI_LIBS = new String[] {
		"htcpputils",
		"htjava_sys_utils_bck",
		XERCES_C_LIB,
		"hthlpstruct_bck",
		"htjava_uid_bck",
		"htjava_dde_server_bck",
		"htjava_quik_receiver_bck",
		"htjava_quote_serializer_bck"
	};

	
	
	
}

@echo off

:: Set environment
call user_data.cmd
call env.cmd

::check env vars
if $%HT_DEBUG_LEVEL%$==$$ goto :ht_debug_level
if $%MYSQL_CONNECTION_URL%$==$$ goto :mysql_empty
if $%JAVA_HOME%$==$$ goto :javahome_empty
if $%HT_SERVER_DIR%$==$$ goto :htserver_dir_empty
if $%HT_CLIENT_DEBUG_LEVEL%$==$$ goto :ht_client_debug_level_empty
if $%WINSTONE_DEBUG_LEVEL%$==$$ goto :winstone_debug_level_empty
if $%JAVA_MEMORY_PARAM%$==$$ goto :java_memory_param_empty
::if $%BIN_ROOT%$==$$ goto :jni_lib_root_param_empty

:: startup Java server
cd "%HT_SERVER_DIR%"

"%JAVA_HOME%/bin/java.exe" -classpath "%CLASSPATH%;%JAVA_HOME%/lib/tools.jar" %JAVA_MEMORY_PARAM% com.fin.httrader.HtMain %MYSQL_CONNECTION_URL% %HT_CLIENT_DEBUG_LEVEL% %WINSTONE_DEBUG_LEVEL%
goto :sucess

:mysql_empty
echo Invalid enviroment entry: MYSQL_CONNECTION_URL
pause
goto :sucess


:javahome_empty
echo Invalid enviroment entry: JAVA_HOME
pause
goto :sucess


:htserver_dir_empty
echo Invalid enviroment entry: HT_SERVER_DIR
pause
goto :sucess


:ht_debug_level
echo Invalid enviroment entry: HT_DEBUG_LEVEL
pause
goto :sucess


:ht_client_debug_level_empty
echo Invalid enviroment entry: HT_CLIENT_DEBUG_LEVEL


:winstone_debug_level_empty
echo Invalid enviroment entry: WINSTONE_DEBUG_LEVEL

:java_memory_param_empty
echo Invalid enviroment entry: JAVA_MEMORY_PARAM, set up for example like: -Xmx512m

:: :jni_lib_root_param_empty
:: echo Invalid enviroment entry: BIN_ROOT

:sucess
pause

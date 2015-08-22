@echo off
set JAVA_HOME="c:\Program Files\Java\JDK1.7.0"
set HT_SERVER_DIR=c:\work\happy_trader\platform\client\clientHT\clientUI
set JAVA_CODE_LIB_PATH=%HT_SERVER_DIR%\lib

::create short file name for java
for /f "delims=" %%A in ('..\..\..\..\install\center\server\supp\shortfilename %JAVA_HOME%') do set JAVA_HOME_SHORT=%%A

if not errorlevel 0 (
   echo. 
   echo Cannot create short path from %JAVA_HOME%
   pause
   goto :sucess
)

set JAVA_HOME=%JAVA_HOME_SHORT%
echo Initialized JAVA_HOME as %JAVA_HOME%


:: Create Java class path
set CLASSPATH=%JAVA_CODE_LIB_PATH%

:: Create Java class path
set CLASSPATH=%CLASSPATH%;%HT_SERVER_DIR%\classes
for %%i in (%JAVA_CODE_LIB_PATH%\*.jar) Do Call :setlist %%i 


%JAVA_HOME%\bin\java.exe -classpath %CLASSPATH% -jar lib/winstone/winstone-0.9.10.jar --warfile=lib/axis2/axis2.war
pause


:: helper function to get file name from each iteration.
:setlist
set CLASSPATH=%CLASSPATH%;%1
GoTo :EOF
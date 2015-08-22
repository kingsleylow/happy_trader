set INPUT_PATH=%1
set INPUT_FILE_NO_EXT=%~n1
set INPUT_DIR=%~dp1
set NS=%2

IF %INPUT_DIR:~-1%==\ SET INPUT_DIR=%INPUT_DIR:~0,-1%

::create separate directory for output
cd %INPUT_DIR%\
md auto
cd auto
md %INPUT_FILE_NO_EXT%

set OUTPUT_DIR=%INPUT_DIR%\auto\%INPUT_FILE_NO_EXT%
IF %OUTPUT_DIR:~-1%==\ SET OUTPUT_DIR=%OUTPUT_DIR:~0,-1%


:: change to toll dir
cd %HT_DEVEL_ROOT%\extern\tools\gsoap-2.8\gsoap\bin\win32\

::set OUTPUT_FILE_STEP1=%INPUT_DIR%\%INPUT_FILE_NO_EXT%.h
set OUTPUT_FILE_STEP1=%OUTPUT_DIR%\%INPUT_FILE_NO_EXT%.h


wsdl2h.exe -t "..\..\typemap.dat" -N "%NS%" -o %OUTPUT_FILE_STEP1% %INPUT_PATH%
soapcpp2 -I "..\..\import" -d "%OUTPUT_DIR%" %OUTPUT_FILE_STEP1%

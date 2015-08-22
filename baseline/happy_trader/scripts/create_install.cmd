@echo off

:: target
set TRG=%1

if $%TRG%$==$$ goto :failure

:: Creates installation structure
rmdir /s /q ..\install\center\server\bin
rmdir /s /q ..\install\center\server\classes
rmdir /s /q ..\install\center\server\lib
rmdir /s /q ..\install\center\server\config
rmdir /s /q ..\install\center\server\platform\client\clientHT\clientUI\console
rmdir /s /q ..\install\center\server\logs
rmdir /s /q ..\install\center\server\jplug  
rmdir /s /q ..\install\center\server\sdata  
rmdir /s /q ..\install\center\server\supp 

:: create some dirs
mkdir ..\install\center\server\supp
mkdir ..\install\center\server\logs
mkdir ..\install\center\server\sdata
mkdir ..\install\center\server\jplug

:: copy files
xcopy /e /y ..\bin\win32_i86\%TRG% ..\install\center\server\bin\

::xcopy /e /y ..\dependencies\win32_i86\msvc7.3\%TRG% ..\install\center\server\bin\
::xcopy /e /y ..\dependencies\win32_i86\msvc8\%TRG% ..\install\center\server\bin\
::xcopy /e /y ..\dependencies\win32_i86\msvc11\%TRG% ..\install\center\server\bin\

:: do not copy classes
::xcopy /e /y ..\platform\client\clientHT\clientUI\build\classes ..\install\center\server\classes\
xcopy /e /y ..\config ..\install\center\server\config\
xcopy /e /y ..\platform\client\clientHT\clientUI\console ..\install\center\server\platform\client\clientHT\clientUI\console\
xcopy /e /y ..\platform\client\clientHT\clientUI\dist\lib ..\install\center\server\lib\
xcopy /e /y ..\platform\client\clientHT\clientUI\dist\clientUI.jar ..\install\center\server\lib\
xcopy /e /y ..\jplug ..\install\center\server\jplug\
xcopy /e /y ..\platform\client\clientrepo\dist\clientrepo.war ..\install\center\server\clientapp\


:: install HTCharting
::xcopy /e /y ..\platform\client\clientHT\HtCharting\dist ..\install\center\server\platform\client\clientHT\clientUI\console\..\..\HtCharting\dist\

:: install HtVolumeAnalizer
::xcopy /e /y ..\platform\client\clientHT\HtVolumeAnalizer\dist ..\install\center\server\platform\client\clientHT\clientUI\console\..\..\HtVolumeAnalizer\dist\

:: install HtRangeBreaker
::xcopy /e /y ..\platform\client\clientHT\HtRangeBreaker\dist ..\install\center\server\platform\client\clientHT\clientUI\console\..\..\HtRangeBreaker\dist\

:: create installation scripts
cd utils
call create_install_scripts.cmd
cd..

goto :success

:failure
echo invalid installation target - Debug or Release
pause
exit -1

:success
echo installation complete, will zip
call create_zipped.cmd
echo zipped OK

pause
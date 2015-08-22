@echo off
set VC_TOOLDIR=%1
set DLL_NAME=%2

call %VC_TOOLDIR%\vcvars32.bat
echo "register: %DLL_NAME%"

regasm %DLL_NAME% /tlb /codebase

exit 0
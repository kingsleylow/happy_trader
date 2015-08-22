@echo off

:: Create EXE console application template to be used inside project (directories, basic header files and project file)
:: parameters
:: %1 - project name

set PRJ_NAME_ORIG=%1

:: convert to lower and upper cases
for /f "tokens=* usebackq" %%t in (`cchange l %PRJ_NAME_ORIG%`) do (
    set PRJ_NAME=%%t
)

for /f "tokens=* usebackq" %%t in (`cchange u %PRJ_NAME_ORIG%`) do (
    set PRJ_NAME_UPPER=%%t
)


if $%PRJ_NAME%$==$$ goto :invalid_prj_name
if $%PRJ_NAME_UPPER%$==$$ goto :invalid_prj_name


:: substitution vars
set BASE_NAME=%PRJ_NAME%_bck

set PCH_NAME=%PRJ_NAME%_bck.pch
set TLB_NAME=%PRJ_NAME%_bck.tlb
set SUB_DIR=%PRJ_NAME%

set PDB_NAME_RELEASE=%PRJ_NAME%.pdb
set PDB_NAME_DEBUG=%PRJ_NAME%_d.pdb

set EXE_NAME_DEBUG=%PRJ_NAME%_d.exe
set EXE_NAME_RELEASE=%PRJ_NAME%.exe

set MAIN_FILE_NAME=main.cpp

:: create UID_GEN env variable
for /f "tokens=* usebackq" %%i in (`uidgen`) do (
    set UID_GEN=%%i
)
 
set PRJ_ROOT_DIR=..\platform\server\corba\server\prj\msvc\
set SCR_ROOT_DIR=..\platform\server\corba\server\src\%PRJ_NAME%\
set PRJ_ROOT_FILE=%PRJ_ROOT_DIR%\%PRJ_NAME%.vcproj

:: create source dir
md %SCR_ROOT_DIR%


:: create project file
call utils\prj_console_exe_tmpl.cmd > %PRJ_ROOT_DIR%\%PRJ_NAME%.vcproj

:: create main file
call utils\maincpp_console_exe_tmpl.cmd > %SCR_ROOT_DIR%\%MAIN_FILE_NAME%


goto :finish


:invalid_dll_name
echo usage: create_project_exe "project name"
goto :finish



:finish
echo Done - %PRJ_NAME%
pause




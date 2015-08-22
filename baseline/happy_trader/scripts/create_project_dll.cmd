@echo off

:: Create DLL template to be used inside project (directories, basic header files and project file)
:: parameters
:: %1 - project name

set PRJ_NAME_ORIG=%1
set CLASS_NAME=%2

:: convert to lower and upper cases
for /f "tokens=* usebackq" %%t in (`cchange l %PRJ_NAME_ORIG%`) do (
    set PRJ_NAME=%%t
)

for /f "tokens=* usebackq" %%t in (`cchange u %PRJ_NAME_ORIG%`) do (
    set PRJ_NAME_UPPER=%%t
)


if $%PRJ_NAME%$==$$ goto :invalid_prj_name
if $%PRJ_NAME_UPPER%$==$$ goto :invalid_prj_name
if $%CLASS_NAME%$==$$ goto :invalid_prj_name goto :invalid_class_name


:: substitution vars

set EXPORT_CPP_DEFINITION=%PRJ_NAME_UPPER%_BCK_EXPORTS
set DEFS_HEADER_FILE_HEADER=_%PRJ_NAME_UPPER%DEFS_INCLUDED
set HEADER_FILE_HEADER=_%PRJ_NAME_UPPER%_INCLUDED
set IMPLEMENT_FILE_HEADER=_%PRJ_NAME_UPPER%_IMPL_INCLUDED

set DEFS_HEADER_FILE_EXPORT_PRFX=%PRJ_NAME_UPPER%_EXP

set BASE_NAME=%PRJ_NAME%_bck

set PCH_NAME=%PRJ_NAME%_bck.pch
set TLB_NAME=%PRJ_NAME%_bck.tlb
set SUB_DIR=%PRJ_NAME%

set PDB_NAME_RELEASE=ht%PRJ_NAME%_bck.pdb
set PDB_NAME_DEBUG=ht%PRJ_NAME%_bck_d.pdb

set LIB_NAME_RELEASE=ht%PRJ_NAME%_bck.lib
set LIB_NAME_DEBUG=ht%PRJ_NAME%_bck_d.lib

set DLL_NAME_DEBUG=ht%PRJ_NAME%_bck_d.dll
set DLL_NAME_RELEASE=ht%PRJ_NAME%_bck.dll

set DEFS_HEADER_FILE_NAME=%PRJ_NAME%defs.hpp
set GENERALINCLUDE_HEADER_FILE_NAME=%PRJ_NAME%.hpp
set GENERAL_IMPLEMENT_HEADER_FILE_NAME=implement.hpp
set GENERAL_IMPLEMENT_CPP_FILE_NAME=implement.cpp

:: create UID_GEN env variable
for /f "tokens=* usebackq" %%i in (`uidgen`) do (
    set UID_GEN=%%i
)
 
set PRJ_ROOT_DIR=..\platform\server\backend\prj\msvc\
set SCR_ROOT_DIR=..\platform\server\backend\src\%PRJ_NAME%\

set PRJ_ROOT_FILE=%PRJ_ROOT_DIR%\%PRJ_NAME%_bck.vcproj

:: create source dir
md %SCR_ROOT_DIR%

:: create external header file
call utils\hpp_dll_tmpl.cmd > %SCR_ROOT_DIR%\%GENERALINCLUDE_HEADER_FILE_NAME%

:: create defs header file
call utils\hppdefs_dll_tmpl.cmd > %SCR_ROOT_DIR%\%DEFS_HEADER_FILE_NAME%


:: create project file
call utils\prj_dll_tmplt.cmd > %PRJ_ROOT_DIR%\%PRJ_NAME%_bck.vcproj

:: create header implementation file
call utils\implement_hpp_tmpl.cmd > %SCR_ROOT_DIR%\%GENERAL_IMPLEMENT_HEADER_FILE_NAME%

:: create cpp implementation file
call utils\implement_cpp_tmpl.cmd > %SCR_ROOT_DIR%\%GENERAL_IMPLEMENT_CPP_FILE_NAME%


goto :finish


:invalid_dll_name
echo usage: create_project_dll "project name" "class name"
goto :finish

:invalid_class_name
echo usage: create_project_dll "project name" "class name"
goto :finish



:finish
echo Done - %PRJ_NAME%
pause




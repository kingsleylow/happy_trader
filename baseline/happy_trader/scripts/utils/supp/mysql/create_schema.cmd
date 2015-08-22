@echo off
call user_data
if $%MYSQL_ROOT_PATH%$ == $$ goto :err_sql_root_path


@echo on
%MYSQL_ROOT_PATH%\bin\mysql --host=%MYSQL_HOST% --user=%MYSQL_ROOT_USER% --password=%MYSQL_ROOT_PWD% --database=%MYSQL_DB% < create_schema.sql
@echo off

if errorlevel 1 goto failure 
if errorlevel 0 goto exitok 

:failure
@echo installation failure
pause
exit -1

:exitok
@echo installation of DB objects successfully done
pause
exit 0

:err_sql_root_path
@echo MYSQL_ROOT_PATH variable is not specified
pause
exit -2

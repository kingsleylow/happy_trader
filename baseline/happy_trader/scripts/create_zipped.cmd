@echo off

for /f "tokens=1-6 delims=0123456789 " %%i in ("%date%") do set StartDate=%%i%%j%%k%

for /f "tokens=1-6 delims=%StartDate% " %%i in ("%date%") do (
set day=%%i
set month=%%j
set year=%%k
)

for /f "tokens=1-6 delims=0123456789 " %%l in ("%time%") do set StartTime=%%l%%m

for /f "tokens=1-6 delims=%StartTime% " %%l in ("%time%") do (
set hour=%%l
set min=%%m
)

set ZIP_FILENAME=install_%day%-%month%-%year%_%hour%%min%


zip -r ..\%ZIP_FILENAME% ..\install


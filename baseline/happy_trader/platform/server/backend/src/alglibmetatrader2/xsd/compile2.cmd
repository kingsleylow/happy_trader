@echo off
cd %HT_DEVEL_ROOT%\platform\server\backend\src\alglibmetatrader2\xsd\
%HT_DEVEL_ROOT%\extern\tools\gsoap-2.8\gsoap\bin\win32\wsdl2h.exe -t "%HT_DEVEL_ROOT%\extern\tools\gsoap-2.8\gsoap\typemap.dat" -o auto\tmp.h ht_settings.xsd
%HT_DEVEL_ROOT%\extern\tools\gsoap-2.8\gsoap\bin\win32\soapcpp2 -I "%HT_DEVEL_ROOT%\extern\tools\gsoap-2.8\gsoap\import" -d auto auto\tmp.h
 
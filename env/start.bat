@echo off
call versions.bat
cd database
start ..\server\mysql_%mysql_version%\exe\mysqld.exe --datadir="%CD%"
cd ..
start server\apache_%apache_version%\exe\apache.exe -f "%CD%\server\apache_%apache_version%\conf\httpd.conf"

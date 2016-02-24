@echo off
cd /d %~dp0
call run-install.bat wavyppt server.exe
rem net start wavyppt
pause
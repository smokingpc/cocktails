@ECHO OFF
REM This script is a workaround to launch unsigned PowerShell script file.
REM No need to make permanent changes to the system settings. 
REM But need Admin privileged console prompt to run this batch.
REM 
REM Usage: run.bat [power shell file] [arguments of the power shell file]
REM        [power shell file] should be fullpath of script file. Put it in quotes.
REM Example: run.bat "C:\my_scripts\my_powershell.ps1" -Verbose
REM

set TARGET=%~1
powershell -executionPolicy RemoteSigned "%TARGET%" %~2 %~3 %~4 %~5 %~6 %~7 %~8 %~9

@echo off

call %~dp0\vars.bat

start "" "%UE58_EDITOR_EXE%" "%UPROJECT_PATH%" %*
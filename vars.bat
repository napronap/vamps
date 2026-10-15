@echo off

set ROOTDIR=%~dp0
set ROOTDIR=%ROOTDIR:~0,-1%

set PROJECT=vamps
set PROJECT_DIR=%ROOTDIR%\%PROJECT%
set UPROJECT_PATH=%ROOTDIR%\%PROJECT%.uproject

set UE58_DIR=C:\Program Files\Epic Games\UE_5.8
set UE58_EDITOR_EXE=%UE58_DIR%\Engine\Binaries\Win64\UnrealEditor.exe
set BUILD_BAT=%UE58_DIR%\Engine\Build\BatchFiles\Build.bat
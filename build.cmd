@echo off
setlocal
setlocal enabledelayedexpansion
set cmd=%1
set SOLUTION_FILE="iron.sln"
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin;%PATH% 
cd iron

if "%cmd%" == "clean" ( 
	echo clean .....
	msbuild.exe %SOLUTION_FILE% /t:Clean
	del /s *.obj
	del /s *.lib
	del /s *.dll
	exit /b 0
)

set configs=Debug,Release
set platforms=x64,x86

for %%c in (%configs%) do (
    for %%p in (%platforms%) do (
        echo Building %%c ^| %%p
        call :build_configuration "%%c" "%%p"
        if !errorlevel! neq 0 goto :error_exit
    )
)

:exit
echo.
echo *** BUILD OK ***
exit /b 0

:error_exit
echo Script terminato per errore
exit /b 1

:build_configuration
echo.
echo =================================================================
echo = BUILD: Configuration=%~1, Platform=%~2
echo =================================================================
msbuild.exe %SOLUTION_FILE% /p:Configuration=%~1 /p:Platform=%~2 /t:Build

if %errorlevel% neq 0 (
    echo.
    echo !!! BUILD ERROR %~1^|%~2 !!!
    echo.
    exit /b 1
)
exit /b 0

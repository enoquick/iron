@echo off
setlocal
setlocal enabledelayedexpansion
set dpath=%1
if "%dpath%" == "" (
	echo "install <dest_path>"
	exit /b 1
)
set SOLUTION_FILE="iron.sln"
set configs=Debug,Release
set platforms=x64,x86
set libs=iron,iron_modules
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin;%PATH% 
cd iron

for %%c in (%configs%) do (
    for %%p in (%platforms%) do (
    	for %%l in (%libs%) do (
        	echo Building %l %%c %%p
        	call :build_configuration "%%c" "%%p" "%%l"
        	if !errorlevel! neq 0 goto :error_exit
        )
    )
)

cd ..

if not exist "!dpath!" mkdir "!dpath!"
if not exist "!dpath!\Debug" mkdir "!dpath!\Debug"
if not exist "!dpath!\Release" mkdir "!dpath!\Release"
if not exist "!dpath!\x64" mkdir "!dpath!\x64"
if not exist "!dpath!\x64\Debug" mkdir "!dpath!\x64\Debug"
if not exist "!dpath!\x64\Release" mkdir "!dpath!\x64\Release"


xcopy /S /I /Y /Q ".\includes" "!dpath!\includes"
xcopy /S /I /Y /Q ".\src" "!dpath!\src"

xcopy /S /I /Y /Q ".\iron\Debug\iron*.lib" "!dpath!\Debug"
xcopy /S /I /Y /Q ".\iron\Debug\iron*.dll" "!dpath!\Debug"

xcopy /S /I /Y /Q ".\iron\Release\iron*.lib" "!dpath!\Release"
xcopy /S /I /Y /Q ".\iron\Release\iron*.dll" "!dpath!\Release"


xcopy /S /I /Y /Q ".\iron\x64\Debug\iron*.lib" "!dpath!\x64\Debug"
xcopy /S /I /Y /Q ".\iron\x64\Debug\iron*.dll" "!dpath!\x64\Debug"

xcopy /S /I /Y /Q ".\iron\x64\Release\iron*.lib" "!dpath!\x64\Release"
xcopy /S /I /Y /Q ".\iron\x64\Release\iron*.dll" "!dpath!\x64\Release"


exit /b 0

:error_exit
echo Script terminated with errors
exit /b 1


:build_configuration
echo.
echo =================================================================
echo = BUILD: Configuration=%~1, Platform=%~2
echo =================================================================
msbuild.exe %SOLUTION_FILE% /p:Configuration=%~1 /p:Platform=%~2 /t:%~3

if %errorlevel% neq 0 (
    echo.
    echo !!! BUILD ERROR %~3 %~1 %~2 !!!
    echo.
    exit /b 1
)

exit /b 0

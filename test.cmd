@echo off
setlocal
FOR /F "delims=" %%F IN ('dir /s /b *.exe') DO (
    
    echo execute: "%%F"    
    CALL "%%F"
    
    IF ERRORLEVEL 1 (
        echo.
        echo !!! ERROR: "%%F" 
        exit /b 1
    )
)
exit /b 0

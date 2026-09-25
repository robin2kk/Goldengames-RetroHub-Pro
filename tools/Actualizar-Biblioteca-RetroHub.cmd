@echo off
set /p PS5_FTP_HOST=Escribe la IP de tu PS5 (ejemplo 192.168.1.184): 
if "%PS5_FTP_HOST%"=="" exit /b 1
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Build-RetroHubLibraryFromFTP.ps1" -HostAddress "%PS5_FTP_HOST%"
echo.
echo Si ves un error, toma una captura de esta ventana.
pause

@echo off

setlocal

set /p "version=version: "

set "zipfile=benji-%version%.zip"

if exist "%zipfile%" del "%zipfile%"

tar -a -c -f "%zipfile%" -C "%CD%\build" benji-installer.exe -C "%CD%\build" benji-service.exe -C "%CD%\tests" config.toml
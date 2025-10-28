@echo off
echo Запуск 2DPainting...
echo.

REM Установка переменных окружения для Qt
set QT_PLUGIN_PATH=%~dp0
set QT_QPA_PLATFORM_PLUGIN_PATH=%~dp0platforms

REM Запуск программы
start "" "%~dp02dpainting.exe"

echo Программа запущена!
pause

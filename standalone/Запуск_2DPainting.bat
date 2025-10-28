@echo off
title 2DPainting - Управление ПБ
echo.
echo ========================================
echo    2DPainting - Управление ПБ
echo ========================================
echo.
echo Запуск программы...
echo.

REM Установка переменных окружения для Qt
set QT_PLUGIN_PATH=%~dp0
set QT_QPA_PLATFORM_PLUGIN_PATH=%~dp0platforms

REM Запуск программы
"%~dp02dpainting.exe"

REM Если программа завершилась с ошибкой
if %ERRORLEVEL% neq 0 (
    echo.
    echo ОШИБКА: Программа завершилась с кодом %ERRORLEVEL%
    echo Возможные причины:
    echo - Отсутствуют необходимые библиотеки
    echo - Проблемы с COM-портом
    echo - Ошибки в конфигурации
    echo.
    pause
) else (
    echo.
    echo Программа завершена успешно.
)

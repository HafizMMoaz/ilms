@echo off
REM Build ILMS (MVC). Requires g++ (MinGW-w64) on PATH.
setlocal

set INCLUDES=-Imodel -Iview -Icontroller
set SOURCES=main.cpp ^
    model\Utils.cpp model\Session.cpp model\Validator.cpp model\Database.cpp model\Backup.cpp model\Logger.cpp model\Export.cpp model\Pdf.cpp ^
    view\Console.cpp view\ConsoleView.cpp ^
    controller\App.cpp

g++ -std=c++17 %INCLUDES% %SOURCES% -o ilms.exe
if %errorlevel% neq 0 (
    echo.
    echo BUILD FAILED.
    exit /b %errorlevel%
)
echo BUILD OK - ilms.exe

@echo off
REM Build ILMS (MVC). Requires g++ / gcc / ar (MinGW-w64) on PATH.
setlocal

set HPDFINC=-Ivendor\libharu\include

REM 1) Build the vendored libHaru into libhpdf.a (cached after the first run;
REM    delete libhpdf.a to force a rebuild).
if not exist libhpdf.a (
    echo Building libHaru ^(one time^) ...
    if exist obj rmdir /s /q obj
    mkdir obj
    for %%f in (vendor\libharu\src\*.c) do gcc -c -w -O2 %HPDFINC% "%%f" -o "obj\%%~nf.o"
    ar rcs libhpdf.a obj\*.o
    if not exist libhpdf.a ( echo libHaru build FAILED. & exit /b 1 )
)

REM 2) Build the application, linking the static libHaru.
set INCLUDES=-Imodel -Iview -Icontroller %HPDFINC%
set SOURCES=main.cpp ^
    model\Utils.cpp model\Session.cpp model\Validator.cpp model\Database.cpp model\Backup.cpp model\Logger.cpp model\Export.cpp model\Pdf.cpp ^
    view\Console.cpp view\ConsoleView.cpp ^
    controller\App.cpp

g++ -std=c++17 %INCLUDES% %SOURCES% libhpdf.a -o ilms.exe
if %errorlevel% neq 0 (
    echo.
    echo BUILD FAILED.
    exit /b %errorlevel%
)
echo BUILD OK - ilms.exe

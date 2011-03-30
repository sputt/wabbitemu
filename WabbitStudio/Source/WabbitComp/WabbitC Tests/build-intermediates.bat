@echo off
call "%VS100COMNTOOLS%vsvars32.bat"

pushd "C Files"

IF NOT EXIST "..\%1_expected.c" cl /MP2 /nologo /DTEST_FUNCTION=%1 main.c %1_expected.c /Fe..\%1_expected.exe
cl /MP2 /nologo /DTEST_FUNCTION=%1 main.c %1_actual.c /Fe..\%1_actual.exe
IF ERRORLEVEL 1 GOTO ERROR
GOTO END
:ERROR
popd
echo "Error level: %ERRORLEVEL%"
exit /B 1
:END
popd
echo "No error"
exit /B 0
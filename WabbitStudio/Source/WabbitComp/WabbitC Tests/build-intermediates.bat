call "%VS100COMNTOOLS%\vsvars32.bat"

del %1_expected.exe > nul
del %1_actual.exe > nul

pushd "C Files"

cl /nologo /DTEST_FUNCTION=%1 main.c %1_expected.c /Fe..\%1_expected.exe
cl /nologo /DTEST_FUNCTION=%1 main.c %1_actual.c /Fe..\%1_actual.exe

popd

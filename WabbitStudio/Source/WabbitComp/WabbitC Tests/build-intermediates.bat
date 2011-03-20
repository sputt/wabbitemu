call "%VS100COMNTOOLS%vsvars32.bat"

pushd "C Files"

cl /MP2 /nologo /DTEST_FUNCTION=%1 main.c %1_expected.c /Fe..\%1_expected.exe
IF NOT EXIST ..\%1_actual.exe cl /MP2 /nologo /DTEST_FUNCTION=%1 main.c %1_actual.c /Fe..\%1_actual.exe

popd

@echo off


echo Assembling BootFree for the TI-73...
spasm -T -D TI73 boot.asm bf73.hex
if errorlevel 1 goto ERR
echo.

echo Assembling BootFree for the TI-83 Plus...
spasm -T -D TI83PBE boot.asm bf83pbe.hex
if errorlevel 1 goto ERR
echo.

echo Assembling BootFree for the TI-83 Plus Silver Edition...
spasm -T -D TI83PSE boot.asm bf83pse.hex
if errorlevel 1 goto ERR
echo.

echo Assembling BootFree for the TI-84 Plus...
spasm -T -D TI84PBE boot.asm bf84pbe.hex
if errorlevel 1 goto ERR
echo.

echo Assembling BootFree for the TI-84 Plus Silver Edition...
spasm -T -D TI84PSE boot.asm bf84pse.hex
if errorlevel 1 goto ERR
echo.

echo Success!

goto DONE
:ERR
echo *** There were errors. ***
:DONE

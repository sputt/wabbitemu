@echo off


echo Assembling Rom8x
spasm -T rom8x.asm rom8x.8xp
if errorlevel 1 goto ERR
echo.

echo Assembling Rom8x for color
spasm -T -D COLOR rom8x.asm rom8xcolor.8xp
if errorlevel 1 goto ERR
echo.

echo Success!

goto DONE
:ERR
echo *** There were errors. ***
:DONE

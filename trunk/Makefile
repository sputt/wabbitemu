all:
	gcc -c -I . -I gui -I core -I debugger -I hardware -I utilities -I interface gui/*.c hardware/*.c core/*.c interface/calc.c interface/state.c utilities/*.c debugger/*.c -DWINVER=0x502 -D_WIN32_WINNT=0x502 -DHIGH_SHADE_GIF -DVERBOSE -D_WIN32_IE=0x501

	windres -Igui -i gui/gui_resources.rc -o gui_resources.o

	gcc -o wabbitemu.exe *.o -lgdi32 -lole32 -lcomdlg32 -lmsimg32 -lcomctl32 -lwinmm -lUuid -lshell32 -lz -lpng
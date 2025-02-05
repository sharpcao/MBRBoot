#-v for Debug

CFLAGS = -q -c -v 
IFLAGS = -q -b:0x100000 -v 
 
default:
	make sector_x.bin

sector_x.bin : bootpack.obj vga.obj hankaku.obj int.obj
	ilink32 $(IFLAGS) ..\common\c0x32_mini.obj bootpack.obj vga.obj int.obj hankaku.obj, peos.exe,,cw32.lib import32.lib
	copy peos.exe sector_x.bin
	del *.il*  

bootpack.obj : bootpack.cpp
	bcc32c $(CFLAGS) bootpack.cpp

vga.obj : vga.cpp
	bcc32c $(CFLAGS) vga.cpp

int.obj: int.cpp
	bcc32c $(CFLAGS) int.cpp

hankaku.obj :
	bcc32c $(CFLAGS) ..\common\hankaku.cpp



clean:
	del *.il* *.tds *.map *.obj *.tds *.exe *.bin peos.txt



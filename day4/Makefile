#-v for Debug

CFLAGS = -q -c -v
IFLAGS = -q -b:0x100000 -v
 

default:
	make sector_x.bin

sector_x.bin : bootpack.cpp Makefile
	bcc32c $(CFLAGS) bootpack.cpp -o peos.obj
	ilink32 $(IFLAGS) ..\common\c0x32_mini.obj peos.obj,peos.exe,,cw32.lib import32.lib
					#cw32.lib import32.lib in  BCC102\lib\win32c\release
	tdump peos.exe > peos.txt
	copy peos.exe sector_x.bin
	del *.il*  


clean:
	del *.il* *.tds *.map *.obj *.tds *.exe *.bin



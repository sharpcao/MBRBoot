default:
	make sector_x.bin

sector_x.bin : bootpack.cpp Makefile
	bcc32c -q -c bootpack.cpp -o peos.obj
	ilink32 -q -b:0x100000 ..\common\c0x32_mini.obj peos.obj,peos.exe,,cw32.lib import32.lib
					#cw32.lib import32.lib in  BCC102\lib\win32c\release
	tdump peos.exe > peos.txt
	copy peos.exe sector_x.bin
	del peos.il* peos.tds peos.map 


clean:
	del sector_x.bin peos.exe peos.obj
	
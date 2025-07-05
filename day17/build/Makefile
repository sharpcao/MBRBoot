#-v for Debug

CFLAGS = -q -c -v -x- -O 
IFLAGS = -q -b:0x100000 -v -Gn -LD:\BCC102\lib\win32c\release

OBJS = vga.obj hankaku.obj gdtidt.obj device.obj handler.obj \
		memory_kit.obj task_kit.obj timer_kit.obj \
		winos.obj layer.obj bootpack.obj console.obj

 
default:
	make sector_x.bin

sector_x.bin : $(OBJS)
	ilink32 $(IFLAGS) ..\..\common\c0x32_mini.obj $(OBJS), peos.exe,,cw32.lib import32.lib
	copy peos.exe sector_x.bin  

hankaku.obj :
	bcc32c $(CFLAGS) ..\..\common\hankaku.cpp
	
{..}.cpp{.}.obj:
	bcc32c $(CFLAGS) $< 


clean:
	del *.tds *.map *.obj *.tds *.exe *.bin *.il*
	del ..\inc\*.gch 



#-v for Debug

CFLAGS = -q -c -v -x- -O 
IFLAGS = -q -b:0x100000 -v -Gn -LD:\BCC102\lib\win32c\release

OBJS = bootpack.obj vga.obj hankaku.obj int.obj gdtidt.obj task_kit.obj input_device.obj handler.obj \
		mem_mgr.obj layer.obj timer.obj global.obj

 
default:
	make sector_x.bin

sector_x.bin : $(OBJS)
	ilink32 $(IFLAGS) ..\common\c0x32_mini.obj $(OBJS), peos.exe,,cw32.lib import32.lib
	copy peos.exe sector_x.bin
	del *.il*  

hankaku.obj :
	bcc32c $(CFLAGS) ..\common\hankaku.cpp
	
.cpp.obj:
	bcc32c $(CFLAGS) $<


clean:
	del *.il* *.tds *.map *.obj *.tds *.exe *.bin
	del inc\*.gch 



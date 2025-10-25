#-v for Debug

CFLAGS = -q -c -x- -O2 -v 
IFLAGS = -q -b:0x100000  -v -Gn -LD:\BCC102\lib\win32c\release

OBJS = vga.obj hankaku.obj gdtidt.obj device.obj handler.obj \
		memory_kit.obj task_kit.obj timer_kit.obj winform_kit.obj \
		winos.obj layer.obj bootpack.obj console.obj os_io.obj

 
default:
	make peos.sys
	fos c.img makeimage 10240
	fos c.img format
	fos c.img addfile peos.sys
	dd if=sector_0_FDAT.bin of=c.img

peos.sys : $(OBJS)
	ilink32 $(IFLAGS) ..\..\common\c0x32_mini.obj $(OBJS), peos.exe,,cw32.lib import32.lib
	copy peos.exe peos.sys  
	dd if=pe_loader.bin of=peos.sys

hankaku.obj :
	bcc32c $(CFLAGS) ..\..\common\hankaku.cpp
	
{..}.cpp{.}.obj:
	bcc32c $(CFLAGS) $< 


clean:
	del *.tds *.map *.obj *.tds *.exe  *.il* peos.sys c.img *.lock
	del ..\inc\*.gch 



c.img:
	nasm ../mbr_ufat_multiboot.asm -o mbr.bin
	nasm ../hello_os.asm -o hello.os
	fos c.img makeimage 10240
	fos c.img format
	fos c.img addfile hello.os
	dd if=mbr.bin of=c.img 
	del mbr.bin hello.os
	


default: c.img

clean:
	del c.img mbr.bin hello.os
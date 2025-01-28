copy ..\sector_0\sector_0.bin+vga.bin test.bin
..\bin\dd if=/dev/zero of=fdimage.img bs=1440k count=1
grabin test.bin -ow fdimage.img

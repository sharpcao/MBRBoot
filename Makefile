DD = bin\dd.exe


defualt:
	make fdimage.img

fdimage.img : sector_0/sector_0.bin sector_1/sector_1.bin sector_x/sector_x.bin
	$(DD) if=/dev/zero of=fdimage.img bs=1440k count=1
	$(DD) if=sector_0/sector_0.bin of=fdimage.img 
	$(DD) if=sector_x/sector_x.bin of=fdimage.img seek=1
	$(DD) if=sector_1/sector_1.bin of=fdimage.img seek=1   #overwrite sector 1

sector_1/sector_1.bin: sector_1/load_peos.asm
	cd sector_1
	make sector_1.bin 
	cd ..

sector_x/sector_x.bin: sector_x/peos.cpp
	cd sector_x
	make sector_x.bin
	cd ..





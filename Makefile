DD = bin\dd.exe


defualt:
	make fdimage.img

fdimage.img : sector_0/sector_0.bin sector_1/sector_1.bin sector_x/sector_x.bin
	copy /b sector_0\sector_0.bin+sector_x\sector_x.bin peos.bin
	$(DD) if=sector_1/sector_1.bin of=peos.bin seek=1
	$(DD) if=/dev/zero of=fdimage.img bs=1440k count=1
	$(DD) if=peos.bin of=fdimage.img 


sector_1/sector_1.bin: sector_1/load_peos.asm
	cd sector_1
	make sector_1.bin 
	cd ..

sector_x/sector_x.bin: sector_x/peos.cpp
	cd sector_x
	make sector_x.bin
	cd ..

clean:
	del fdimage.img peos.bin
	del sector_1\*.bin
	del sector_x\*.bin






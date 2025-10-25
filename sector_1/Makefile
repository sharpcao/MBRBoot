SOURCE = pe_s0_loados.asm
sector_1.bin : $(SOURCE)
	nasm $(SOURCE) -o pe_loader.bin -l pe_loader.lst
	copy pe_loader.bin sector_1.bin

clean:
	del *.bin *.lst
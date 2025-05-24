DD=bin\dd.exe
sector_x_dir=day16


defualt:
    make fdimage.img

fdimage.img : sector_0/sector_0.bin sector_1/sector_1.bin $(sector_x_dir)/sector_x.bin
    copy /b sector_0\sector_0.bin+$(sector_x_dir)\sector_x.bin peos.bin
    $(DD) if=sector_1/sector_1.bin of=peos.bin seek=1
    $(DD) if=/dev/zero of=fdimage.img bs=1440k count=1
    $(DD) if=peos.bin of=fdimage.img 


sector_1/sector_1.bin: sector_1/load_peos.asm
    cd sector_1
    make sector_1.bin 
    cd ..

$(sector_x_dir)/sector_x.bin: 
    cd $(sector_x_dir)
    make sector_x.bin
    cd ..

clean:
    del fdimage.img peos.bin



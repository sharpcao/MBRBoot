default : 
	make hankaku.bin 

hankaku.bin: 
	make osfont.exe
	osfont hankaku.txt hankaku.bin

osfont.exe : osfont.cpp
	bcc32c osfont.cpp
	del osfont.tds

clean:
	del *.exe *.bin *.tds
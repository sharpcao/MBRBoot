/*
    Use Turbo C++ to compile 16bit obj file, then tdump the file to mbr.txt
    1) tcc -c mbr.cpp 
    2) tdump mbr.obj > mbr.txt
    3) _TEXT segment is the MBR code. Copy&Paste it to sector_0.bin(512 bytes), modify the last 2 bytes "55 AA" .

    Author:Sharp.Cao
    Date:2025/9/27

*/


#define PE_MEM_LOCATION     0x8000
#define HDD_META_OFFSET     0X200
#define HDD_META_FDAT_INDEX 0x4
#define LOAD_SECTS            128u
#define BOOTINFO_ADDR       0x0ff0


typedef unsigned int uint;
typedef unsigned long ulong;

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx, scrny;
    long  vram;
};

struct DAP{
    char    size;
    char    reserved;
    uint    n_sector;
    uint    offset;
    uint    segment;
    uint    lba_w0;
    uint    lba_w1;
    uint    lba_w2;
    uint    lba_w3;

};

struct META_REC{
    char    name[8];
    ulong   start_addr;
    ulong   size;
};


void read_hdd(ulong hdd_addr, uint to_ram_addr, uint nsector, char driver);
void load();
void config_video();

void print_char(char x);
void print_hex(char h);

void main()
{
    asm { 
        mov ax,0x0
        mov ds,ax
        mov es,ax
        mov ss,ax
        mov sp, 0x7c00 // Default sp is FFFF, copy sectors would overwrite stack
    }  
    load();

}

void load()
{

    char driver;
    asm {  mov driver, dl }


    read_hdd(HDD_META_OFFSET, 0x7e00 >> 4, 1, driver); // Read Meta;
    META_REC* pmeta = (META_REC*)(0x7e00);

	//read_hdd(pmeta[HDD_META_FDAT_INDEX].start_addr, PE_MEM_LOCATION >> 4, LOAD_SECTS, driver);

    ulong start_addr = pmeta[HDD_META_FDAT_INDEX].start_addr;
    for(uint i = 0;i < LOAD_SECTS; ++i){
        uint  mem_seg = PE_MEM_LOCATION / 16+ i*512 /16;
        read_hdd(start_addr + i*512, mem_seg, 1, driver);

    }

	config_video();
	
	asm{
		mov dx, BOOTINFO_ADDR
		mov ax, PE_MEM_LOCATION //jump to entry pointer
		jmp ax
	}


}


void read_hdd(ulong hdd_addr, uint to_ram_seg, uint nsector, char driver)
{

	DAP dap;
    dap.size = (char)0x10;
    dap.reserved = (char)0x0;
    dap.n_sector = nsector;
	dap.offset = 0; 
    dap.segment = to_ram_seg;

// convert hdd_addr to LBA sector number 

    uint* p_tmp = (uint*)&hdd_addr;
    uint w0 = p_tmp[0];
    uint w1 = p_tmp[1];
    w0 = (((w1 & 0x00FF) <<8) | (w0 >> 8));
    w1 >>= 8;

	dap.lba_w0 = ((w0 >> 1) | (w1 & 0x0001) << 15);
    dap.lba_w1 = (w1 >> 1);
    dap.lba_w2 = 0;
    dap.lba_w3 = 0;


    asm{
        lea si, dap
        mov dl, driver
        mov ah, 0x42
        int 0x13
    }

}

void config_video()
{
    BOOTINFO* p = (BOOTINFO*)BOOTINFO_ADDR;
    p->vmode = 8;
    p->scrnx = 320;
    p->scrny = 200;
    p->vram = 0xa0000;

    asm{
        mov al, 0x13
        mov ah, 0x0
        int 0x10
        mov ah,0x02
        int 0x16
        mov bx, p
        mov [bx+1],al // mov p->leds,al
    }
}


/*

void print_char(char x)
{
    asm{
        mov ah, 0xe
        mov al, x
        int 0x10
    }
}

void print_hex(char h)
{
    char x = (h & 0xF0)>>4;
    print_char( (x<=9)? x+ '0' - 0 : x + 'A' - 10);
    x = (h & 0x0F);
    print_char( (x<=9)? x+ '0' - 0 : x + 'A' - 10);
    print_char(' ');
}

*/


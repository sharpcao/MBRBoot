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


void read_hdd(ulong hdd_addr, uint to_ram_addr, uint nsector, uint driver);
void load();
void config_video();
void print_str(char* p_str);
void print_char(char x);

void main()
{
    asm { 
        mov sp, 0x7c00 // Default sp is FFFF, copy sectors would overwrite stack
    }  
    load();

}

void load()
{
    // char *str1 = "read s1\r\n";
    // char *str2 = "read s_more\r\n";
    // char *str3 = "config_video\r\n";

    uint driver;
    asm { mov driver, dx}

    print_char('1');
    read_hdd(HDD_META_OFFSET, 0x7e00,1, driver); // Read Meta;
    META_REC* pmeta = (META_REC*)(0x7e00);

    print_char('2');
	read_hdd(pmeta[HDD_META_FDAT_INDEX].start_addr, PE_MEM_LOCATION, LOAD_SECTS, driver);


    print_char('3');
    //config_video();
    L1:
    asm{
        mov dx, BOOTINFO_ADDR
        mov ax, PE_MEM_LOCATION //jump to entry pointer
        //jmp ax
        jmp L1
    }


}


void read_hdd(ulong hdd_addr, uint to_ram_addr, uint nsector, uint driver)
{

	DAP dap;
    dap.size = (char)0x10;
    dap.reserved = (char)0x0;
    dap.n_sector = nsector;
    dap.offset = to_ram_addr;
    dap.segment = 0;

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
        mov dx, driver
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


void print_char(char x)
{
    asm{
        mov ah, 0xe
        mov al, x
        int 0x10
    }
}
void print_str(char* p_str)
{

    char* p;
    for(p=p_str; *p!=0 ;++p) print_char(*p);
    
}



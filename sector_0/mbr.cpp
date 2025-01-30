/*
    Use Turbo C++ to compile 16bit obj file, then tdump the file to mbr.txt
    1) tcc -c mbr.cpp 
    2) tdump mbr.obj > mbr.txt
    3) _TEXT segment is the MBR code. Copy&Paste it to sector_0.bin(512 bytes), modify the last 2 bytes "55 AA" .

    Author:Sharp.Cao
    Date:2025/1/25

*/

#define PE_MEM_LOCATION     0x8000
#define LOAD_SECTS            128u
#define BOOTINFO_ADDR       0x0ff0

struct BOOTINFO{
    char cyls,leds,vmode,reserve;
    short scrnx, scrny;
    long  vram;
};

class FDD
{
public:
    static void read_sector(unsigned serial_no, long address); 
    //Read a sector from FDD; @serial_no: serial Number from 0; @address: load to ram address  
};

void load();
void load_to_ram(FDD& from_dev, unsigned sector_start, unsigned num_of_sectors, long to_ram_address);
void config_video();

void main()
{
    asm { mov sp, 0x7c00 }  // Default sp is FFFF, copy sectors would overwrite stack
    load();
}

void load()
{
    FDD dev;
    load_to_ram(dev,1u,LOAD_SECTS,PE_MEM_LOCATION); 
    config_video();

    asm{
        mov dx, BOOTINFO_ADDR
        mov ax, PE_MEM_LOCATION //jump to entry pointer
        jmp ax
    }
}

void load_to_ram(FDD& from_dev, unsigned sector_start, unsigned num_of_sectors, long to_ram_address)
{

    for(unsigned i = 0; i != num_of_sectors; ++i){
        from_dev.read_sector(sector_start + i, to_ram_address + i * 0x200);
    }
}

void FDD::read_sector(unsigned serial_n, long address)
{
    unsigned n_sector = serial_n % 18 + 1;
    unsigned n_head = (serial_n / 18) % 2;
    unsigned n_cylinder = serial_n / 36;
    unsigned m_cx = n_cylinder << 8 | n_sector;
    unsigned m_dx = n_head <<8 | 0x00;
    unsigned* paddr = (unsigned*)&address;
    unsigned m_bx = paddr[0] & 0x000f;
    unsigned m_es = (paddr[1] << 12 | paddr[0] >> 4);
    
    asm{
        mov     bx, m_es
        mov     es,bx
        mov     bx,m_bx     // read to address of ES:BX
        mov     ax,0201h    // AH=2 mean read disk; AL=1 read one disk;
        mov     cx,m_cx     // CH=X,cylinder number; CL=X sector number
        mov     dx,m_dx     // DH=X,head number; DL=X,00 mean the first FDD, disk_C=80h, Disk_D=81h
        int     13h         
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




/*
	Use Turbo C++ to compile 16bit obj file, then tdump the file to mbr.txt
	1) tcc -c mbr.cpp 
	2) tdump mbr.obj > mbr.txt
	3) _TEXT segment is the MBR code. Copy&Paste it to sector_0.bin(512 bytes), modify the last 2 bytes "55 AA" .

	Author:Sharp.Cao
	Date:2025/1/25

*/

class FDD
{
public:
	static void read_sector(unsigned serial_no, long address); 
	//Read a sector from FDD; @serial_no: serial Number from 0; @address: load to ram address  

};

/* using the template, the compiled obj file is hard to use.  
template <class D>
void load_to_ram(D& from_dev, unsigned sector_start, unsigned num_of_sectors, long to_ram_address);
*/

void load_to_ram(FDD& from_dev, unsigned sector_start, unsigned num_of_sectors, long to_ram_address);


void main()
{
	FDD dev;
	load_to_ram(dev,1u,1,0x7e00);
	load_to_ram(dev,2u,128u,0x10000); //read to contiguous ram address. (load into 0xfe00 failed) 
	asm{

		mov ax, 0x7e00 //jump to entry pointer
		jmp ax

	}
}


void FDD::read_sector(unsigned serial_n, long address)
{
	unsigned n_sector = serial_n % 18 + 1;
	unsigned n_head = (serial_n / 18) % 2;
	unsigned n_cylinder = serial_n / 36;
	unsigned m_cx = n_cylinder << 8 | n_sector;
	unsigned m_dx = n_head <<8 | 0x00;
	//unsigned m_es = address / 16 ;
	//unsigned m_bx = address & 0x000F;
	unsigned* paddr = (unsigned*)&address;
	unsigned m_bx = paddr[0] & 0x000f;
	unsigned m_es = (paddr[1] << 12 | paddr[0] >> 4);
	
	asm{
		mov		bx, m_es
		mov		es,bx
		mov		bx,m_bx	// read to address of ES:BX
		mov 	ax,0201h 	// AH=2 mean read disk; AL=1 read one disk;
		mov 	cx,m_cx		// CH=X,cylinder number; CL=X sector number
		mov 	dx,m_dx		// DH=X,head number; DL=X,00 mean the first FDD, disk_C=80h, Disk_D=81h

		int		13h			
	}
}

/* use template to compile, the obj file is hard to use.
template <class D>
void load_to_ram(D& from_dev, unsigned sector_start, unsigned num_of_sectors, unsigned to_ram_address)
*/
void load_to_ram(FDD& from_dev, unsigned sector_start, unsigned num_of_sectors, long to_ram_address)
{

	for(unsigned i = 0; i != num_of_sectors; ++i){
		from_dev.read_sector(sector_start + i, to_ram_address + i * 0x200);
	}
}




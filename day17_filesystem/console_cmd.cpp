#include "inc\functions.h"
#include "inc\os_io.h"
#include "inc\console.h"

#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_FEATURES    0x1F1
#define ATA_SECTOR_COUNT 0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE_HEAD  0x1F6
#define ATA_STATUS      0x1F7
#define ATA_COMMAND     0x1F7

// 状态寄存器位
#define ATA_SR_BSY      0x80
#define ATA_SR_DRDY     0x40
#define ATA_SR_DF       0x20
#define ATA_SR_DSC      0x10
#define ATA_SR_DRQ      0x08
#define ATA_SR_CORR     0x04
#define ATA_SR_IDX      0x02
#define ATA_SR_ERR      0x01

// 命令
#define ATA_CMD_READ_PIO    0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_IDENTIFY    0xEC

static void ata_wait_bsy() {
    while (io_in8(ATA_STATUS) & ATA_SR_BSY);
}

static void ata_wait_drq() {
    while (!(io_in8(ATA_STATUS) & ATA_SR_DRQ));
}

using uint16 = short unsigned int;
using uint8 = unsigned char;

// LBA28读取单个扇区 https://wiki.osdev.org/ATA_PIO_Mode#28_bit_PIO
int ata_lba28_read(uint lba, uint16* buffer, uint8 slavebit ) {
    ata_wait_bsy();
    
    // 选择驱动器和LBA地址的高4位
    io_out8(ATA_DRIVE_HEAD, 0xE0 | (slavebit  << 4) | ((lba >> 24) & 0x0F));
    
    // 设置扇区数量
    io_out8(ATA_SECTOR_COUNT, 1);
    
    // 设置LBA地址
    io_out8(ATA_LBA_LOW, lba & 0xFF);
    io_out8(ATA_LBA_MID, (lba >> 8) & 0xFF);
    io_out8(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    
    // 发送读取命令
    io_out8(ATA_COMMAND, ATA_CMD_READ_PIO);
    
    // 等待数据就绪
    ata_wait_bsy();
    ata_wait_drq();
    
    // 读取数据
    for (int i = 0; i < 256; i++) {
        buffer[i] = io_in16(ATA_DATA);
    }
    
    return 0;
}


void console_dump(stringbuf<>& cout_str, const Cmd_Parser& cmd)
{
	
	uint dump_size;
	uint param_num = cmd.size();
	if (param_num == 3){
		dump_size = cmd[2].to_uint();
		if (dump_size == 0){
			cout_str <<"dumpsize is illegal!\n";
			return;
		}
	}else if (param_num == 2)
	{
		dump_size = 16;
	}else{

		cout_str <<"dump [address] [size = 16]\n";
		return;
	}

	stringbuf<> saddr = cmd[1];
	saddr << stringbuf<>::flag::hex;

	uint addr = saddr.to_uint();

	cout_str << stringbuf<>::flag::hex;

	cout_str << saddr <<":" ;

	for(uint i = 0; i < dump_size; ++i){
		uchar c = ((uchar*) addr)[i];
		cout_str << (uint)c <<" ";
	}

}

void console_readhd(stringbuf<>& cout_str, const Cmd_Parser& cmd)
{

	uint byte_size;
	uint param_num = cmd.size();
	if (param_num == 3){
		byte_size = cmd[2].to_uint();
		if (byte_size == 0){
			cout_str <<"bytesize is illegal!\n";
			return;
		}
	}else if (param_num == 2)
	{
		byte_size = 16;
	}else{

		cout_str <<"readhd [sector] [bytesize = 16]\n";
		return;
	}
	uint lba = cmd[1].to_uint();
	uint16 buf[256];
	ata_lba28_read(lba, buf, 0);

	cout_str << stringbuf<>::flag::hex;
	for(uint i = 0; i < byte_size; ++i){
		uchar c = ((uchar*) buf)[i];
		cout_str << (uint)c <<" ";
	}

}

void console_loadhd(stringbuf<>& cout_str, const Cmd_Parser& cmd)
{

	uint mem_addr;
	uint param_num = cmd.size();
	if (param_num == 3){
		stringbuf<> saddr = cmd[2];
		saddr << stringbuf<>::flag::hex;
		mem_addr = saddr.to_uint();
		if (mem_addr == 0){
			cout_str <<"bytesize is illegal!\n";
			return;
		}
	}else{

		cout_str <<"Loadhd [sector] [memory]\n";
		return;
	}
	uint lba = cmd[1].to_uint();

	ata_lba28_read(lba, (uint16*)mem_addr, 0);

	cout_str << "load done!\n" ;

}


void console_run_at(stringbuf<>& cout_str, const Cmd_Parser& cmd)
{
	uint param_num = cmd.size();
	if (param_num == 2){
		stringbuf<> saddr = cmd[1];
		saddr << stringbuf<>::flag::hex;
		uint mem_addr = saddr.to_uint();
		GDTIDT gdtidt;
		gdtidt[1004]->set(4*1024 - 1, mem_addr, AR_CODE32_ER);
		far_call(0,1004*8);

	}else{
		cout_str <<"runat [memory]\n";
		return;
	}

}



ConsoleLayer* cur_ConsoleLayer;

void __stdcall console_print_char(uchar c)
{
	cur_ConsoleLayer->add_char(c);
	cur_ConsoleLayer->refresh();
}


void* p_func = (void*)console_print_char;

__declspec(naked) extern "C"
void __stdcall api_print_char(uchar c)
{
	asm{
		mov eax, [esp + 8]
		and eax, 0xff
		mov ecx, p_func
		push eax
		call ecx
		retf 4
	}
}

void __stdcall (* p_api)(uchar c) = api_print_char;


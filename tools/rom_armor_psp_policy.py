import struct
import sys

"""
0083d000  24 42 4c 32 2a 70 14 8c  1b 00 00 00 58 04 00 40  |$BL2*p......X..@|

0083d010  05 00 00 00 40 04 00 00  00 04 00 00 00 00 00 80  |....@...........|
0083d020  ff ff ff ff ff ff ff ff  07 00 00 00 00 01 00 00  |................|
0083d030  00 09 00 00 00 00 00 80  ff ff ff ff ff ff ff ff  |................|
0083d040  60 00 00 20 00 30 00 00  00 10 00 00 00 00 00 80  |`.. .0..........|
0083d050  ff ff ff ff ff ff ff ff  68 00 00 00 00 d0 00 00  |........h.......|
0083d060  00 40 00 00 00 00 00 80  ff ff ff ff ff ff ff ff  |.@..............|
0083d070  68 00 80 00 00 30 00 00  00 10 01 00 00 00 00 80  |h....0..........|
0083d080  ff ff ff ff ff ff ff ff  61 00 00 00 00 00 00 00  |........a.......|
0083d090  00 00 00 00 00 00 00 00  00 00 f0 09 00 00 00 00  |................|
0083d0a0  62 00 0b 00 00 00 1a 00  00 00 f6 01 00 00 00 40  |b..............@|
0083d0b0  00 00 c0 09 00 00 00 00  63 00 00 20 00 80 02 00  |........c.. ....|
0083d0c0  00 40 01 00 00 00 00 80  ff ff ff ff ff ff ff ff  |.@..............|
0083d0d0  64 00 10 00 10 63 00 00  00 c0 03 00 00 00 00 80  |d....c..........|
0083d0e0  ff ff ff ff ff ff ff ff  65 00 10 00 e0 04 00 00  |........e.......|
0083d0f0  00 24 04 00 00 00 00 80  ff ff ff ff ff ff ff ff  |.$..............|
0083d100  64 00 20 00 80 61 00 00  00 29 04 00 00 00 00 80  |d. ..a...)......|
0083d110  ff ff ff ff ff ff ff ff  65 00 20 00 50 02 00 00  |........e. .P...|
0083d120  00 8b 04 00 00 00 00 80  ff ff ff ff ff ff ff ff  |................|
0083d130  66 00 00 00 c0 17 00 00  00 8e 04 00 00 00 00 80  |f...............|
0083d140  ff ff ff ff ff ff ff ff  66 00 10 00 c0 17 00 00  |........f.......|
0083d150  00 a6 04 00 00 00 00 80  ff ff ff ff ff ff ff ff  |................|
0083d160  66 00 20 00 c0 17 00 00  00 be 04 00 00 00 00 80  |f. .............|
0083d170  ff ff ff ff ff ff ff ff  6a 00 00 00 70 05 00 00  |........j...p...|
0083d180  00 d6 04 00 00 00 00 80  ff ff ff ff ff ff ff ff  |................|
0083d190  6d 00 00 20 00 00 04 00  00 00 00 01 00 00 00 40  |m.. ...........@|
0083d1a0  ff ff ff ff ff ff ff ff  6d 00 10 20 00 00 04 00  |........m.. ....|
0083d1b0  00 00 04 01 00 00 00 40  ff ff ff ff ff ff ff ff  |.......@........|
0083d1c0  6d 00 20 20 00 00 04 00  00 00 08 01 00 00 00 40  |m.  ...........@|
"""

ROM_ARMOR_BIOS_DIR_MAGIC = b"$BL2"
ROM_ARMOR_ENTRY_TYPE     = 0x6D


BIOS_TABLE_HEADER_SIZE = 0x10
BIOS_TABLE_ENTRY_SIZE  = 0x18
PAGE_SIZE = 0x1000

BIOS_TABLE_ENTRY_TYPE_OFFSET = 0
BIOS_TABLE_ENTRY_FLAGS_OFFSET = 2
BIOS_TABLE_ENTRY_SIZE_OFFSET = 4
BIOS_TABLE_ENTRY_S_ADDRESS_OFFSET = 8
BIOS_TABLE_ENTRY_D_ADDRESS_OFFSET = 0x10

EA_MODE_PHYSICAL_ADDRESS   = 0x00
EA_MODE_BIOS_FLASH_OFFSET  = 0x01
EA_MODE_DIRECTORY_OFFSET   = 0x02
EA_MODE_PARTITION_OFFSET   = 0x03

def main(filename):
    f = open(filename, "rb")
    content = f.read()
    f.close()

    # Read page by page and attempt to match the bios directory magic
    aligned_len = len(content) & 0xFFFFFFFFFFFFF000
    offset = -1
    for i in range(0, aligned_len, PAGE_SIZE):
        if content[i:i+4] == ROM_ARMOR_BIOS_DIR_MAGIC:
            #print(f"Found at offset {i:08x}") 
            offset = i
            break

    if offset == -1:
        print(f"-> error: the magic {ROM_ARMOR_BIOS_DIR_MAGIC} was not found")
        sys.exit(-1)
    
    number_of_entries  = struct.unpack("<L", content[offset+0x8:offset+0xC])[0]
    entry_offset = offset + BIOS_TABLE_HEADER_SIZE
    for i in range(number_of_entries):
        entry_type = content[entry_offset]
        
        entry_flags = content[entry_offset + BIOS_TABLE_ENTRY_FLAGS_OFFSET]
        b_writable = ((entry_flags >> 2) & 1) == 1

        entry_size = struct.unpack("<L", content[entry_offset + BIOS_TABLE_ENTRY_SIZE_OFFSET 
            : entry_offset + BIOS_TABLE_ENTRY_S_ADDRESS_OFFSET])[0]
        entry_s_addr = struct.unpack("<Q", content[entry_offset + BIOS_TABLE_ENTRY_S_ADDRESS_OFFSET 
            : entry_offset + BIOS_TABLE_ENTRY_D_ADDRESS_OFFSET])[0]
        entry_ea_mode = entry_s_addr & 0b11
        entry_s_addr  &= 0xFFFFFFFFFFFFFFFC
        entry_d_addr = struct.unpack("<Q", content[entry_offset + BIOS_TABLE_ENTRY_D_ADDRESS_OFFSET 
            : entry_offset + BIOS_TABLE_ENTRY_D_ADDRESS_OFFSET + 8])[0]

        if entry_type == ROM_ARMOR_ENTRY_TYPE:            
            print(f"[{entry_type:02x}] - Size:[{entry_size:08x}] ({entry_size//1024:4}-KB) - |{entry_flags:02x}| S_ADDR:[{entry_s_addr:016x}] D_ADDR:[{entry_d_addr:016x}]")

        entry_offset += BIOS_TABLE_ENTRY_SIZE
        


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: \n$ python {sys.argv[0]} <spi_dump.bin>")
    else:
        main(sys.argv[1])

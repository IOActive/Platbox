from sys import argv
from os import path, mkdir, getcwd
from struct import unpack
import zlib


def extract_efs(rom):
    efs_magic = b"\xAA\x55\xAA\x55"
    
    efs_offsets = [
        0x020000,
        0x820000,
        0xC20000,
        0xE20000,
        0xF20000,
        0xFA0000,
    ]

    for offset in efs_offsets:
        if rom[offset:offset+4] == efs_magic:
            return rom[offset:offset+0x4B]

def parse_efs(efs):
    print("Parsing EFS")
    efs_magic = b"\xAA\x55\xAA\x55"

    if efs[0x0:0x4] != efs_magic:
        print(f"Invalid EFS magic {efs[0:4]}")
        exit()


    dir_offsets = []
    for offset in range(0x14, 0x30, 4):
        ptr = int.from_bytes(efs[offset:offset+4], "little")
        if ptr != 0x00000000 and ptr != 0xFFFFFFFE:
            dir_offsets.append(ptr)

    print(f"# EFS entries: {len(dir_offsets)}")
    for i in range(0, len(dir_offsets)):
        print(f"EFS entry {i}: {hex(dir_offsets[i])}")

    return dir_offsets


def parse_dir(rom, offset):
    magic = rom[offset:offset+4]

    if magic == b"$PSP":
        parse_psp_dir(rom, offset)
        pass
    elif magic == b"2PSP" or magic == b"2BHD":
        parse_psp_combodir(rom, offset)
        pass
    elif magic == b"$BHD":
        parse_bios_dir(rom, offset)
        pass
    else:
        print(f"Invalid directory magic {magic}")
        exit()


def parse_psp_dir(rom, offset):
    print("\nParsing PSP directory")
    print(f"Offset: {hex(offset)}")

    psp_hdr = rom[offset:offset+0x10]

    psp_magic = psp_hdr[0:4]
    if psp_magic != b"$PSP" and psp_magic != b"$PL2":
        print(f"Invalid magic {psp_magic} when parsing PSP directory")
        exit()

    psp_checksum = int.from_bytes(psp_hdr[0x4:0x8], "little")
    psp_num_entries = int.from_bytes(psp_hdr[0x8:0xC], "little")
    psp_add_info = psp_hdr[0xC:0x10]

    print(f"# PSP entries: {psp_num_entries}")

    if not path.exists("PSP"):
        mkdir("PSP")

    psp_entries = rom[offset+0x10:offset+0x10+0x10*psp_num_entries]
    for idx in range(0, psp_num_entries):
        entry = psp_entries[0x10*idx:0x10*(idx+1)]

        entry_type = entry[0]

        if entry_type == 0x0B:
            entry_fuse = int.from_bytes(entry[8:16], "little")
            print(f"PSP entry {idx}: Type: {hex(entry_type)} Fuse: {hex(entry_fuse)}")
            continue

        entry_subprogram = entry[1]
        entry_romid = entry[2] & 0x03
        entry_writable = entry[2] & 0x04
        entry_instancetype = entry[2] & 0x78
        entry_reserved = int.from_bytes(entry[2:3], "little") & 0x08FF
        entry_size = int.from_bytes(entry[4:8], "little")
        entry_location_addr = int.from_bytes(entry[8:16], "little") & 0x3FFFFFFFFFFFFFFF
        entry_location_mode = int.from_bytes(entry[8:16], "little") >> 62

        # physical address
        if entry_location_mode == 0x00:
            entry_location = entry_location_addr - 0xFF000000

        # offset from start of flash
        elif entry_location_mode == 0x01:
            entry_location = entry_location_addr

        # offset from start of directory header
        elif entry_location_mode == 0x02:
            entry_location = entry_location_addr + offset

        else:
            print(f"Unsupported location mode {entry_location_mode}")
            exit()

        # exception if entry is of L2A or L2B PSP directory type; interpret as offset
        if entry_type == 0x48 or entry_type == 0x4a:
            entry_location = entry_location_addr

        # print(f"PSP entry {idx}")
        # print(f"- Type: {hex(entry_type)}")
        # print(f"- Size: {hex(entry_size)}")
        # print(f"- Location: {hex(entry_location)}")
        print(f"PSP entry {idx}: Type: {hex(entry_type)} Size: {hex(entry_size)} Location: {hex(entry_location)}")

        filepath = path.join("PSP", "PSP_Type_0x{:02X}_Offset_0x{:02X}.bin".format(entry_type, entry_location))
        entry_payload = rom[entry_location:entry_location+entry_size]

        fd = open(filepath, "wb")
        fd.write(entry_payload)
        fd.close()

        # if entry is of L2A or L2B PSP directory type
        if entry_type == 0x48 or entry_type == 0x4a:
            parse_ish(rom, entry_location)

        # if entry is of L2 BIOS directory type
        if entry_type == 0x49:
            parse_bios_dir(rom, entry_location)
    pass


def parse_psp_combodir(rom, offset):
    print("\nParsing PSP combo directory")
    print(f"Offset: {hex(offset)}")

    psp_hdr = rom[offset:offset+0x20]

    psp_magic = psp_hdr[0:4]
    if psp_magic != b"2PSP" and psp_magic != b"2BHD":
        print(f"Invalid magic {psp_magic} when parsing PSP directory")
        exit()

    psp_checksum = int.from_bytes(psp_hdr[0x4:0x8], "little")
    psp_num_entries = int.from_bytes(psp_hdr[0x8:0xC], "little")
    psp_lookup_mode = psp_hdr[0xC:0x10]
    psp_reserved = psp_hdr[0x10:0x20]

    print(f"# PSP entries: {psp_num_entries}")

    # if not path.exists("PSP-Combodir"):
    #     mkdir("PSP-Combodir")

    psp_entries = rom[offset+0x20:offset+0x20+0x10*psp_num_entries]
    for idx in range(0, psp_num_entries):
        entry = psp_entries[0x10*idx:0x10*(idx+1)]

        entry_id_select = int.from_bytes(entry[0:4], "little")
        entry_id = int.from_bytes(entry[4:8], "little")
        entry_location = int.from_bytes(entry[8:16], "little")
        
        # print(f"PSP entry {idx}")
        # print(f"- Type: {hex(entry_type)}")
        # print(f"- Size: {hex(entry_size)}")
        # print(f"- Location: {hex(entry_location)}")
        print(f"PSP entry {idx}: ID Select: {hex(entry_id_select)} ID: {hex(entry_id)} Location: {hex(entry_location)}")

        parse_dir(rom, entry_location)

    pass

def parse_ish(rom, offset):
    print("\nParsing ISH structure")
    print(f"Offset: {hex(offset)}")

    ish = rom[offset:offset+0x20]

    ish_checksum = ish[0x0:0x4]
    ish_boot_priority = ish[0x4:0x8]
    ish_update_retry_cnt = ish[0x8:0xC]
    ish_glitch_retry_cnt = ish[0xC:0xD]
    ish_reserved = ish[0xD:0x10]
    ish_location_rom = int.from_bytes(ish[0x10:0x14], "little")
    ish_pspid = int.from_bytes(ish[0x14:0x18], "little")
    ish_maxslotsize = ish[0x18:0x1C]
    ish_reserved2 = ish[0x1C:0x20]

    # print(f"ISH entry:")
    # print(f"- Location: {hex(ish_location_rom)}")
    # print(f"- PSP ID: {hex(ish_pspid)}")
    print(f"ISH entry: Location: {hex(ish_location_rom)} PSP ID: {hex(ish_pspid)}")

    # print(f"Magic: {rom[ish_location_rom:ish_location_rom+4]}")

    parse_psp_dir(rom, ish_location_rom)


def parse_bios_dir(rom, offset):
    print("\nParsing BIOS directory")
    print(f"Offset: {hex(offset)}")

    bios_hdr = rom[offset:offset+0x10]

    bios_magic = bios_hdr[0x0:0x4]
    if bios_magic != b"$BHD" and bios_magic != b"$BL2":
        print(f"Invalid magic {bios_magic} when parsing BIOS directory")
        exit()

    bios_checksum = bios_hdr[0x4:0x8]
    bios_num_entries = int.from_bytes(bios_hdr[0x8:0xC], "little")
    bios_add_info = bios_hdr[0xC:0x10]

    bios_entries = rom[offset+0x10:offset+0x10+0x18*bios_num_entries]
    print(f"# BIOS entries {bios_num_entries}")

    if not path.exists("BHD"):
        mkdir("BHD")
    
    rom_armor_entries = []
    for idx in range(0, bios_num_entries):
        entry = bios_entries[0x18*idx:0x18*(idx+1)]

        entry_type = entry[0]
        entry_region_type = entry[1]

        entry_misc1 = entry[2]

        entry_subprogram = (entry[3] >> 0) & 0x07
        entry_romid = (entry[3] >> 3) & 0x03
        entry_writable = (entry[3] >> 5) & 0x01
        entry_reserved = (entry[3] >> 6) & 0x03

        entry_size = int.from_bytes(entry[4:8], "little")
        entry_location_addr = int.from_bytes(entry[8:16], "little") & 0x3FFFFFFFFFFFFFFF
        entry_location_mode = int.from_bytes(entry[8:16], "little") >> 62

        # physical address
        if entry_location_mode == 0x00:
            entry_location = entry_location_addr - 0xFF000000

        # offset from start of flash
        elif entry_location_mode == 0x01:
            entry_location = entry_location_addr

        # offset from start of directory header
        elif entry_location_mode == 0x02:
            entry_location = entry_location_addr + offset

        else:
            print(f"Unsupported location mode {entry_location_mode}")
            exit()

        # print(f"BIOS entry {idx}")
        # print(f"- Type: {hex(entry_type)}")
        # print(f"- Writable: {hex(entry_writable)}")
        # print(f"- Size: {hex(entry_size)}")
        # print(f"- Location: {hex(entry_location)}")
        print(f"BIOS entry {idx}: Type: {hex(entry_type)} Size: {hex(entry_size)} Location: {hex(entry_location)}")

        if entry_type == 0x70:
            parse_bios_dir(rom, entry_location)
            continue

        filepath = path.join("BHD", "BHD_Type_0x{:02X}_Offset_0x{:02X}.bin".format(entry_type, entry_location))
        entry_payload = rom[entry_location:entry_location+entry_size]

        fd = open(filepath, "wb")
        fd.write(entry_payload)
        fd.close()

        zlib_magic = b"\x78\xDA"
        zlib_search_offset = entry_payload.find(zlib_magic)
        
        if zlib_search_offset >= 0 and zlib_search_offset <= 512:
            # print("Found ZLIB compressed data")
            try:
                entry_payload_decompressed = zlib.decompress(entry_payload[zlib_search_offset:])
                filepah = path.join("BHD", "BHD_Type_0x{:02X}_Offset_0x{:02X}_decompressed.bin".format(entry_type, entry_location))
                fd = open(filepah, "wb")
                fd.write(entry_payload_decompressed)
                fd.close()
            except:
                print("Failed to decompress zlib")

        # if entry type is used for ROM armor
        if entry_type == 0x6d:
            rom_armor_entries.append((entry_location, entry_size, entry_writable))

    if len(rom_armor_entries) != 0:
        parse_romarmor(rom_armor_entries)

    pass

def parse_romarmor(rom_armor_regions):
    print(f"\nParsing {len(rom_armor_regions)} ROM Armor entries")

    for entry in rom_armor_regions:
        (location, size, writable) = entry
        print(f"Address: {hex(location)} Size: {hex(size)} Writable: {hex(writable)}")
    pass


# Check parameters
if len(argv) != 2:
    print(f"Usage: {path.basename(argv[0])} <spi_dump>")
    exit()

# Check if file is valid
file_path = argv[1]

if not path.isfile(file_path):
    print(f"No file found at {file_path}")
    exit()

# Read file
rom = open(file_path, "rb").read()

# Parse FET
efs = extract_efs(rom)
dir_offsets = parse_efs(efs)

for offset in dir_offsets:
    parse_dir(rom, offset)


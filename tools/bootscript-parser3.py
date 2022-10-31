import sys
from struct import unpack

# Boot script table opcode
FRAMEWORK_EFI_BOOT_SCRIPT_TABLE_OPCODE         = 0xAA

# Boot script opcodes
EFI_BOOT_SCRIPT_IO_WRITE_OPCODE                = 0x00
EFI_BOOT_SCRIPT_IO_READ_WRITE_OPCODE           = 0x01
EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE               = 0x02
EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE          = 0x03
EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE        = 0x04
EFI_BOOT_SCRIPT_PCI_CONFIG_READ_WRITE_OPCODE   = 0x05
EFI_BOOT_SCRIPT_SMBUS_EXECUTE_OPCODE           = 0x06
EFI_BOOT_SCRIPT_STALL_OPCODE                   = 0x07
EFI_BOOT_SCRIPT_DISPATCH_OPCODE                = 0x08
EFI_BOOT_SCRIPT_DISPATCH_2_OPCODE              = 0x09
EFI_BOOT_SCRIPT_INFORMATION_OPCODE             = 0x0A
EFI_BOOT_SCRIPT_PCI_CONFIG2_WRITE_OPCODE       = 0x0B
EFI_BOOT_SCRIPT_PCI_CONFIG2_READ_WRITE_OPCODE  = 0x0C
EFI_BOOT_SCRIPT_IO_POLL_OPCODE                 = 0x0D
EFI_BOOT_SCRIPT_MEM_POLL_OPCODE                = 0x0E
EFI_BOOT_SCRIPT_PCI_CONFIG_POLL_OPCODE         = 0x0F
EFI_BOOT_SCRIPT_PCI_CONFIG2_POLL_OPCODE        = 0x10

FRAMEWORK_EFI_BOOT_SCRIPT_TERMINATE_OPCODE	   = 0xFF


def read_table_header(data):
	opcode, hdrLen, version, bsLen, _ = unpack('=HBHII', data[:13])

	if opcode == FRAMEWORK_EFI_BOOT_SCRIPT_TABLE_OPCODE:
		print("Bootscript table opcode identified")
		print(f"- Header length: {hdrLen}")
		print(f"- Version: {version}")
		print(f"- Bootscript length: {bsLen}")

	return hdrLen, bsLen

def read_opcode_header(data):
	opcode, length = unpack('=HB', data[:3])

	if opcode == EFI_BOOT_SCRIPT_IO_WRITE_OPCODE:
		print("[*] IO write")
		width, count, address = unpack('=IIQ', data[3:19])
		print(f"- Width: {width}")
		print(f"- Count: {count}")
		print(f"- Address: {hex(address)}")

	elif opcode == EFI_BOOT_SCRIPT_IO_READ_WRITE_OPCODE:
		print("[*] IO read")
		width, address = unpack('=IQ', data[3:15])
		print(f"- Width: {width}")
		print(f"- Address: {hex(address)}")

	elif opcode == EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE:
		print("[*] Mem write")
		width, count, address = unpack('=IIQ', data[3:19])
		print(f"- Width: {width}")
		print(f"- Count: {count}")
		print(f"- Address: {hex(address)}")

	elif opcode == EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE:
		print("[*] Mem read")
		width, address = unpack('=IQ', data[3:15])
		print(f"- Width: {width}")
		print(f"- Address: {hex(address)}")

	elif opcode == EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE:
		print("[*] PCI config write")
		width, count, address = unpack('=IIQ', data[3:19])
		bus, dev, func, offset = (address >> 24) & 0xFF, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF
		print(f"- Width: {width}")
		print(f"- Address: ({bus}, {dev}, {func}, {offset})")

	elif opcode == EFI_BOOT_SCRIPT_PCI_CONFIG_READ_WRITE_OPCODE:
		print("[*] PCI config read")
		width, address = unpack('=IQ', data[3:15])
		bus, dev, func, offset = (address >> 24) & 0xFF, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF
		print(f"- Width: {width}")
		print(f"- Address: ({bus}, {dev}, {func}, {offset})")

	elif opcode == EFI_BOOT_SCRIPT_SMBUS_EXECUTE_OPCODE:
		print("[*] SMBus execute")
		address, operation, dataSize = unpack('=QII', data[3:19])
		print(f"- Address: {hex(address)}")
		print(f"- Operation: {operation}")
		print(f"- Data size: {dataSize}")

	elif opcode == EFI_BOOT_SCRIPT_STALL_OPCODE:
		print("[*] Stall")

	elif opcode == EFI_BOOT_SCRIPT_DISPATCH_OPCODE:
		print("[!] Dispatch")
		entrypoint = unpack('=Q', data[3:])
		print(f"- Entrypoint {hex(entrypoint)}")

	elif opcode == EFI_BOOT_SCRIPT_DISPATCH_2_OPCODE:
		print("[!] Dispatch2")
		entrypoint, context = unpack('=QQ', data[3:])
		print(f"- Entrypoint {hex(entrypoint)}")
		print(f"- Context {hex(context)}")

	elif opcode == EFI_BOOT_SCRIPT_INFORMATION_OPCODE:
		print("[*] Info")

	elif opcode == EFI_BOOT_SCRIPT_PCI_CONFIG2_WRITE_OPCODE:
		print("[*] PCI config2 write")
		width, address = unpack('=IQ', data[3:13])
		print(f"- Width: {width}")
		print(f"- Address: {address}")

	elif opcode == EFI_BOOT_SCRIPT_PCI_CONFIG2_READ_WRITE_OPCODE:
		print("[*] PCI config2 read write")
		width, address, segment = unpack('=IQH', data[3:17])
		print(f"- Width: {width}")
		print(f"- Address: {address}")
		print(f"- Segment: {segment}")

	elif opcode == EFI_BOOT_SCRIPT_IO_POLL_OPCODE:
		print("[*] IO poll")
		width, address, delay = unpack('=IQQ', data[3:23])
		print(f"- Width: {width}")
		print(f"- Address: {hex(address)}")
		print(f"- Delay: {delay}")

	elif opcode == EFI_BOOT_SCRIPT_MEM_POLL_OPCODE:
		print("[*] Mem poll")
		width, address, duration, loopTimes = unpack('=IQQQ', data[3:31])
		print(f"- Width: {width}")
		print(f"- Address: {hex(address)}")
		print(f"- Duration: {duration}")
		print(f"- Loop times: {loopTimes}")

	elif opcode == EFI_BOOT_SCRIPT_PCI_CONFIG_POLL_OPCODE:
		print("[*] PCI config poll")
		width, address, delay = unpack('=IQQ', data[3:23])
		print(f"- Width: {width}")
		print(f"- Address: {address}")
		print(f"- Delay: {delay}")

	elif opcode == EFI_BOOT_SCRIPT_PCI_CONFIG2_POLL_OPCODE:
		print("[*] PCI config2 poll")
		width, address, segment, delay = unpack('=IQHQ', data[3:25])
		print(f"- Width: {width}")
		print(f"- Address: {address}")
		print(f"- Segment: {segment}")
		print(f"- Delay: {delay}")

	elif opcode == FRAMEWORK_EFI_BOOT_SCRIPT_TERMINATE_OPCODE:
		print("[*] Terminate")

	return length


def parse(script):
	hdrLen, bsLen = read_table_header(script)
	script = script[hdrLen:bsLen]

	while True:
		opcodeLen = read_opcode_header(script)
		script = script[opcodeLen:]

		if len(script) == 0:
			break


if len(sys.argv) != 2:
	print("Usage: {sys.argv[0]} <bootscript-file>")
	exit()

script = open(sys.argv[1], "rb").read()
parse(script)
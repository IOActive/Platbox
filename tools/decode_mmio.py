import sys


MMIO_BASE = 0xF0000000

def decode_device(addr):
    function = (addr >> 12) & 0x7
    device   = (addr >> 15) & 0x1f
    bus      = (addr >> 20) & 0xff
    offset   = addr & 0xFFF
    print(f"B:D:F -> {bus:02x}:{device:02x}:{function:02x} [{offset:08x}] ")


if __name__ == "__main__":
    addr = int(sys.argv[1], 16)
    decode_device(addr)
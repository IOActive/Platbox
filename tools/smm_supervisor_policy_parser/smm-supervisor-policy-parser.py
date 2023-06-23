#!/usr/bin/python
import os
import sys
import struct
import subprocess
from shutil import which, rmtree
from policy_entry import *
import argparse

POLICY_GUIDS = [
    "83E1F409-21A3-491D-A415-B163A153776D"  # SmmSupvBin
]

def get_uefitool_path():
    path = which("uefiextract")
    if not path:
        path = which("UEFIextract")
    
    return path

def extract_files(rom, guid):
    subprocess.call([get_uefitool_path(), rom, guid, '-o', 'policy_tmp'], 
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def get_policy_bin(guid):
    for root, dirs, files in os.walk("./policy_tmp"):
        path = root.split(os.sep)
        for file in files:
            if file == "info.txt":
                with open(os.path.join(root, file), 'r') as fp:
                    for line in fp.readlines():
                        if guid in line:
                            policy_path = os.path.join(root, '0 Raw section/body.bin')
                            return open(f"{policy_path}", "rb").read()    
    return None

def parse_policy(data):
    # Example container header:
    # 24 50 53 50 BB 61 AD DE 05 00 00 00 10 04 00 20   $PSP.a......... 
    # C0 00 00 00 00 02 00 00 00 04 00 00 00 00 00 00   ................
    # C1 00 00 00 A4 02 00 00 00 06 00 00 00 00 00 00   ................
    # C2 00 00 00 00 02 00 00 00 09 00 00 00 00 00 00   ................
    # C3 00 00 00 40 04 00 00 00 0B 00 00 00 00 00 00   ....@...........
    # C4 00 00 00 F8 00 00 00 00 10 00 00 00 00 00 00   ................
    if data[80] != 0xc4:
        print("Not a valid PSP header, treating as raw binary policy\n")
        policy = data
        version = struct.unpack('<L', policy[0:4])[0]
        Policy = Supervisor_Policy(version) 
        Policy.Decode(policy)
        Policy.DumpInfo(prefix="  ")
        return
    
    size = struct.unpack('<I', data[84:88])[0]
    offset = struct.unpack('<I', data[88:92])[0]
    #print(f"Size: 0x{size:x} Offset: 0x{offset:x}")
    policy = data[offset:offset+size]

    # Example policy:
    # 00 00 01 00 F8 00 00 00 00 00 00 00 00 00 00 00   ................
    # 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
    # 28 00 00 00 04 00 00 00 01 00 00 00 18 00 00 00   (...............
    # 02 00 00 00 88 00 00 00 02 00 00 00 01 00 00 00   ................
    # 01 00 00 00 18 00 00 00 03 00 00 00 98 00 00 00   ................
    # 09 00 00 00 01 00 00 00 01 00 00 00 18 00 00 00   ................
    # 04 00 00 00 E0 00 00 00 03 00 00 00 00 00 00 00   ................
    # 01 00 00 00 18 00 00 00 01 00 00 00 F8 00 00 00   ................
    # 00 00 00 00 01 00 00 00 F8 0C 04 00 0A 00 00 00   ................
    # FC 0C 04 00 02 00 00 00 80 00 00 C0 01 00 07 00   ................
    # 81 00 00 C0 04 00 07 00 10 00 01 C0 01 00 07 00   ................
    # 11 01 01 C0 01 00 07 00 D9 01 00 00 01 00 07 00   ................
    # A0 0D 00 00 01 00 07 00 A0 06 00 00 01 00 07 00   ................
    # A2 06 00 00 01 00 07 00 A4 06 00 00 05 00 07 00   ................
    # 00 00 04 00 00 00 00 00 01 00 04 00 00 00 00 00   ................
    # 02 00 04 00 00 00 00 00                           ........
    version = struct.unpack('<L', policy[0:4])[0]
    Policy = Supervisor_Policy(version) 
    Policy.Decode(policy)
    Policy.DumpInfo(prefix="  ")

def cleanup():
    rmtree("./policy_tmp")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='SMM Supervisor Policy Binary Parser')

    group = parser.add_mutually_exclusive_group(required=True)

    group.add_argument('--binary', metavar='PATH', help='Parse a policy binary (Raw SmmSupvBin / Raw Policy Binary)')
    group.add_argument('--uefi', metavar='PATH', help='Parse a policy from a UEFI container')

    args = parser.parse_args()

    if args.uefi:
        if not get_uefitool_path():
            print("uefiextract/UEFIextract needs to be in your $PATH")
            sys.exit(1)
        
        for guid in POLICY_GUIDS:
            extract_files(args.uefi, guid)
            pbin = get_policy_bin(guid)
            if not pbin:
                print("Failed to extract policy binary from image.")
                print(f"Consider manually checking container for GUID: {POLICY_GUIDS[0]}")
                cleanup()
            else:
                parse_policy(pbin)
                cleanup()

    if args.binary:
        binary = open(args.binary, 'rb').read()
        parse_policy(binary)

    if not args.binary and not args.uefi:
        parser.print_help()

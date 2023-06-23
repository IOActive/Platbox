import sys
import struct
from ctypes_utils import *
from ctypes import sizeof
import os
import hexdump
import re
import shutil
import time

"""
Based on https://github.com/Cr4sh/smram_parse

Using https://github.com/theopolis/uefi-firmware-parser/tree/master/uefi_firmware/guids
"""

EFIUTILS_PATH = f"{os.getcwd()}/efiguids"
EFIUTILS_URL = 'https://github.com/theopolis/uefi-firmware-parser'
sys.path.append(EFIUTILS_PATH)


UEFIEXTRACT_PATH = './UEFIExtract'
UEFIEXTRACT_URL = 'https://github.com/LongSoft/UEFITool/releases'



Additional_GUIDs = {
    'EDKII_S3_SMM_INIT_DONE_GUID':                             [0x8f9d4825, 0x797d, 0x48fc, 0x84, 0x71, 0x84, 0x50, 0x25, 0x79, 0x2e, 0xf6],
    'EDKII_SMM_END_OF_S3_RESUME_PROTOCOL_GUID':                [0x96f5296d, 0x05f7, 0x4f3c, 0x84, 0x67, 0xe4, 0x56, 0x89, 0x0e, 0x0c, 0xb5],
    'SMM_CPU_SYNC_PROTOCOL_GUID':                              [0xd5950985, 0x8be3, 0x4b1c, 0xb6, 0x3f, 0x95, 0xd1, 0x5a, 0xb3, 0xb6, 0x5f],
    'SMM_CPU_SYNC2_PROTOCOL_GUID':                             [0x9db72e22, 0x9262, 0x4a18, 0x8f, 0xe0, 0x85, 0xe0, 0x3d, 0xfa, 0x96, 0x73],
    'EFI_SMM_CPU_SERVICE_PROTOCOL_GUID':                       [0x1d202cab, 0xc8ab, 0x4d5c, 0x94, 0xf7, 0x3c, 0xfc, 0xc0, 0xd3, 0xd3, 0x35],
    'PCH_SPI_PROTOCOL':                                        [0x56521f06, 0x0a62, 0x4822, 0x99, 0x63, 0xdf, 0x01, 0x9d, 0x72, 0xc7, 0xe1],
    'PCH_TCO_SMI_DISPATCH_PROTOCOL_GUID':                      [0x9e71d609, 0x6d24, 0x47fd, 0xb5, 0x72, 0x61, 0x40, 0xf8, 0xd9, 0xc2, 0xa4],
    'PCH_PCIE_SMI_DISPATCH_PROTOCOL_GUID':                     [0x3e7d2b56, 0x3f47, 0x42aa, 0x8f, 0x6b, 0x22, 0xf5, 0x19, 0x81, 0x8d, 0xab],
    'PCH_ACPI_SMI_DISPATCH_PROTOCOL_GUID':                     [0xd52bb262, 0xf022, 0x49ec, 0x86, 0xd2, 0x7a, 0x29, 0x3a, 0x7a, 0x05, 0x4b],
    'PCH_GPIO_UNLOCK_SMI_DISPATCH_PROTOCOL_GUID':              [0x83339ef7, 0x9392, 0x4716, 0x8d, 0x3a, 0xd1, 0xfc, 0x67, 0xcd, 0x55, 0xdb],
    'PCH_SMM_IO_TRAP_CONTROL_PROTOCOL':                        [0x514d2afd, 0x2096, 0x4283, 0x9d, 0xa6, 0x70, 0x0c, 0xd2, 0x7d, 0xc7, 0xa5],
    'PCH_ESPI_SMI_DISPATCH_PROTOCOL_GUID':                     [0xB3C14FF3, 0xBAE8, 0x456C, 0x86, 0x31, 0x27, 0xFE, 0x0C, 0xEB, 0x34, 0x0C],
    'EFI_ACPI_EN_DISPATCH_PROTOCOL_GUID':                      [0xbd88ec68, 0xebe4, 0x4f7b, 0x93, 0x5a, 0x4f, 0x66, 0x66, 0x42, 0xe7, 0x5f],
    'EFI_EC_ACCESS_PROTOCOL_GUID':                             [0x70eeecbe, 0x727a, 0x4244, 0x90, 0x4c, 0xdb, 0x6b, 0xf0, 0x05, 0x53, 0x92],
    'EFI_ACPI_EN_DISPATCH_PROTOCOL_GUID':                      [0xbd88ec68, 0xebe4, 0x4f7b, 0x93, 0x5a, 0x4f, 0x66, 0x66, 0x42, 0xe7, 0x5f],
    'EFI_ACPI_DIS_DISPATCH_PROTOCOL_GUID':                     [0x9c939ba6, 0x1fcc, 0x46f6, 0xb4, 0xe1, 0x10, 0x2d, 0xbe, 0x18, 0x65, 0x67],
    'FchSmmDiagInterfaceGUID':                            [0x89fef87c, 0xcc96, 0x4d20, 0xa4, 0x62, 0xcd, 0x11, 0x92, 0x75, 0xa3, 0x89],
    'AmdPspP2CmboxV2SmmBuffer_GUID':                      [0xaf62673d, 0xdeda, 0x43ae, 0x8f, 0x3,  0x9c, 0x2d, 0x89, 0xfd, 0x78, 0xda],
    'gFchSmmSwDispatch2Protocol_GUID':                    [0x881b4ab6,0x17b0,0x4bdf,0x88,0xe2,0xd4,0x29,0xda,0x42,0x5f,0xfd],
    'gAmdPspResumeServiceProtocol_GUID':                  [0x6c5ae0f9,0xaad3,0x47f8,0x8f,0x59,0xa5,0x3a,0x54,0xce,0x5a,0xe2],
    'AmdCpmInitSmm_GUID':                                 [0xaf6efacf,0x7a13,0x45a3,0xb1,0xa5,0xaa,0xfc,0x6,0x1c,0x4b,0x79],
    'AmdSmmCommunication_AmdS3SaveSmmCommunicateHandler_GUID': [0x5c4061f0,0xd051,0x4f43,0xbc,0x23,0x5e,0x5e,0x89,0xb3,0xce,0xc0],
    'AmdFabricRnSmm_GUID':                                [0x956a4d54,0xfaab,0x4916,0xac,0x99,0x3d,0x5d,0x9a,0xd0,0xd7,0xc0],
    'AmdPspRomArmorSmmInterfaceProtocol_GUID':            [0x4a9c5084,0xc8af,0x403c,0x99,0x31,0x63,0xc9,0x36,0x90,0xec,0xb3],    
    'H19SgxSmmInterface_GUID':                                 [0x468907b3,0x174b,0x4617,0xa0,0x23,0x8f,0x7c,0xe6,0x0,0x36,0x48],
    'H19PlatformServiceSmmInterface_GUID':                     [0x5a3a840d,0x2432,0x4a59,0xa9,0xef,0x6d,0xa0,0x95,0xe,0x37,0x54],
    'AutoPowerOnSmmInterface_GUID':                            [0x5a72a74f,0x4842,0x44d8,0x94,0xd8,0xa3,0x0e,0x5,0xa9,0x85,0x4e],
    'H19HddPasswordInterface_GUID':                            [0xafa92c3,0x9d71,0x47cb,0x89,0x24,0x21,0xb6,0xc2,0x45,0x7c,0x0],
    'H19MeUpdateSmmInterface_GUID':                            [0x1e686608,0x4a96,0x49f8,0xa7,0xd2,0x43,0xc5,0x6,0x92,0x83,0x55],
    'H19ServiceBodySmmInterface_GUID':                         [0x9156c0ad,0x57ca,0x42c7,0xbc,0xd3,0xe5,0xd8,0x2d,0xe0,0x5f,0x34],
    'BiosGuardServicesInterface_GUID':                         [0x17565311,0x4b71,0x4340,0x88,0xaa,0xdc,0x9f,0x44,0x22,0xe5,0x3a],
    'OFCSmmDriverInterface_GUID':                              [0xa618df46,0x1b05,0x463e,0xa6,0xfd,0xf4,0x3b,0x9b,0xfb,0x27,0xcc],
    'H19RecordLogSmmInterface_GUID':                           [0x25bbabe3,0xf185,0x461b,0x80,0xc0,0x58,0x3d,0xb8,0xc0,0xa4,0x41],
    'AspmOverrideDxeInterface_GUID':                           [0x4cd150e9,0x16b,0x4e54,0xbd,0x0,0x5c,0x86,0xeb,0xe6,0x37,0xd6],
    'EFI_LEGACY_BIOS_PROTOCOL_GUID':                           [0xdb9a1e3d,0x45cb,0x4abb,0x85,0x3b,0xe5,0x38,0x7f,0xdb,0x2e,0x2d],
    'IhisiServicesSmmInterface_GUID':                          [0x6c23a1ef,0x2cb7,0x4a60,0x8f,0x8c,0x8,0xa3,0xde,0x8d,0x7a,0xcf],
    'Int15ServiceSmmInterface_GUID':                           [0x1fa493a8,0xb360,0x4205,0xb8,0xfe,0xcc,0x83,0xbc,0x57,0xb7,0x3a],
    'PchSmiDispatcherInterface1_GUID':                         [0x6906e93b,0x603b,0x4a0f,0x86,0x92,0x83,0x20,0x4,0xaa,0xf2,0xdb],
    'EFI_MM_IO_TRAP_DISPATCH_PROTOCOL':                         [0x58dc368d,0x7bfa,0x4e77,0xab,0xbc,0xe,0x29,0x41,0x8d,0xf9,0x30],
}


EFI_SMM_SYSTEM_TABLE_SIGNATURE = b'SMST\x00\x00\x00\x00'
SMI_ENTRY_SIGNATURE            = b'smie\x00\x00\x00\x00'
SMI_HANDLER_SIGNATURE          = b'smih\x00\x00\x00\x00'

SMM_POOL_HEADER_SIGNATURE      = b'sphd'
SMM_POOL_TAIL_SIGNATURE        = b'sptl'
POOL_HEADER_SIZE = 0x18
POOL_HEADER_SIZE_MEMBER_OFFSET = 0x10
POOL_TAIL_SIZE   = 0x10

#PROTOCOL_ENTRY_SIGNATURE2      = b'prte'
PROTOCOL_NOTIFY_SIGNATURE      = b'prtn'
OPEN_PROTOCOL_DATA_SIGNATURE   = b'podl'

PROTOCOL_ENTRY_SIGNATURE       = b'spte'
EFI_HANDLE_SIGNATURE           = b'shdl'
EFI_SMM_DRIVER_ENTRY_SIGNATURE = b'sdrv'
PROTOCOL_INTERFACE_SIGNATURE   = b'spif'
PROTOCOL_NOTIFY_SIGNATURE      = b'sptn'
SMM_LOCK_BOX_DATA_SIGNATURE    = b'LOCKBOXD'


# PE format constants
HEADERS_SIZE                                    = 0x400
IMAGE_DOS_HEADER_e_lfanew                       = 0x3c
IMAGE_NT_HEADERS64_OptionalHeader               = 0x18
IMAGE_OPTIONAL_HEADER64_SizeOfCode              = 0x04
IMAGE_OPTIONAL_HEADER64_SizeOfInitializedData   = 0x08
IMAGE_OPTIONAL_HEADER64_AddressOfEntryPoint     = 0x10
IMAGE_OPTIONAL_HEADER64_SizeOfImage             = 0x38


################################################
################################################
##### Globals 

g_smram_data = None
g_tseg_base  = 0
g_tseg_end   = 0
guid_db      = None

g_smi_entries  = []
g_smi_handlers = set()
g_smm_heap_blocks = [] # array of tuples ( address, block_size, data[] )
g_fw = None
g_smm_modules = [] # array of dictionaries

################################################
################################################
################################################

def tseg_size():
    global g_tseg_base
    global g_tseg_end
    return g_tseg_end - g_tseg_base + 1

def is_in_tseg(v):
    global g_tseg_base
    global g_tseg_end
    if v >= g_tseg_base and v <= g_tseg_end:
        return True
    return False    

def add_base(v):
    global g_tseg_base
    return v + g_tseg_base

def offset_rel(v):
    global g_tseg_base
    return v - g_tseg_base

def read_ptr_at_offset(offset):    
    return offset_rel(struct.unpack("<Q", g_smram_data[offset:offset+8])[0])


def usage():
    print(f"{sys.argv[0]} smram.bin <tseg_base> <tseg_end> <bios_img.bin (opt)>")
    print(f"-> Example: {sys.argv[0]} smram.bin 0xAE000000 0xAEFFFFFF bios.bin")


def load_smram_dump(filepath):
    global g_smram_data
    f = open(filepath, "rb")
    g_smram_data = bytearray(f.read())
    f.close()


guid_parse = lambda data: struct.unpack('=IHHBBBBBBBB', data)

def guid_to_str(guid):
    if type(guid) is tuple:
        res = '%.8X-%.4X-%.4X-%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X' % guid
    else:
        if (len(guid) != 16):
            print("guid_to_str parameter error")
            return None
        data1 = struct.unpack("<L", guid[0:4])[0]
        data2 = struct.unpack("<H", guid[4:6])[0]
        data3 = struct.unpack("<H", guid[6:8])[0]
        res = "%08X-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x" % (data1, data2, data3,
                guid[8], guid[9], guid[10], guid[11],
                guid[12], guid[13], guid[14], guid[15])
    return res.upper()


################################################
################################################


class FwImage(object):

    def __init__(self, path = None):

        self.data = {}

        if path is not None:

            if os.path.isdir(path):

                self.data = self.scan_dir(path)

            elif os.path.isfile(path):

                self.data = self.scan_file(path)

    def scan_dir(self, path):

        ret = {}

        for fname in os.listdir(path):

            # check for PE image file
            m = re.match('Section_PE32_image_\w{8}-\w{4}-\w{4}-\w{4}-\w{12}_(\w+)_body.bin', fname)
            if m:                
                image_name = m.group(1).strip()
                if len(image_name) == 0: continue

                # read file contents
                fpath = os.path.join(path, fname)
                fdata = open(fpath, 'rb').read(HEADERS_SIZE)

                # use image info as key and image name as value
                ret.update([( self.image_info(fdata), image_name )])

        return ret


    def scan_file(self, path):

        dir_name = os.path.dirname(path)
        temp_name = 'fw_image_%d' % int(time.time())
        temp_path = os.path.join(dir_name, temp_name)

        print('[+] Copying "%s" to "%s"...' % (path, temp_path))

        # copy firmware image to temporary file
        shutil.copyfile(path, temp_path)

        print('[+] Unpacking "%s"...\n' % temp_path)

        # extract image contents
        code = os.system('"%s" "%s" unpack' % (UEFIEXTRACT_PATH, temp_path))
        if code != 0:

            print('WARNING: Error while running %s' % UEFIEXTRACT_PATH)
            print('You can download UEFIExtract here: %s\n' % UEFIEXTRACT_URL)
            return {}

        else:

            print('')

        # scan contents for PE files
        dump_path = temp_path + '.dump'
        report_path = temp_path + '.report.txt'
        ret = self.scan_dir(dump_path)

        # cleanup
        shutil.rmtree(dump_path)
        os.unlink(temp_path)
        os.unlink(report_path)

        return ret

    def image_info(self, data):

        # read e_lfanew field
        offset = struct.unpack('I', data[IMAGE_DOS_HEADER_e_lfanew : \
                                  IMAGE_DOS_HEADER_e_lfanew + 4])[0]

        offset += IMAGE_NT_HEADERS64_OptionalHeader
                  
        get_field = lambda field: \
                    struct.unpack('I', data[offset + field : offset + field + 4])[0]    

        # read optional header fields and use them to identify an image
        return ( get_field(IMAGE_OPTIONAL_HEADER64_SizeOfCode),
                 get_field(IMAGE_OPTIONAL_HEADER64_SizeOfInitializedData),
                 get_field(IMAGE_OPTIONAL_HEADER64_AddressOfEntryPoint),
                 get_field(IMAGE_OPTIONAL_HEADER64_SizeOfImage) )

    def image_name(self, data):
        
        try: 

            # find image name by image info
            return self.data[self.image_info(data)]

        except KeyError: 

            return None


class EFI_GUID(PrintingStructure):
    _fields_ = [
		#("Data1", u32),
		#("Data2", u16),
        #("Data3", u16),
        #("Data4", u8*8),
        ("guid", u8*16)
	]

class LIST_ENTRY(PrintingStructure):
    _fields_ = [
		("flink", u64),
		("blink", u64)		
	]

class SmiEntryStruct(PrintingStructure):
    _fields_ = [
		("Signature", u64),
		("AllEntries", LIST_ENTRY),
		("HandlerType", EFI_GUID),
		("SmiHandlers", LIST_ENTRY)        
	]

class SmiEntry:

    def __init__(self, address):        
        self.obj = None
        self.address = address
        self.smi_handlers = []

    @classmethod
    def from_data(cls, address, data):
        instance = cls(address)
        instance.obj = SmiEntryStruct.from_buffer(data)
        return instance

    def __eq__(self, obj):
        return obj.address == self.address

    def __hash__(self):
        return hash((type(self), self.address))

    def handler_type_name(self):
        v = bytearray(self.obj.HandlerType.guid)
        name = guid_db.lookup(v)
        if name is None:
            return guid_to_str(v)
        return name

    def smie_flink(self):
        return offset_rel(self.obj.AllEntries.flink)
    
    def smie_blink(self):
        return offset_rel(self.obj.AllEntries.blink)

    def smih_flink(self):
        return offset_rel(self.obj.SmiHandlers.flink)
    
    def smih_blink(self):
        return offset_rel(self.obj.SmiHandlers.blink)

    @staticmethod
    def process_smi_handlers(smie_obj):
        """
        Walks the SmiHandlers flink and appends a SmiHandler record
        """
        assert(type(smie_obj) is SmiEntry)

        head = offset_rel(smie_obj.address + 5 * 8)
        cur  = smie_obj.smih_flink()
        #last = smie_obj.smih_blink()
        while cur != head:
            
            smih_r = cur - 0x8 # adjust to signature
            if g_smram_data[smih_r:smih_r+8] != SMI_HANDLER_SIGNATURE:            
                raise Exception("Broken smih record?")   
            
            smi_handler = SmiHandler.from_data(add_base(smih_r), g_smram_data[smih_r:]) 
            #print("new smih appended")

            ## append to global as well
            g_smi_handlers.add(smi_handler) 
            smie_obj.smi_handlers.append(smi_handler)

            cur = smi_handler.flink()
            #print(f"{cur:08x} - {head:08x}")



class SmiHandlerStruct(PrintingStructure):
    _fields_ = [
		("Signature", u64),
		("Link", LIST_ENTRY),
		("Handler", u64),
        ("CallerAddr", u64),
		("SmiEntry", u64),
        ("Context", u64),
        ("ContextSize", u64)
    ]

class SmiHandler:

    def __init__(self, address):
        self.obj         = None
        self.address     = address
        self.module_name = ""

    def set_module_name(self, name):
        self.module_name = name

    @classmethod
    def from_data(cls, address, data):
        instance = cls(address)
        instance.obj = SmiHandlerStruct.from_buffer(data)
        return instance

    def __eq__(self, obj):
        return obj.address == self.address

    def __hash__(self):
        return hash((type(self), self.address))

    def flink(self):
        return offset_rel(self.obj.Link.flink)
    
    def blink(self):
        return offset_rel(self.obj.Link.blink)

    def handler(self):
        return self.obj.Handler

    def __str__(self):
        val = f" - SMIH - Handler at {self.obj.Handler:08x} - [{self.module_name}]"
        return val


"""
typedef struct _FchSwSmiRecord { /*size 0x28 */
  /* 0x00 */  void *pDispatchHandle;
  /* 0x08 */  UINT64 SwSmiInputValue;
  /* 0x10 */  UINT64 Unk;
  /* 0x18 */  EFI_SMM_SW_DISPATCH handler;
  /* 0x20 */ struct FchSwSmiRecord *next;
} FchSwSmiRecord;
"""

class FchSwSmiRecordStruct(PrintingStructure):

    _fields_ = [
		("pDispatchHandle", u64),
		("SwSmiInputValue", u64),
        ("Unk", u64),
		("handler", u64),
        ("next_record", u64),
    ]

class FchSwSmiRecord:

    def __init__(self, address):
        self.obj = None
        self.address = address
        self.module_name = ""

    def set_module_name(self, name):
        self.module_name = name

    @classmethod
    def from_data(cls, address, data):
        instance = cls(address)
        instance.obj = FchSwSmiRecordStruct.from_buffer(data)
        if not is_in_tseg(instance.obj.pDispatchHandle):
            return None
        if instance.obj.SwSmiInputValue > 0xff or instance.obj.SwSmiInputValue < 0:
            return None
        if not is_in_tseg(instance.obj.handler):
            return None
        if not is_in_tseg(instance.obj.next_record):
            return None

        return instance

    def print_detail(self):
        val =  f"FchSwSmiRecord at {self.address:08x} ({add_base(self.address):08x})\n"
        val += f" - pDispatchHandle: {offset_rel(self.obj.pDispatchHandle):08x} ({self.obj.pDispatchHandle:08x})\n"
        val += f" - SwSmiInputValue: {self.obj.SwSmiInputValue:08x}\n"
        val += f" - handler: {self.obj.handler:08x}\n"
        val += f" - next_record: {self.obj.next_record:08x} ({self.next_record():08x})\n"
        print(val)


    def __str__(self):
        val =  f"FchSwSmiRecord at {self.address:08x} - {self.obj.SwSmiInputValue:02x} - handler: {self.obj.handler:08x} [{self.module_name}]"
        return val

    def next_record(self):
        return offset_rel(self.obj.next_record)

    def handler(self):
        return self.obj.handler





class GuidDb:

    def __init__(self, guids = None):

        # load
        #   efiguids_ami.py  
        #   efiguids_asrock.py
        #   efiguids_dell.py
        #   efiguids_lenovo.py
        #   efiguids.py


        self.guids = {}

        try: 
            import efiguidsx
            #print(efiguidsx.GUIDs)            
            self.load(efiguidsx.GUIDs)

        except ImportError:
            print("Error importing efiguidsx")

        try: 

            import efiguids_ami
            self.load(efiguids_ami.GUIDs)

        except ImportError: pass

        try: 

            import efiguids_asrock
            self.load(efiguids_asrock.GUIDs)

        except ImportError: pass

        try: 

            import efiguids_dell
            self.load(efiguids_dell.GUIDs)

        except ImportError: pass

        try: 

            import efiguids_lenovo
            self.load(efiguids_lenovo.GUIDs)

        except ImportError: pass

        if len(self.guids) == 0:

            print('Error while loading GUIDs from %s' % EFIUTILS_PATH)
            print('You can download ida-efiutils here: %s\n' % EFIUTILS_URL)

        if guids is not None: self.load(guids)

    def load(self, guids):

        # load GUID's database from ida-efiutils
        for name, value in guids.items():            
            key = guid_to_str(tuple(value))       
            #print(key)     
            self.guids[key] = name


    def lookup(self, guid):
        # get name by GUID
        try: 
            key = guid_to_str(guid)    
            #print(key)        
            return self.guids[key]
        except KeyError: 
            return None



def find_smi_handlers():

    offset = 0
    while offset < len(g_smram_data):
        try:
            index = g_smram_data[offset:].index(SMI_ENTRY_SIGNATURE)
            offset += index
            #print(f"+ found SMI_ENTRY at {offset:08x}")
            smi_entry = SmiEntry.from_data(add_base(offset), g_smram_data[offset:])
            
            SmiEntry.process_smi_handlers(smi_entry)
            #smi_entry.show()    
            offset += sizeof(SmiEntryStruct)

            g_smi_entries.append(smi_entry)
        except ValueError:
            break
           

    ## Make sure the parsing of the internal linked lists of 'smie'
    ## have the same result of going through the dump looking for 'smih'

    offset = 0
    while offset < len(g_smram_data):
        try:
            index = g_smram_data[offset:].index(SMI_HANDLER_SIGNATURE)
            offset += index            
            smi_handler = SmiHandler.from_data(add_base(offset), g_smram_data[offset:])  
            offset += sizeof(SmiHandlerStruct)
            
            if smi_handler not in g_smi_handlers:
                raise Exception("missing SMIH records from SMIE linked lists?")

        except ValueError:
            break


def print_smie_handlers():
    print('\nRegistered SMI Handlers:\n')
    for smie in g_smi_entries:          
        print(smie.handler_type_name())

        for smih in smie.smi_handlers:
            handler_addr = smih.handler()
            for mm in g_smm_modules:
                mm_address = mm["address"]
                mm_size    = mm["size"]
                mm_end = mm_address + mm_size

                if handler_addr >= mm_address and handler_addr <= mm_end:
                    smih.set_module_name(mm["name"])                        
                    break             

            print(smih) 
        



def find_heap_allocations():

    offset = 0
    while offset < len(g_smram_data):
        try:
            index = g_smram_data[offset:].index(SMM_POOL_HEADER_SIGNATURE)
            offset += index
            
            sphd_size = struct.unpack("<Q",
                g_smram_data[offset+POOL_HEADER_SIZE_MEMBER_OFFSET:offset + POOL_HEADER_SIZE])[0]
            
            if sphd_size > tseg_size():
                break
            
            sptl_magic = offset + sphd_size - POOL_TAIL_SIZE
            if g_smram_data[sptl_magic:sptl_magic+4] != SMM_POOL_TAIL_SIGNATURE:
                raise Exception("unmatched sphd -> broken SMM pool?")

            g_smm_heap_blocks.append(
                ( offset, sphd_size, g_smram_data[offset+POOL_HEADER_SIZE:sptl_magic])
            )
  
            offset += sphd_size
        except ValueError:
            break

def print_fch_sw_smi_handlers():

    print('\nRegistered (FCH) SW SMI Handlers:\n')

    ## take into account POOL HEADER and POOL TAIL 
    FCH_SW_SMI_RECORD_SIZE_IN_HEAP = sizeof(FchSwSmiRecordStruct) + POOL_TAIL_SIZE + POOL_HEADER_SIZE 

    for block in g_smm_heap_blocks:
        address, size, data = block

        if size >= FCH_SW_SMI_RECORD_SIZE_IN_HEAP and size <= 0x80:

            if SMI_ENTRY_SIGNATURE in data or SMI_HANDLER_SIGNATURE in data \
                or PROTOCOL_ENTRY_SIGNATURE in data or EFI_HANDLE_SIGNATURE in data \
                or EFI_SMM_DRIVER_ENTRY_SIGNATURE in data or PROTOCOL_INTERFACE_SIGNATURE in data \
                or PROTOCOL_NOTIFY_SIGNATURE in data or SMM_LOCK_BOX_DATA_SIGNATURE in data:
                continue
            
            record = FchSwSmiRecord.from_data(address + POOL_HEADER_SIZE, data)
            if record is not None:

                handler_addr = record.handler()

                for mm in g_smm_modules:
                    mm_address = mm["address"]
                    mm_size    = mm["size"]
                    mm_end = mm_address + mm_size

                    if handler_addr >= mm_address and handler_addr <= mm_end:
                        record.set_module_name(mm["name"])                        
                        break

                
                print(record)
                    
                



def retrieve_smm_images():


    _align = lambda val: '%s' % val

    ptr = 0

    while ptr < len(g_smram_data):

        # check for DOS image header
        if g_smram_data[ptr : ptr + 2] == b'MZ':

            # read e_lfanew field
            e_lfanew = struct.unpack('I', g_smram_data[ptr + IMAGE_DOS_HEADER_e_lfanew : \
                                                ptr + IMAGE_DOS_HEADER_e_lfanew + 4])[0] + ptr

            # check for PE image header
            if g_smram_data[e_lfanew : e_lfanew + 2] == b'PE':

                addr = add_base(ptr)                
                
                name = g_fw.image_name(g_smram_data[ptr : ptr + HEADERS_SIZE]) 

                _, _, entry, size = g_fw.image_info(g_smram_data[ptr : ptr + HEADERS_SIZE])

                g_smm_modules.append({
                    "name": _align('' if name is None else name),
                    "address": addr,
                    "entry_point": add_base(ptr + entry),
                    "size": size
                })
                

        ptr += 0x100
        




if __name__ == "__main__":
    if len(sys.argv) < 4:
        usage()
        exit(-1)

    load_smram_dump(sys.argv[1])
    g_tseg_base = int(sys.argv[2], 16)
    g_tseg_end  = int(sys.argv[3], 16)
    guid_db = GuidDb(Additional_GUIDs)

    if len(sys.argv) == 5:
        # load BIOS image
        g_fw = FwImage(sys.argv[4])

        retrieve_smm_images()
    
        print('\nLOADED SMM DRIVERS:\n')
        for mm in g_smm_modules:
            print('0x%x: size = 0x%.8x, ep = 0x%x, name = %s' % \
                          (mm["address"], mm["size"], mm["entry_point"], \
                           mm["name"]))


    find_smi_handlers()
    print_smie_handlers()
    

    find_heap_allocations()
    print_fch_sw_smi_handlers()

    


    
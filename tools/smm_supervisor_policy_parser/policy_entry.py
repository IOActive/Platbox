# @file
# Library supporting Supervisor Policy binaries
#
# Copyright (c) Microsoft Corporation
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import struct
from enum import IntEnum
from typing import Type
import logging


class POLICY_TYPE(IntEnum):
    ''' Type of Supervisor policy.  Each resource type has it's own policy type'''
    INVALID = 0
    MEMORY = 1
    IO = 2
    MSR = 3
    INSTRUCTION = 4
    SAVESTATE = 5


class SUPPORTED_INSTRUCTION(IntEnum):
    ''' Supported privileged instructions by SMM supervisor.

        typedef enum {
            SECURE_POLICY_INSTRUCTION_CLI,
            SECURE_POLICY_INSTRUCTION_WBINVD,
            SECURE_POLICY_INSTRUCTION_HLT,
            // Do not append after COUNT entry
            SECURE_POLICY_INSTRUCTION_COUNT
        } SECURE_POLICY_INSTRUCTION;
    '''
    CLI = 0
    WBINVD = 1
    HLT = 2


class ALLOWED_SAVE_STATE_FIELD(IntEnum):
    ''' Supported access in save state through SMM supervisor.

        typedef enum {
        SECURE_POLICY_SVST_RAX              = 0,
        SECURE_POLICY_SVST_IO_TRAP          = 1,
        // Do not append after COUNT entry
        SECURE_POLICY_SVST_COUNT            = 2
        } SECURE_POLICY_SVST;
    '''
    RAX = 0
    IO_TRAP = 1


class ALLOWED_SAVE_STATE_ACCESS_CONDITION(IntEnum):
    ''' Supported access condition in save state corresponding to the access field.

        // Index of SMM save state access conditions
        typedef enum {
        SECURE_POLICY_SVST_UNCONDITIONAL    = 0,
        SECURE_POLICY_SVST_CONDITION_IO_RD  = 1,
        SECURE_POLICY_SVST_CONDITION_IO_WR  = 2,
        // Do not append after COUNT entry
        SECURE_POLICY_SVST_CONDITION_COUNT  = 3
        } SECURE_POLICY_SVST_CONDITION;
    '''
    UNCONDITIONAL = 0
    IOREAD = 1
    IOWRITE = 2


class AccessAttribute(object):
    ''' Access Attributes are bits in a bitfield and can not exceed 8 bits
    '''
    ACCESS_ATTR_ALLOW           = 0
    ACCESS_ATTR_DENY            = 1
    ACCESS_ATTR_MAX_SUPPORTED   = 1

    bits = ["ALLOW", "DENY"]

    def __init__(self, value: int):
        if(value > self.ACCESS_ATTR_MAX_SUPPORTED):
            raise Exception("Invalid value")
        self.value = value

    def __str__(self):
        string = ""

        for i in range(len(self.bits)):
            if self.value == i:
                string = self.bits[i]
                break
        return string


class AccessType(object):
    ''' Access Type Attributes are bits in a bitfield and can not exceed 16 bits
    '''
    INHERITED               = 0
    READ_INHERITED          = 1     # BIT0
    WRITE_INHERITED         = 2     # BIT1
    EXECUTE_INHERITED       = 4     # BIT2
    STRICT_WIDTH_INHERITED  = 8     # BIT3
    COND_READ_INHERITED     = 16    # BIT4
    COND_WRITE_INHERITED    = 32    # BIT5

    bits = ["READ", "WRITE", "EXECUTE", "STRICT WIDTH", "CONDITIONAL READ", "CONDITIONAL WRITE"]

    def __init__(self, value: int):
        if(value > (AccessType.READ_INHERITED + AccessType.WRITE_INHERITED +\
                    AccessType.EXECUTE_INHERITED + AccessType.STRICT_WIDTH_INHERITED +\
                    AccessType.COND_READ_INHERITED + AccessType.COND_WRITE_INHERITED)):
            raise Exception("Invalid value")
        self.value = value

    def __str__(self):
        if self.value == 0:
            return "INHERITED"
        string = ""

        for i in range(len(self.bits)):
            if self.value & (1 << i):
                string += self.bits[i] + " | "
        return string.strip(" |")


class PolicyEntryHeader(object):
    '''
        Common header in each policy entry

        // The version of this descriptor.
        UINT32 Version; //1
        // The Type of this Parameter.
        UINT32 Type;
        // The size of the descriptor in bytes including the header.
        UINT32 EntrySize;
    '''
    _VERSION = 1
    _StructFormat = '<III'
    _StructSize = struct.calcsize(_StructFormat)

    def __init__(self, type: POLICY_TYPE, additional_size: int):
        self.Version = self._VERSION  # descriptor version
        self.Type = type
        # size of Entry descriptor including header
        self.EntrySize = additional_size + self._StructSize
        self.BodySize = additional_size

    def Encode(self) -> bytes:
        ''' Write self to a binary bytes object'''
        return struct.pack(self._StructFormat, self.Version, int(self.Type), self.EntrySize)

    def Decode(self, Buffer) -> bytes:
        ''' Decode the Buffer and confirm it matches self data
        return any remaining data not part of self
        '''
        (v, t, es) = struct.unpack_from(self._StructFormat, Buffer)
        if v != self._VERSION:
            print (Buffer)
            raise Exception("Invalid Version")
        if POLICY_TYPE(t) != self.Type:
            raise Exception("Decoding wrong Type")
        if es != self.EntrySize:
            raise Exception("Unexpected Entry Size")

        # slice off buffer bytes decoded
        return Buffer[self._StructSize:]

    def DumpInfo(self, prefix: str = "") -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        '''
        print(f"{prefix}Version: 0x{self.Version:08X}")
        print(f"{prefix}Type: {self.Type}")
        print(f"{prefix}EntrySize: {self.EntrySize}")


class PolicyEntry(object):
    ''' Base Policy Entry Object '''

    def GetSize(self) -> int:
        return self.Header.EntrySize

    def GetType(self) -> POLICY_TYPE:
        return self.Header.Type

    def GetBodySize(self) -> int:
        return self.Header.BodySize

    def Encode(self) -> bytes:
        ''' Write self to a binary bytes object'''
        raise NotImplementedError("Base Class does not implement this method")

    def Decode(self, Buffer) -> None:
        ''' Update self to match the data found in Buffer.
        return any remaining data not part of self
        '''
        raise NotImplementedError("Base Class does not implement this method")

    def DumpInfo(self, prefix: str = "", short: bool = True) -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        short: True (default): print short form.  False: print long form including header
        '''
        raise NotImplementedError("Base Class does not implement this method")


class PolicyRoot(object):
    '''
    Policy Root object for different policy descriptor types

    UINT32 Version;         // The version of this descriptor. Current Version is 1.
    UINT32 PolicyRootSize;  // The size of the SMM_SUPV_POLICY_ROOT_V* in bytes.
    UINT32 Type;            // The Type of this parameter, defined as SMM_SUPV_SECURE_POLICY_DESCRIPTOR_TYPE_*
    UINT32 Offset;          // The offset of the leaf descriptors in bytes from the beginning of this table.
    UINT32 Count;           // Number of entries of 'Type', starting from 'Offset'
    UINT8  AccessAttr;      // The access attribute of all policy entries of this Type, value defined as 
                            // SMM_SUPV_ACCESS_ATTR_ALLOW or SMM_SUPV_ACCESS_ATTR_DENY.
    UINT8  Reserved[3];     // Reserved, must be 0s.
    // Note: For backwards compatibility concern, one should not remove/rearrange the above fields if/when updating
    // the structure version to higher than 1.
    '''
    _StructFormat = '<LLLLLBBBB'
    _StructSize = struct.calcsize(_StructFormat)
    _Version = 1

    def __init__(self, Type: Type[POLICY_TYPE] = POLICY_TYPE.INVALID, AccessAttr: int = AccessAttribute.ACCESS_ATTR_ALLOW):
        self.Version = self._Version  # UINT32
        self.PolicyRootSize = self._StructSize #UINT32
        self.Type = Type  # UINT32
        self.Offset = 0  # UINT32
        self.Count = 0  # UINT32
        self.AccessAttr = AccessAttribute(AccessAttr)
        self.PolicyEntries = []

    def AddPolicy(self, policy: Type[PolicyEntry]) -> None:
        ''' Add a policy to this root'''

        if policy.GetType() == self.Type:
            self.PolicyEntries.append(policy)
        else:
            raise RuntimeError(
                f"Can't add mismatched policy of type {policy.GetType()}")

        self.Count = len (self.PolicyEntries)

    def Encode(self) -> bytes:
        ''' Write self to a binary bytes object'''

        if self.Version != self._Version:
            raise Exception("Invalid Version")
        if self.Type == POLICY_TYPE.INVALID:
            raise Exception("Invalid Data")

        return struct.pack(self._StructFormat,
                           self.Version,
                           self.PolicyRootSize,
                           self.Type,
                           self.Offset, # Offset
                           self.Count,
                           self.AccessAttr.value,
                           0, 0, 0 # Reserved
                           )

    def Decode(self, Buffer) -> bytes:
        ''' Update self to match the data found in Buffer.
        return any remaining data not part of self
        '''
        (ver, prs, tp, os, cnt, aa, _, _, _) = struct.unpack_from(self._StructFormat, Buffer)
        if ver != self._Version:
            raise Exception("Invalid Version")
        self.PolicyRootSize = prs
        self.Type = tp
        self.Offset = os
        self.Count = cnt
        self.AccessAttr = AccessAttribute(aa)
        return Buffer[self._StructSize:]

    def GetSize(self) -> int:
        return self.PolicyRootSize

    def SetOffset(self, offset: Type[int]):
        self.Offset = offset

    def GetType(self) -> POLICY_TYPE:
        return self.Type

    def Print(self):
        self.DumpInfo()

    def DumpInfo(self, prefix: str = "", short: bool = True) -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        short: True (default): print short form.  False: print long form including header
        '''
        print(f"{prefix}Policy Root")
        print(f"{prefix}  Version: 0x{self.Version:016X}")
        print(f"{prefix}  Type: {POLICY_TYPE(self.Type).name}")
        print(f"{prefix}  Count: 0x{self.Count:016X}")
        print(f"{prefix}  Access Attribute: {self.AccessAttr}")
        for pe in self.PolicyEntries:
            pe.DumpInfo(prefix=prefix + "    ", short=short)


class MemoryPolicyEntry(PolicyEntry):
    '''
    Policy Entry object for Memory Resources

    PolicyEntryHeader Header; //common header
    UINT64 BaseAddress;       //Base address of memory
    UINT64 Size;              //Size of memory
    UINT32 MemAttributes;     //Attributes of memory
    '''
    _StructFormatLegacy = '<QQI'
    _StructSizeLegacy = struct.calcsize(_StructFormatLegacy)
    '''
    Policy Entry object for Memory Resources

    UINT64 BaseAddress;                           // Base address of memory
    UINT64 Size;                                  // Size of memory
    UINT32 MemAttributes;                         // Attributes of memory
    UINT32 Reserved;                              // Reserved, must be 0
    '''
    _StructFormat = '<QQII'
    _StructSize = struct.calcsize(_StructFormat)

    def __init__(self, BaseAddress: int = 0, Size: int = 0, MemoryAttributes: int = 0):
        self.Header = PolicyEntryHeader(POLICY_TYPE.MEMORY, self._StructSizeLegacy)
        self.BaseAddress = BaseAddress  # UINT64
        self.Size = Size  # UINT64
        self.MemoryAttributes = MemoryAttributes  # UINT32

    def GetBodySize(self) -> int:
        return self._StructSize

    def Encode(self, legacy: bool = False) -> bytes:
        ''' Write self to a binary bytes object'''

        if self.Size == 0:
            raise Exception("Invalid Data")

        if legacy:
            return self.Header.Encode() + struct.pack(self._StructFormatLegacy,
                                                    self.BaseAddress,
                                                    self.Size,
                                                    self.MemoryAttributes)
        else:
            return struct.pack(self._StructFormat,
                            self.BaseAddress,
                            self.Size,
                            self.MemoryAttributes,
                            0)

    def Decode(self, Buffer, legacy: bool = False) -> bytes:
        ''' Update self to match the data found in Buffer.
        return any remaining data not part of self
        '''
        if legacy:
            Buffer = self.Header.Decode(Buffer)
            (ba, s, ma) = struct.unpack_from(self._StructFormatLegacy, Buffer)
            self.BaseAddress = ba
            self.Size = s
            self.MemoryAttributes = ma
            return Buffer[self._StructSizeLegacy:]
        else:
            (ba, s, ma, _) = struct.unpack_from(self._StructFormat, Buffer)
            self.BaseAddress = ba
            self.Size = s
            self.MemoryAttributes = ma
            return Buffer[self._StructSize:]

    def Print(self):
        self.DumpInfo()

    def DumpInfo(self, prefix: str = "", short: bool = True) -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        short: True (default): print short form.  False: print long form including header
        '''
        print(f"{prefix}Memory Policy Entry")
        if not short:
            self.Header.DumpInfo(prefix + "  ")
        print(f"{prefix}  BaseAddress: 0x{self.BaseAddress:016X}")
        print(f"{prefix}  Size: 0x{self.Size:016X}")
        print(f"{prefix}  Memory Attributes: 0x{self.MemoryAttributes:08X}")


class IoPolicyEntry(PolicyEntry):
    _StructFormatLegacy = '<HHH'
    _StructSizeLegacy = struct.calcsize(_StructFormatLegacy)
    _StructFormat = '<HHHH'
    _StructSize = struct.calcsize(_StructFormat)

    def __init__(self, IoAddress: int = 0, Size: int = 0, Attributes: int = AccessType.INHERITED):
        self.Header = PolicyEntryHeader(POLICY_TYPE.IO, self._StructSizeLegacy)
        self.IoAddress = IoAddress  # UINT16
        self.Size = Size  # UINT16
        self.Attributes = AccessType(Attributes)  # UINT16

    def GetBodySize(self) -> int:
        return self._StructSize

    def Encode(self, legacy: bool = False) -> bytes:
        ''' Write self to a binary bytes object'''
        if self.Size == 0:
            raise Exception("Invalid Data")

        if legacy:
            return self.Header.Encode() + struct.pack(self._StructFormatLegacy,
                                                    self.IoAddress,
                                                    self.Size,
                                                    self.Attributes.value)
        else:
            return struct.pack(self._StructFormat,
                                self.IoAddress,
                                self.Size,
                                self.Attributes.value,
                                0)

    def Decode(self, Buffer, legacy: bool = False) -> bytes:
        ''' Update self to match the data found in Buffer.
        return any remaining data not part of self
        '''
        if legacy:
            Buffer = self.Header.Decode(Buffer)
            (ia, s, a) = struct.unpack_from(self._StructFormatLegacy, Buffer)
            self.IoAddress = ia
            self.Size = s
            self.Attributes = AccessType(a)
            return Buffer[self._StructSizeLegacy:]
        else:
            (ia, s, a, _) = struct.unpack_from(self._StructFormat, Buffer)
            self.IoAddress = ia
            self.Size = s
            self.Attributes = AccessType(a)
            return Buffer[self._StructSize:]

    def Print(self):
        self.DumpInfo()

    def DumpInfo(self, prefix: str = "", short: bool = True) -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        short: True (default): print short form.  False: print long form including header
        '''
        print(f"{prefix}Io Policy Entry")
        if not short:
            self.Header.DumpInfo(prefix + "  ")
        print(f"{prefix}  IoAddress: 0x{self.IoAddress:04X}")
        print(f"{prefix}  Size: 0x{self.Size:04X}")
        print(f"{prefix}  Attributes: {self.Attributes}")


class MsrPolicyEntry(PolicyEntry):
    '''
    example {Header, 0xC0012000, 0x1000, SECURE_POLICY_RESOURCE_ATTR_WRITE_DIS}
    '''
    _StructFormat = '<IHH'
    _StructSize = struct.calcsize(_StructFormat)

    def __init__(self, MsrAddress: int = 0, Size: int = 0, Attributes: int = AccessType.INHERITED):
        self.Header = PolicyEntryHeader(POLICY_TYPE.MSR, self._StructSize)
        self.MsrAddress = MsrAddress  # UINT32
        self.Size = Size  # UINT16
        self.Attributes = AccessType(Attributes)  # UINT16

    def Encode(self, legacy: bool = False) -> bytes:
        ''' Write self to a binary bytes object'''
        if self.Size == 0:
            raise Exception("Invalid Data")

        header_bytes = b''
        if legacy:
            header_bytes = self.Header.Encode()

        return header_bytes + struct.pack(self._StructFormat,
                                        self.MsrAddress,
                                        self.Size,
                                        self.Attributes.value)

    def Decode(self, Buffer, legacy: bool = False) -> bytes:
        ''' Update self to match the data found in Buffer.
        return any remaining data not part of self
        '''
        if legacy:
            Buffer = self.Header.Decode(Buffer)
        (ma, s, a) = struct.unpack_from(self._StructFormat, Buffer)
        self.MsrAddress = ma
        self.Size = s
        self.Attributes = AccessType(a)
        return Buffer[self._StructSize:]

    def Print(self):
        self.DumpInfo()

    def DumpInfo(self, prefix: str = "", short: bool = True) -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        short: True (default): print short form.  False: print long form including header
        '''
        print(f"{prefix}MSR Policy Entry")
        if(not short):
            self.Header.DumpInfo(prefix + "  ")
        print(f"{prefix}  MsrAddress: {self.MsrAddress:08X}")
        print(f"{prefix}  Size: {self.Size:04X}")
        print(f"{prefix}  Attributes: {self.Attributes}")


class InstructionPolicyEntry(PolicyEntry):
    '''
    example {Header, SECURE_POLICY_INSTRUCTION_CLI, 0}
    '''
    _StructFormatLegacy = '<HH'
    _StructSizeLegacy = struct.calcsize(_StructFormatLegacy)
    _StructFormat = '<HHI'
    _StructSize = struct.calcsize(_StructFormat)

    def __init__(self, InstructionName: str = '', Attributes: int = AccessType.INHERITED):
        self.Header = PolicyEntryHeader(POLICY_TYPE.INSTRUCTION, self._StructSizeLegacy)
        try:
            self.InstructionIndex = int(SUPPORTED_INSTRUCTION[InstructionName.upper()]) # UINT16
        except:
            self.InstructionIndex = -1
        self.Attributes = AccessType(Attributes)  # UINT16

    def GetBodySize(self) -> int:
        return self._StructSize

    def Encode(self, legacy: bool = False) -> bytes:
        ''' Write self to a binary bytes object'''
        if self.InstructionIndex < 0:
            raise Exception("Invalid Data")

        if legacy:
            return self.Header.Encode() + struct.pack(self._StructFormatLegacy,
                                                    self.InstructionIndex,
                                                    self.Attributes.value)
        else:
            return struct.pack(self._StructFormat,
                                self.InstructionIndex,
                                self.Attributes.value,
                                0)

    def Decode(self, Buffer, legacy: bool = False) -> bytes:
        ''' Update self to match the data found in Buffer.
        return any remaining data not part of self
        '''
        if legacy:
            Buffer = self.Header.Decode(Buffer)
            (ii, a) = struct.unpack_from(self._StructFormatLegacy, Buffer)
            self.InstructionIndex = ii
            self.Attributes = AccessType(a)
            return Buffer[self._StructSizeLegacy:]
        else:
            (ii, a, _) = struct.unpack_from(self._StructFormat, Buffer)
            self.InstructionIndex = ii
            self.Attributes = AccessType(a)
            return Buffer[self._StructSize:]

    def Print(self):
        self.DumpInfo()

    def DumpInfo(self, prefix: str = "", short: bool = True) -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        short: True (default): print short form.  False: print long form including header
        '''
        print(f"{prefix}Instruction Policy Entry")
        if(not short):
            self.Header.DumpInfo(prefix + "  ")
        print(f"{prefix}  Instruction: {SUPPORTED_INSTRUCTION(self.InstructionIndex).name}")
        print(f"{prefix}  Attributes: {self.Attributes}")

class SaveStatePolicyEntry(PolicyEntry):
    '''
    example {SECURE_POLICY_SVST_RAX, COND_READ_INHERITED, SECURE_POLICY_SVST_CONDITION_IO_WR, 0}
    '''
    _StructFormat = '<IIII'
    _StructSize = struct.calcsize(_StructFormat)

    def __init__(self, SaveStateFieldName: str = '', Attributes: int = AccessType.INHERITED, AccessCondition: str = ''):
        self.Header = PolicyEntryHeader(POLICY_TYPE.SAVESTATE, self._StructSize)
        try:
            self.SaveStateIndex = int(ALLOWED_SAVE_STATE_FIELD[SaveStateFieldName.upper()]) # UINT32
        except:
            self.SaveStateIndex = -1
        try:
            self.AccessCondition = int(ALLOWED_SAVE_STATE_ACCESS_CONDITION[AccessCondition.upper()]) # UINT32
        except:
            self.AccessCondition = 0
        self.Attributes = AccessType(Attributes)  # UINT32

    def Encode(self) -> bytes:
        ''' Write self to a binary bytes object'''
        if self.SaveStateIndex < 0:
            raise Exception("Invalid Data")

        return struct.pack(self._StructFormat,
                            self.SaveStateIndex,
                            self.Attributes.value,
                            self.AccessCondition,
                            0)

    def Decode(self, Buffer) -> bytes:
        ''' Update self to match the data found in Buffer.
        return any remaining data not part of self
        '''
        (ss, a, ac, _) = struct.unpack_from(self._StructFormat, Buffer)
        self.SaveStateIndex = ss
        self.Attributes = AccessType(a)
        self.AccessCondition = ac
        return Buffer[self._StructSize:]

    def Print(self):
        self.DumpInfo()

    def DumpInfo(self, prefix: str = "", short: bool = True) -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        short: True (default): print short form.  False: print long form including header
        '''
        print(f"{prefix}Save State Policy Entry")
        if(not short):
            self.Header.DumpInfo(prefix + "  ")
        print(f"{prefix}  SaveStateField: {ALLOWED_SAVE_STATE_FIELD(self.SaveStateIndex).name}")
        print(f"{prefix}  Attributes: {self.Attributes}")
        print(f"{prefix}  AccessCondition: {ALLOWED_SAVE_STATE_ACCESS_CONDITION(self.AccessCondition).name}")


class PolicyDataCommonHeader(object):
    _StructFormat = '<IIII'
    _StructSize = struct.calcsize(_StructFormat)

    def __init__(self, version):
        self.Version = version


class Supervisor_Policy(object):
    FLEXBILE_STRUCTURE_VERSION = 0x00010000

    _StructFormat_v0_2 = '<IIIIIIIIII'
    _StructSize_v0_2 = struct.calcsize(_StructFormat_v0_2)

    _StructFormat_v1_0 = '<HHIIIIIQII'
    _StructSize_v1_0 = struct.calcsize(_StructFormat_v1_0)

    def __init__(self, version: int = FLEXBILE_STRUCTURE_VERSION):
        self.Version = version
        self.PolicyRoots = []

    def AddPolicyRoot(self, policyroot: Type[PolicyRoot]) -> None:
        ''' Add a policy root'''

        for pr in self.PolicyRoots:
            if policyroot.GetType() == pr.GetType():
                raise RuntimeError (
                        f"Can't add duplicated policy of same type {pr.GetType()}")

        self.PolicyRoots.append(policyroot)

    def Encode(self) -> bytes:
        if self.Version < Supervisor_Policy.FLEXBILE_STRUCTURE_VERSION:
            ''' Encode the supervisor_policy object into a bytes
            binary object.

            UINT32 Version;   //2
            UINT32 Size;      //Size in bytes of the whole blocks include Memory/IO/Msr Policy
            UINT32 MemoryPolicyOffset;
            UINT32 MemoryPolicyCount;     //Count of MemoryPolicy
            UINT32 IoPolicyOffset;
            UINT32 IoPolicyCount;     //Count of IoPolicy
            UINT32 MsrPolicyOffset;
            UINT32 MsrPolicyCount;    // Count of MsrPolicy
            UINT32 InstructionPolicyOffset;
            UINT32 InstructionPolicyCount;  // Count of InstructionPolicy
            '''
            memory_offset = self._StructSize_v0_2  # counter for offset
            memory_count = 0  # counter for policy entries
            io_offset = self._StructSize_v0_2
            io_count = 0
            msr_offset = self._StructSize_v0_2
            msr_count = 0
            instruction_offset = self._StructSize_v0_2
            instruction_count = 0

            data = b''
            for pr in self.PolicyRoots:
                if pr.AccessAttr == AccessAttribute.ACCESS_ATTR_ALLOW:
                    raise RuntimeError ("Cannot encode allow list for legacy descriptors")
                if pr.GetType() == POLICY_TYPE.MEMORY:
                    memory_count = pr.Count
                elif pr.GetType() == POLICY_TYPE.IO:
                    io_offset = memory_offset + len(data)
                    io_count = pr.Count
                elif pr.GetType() == POLICY_TYPE.MSR:
                    msr_offset = memory_offset + len(data)
                    msr_count = pr.Count
                elif pr.GetType() == POLICY_TYPE.INSTRUCTION:
                    instruction_offset = memory_offset + len(data)
                    instruction_count = pr.Count
                else:
                    raise NotImplementedError(
                        f"Discovered unsupported policy descriptor type {pr.GetType()}")

                for pe in pr.PolicyEntries:
                    data += pe.Encode(legacy=True)

            output = struct.pack(self._StructFormat_v0_2,
                                self.Version,
                                self.GetSize(),
                                memory_offset,
                                memory_count,
                                io_offset,
                                io_count,
                                msr_offset,
                                msr_count,
                                instruction_offset,
                                instruction_count
                                )

            return output + data

        else:
            ''' Encode the supervisor_policy object into a bytes
            binary object.

            UINT16 VersionMinor;           // 0x0000
            UINT16 VersionMajor;           // 0x0001
            UINT32 Size;                   // Size in bytes of the entire policy block
            UINT32 MemoryPolicyOffset;     // Offset of legacy memory policy, if supported, otherwise 0
            UINT32 MemoryPolicyCount;      // Count of MemoryPolicy, if supported, otherwise 0
            //Note: For backwards compatibility concern, one should not change the above fields even for future versions.
            UINT32 Flags;                  // Flag field to indicate supervisor status when policy is requested/reported
            UINT32 Capabilities;           // Capability field to indicate features supoorted by supervisor
            UINT64 Reserved;               // Reserved, must be 0
            UINT32 PolicyRootOffset;       // Offset from this structure to the beginning of the policy root array.
            UINT32 PolicyRootCount;        // Count of policy roots
            '''

            memory_count = 0  # counter for policy entries
            memory_entries = b''  # counter for policy entries
            pr_offset = self._StructSize_v1_0
            offset = pr_offset
            has_mem_policy = False
            for pr in self.PolicyRoots:
                offset += pr.GetSize()
                # For backwards compatibility purpose
                if pr.GetType() == POLICY_TYPE.MEMORY:
                    has_mem_policy = True
                    memory_count = pr.Count
                    for pe in pr.PolicyEntries:
                        memory_entries += pe.Encode(legacy=True)

            if not has_mem_policy:
                # Memory descriptors needs a placeholder for runtime update, if there is none
                pr = PolicyRoot (POLICY_TYPE.MEMORY, AccessAttribute.ACCESS_ATTR_DENY)
                self.AddPolicyRoot (pr)
                offset += pr.GetSize()

            root = b''
            body = b''
            for pr in self.PolicyRoots:
                pr.SetOffset(offset)
                root += pr.Encode()
                for pe in pr.PolicyEntries:
                    body += pe.Encode()
                    offset += pe.GetBodySize()

            memory_offset = offset if memory_count != 0 else 0  # counter for offset

            header = struct.pack(self._StructFormat_v1_0,
                                self.Version & 0xFFFF,
                                (self.Version >> 16) & 0xFFFF,
                                self.GetSize(),
                                memory_offset,
                                memory_count,
                                0, 0, 0,
                                pr_offset,
                                len(self.PolicyRoots)
                                )

            return header + root + body + memory_entries

    def GetSize(self) -> int:
        ''' Calculate the size of the output binary encoded policy'''
        if self.Version < Supervisor_Policy.FLEXBILE_STRUCTURE_VERSION:
            size = self._StructSize_v0_2
            for pr in self.PolicyRoots:
                for pe in pr.PolicyEntries:
                    size += pe.GetSize()
            return size
        else:
            size = self._StructSize_v1_0
            for pr in self.PolicyRoots:
                size += pr.GetSize()
                for pe in pr.PolicyEntries:
                    size += pe.GetBodySize()
            return size

    def Decode(self, Buffer: bytes) -> bytes:
        '''Decode the bytes object into a supervisor policy.
        Assume the first byte of Buffer is the first byte of the policy.
        return the bytes remaining
        '''
        (version, _, _, _) = struct.unpack_from (PolicyDataCommonHeader._StructFormat, Buffer)
        self.Version = version
        if version < Supervisor_Policy.FLEXBILE_STRUCTURE_VERSION:
            (v, s, mpo, mpc, ipo, ipc, msrpo, msrpc, instrpo, instrpc) = struct.unpack_from(
                self._StructFormat_v0_2, Buffer)
            if s > len(Buffer):
                raise Exception("Invalid Size")
            if mpo >= s:
                raise Exception("Invalid MemoryPolicyOffset")
            if ipo >= s:
                raise Exception("Invalid IoPolicyOffset")
            if msrpo >= s:
                raise Exception("Invalid MsrPolicyOffset")
            if instrpo >= s:
                raise Exception("Invalid InstructionPolicyOffset")

            ReturnBuffer = Buffer[s:]  # return this buffer

            PolicyBuffer = Buffer[:s]  #

            # Decode Memory Policies
            LoopBuffer = PolicyBuffer[mpo:]
            a = PolicyRoot(Type=POLICY_TYPE.MEMORY, AccessAttr=AccessAttribute.ACCESS_ATTR_DENY)
            a.Count = mpc
            for i in range(mpc):
                b = MemoryPolicyEntry()
                LoopBuffer = b.Decode(LoopBuffer, legacy=True)
                a.AddPolicy(b)
            if mpc > 0:
                self.AddPolicyRoot(a)
            # Decode Io Policies
            LoopBuffer = PolicyBuffer[ipo:]
            a = PolicyRoot(Type=POLICY_TYPE.IO, AccessAttr=AccessAttribute.ACCESS_ATTR_DENY)
            for i in range(ipc):
                b = IoPolicyEntry()
                LoopBuffer = b.Decode(LoopBuffer, legacy=True)
                a.AddPolicy(b)
            if ipc > 0:
                self.AddPolicyRoot(a)
            # Decode Msr Policies
            LoopBuffer = PolicyBuffer[msrpo:]
            a = PolicyRoot(Type=POLICY_TYPE.MSR, AccessAttr=AccessAttribute.ACCESS_ATTR_DENY)
            for i in range(msrpc):
                b = MsrPolicyEntry()
                LoopBuffer = b.Decode(LoopBuffer, legacy=True)
                a.AddPolicy(b)
            if msrpc > 0:
                self.AddPolicyRoot(a)
            # Decode Instruction Policies
            LoopBuffer = PolicyBuffer[instrpo:]
            a = PolicyRoot(Type=POLICY_TYPE.INSTRUCTION, AccessAttr=AccessAttribute.ACCESS_ATTR_DENY)
            for i in range(instrpc):
                b = InstructionPolicyEntry()
                LoopBuffer = b.Decode(LoopBuffer, legacy=True)
                a.AddPolicy(b)
            if instrpc > 0:
                self.AddPolicyRoot(a)

            # Check size
            if self.GetSize() != s:
                raise Exception("Incorrect decoding.  Size doesn't match")

            # slice off buffer bytes decoded
            return ReturnBuffer

        else:
            (vma, vmi, s, mpo, mpc, fg, cap, _, pro, prc) = struct.unpack_from(
                self._StructFormat_v1_0, Buffer)
            if s > len(Buffer):
                raise Exception("Invalid Size")
            if mpo >= s:
                raise Exception("Invalid MemoryPolicyOffset")
            if pro >= s:
                raise Exception("Invalid PolicyRootOffset")
            if prc == 0:
                raise Exception("Invalid PolicyRootCount")

            ReturnBuffer = Buffer[s:]  # return this buffer

            PolicyBuffer = Buffer[:s]  #

            # Decode Memory Policies
            LoopBuffer = PolicyBuffer[mpo:]
            LegacyMemDsc = PolicyRoot(Type=POLICY_TYPE.MEMORY, AccessAttr=AccessAttribute.ACCESS_ATTR_DENY)
            for i in range(mpc):
                for i in range(LegacyMemDsc.Count):
                    b = MemoryPolicyEntry()
                    LoopBuffer = b.Decode(LoopBuffer, legacy=True)
                    LegacyMemDsc.AddPolicy(b)

            has_mem_policy = False
            # Decode Policy Roots
            LoopBuffer = PolicyBuffer[pro:]
            for i in range(prc):
                a = PolicyRoot()
                LoopBuffer = a.Decode(LoopBuffer)
                PolicyEntryBuffer = PolicyBuffer[a.Offset:]
                for i in range(a.Count):
                    if a.GetType() == POLICY_TYPE.MEMORY:
                        b = MemoryPolicyEntry()
                    elif a.GetType() == POLICY_TYPE.IO:
                        b = IoPolicyEntry()
                    elif a.GetType() == POLICY_TYPE.MSR:
                        b = MsrPolicyEntry()
                    elif a.GetType() == POLICY_TYPE.INSTRUCTION:
                        b = InstructionPolicyEntry()
                    elif a.GetType() == POLICY_TYPE.SAVESTATE:
                        b = SaveStatePolicyEntry()
                    else:
                        raise NotImplementedError (
                            f"Can't decode policy of type {a.GetType()}")
                    PolicyEntryBuffer = b.Decode(PolicyEntryBuffer)
                    a.AddPolicy(b)
                if a.GetType() == POLICY_TYPE.MEMORY:
                    if mpc != 0:
                        # If there is legacy memory descriptors, the 2 versions better match...
                        if LegacyMemDsc.Version != a.Type or \
                            LegacyMemDsc.PolicyRootSize != a.PolicyRootSize or \
                            LegacyMemDsc.Type != a.Type or \
                            LegacyMemDsc.Count != a.Count or \
                            LegacyMemDsc.AccessAttr != a.AccessAttr:
                            raise RuntimeError ("Legacy memory descriptor mismatched with new versions")
                    has_mem_policy = True
                self.AddPolicyRoot(a)

            if not has_mem_policy:
                # This means the policy roots does not have one, which is not a good sign
                self.AddPolicyRoot(LegacyMemDsc)
                raise RuntimeWarning("No memory policy root found from input binary, might want to check this")

            # Check size
            if self.GetSize() != s:
                raise Exception("Incorrect decoding.  Size doesn't match")

            # slice off buffer bytes decoded
            return ReturnBuffer

    def Print(self):
        self.DumpInfo()

    def DumpInfo(self, prefix: str = "", short: bool = True) -> None:
        '''Print the object in human readable form.

        prefix: str printed before each line of output.  Best used for whitespace and alignment
        short: True (default): print short form.  False: print long form including header
        '''
        print(f"{prefix}Supervisor Policy Object")
        print(f"{prefix}  Version: {self.Version}")
        print(f"{prefix}  Size: {self.GetSize()}")
        print(f"{prefix}  Policy Roots: {len(self.PolicyRoots)}")
        for pr in self.PolicyRoots:
            pr.DumpInfo(prefix=prefix + "    ", short=short)

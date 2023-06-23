# SMM Supervisor Policy Parser

This is a simple utility which will extract an SMM Supervisor policy binary from a UEFI container and print the policy details. 

This requires `UEFIextract` to be in your path.

## Usage

```
$ python smm-supervisor-policy-parser.py huawei_rom.bin 
  Supervisor Policy Object
    Version: 65536
    Size: 248
    Policy Roots: 4
      Policy Root
        Version: 0x0000000000000001
        Type: IO
        Count: 0x0000000000000002
        Access Attribute: DENY
          Io Policy Entry
            IoAddress: 0x0CF8
            Size: 0x0004
            Attributes: WRITE | STRICT WIDTH
          Io Policy Entry
            IoAddress: 0x0CFC
            Size: 0x0004
            Attributes: WRITE
      Policy Root
        Version: 0x0000000000000001
        Type: MSR
        Count: 0x0000000000000009
        Access Attribute: DENY
          MSR Policy Entry
            MsrAddress: C0000080
            Size: 0001
            Attributes: READ | WRITE | EXECUTE
          MSR Policy Entry
            MsrAddress: C0000081
            Size: 0004
            Attributes: READ | WRITE | EXECUTE
          MSR Policy Entry
            MsrAddress: C0010010
            Size: 0001
            Attributes: READ | WRITE | EXECUTE
          MSR Policy Entry
            MsrAddress: C0010111
            Size: 0001
            Attributes: READ | WRITE | EXECUTE
          MSR Policy Entry
            MsrAddress: 000001D9
            Size: 0001
            Attributes: READ | WRITE | EXECUTE
          MSR Policy Entry
            MsrAddress: 00000DA0
            Size: 0001
            Attributes: READ | WRITE | EXECUTE
          MSR Policy Entry
            MsrAddress: 000006A0
            Size: 0001
            Attributes: READ | WRITE | EXECUTE
          MSR Policy Entry
            MsrAddress: 000006A2
            Size: 0001
            Attributes: READ | WRITE | EXECUTE
          MSR Policy Entry
            MsrAddress: 000006A4
            Size: 0005
            Attributes: READ | WRITE | EXECUTE
      Policy Root
        Version: 0x0000000000000001
        Type: INSTRUCTION
        Count: 0x0000000000000003
        Access Attribute: ALLOW
          Instruction Policy Entry
            Instruction: CLI
            Attributes: EXECUTE
          Instruction Policy Entry
            Instruction: WBINVD
            Attributes: EXECUTE
          Instruction Policy Entry
            Instruction: HLT
            Attributes: EXECUTE
      Policy Root
        Version: 0x0000000000000001
        Type: MEMORY
        Count: 0x0000000000000000
        Access Attribute: DENY
```


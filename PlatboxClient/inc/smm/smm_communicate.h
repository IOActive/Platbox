#pragma once
#include "global.h"


#define SMM_CORE_PRIVATE_DATA_SIGNATURE "smmc"

void smm_dump_s3_bootscript(const char *output_filename);
void smm_dump_s3_bootscript_manual(UINT64 rtcode_start, UINT64 rtcode_end, const char *output_filename);

#define RETURN_SUCCESS  0

///
/// The image failed to load.
///
#define RETURN_LOAD_ERROR  ENCODE_ERROR (1)

///
/// The parameter was incorrect.
///
#define RETURN_INVALID_PARAMETER  ENCODE_ERROR (2)

///
/// The operation is not supported.
///
#define RETURN_UNSUPPORTED  ENCODE_ERROR (3)

///
/// The buffer was not the proper size for the request.
///
#define RETURN_BAD_BUFFER_SIZE  ENCODE_ERROR (4)

///
/// The buffer was not large enough to hold the requested data.
/// The required buffer size is returned in the appropriate
/// parameter when this error occurs.
///
#define RETURN_BUFFER_TOO_SMALL  ENCODE_ERROR (5)

///
/// There is no data pending upon return.
///
#define RETURN_NOT_READY  ENCODE_ERROR (6)

///
/// The physical device reported an error while attempting the
/// operation.
///
#define RETURN_DEVICE_ERROR  ENCODE_ERROR (7)

///
/// The device can not be written to.
///
#define RETURN_WRITE_PROTECTED  ENCODE_ERROR (8)

///
/// The resource has run out.
///
#define RETURN_OUT_OF_RESOURCES  ENCODE_ERROR (9)

///
/// An inconsistency was detected on the file system causing the
/// operation to fail.
///
#define RETURN_VOLUME_CORRUPTED  ENCODE_ERROR (10)

///
/// There is no more space on the file system.
///
#define RETURN_VOLUME_FULL  ENCODE_ERROR (11)

///
/// The device does not contain any medium to perform the
/// operation.
///
#define RETURN_NO_MEDIA  ENCODE_ERROR (12)

///
/// The medium in the device has changed since the last
/// access.
///
#define RETURN_MEDIA_CHANGED  ENCODE_ERROR (13)

///
/// The item was not found.
///
#define RETURN_NOT_FOUND  ENCODE_ERROR (14)

///
/// Access was denied.
///
#define RETURN_ACCESS_DENIED  ENCODE_ERROR (15)

///
/// The server was not found or did not respond to the request.
///
#define RETURN_NO_RESPONSE  ENCODE_ERROR (16)

///
/// A mapping to the device does not exist.
///
#define RETURN_NO_MAPPING  ENCODE_ERROR (17)

///
/// A timeout time expired.
///
#define RETURN_TIMEOUT  ENCODE_ERROR (18)

///
/// The protocol has not been started.
///
#define RETURN_NOT_STARTED  ENCODE_ERROR (19)

///
/// The protocol has already been started.
///
#define RETURN_ALREADY_STARTED  ENCODE_ERROR (20)

///
/// The operation was aborted.
///
#define RETURN_ABORTED  ENCODE_ERROR (21)

///
/// An ICMP error occurred during the network operation.
///
#define RETURN_ICMP_ERROR  ENCODE_ERROR (22)

///
/// A TFTP error occurred during the network operation.
///
#define RETURN_TFTP_ERROR  ENCODE_ERROR (23)

///
/// A protocol error occurred during the network operation.
///
#define RETURN_PROTOCOL_ERROR  ENCODE_ERROR (24)

///
/// A function encountered an internal version that was
/// incompatible with a version requested by the caller.
///
#define RETURN_INCOMPATIBLE_VERSION  ENCODE_ERROR (25)

///
/// The function was not performed due to a security violation.
///
#define RETURN_SECURITY_VIOLATION  ENCODE_ERROR (26)

///
/// A CRC error was detected.
///
#define RETURN_CRC_ERROR  ENCODE_ERROR (27)

///
/// The beginning or end of media was reached.
///
#define RETURN_END_OF_MEDIA  ENCODE_ERROR (28)

///
/// The end of the file was reached.
///
#define RETURN_END_OF_FILE  ENCODE_ERROR (31)

///
/// The language specified was invalid.
///
#define RETURN_INVALID_LANGUAGE  ENCODE_ERROR (32)

///
/// The security status of the data is unknown or compromised
/// and the data must be updated or replaced to restore a valid
/// security status.
///
#define RETURN_COMPROMISED_DATA  ENCODE_ERROR (33)

///
/// A HTTP error occurred during the network operation.
///
#define RETURN_HTTP_ERROR  ENCODE_ERROR (35)



#define EFI_SUCCESS               RETURN_SUCCESS
#define EFI_LOAD_ERROR            RETURN_LOAD_ERROR
#define EFI_INVALID_PARAMETER     RETURN_INVALID_PARAMETER
#define EFI_UNSUPPORTED           RETURN_UNSUPPORTED
#define EFI_BAD_BUFFER_SIZE       RETURN_BAD_BUFFER_SIZE
#define EFI_BUFFER_TOO_SMALL      RETURN_BUFFER_TOO_SMALL
#define EFI_NOT_READY             RETURN_NOT_READY
#define EFI_DEVICE_ERROR          RETURN_DEVICE_ERROR
#define EFI_WRITE_PROTECTED       RETURN_WRITE_PROTECTED
#define EFI_OUT_OF_RESOURCES      RETURN_OUT_OF_RESOURCES
#define EFI_VOLUME_CORRUPTED      RETURN_VOLUME_CORRUPTED
#define EFI_VOLUME_FULL           RETURN_VOLUME_FULL
#define EFI_NO_MEDIA              RETURN_NO_MEDIA
#define EFI_MEDIA_CHANGED         RETURN_MEDIA_CHANGED
#define EFI_NOT_FOUND             RETURN_NOT_FOUND
#define EFI_ACCESS_DENIED         RETURN_ACCESS_DENIED
#define EFI_NO_RESPONSE           RETURN_NO_RESPONSE
#define EFI_NO_MAPPING            RETURN_NO_MAPPING
#define EFI_TIMEOUT               RETURN_TIMEOUT
#define EFI_NOT_STARTED           RETURN_NOT_STARTED
#define EFI_ALREADY_STARTED       RETURN_ALREADY_STARTED
#define EFI_ABORTED               RETURN_ABORTED
#define EFI_ICMP_ERROR            RETURN_ICMP_ERROR
#define EFI_TFTP_ERROR            RETURN_TFTP_ERROR
#define EFI_PROTOCOL_ERROR        RETURN_PROTOCOL_ERROR
#define EFI_INCOMPATIBLE_VERSION  RETURN_INCOMPATIBLE_VERSION
#define EFI_SECURITY_VIOLATION    RETURN_SECURITY_VIOLATION
#define EFI_CRC_ERROR             RETURN_CRC_ERROR
#define EFI_END_OF_MEDIA          RETURN_END_OF_MEDIA
#define EFI_END_OF_FILE           RETURN_END_OF_FILE
#define EFI_INVALID_LANGUAGE      RETURN_INVALID_LANGUAGE
#define EFI_COMPROMISED_DATA      RETURN_COMPROMISED_DATA
#define EFI_HTTP_ERROR            RETURN_HTTP_ERROR



typedef HANDLE EFI_HANDLE;
typedef UINT64 UINTN;
typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef UINT64 EFI_SMM_ENTRY_POINT;
typedef UINT64 EFI_SMM_SYSTEM_TABLE2;

typedef GUID EFI_GUID;

typedef UINTN RETURN_STATUS;
typedef RETURN_STATUS EFI_STATUS;

typedef struct {
  ///
  /// Designates the physical address of the MMRAM in memory. This view of memory is
  /// the same as seen by I/O-based agents, for example, but it may not be the address seen
  /// by the processors.
  ///
  EFI_PHYSICAL_ADDRESS    PhysicalStart;
  ///
  /// Designates the address of the MMRAM, as seen by software executing on the
  /// processors. This address may or may not match PhysicalStart.
  ///
  EFI_PHYSICAL_ADDRESS    CpuStart;
  ///
  /// Describes the number of bytes in the MMRAM region.
  ///
  UINT64                  PhysicalSize;
  ///
  /// Describes the accessibility attributes of the MMRAM.  These attributes include the
  /// hardware state (e.g., Open/Closed/Locked), capability (e.g., cacheable), logical
  /// allocation (e.g., allocated), and pre-use initialization (e.g., needs testing/ECC
  /// initialization).
  ///
  UINT64                  RegionState;
} EFI_MMRAM_DESCRIPTOR;

typedef EFI_MMRAM_DESCRIPTOR EFI_SMRAM_DESCRIPTOR;


typedef struct {
  UINTN                   Signature;

  ///
  /// The ImageHandle passed into the entry point of the SMM IPL.  This ImageHandle
  /// is used by the SMM Core to fill in the ParentImageHandle field of the Loaded
  /// Image Protocol for each SMM Driver that is dispatched by the SMM Core.
  ///
  EFI_HANDLE              SmmIplImageHandle;

  ///
  /// The number of SMRAM ranges passed from the SMM IPL to the SMM Core.  The SMM
  /// Core uses these ranges of SMRAM to initialize the SMM Core memory manager.
  ///
  UINTN                   SmramRangeCount;

  ///
  /// A table of SMRAM ranges passed from the SMM IPL to the SMM Core.  The SMM
  /// Core uses these ranges of SMRAM to initialize the SMM Core memory manager.
  ///
  EFI_SMRAM_DESCRIPTOR    *SmramRanges;

  ///
  /// The SMM Foundation Entry Point.  The SMM Core fills in this field when the
  /// SMM Core is initialized.  The SMM IPL is responsible for registering this entry
  /// point with the SMM Configuration Protocol.  The SMM Configuration Protocol may
  /// not be available at the time the SMM IPL and SMM Core are started, so the SMM IPL
  /// sets up a protocol notification on the SMM Configuration Protocol and registers
  /// the SMM Foundation Entry Point as soon as the SMM Configuration Protocol is
  /// available.
  ///
  EFI_SMM_ENTRY_POINT      SmmEntryPoint;

  ///
  /// Boolean flag set to TRUE while an SMI is being processed by the SMM Core.
  ///
  BOOLEAN                  SmmEntryPointRegistered;

  ///
  /// Boolean flag set to TRUE while an SMI is being processed by the SMM Core.
  ///
  BOOLEAN                  InSmm;

  ///
  /// This field is set by the SMM Core then the SMM Core is initialized.  This field is
  /// used by the SMM Base 2 Protocol and SMM Communication Protocol implementations in
  /// the SMM IPL.
  ///
  EFI_SMM_SYSTEM_TABLE2    *Smst;

  ///
  /// This field is used by the SMM Communication Protocol to pass a buffer into
  /// a software SMI handler and for the software SMI handler to pass a buffer back to
  /// the caller of the SMM Communication Protocol.
  ///
  VOID                     *CommunicationBuffer;

  ///
  /// This field is used by the SMM Communication Protocol to pass the size of a buffer,
  /// in bytes, into a software SMI handler and for the software SMI handler to pass the
  /// size, in bytes, of a buffer back to the caller of the SMM Communication Protocol.
  ///
  UINTN                    BufferSize;

  ///
  /// This field is used by the SMM Communication Protocol to pass the return status from
  /// a software SMI handler back to the caller of the SMM Communication Protocol.
  ///
  EFI_STATUS               ReturnStatus;

  EFI_PHYSICAL_ADDRESS     PiSmmCoreImageBase;
  UINT64                   PiSmmCoreImageSize;
  EFI_PHYSICAL_ADDRESS     PiSmmCoreEntryPoint;
} SMM_CORE_PRIVATE_DATA;

typedef struct {
  ///
  /// Allows for disambiguation of the message format.
  ///
  EFI_GUID    HeaderGuid;
  ///
  /// Describes the size of Data (in bytes) and does not include the size of the header.
  ///
  UINTN       MessageLength;
  ///
  /// Designates an array of bytes that is MessageLength in size.
  ///
  UINT8       Data[1];
} EFI_MM_COMMUNICATE_HEADER;


typedef EFI_MM_COMMUNICATE_HEADER EFI_SMM_COMMUNICATE_HEADER;


#define EFI_SMM_LOCK_BOX_COMMAND_SAVE                  0x1
#define EFI_SMM_LOCK_BOX_COMMAND_UPDATE                0x2
#define EFI_SMM_LOCK_BOX_COMMAND_RESTORE               0x3
#define EFI_SMM_LOCK_BOX_COMMAND_SET_ATTRIBUTES        0x4
#define EFI_SMM_LOCK_BOX_COMMAND_RESTORE_ALL_IN_PLACE  0x5

typedef struct {
  UINT32    Command;
  UINT32    DataLength;
  UINT64    ReturnStatus;
} EFI_SMM_LOCK_BOX_PARAMETER_HEADER;

typedef struct {
  EFI_SMM_LOCK_BOX_PARAMETER_HEADER  Header;
  GUID                               Guid;
  EFI_PHYSICAL_ADDRESS               Buffer;
  UINT64                             Length;
} EFI_SMM_LOCK_BOX_PARAMETER_RESTORE;
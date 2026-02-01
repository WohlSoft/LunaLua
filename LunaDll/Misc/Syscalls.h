#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <Windows.h>
#include <WinNT.h>
#include <winternl.h>
#include "DeclareCall.h"

IMPORT_FROM(ntdll) {
    DECLARE_CALL( 
        NTSTATUS NTAPI NtCreateFile(
            PHANDLE FileHandle,
            ACCESS_MASK DesiredAccess,
            POBJECT_ATTRIBUTES ObjectAttributes,
            PIO_STATUS_BLOCK IoStatusBlock,
            PLARGE_INTEGER AllocationSize,
            ULONG FileAttributes,
            ULONG ShareAccess,
            ULONG CreateDisposition,
            ULONG CreateOptions,
            PVOID EaBuffer,
            ULONG EaLength
        )
    )

    DECLARE_CALL( 
        NTSTATUS NTAPI NtQueryDirectoryFile(
            HANDLE FileHandle,
            HANDLE Event,
            PIO_APC_ROUTINE ApcRoutine,
            PVOID ApcContext,
            PIO_STATUS_BLOCK IoStatusBlock,
            PVOID FileInformation,
            ULONG Length,
            FILE_INFORMATION_CLASS FileInformationClass,
            BOOLEAN ReturnSingleEntry,
            PUNICODE_STRING FileName,
            BOOLEAN RestartScan
        )
    )

    DECLARE_CALL( 
        VOID NTAPI RtlInitUnicodeString(
            PUNICODE_STRING DestinationString,
            __drv_aliasesMem PCWSTR SourceString
        )
    )

    DECLARE_CALL( 
        ULONG NTAPI RtlNtStatusToDosError(
            NTSTATUS Status
        )
    )

    DECLARE_CALL(
        NTSTATUS NTAPI NtSetInformationFile(
            HANDLE FileHandle,
            PIO_STATUS_BLOCK IoStatusBlock,
            PVOID FileInformation,
            ULONG Length,
            FILE_INFORMATION_CLASS FileInformationClass
        )
    )
}

#endif
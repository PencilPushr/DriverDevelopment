#pragma once

#include <ntddk.h>
#include <ntifs.h>
#include <processthreadsapi.h>

NTSTATUS EnumerateProcessMemory(
    HANDLE processId
)
{
    NTSTATUS status;
    PEPROCESS process;
    HANDLE processHandle;
    PVOID address = NULL;

    // Lookup the EPROCESS structure
    status = PsLookupProcessByProcessId(processId, &process);
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to find process\n");
        return status;
    }

    // Open a handle to the process
    status = ObOpenObjectByPointer(
        process,
        OBJ_KERNEL_HANDLE,
        NULL,
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        *PsProcessType,
        KernelMode,
        &processHandle
    );
    ObDereferenceObject(process);

    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to open process handle\n");
        return status;
    }

    // Enumerate the memory regions
    while (TRUE) 
    {
        MEMORY_BASIC_INFORMATION memInfo;
        SIZE_T returnLength;

        status = ZwQueryVirtualMemory(
            processHandle,
            address,
            MemoryBasicInformation,
            &memInfo,
            sizeof(memInfo),
            &returnLength
        );

        if (!NT_SUCCESS(status)) 
        {
            if (status == STATUS_INVALID_PARAMETER || status == STATUS_ACCESS_VIOLATION) 
            {
                // Reached the end of the address space, we are now violating memory constraints
                DbgPrint("Reached end of the address space");
                break;
            }
            else 
            {
                DbgPrint("ZwQueryVirtualMemory failed: 0x%X\n", status);
                break;
            }
        }

        DbgPrint("BaseAddress: %p, RegionSize: %zu, State: %lu\n",
            memInfo.BaseAddress, memInfo.RegionSize, memInfo.State);

        // Move to the next region
        address = (PVOID)((ULONG_PTR)memInfo.BaseAddress + memInfo.RegionSize);
    }

    // Close the process handle
    ZwClose(processHandle);
    return STATUS_SUCCESS;
}

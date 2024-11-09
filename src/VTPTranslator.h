#pragma once

#include <ntddk.h>

// Kernel addresses only
void TranslateKernelAddress(
    PVOID virtualAddr
)
{
    PHYSICAL_ADDRESS physAddr = MmGetPhysicalAddress(virtualAddr);
    DbgPrint("Physical Address: 0x%llx\n", physAddr.QuadPart);
}

/*
* Requirements for User Mode addreses:
* 1. Probe and Lock Pages: Before accessing user-mode memory, 
*    you need to ensure the pages are resident in memory and locked to prevent paging.
* 
* 2. Use an MDL (Memory Descriptor List): Create an MDL to describe the memory pages.
*
* 3. Map the MDL to System Address Space: This allows you to safely access the user-mode memory from kernel mode.
* 
* 5. Translate the Address: Use MmGetPhysicalAddress on the system-space virtual address obtained from the MDL.
* 
*/

NTSTATUS TranslateUserAddress(
    PVOID virtualAddr
    , SIZE_T length
)
{
    NTSTATUS status = STATUS_SUCCESS;
    PMDL mdl = IoAllocateMdl(virtualAddr, (ULONG)length, FALSE, FALSE, NULL);
    if (mdl == NULL) 
    {
        DbgPrint("Failed to allocate MDL\n");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // NEED THIS __try __except
    // OTHERWISE YOU WILL EVENTUALLY GET AN ACCESS VIOLATION
    // AND BSOD.
    __try 
    {
        MmProbeAndLockPages(mdl, UserMode, IoReadAccess);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) 
    {
        DbgPrint("Exception in MmProbeAndLockPages\n");
        IoFreeMdl(mdl);
        return GetExceptionCode();
    }

    PVOID mappedAddr = MmGetSystemAddressForMdlSafe(mdl, NormalPagePriority);
    if (mappedAddr == NULL) 
    {
        DbgPrint("Failed to map MDL\n");
        MmUnlockPages(mdl);
        IoFreeMdl(mdl);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PHYSICAL_ADDRESS physAddr = MmGetPhysicalAddress(mappedAddr);
    DbgPrint("Physical Address: 0x%llx\n", physAddr.QuadPart);

    // Clean up
    MmUnlockPages(mdl);
    IoFreeMdl(mdl);

    return status;
}
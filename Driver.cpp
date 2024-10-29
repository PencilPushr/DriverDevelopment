#include <ntddk.h>

// Function declarations
extern "C" {
    DRIVER_INITIALIZE DriverEntry;
    DRIVER_UNLOAD DriverUnload;
}

// Main driver entry point
extern "C" NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    // Set up driver unload routine
    DriverObject->DriverUnload = DriverUnload;

    DbgPrint("Kernel Driver: Driver loaded\n");
    return STATUS_SUCCESS;
}

// Driver unload routine
void DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    DbgPrint("Kernel Driver: Driver unloaded\n");
}
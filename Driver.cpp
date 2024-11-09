#include <ntddk.h>

// Function declarations
extern "C" {
    DRIVER_INITIALIZE DriverEntry;
    DRIVER_UNLOAD DriverUnload;
}

// Forward declarations
NTSTATUS DriverDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);


extern "C" NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\MyDriver");
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyDriver");
    PDEVICE_OBJECT deviceObject = NULL;

    // Create the device object
    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &deviceObject
    );
    if (!NT_SUCCESS(status)) 
    {
        DbgPrint("Failed to create device object\n");
        return status;
    }

    // Create a symbolic link to the device
    status = IoCreateSymbolicLink(&symbolicLink, &deviceName);
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to create symbolic link\n");
        IoDeleteDevice(deviceObject);
        return status;
    }

    // Set up driver unload routine
    DriverObject->DriverUnload = DriverUnload;

    // Set up dispatch routines
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDeviceControl;

    DbgPrint("Kernel Driver: Driver loaded\n");
    return STATUS_SUCCESS;
}

// Driver unload routine
void DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\MyDriver");
    IoDeleteSymbolicLink(&symbolicLink);
    IoDeleteDevice(DriverObject->DeviceObject);
    DbgPrint("Kernel Driver: Driver unloaded\n");
}

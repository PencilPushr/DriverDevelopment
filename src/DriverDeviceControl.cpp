#include <ntddk.h>

// IOCTL codes:
// Arbitrary base
#define IOCTL_BASE FILE_DEVICE_UNKNOWN

// Define custom IOCTL codes for the functions
#define IOCTL_TRANSLATE_ADDRESS CTL_CODE(IOCTL_BASE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUMERATE_VAD     CTL_CODE(IOCTL_BASE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Function prototypes
extern "C" NTSTATUS TranslateUserAddress(PVOID virtualAddr, SIZE_T length);
extern "C" NTSTATUS EnumerateProcessMemory(HANDLE processId);

// Device control handler
NTSTATUS DriverDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG_PTR information = 0;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) 
    {
    case IOCTL_TRANSLATE_ADDRESS:
    {
        // Input buffer contains the virtual address and length
        if (irpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof(PVOID) + sizeof(SIZE_T)) 
        {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        PVOID* inputBuffer = (PVOID*)Irp->AssociatedIrp.SystemBuffer;
        PVOID virtualAddr = inputBuffer[0];
        SIZE_T length = (SIZE_T)inputBuffer[1];

        status = TranslateUserAddress(virtualAddr, length);
        break;
    }
    case IOCTL_ENUMERATE_VAD:
    {
        // Input buffer contains the process ID
        if (irpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof(HANDLE)) 
        {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        HANDLE processId = *(HANDLE*)Irp->AssociatedIrp.SystemBuffer;
        status = EnumerateProcessMemory(processId);
        break;
    }
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = information;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

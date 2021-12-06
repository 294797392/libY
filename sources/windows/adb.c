#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Windows.h>
#include <SetupAPI.h>

#include "adb/adb_api.h"

//#define ADB_CTL_GET_SERIAL_NUMBER                 16
//
//#define ADB_IOCTL_GET_SERIAL_NUMBER \
//              CTL_CODE(FILE_DEVICE_UNKNOWN, \
//                       ADB_CTL_GET_SERIAL_NUMBER, \
//                       METHOD_BUFFERED, \
//                       FILE_READ_ACCESS)


int main(int argc, char **argv)
{
	//GUID guid = ANDROID_USB_CLASS_ID;

	//ADBAPIHANDLE enum_handle = AdbEnumInterfaces(guid, true, true, true);

 //   AdbInterfaceInfo interface_info;

 //   unsigned long buf_size = 4096;

 //   
 //   ERROR_INSUFFICIENT_BUFFER
 //   // Enumerate (and count) interfaces, printing information for each found
 //   // interface.
 //   int interface_count = 0;
 //   while (AdbNextInterface(enum_handle, &interface_info, &buf_size)) {
 //       interface_count++;
 //       printf("\nFound interface %ws:", interface_info.device_name);
 //       if (interface_info.flags & SPINT_ACTIVE)
 //           printf(" ACTIVE");
 //       if (interface_info.flags & SPINT_DEFAULT)
 //           printf(" DEFAULT");
 //       if (interface_info.flags & SPINT_REMOVED)
 //           printf(" REMOVED");

 //       buf_size = 4096;

 //       //HANDLE usb_device_handle = CreateFile(interface_info.device_name,
 //       //    NULL,
 //       //    NULL,
 //       //    NULL,
 //       //    OPEN_EXISTING,
 //       //    0,
 //       //    NULL);

 //       //HANDLE usb_device_handle = CreateFile(interface_info.device_name,
 //       //    GENERIC_READ,
 //       //    FILE_SHARE_READ | FILE_SHARE_WRITE,
 //       //    NULL,
 //       //    OPEN_EXISTING,
 //       //    0,
 //       //    NULL);
 //       //if (INVALID_HANDLE_VALUE == usb_device_handle)
 //       //    return NULL;

 //       //printf(" FAILED:\nUnable to create interface by name: %u\n", GetLastError());

 //       //WCHAR serial_number[512] = { '\0' };
 //       //// Send IOCTL
 //       //DWORD ret_bytes = 0;
 //       //BOOL ret = DeviceIoControl(usb_device_handle,
 //       //    ADB_IOCTL_GET_SERIAL_NUMBER,
 //       //    NULL, 0,
 //       //    serial_number, sizeof(serial_number),
 //       //    &ret_bytes,
 //       //    NULL);

 //       //// Preserve error accross CloseHandle

 //       //printf("%u, %d\n", GetLastError(), ret_bytes);

 //       //if (INVALID_HANDLE_VALUE == usb_device_handle)
 //       //    return NULL;

 //       ADBAPIHANDLE interface_handle = AdbCreateInterfaceByName(interface_info.device_name);
 //       if (NULL == interface_handle) {
 //           printf(" FAILED:\nUnable to create interface by name: %u\n", GetLastError());
 //       }

 //       interface_handle = AdbCreateInterfaceByName(interface_info.device_name);
 //       if (NULL == interface_handle) {
 //           printf(" FAILED:\nUnable to create interface by name: %u\n", GetLastError());
 //       }

 //       int size = 512;
 //       char serial_number[512] = { '\0' };
 //       AdbGetSerialNumber(interface_handle, serial_number, &size, 1);

 //       printf("s\n");
 //   }


 //   bool ret = true;
 //   if (GetLastError() != ERROR_NO_MORE_ITEMS) {
 //       printf("\n--- AdbNextInterface failure %u", GetLastError());
 //       ret = false;
 //   }

 //   if (!AdbCloseHandle(enum_handle)) {
 //       printf("\n--- AdbCloseHandle failure %u", GetLastError());
 //       ret = false;
 //   }


	//ADBAPIHANDLE handle = AdbCreateInterface(guid, 8722, 0006, 0xFF);

	return 1;
}
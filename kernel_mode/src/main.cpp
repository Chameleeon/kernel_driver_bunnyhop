#include <ntifs.h>

void debug_print(PCSTR text) {

#ifndef DEBUG
	UNREFERENCED_PARAMETER(text);
#endif // DEBUG

	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
}

NTSTATUS DriverEntry() {
	debug_print("[+] Hello World from the kernel!");

	return STATUS_SUCCESS;
}
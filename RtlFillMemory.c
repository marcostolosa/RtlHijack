/*
Writing primitive using RtlFillMemory.
References: https://trickster0.github.io/posts/Primitive-Injection/
Explaination: RtlFillMemory takes three arguments: a pointer to the destination memory, the number
of bytes to write, and a single byte value. Writes only a byte into the specified memory block.
Author: kleiton0x00
*/

#include <windows.h>
#include <stdio.h>

typedef VOID (WINAPI *PRTLFILLMEMORY)(
    PVOID Destination,
    SIZE_T Length,
    BYTE Fill
);

int main() {
    const char* data = "This is a test string written using only RtlFillMemory.";
    SIZE_T dataLen = strlen(data);

    /* Allocate memory to write into */
    LPVOID targetMemory = VirtualAlloc(NULL, dataLen + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!targetMemory) {
        printf("[-] VirtualAlloc failed: %lu\n", GetLastError());
        return 1;
    }

    /* Get RtlFillMemory */
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    PRTLFILLMEMORY RtlFillMemory = (PRTLFILLMEMORY)GetProcAddress(ntdll, "RtlFillMemory");

    if (!RtlFillMemory) {
        printf("[-] Failed to resolve RtlFillMemory.\n");
        return 1;
    }

    /* Write each byte one at a time using RtlFillMemory */
    for (SIZE_T i = 0; i < dataLen; i++) {
        RtlFillMemory((BYTE*)targetMemory + i, 1, data[i]);
    }

    /* Null-terminate the string */
    RtlFillMemory((BYTE*)targetMemory + dataLen, 1, 0x00);

    /* Output */
    printf("[+] Written string: %s\n", (char*)targetMemory);

    VirtualFree(targetMemory, 0, MEM_RELEASE);
    return 0;
}

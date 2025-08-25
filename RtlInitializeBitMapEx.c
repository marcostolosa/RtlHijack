/*
Writing primitive using a combination of RtlInitializeBitMapEx and RtlFillMemory.
References: https://trickster0.github.io/posts/Primitive-Injection/
Explaination: First, it loops over the string in 16-byte chunks, splitting each block into 
two 8-byte values and writing them with RtlInitializeBitMapEx. The second 8-byte block is
stored into the second argument and the first 8-byte into the first one.
Then, if an extra 8-byte block remains, it writes that using RtlInitializeBitMapEx with a NULL 
second parameter.
Finally, if fewer than 8 bytes remain, it writes them one  byte at a time using RtlFillMemory
until the entire string is copied into the destination buffer.
Author: kleiton0x00
*/

#include <windows.h>
#include <stdio.h>

typedef BOOL (WINAPI *PRTLINITIALIZEBITMAPEX)(
    PVOID destination,
    PVOID second8bytes,
    PVOID first8bytes
);

VOID (WINAPI *pRtlFillMemory)(
    PVOID Destination,
    SIZE_T Length,
    BYTE Fill
) = NULL;

PRTLINITIALIZEBITMAPEX pRtlInitializeBitMapEx = NULL;

void WriteMemoryWithPrimitives(LPVOID destination, const char* str) {
    SIZE_T dataLen = strlen(str);
    SIZE_T offset = 0;

    /* Write memory using 16-byte chunks */
    while (offset + 16 <= dataLen) {
        ULONG_PTR first8 = 0, second8 = 0;
        memcpy(&first8, str + offset, 8);
        memcpy(&second8, str + offset + 8, 8);

        pRtlInitializeBitMapEx(
            (BYTE*)destination + offset,
            (PVOID)second8,
            (PVOID)first8
        );

        offset += 16;
    }

    /* Handle remaining 8 bytes */
    if (dataLen - offset >= 8) {
        ULONG_PTR first8 = 0;
        memcpy(&first8, str + offset, 8);

        pRtlInitializeBitMapEx(
            (BYTE*)destination + offset,
            NULL,
            (PVOID)first8
        );

        offset += 8;
    }

    /* Handle remaining <8 bytes */
    while (offset < dataLen) {
        pRtlFillMemory((BYTE*)destination + offset, 1, str[offset]);
        offset++;
    }
}

int main() {
    const char* data = "kleiton0x7e was here";

    /* Allocate memory to write into */
    LPVOID targetMemory = VirtualAlloc(NULL, sizeof(data), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!targetMemory) {
        printf("[-] VirtualAlloc failed: %lu\n", GetLastError());
        return 1;
    }
    
    printf("[+] Address Allocated: 0x%p\n", targetMemory);

    /* Resolve functions */
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    pRtlInitializeBitMapEx = (PRTLINITIALIZEBITMAPEX)GetProcAddress(ntdll, "RtlInitializeBitMapEx");
    pRtlFillMemory = (VOID (WINAPI *)(PVOID, SIZE_T, BYTE))GetProcAddress(ntdll, "RtlFillMemory");

    if (!pRtlInitializeBitMapEx || !pRtlFillMemory) {
        printf("[-] Failed to resolve primitives.\n");
        return 1;
    }

    /* Write string into target memory */
    WriteMemoryWithPrimitives(targetMemory, data);

    printf("[+] Memory: %s\n", (char*)targetMemory);
    
    getchar();
    VirtualFree(targetMemory, 0, MEM_RELEASE);
    return 0;
}

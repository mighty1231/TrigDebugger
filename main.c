#include <windows.h>
#include "trigger.h"
#include "stream.h"
#include "hook.h"

#ifdef _MSC_VER
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "User32")
#endif

// BOOL test(void);

void triggerHook();
extern void conditionHook();

struct stream *mainstream;
extern int CONDITIONFUNCS[];

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved) {
    // HANDLE hConsole;
    DWORD oldProtect;
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:;
            //test();
            // GetCurrentDirectory(140, (LPTSTR) 0x58A364);
            // AllocConsole();
            // hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            // freopen("CONOUT$", "w", hConsole);
            // printf("Hello World!");
            // MessageBox(NULL, "Hi", "Success", MB_OK);
            // *(int *)0x58A364 = 1;

            mainstream = (struct stream *) malloc(sizeof(mainstream));
            stream_init(mainstream);

            // code injection to trigger
            VirtualProtect((void *)0x489462, 9, PAGE_EXECUTE_READWRITE, &oldProtect);
            // trigger patch : 8 byte patch
            *(char *)0x489462 = 0xBA;
            *(int *)(0x489462+1) = (int)triggerHook; // mov edx, triggerHook
            *(short *)(0x489462+5) = 0xE2FF;         // jmp edx
            *(char *)(0x489462+7) = 0x90;            // nop, covering previous code

            VirtualProtect((void *)0x489462, 9, oldProtect, &oldProtect);
            // 
            break;
        case DLL_PROCESS_DETACH:;
            // FreeConsole();
            // MessageBox(NULL, "Bye", "Success", MB_OK);
            VirtualProtect((void *)0x489462, 9, PAGE_EXECUTE_READWRITE, &oldProtect);
            // trigger unpatch
            *(int *)0x489462 = 0x0948868B;
            *(int *)0x489466 = 0x08A80000;

            VirtualProtect((void *)0x489462, 9, oldProtect, &oldProtect);

            free(mainstream);
            break;
    }

    return TRUE;
}

// BOOL test(void) {
//     HANDLE hFile;
//     char test_data[20] = "testabcd";
//     DWORD written;

//     hFile = CreateFile("C:/Workspace/test.txt",
//                        GENERIC_WRITE,
//                        0,
//                        NULL,
//                        CREATE_NEW,
//                        FILE_ATTRIBUTE_NORMAL,
//                        NULL);
//     if (hFile == INVALID_HANDLE_VALUE)
//         return FALSE;

//     WriteFile(hFile, test_data, strlen(test_data), &written, NULL);
//     CloseHandle(hFile);
//     return TRUE;
// }
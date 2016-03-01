#include <windows.h>


BOOL test(void);

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved) {

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            test();
            GetCurrentDirectory(140, (LPTSTR) 0x58A364);
            // *(int *)0x58A364 = 1;
            break;
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
BOOL test(void) {
    HANDLE hFile;
    char test_data[20] = "testabcd";
    DWORD written;

    hFile = CreateFile("C:/Workspace/test.txt",
                       GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    WriteFile(hFile, test_data, strlen(test_data), &written, NULL);
    CloseHandle(hFile);
    return TRUE;
}
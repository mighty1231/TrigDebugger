#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <psapi.h>

#ifdef _MSC_VER
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "User32")
#pragma comment(lib, "Psapi")
#endif

#define TRIGDEBUGGER_DLL "TrigDebugger.dll"

HMODULE getModuleByName (HANDLE, char *);

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    {
        void *tokenHandle;
        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenHandle);
        TOKEN_PRIVILEGES privilegeToken;
        LookupPrivilegeValue(0, SE_DEBUG_NAME, &privilegeToken.Privileges[ 0 ].Luid);
        privilegeToken.PrivilegeCount = 1;
        privilegeToken.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(tokenHandle, 0, &privilegeToken, sizeof(TOKEN_PRIVILEGES), 0, 0);
        assert(CloseHandle(tokenHandle));
    }

    DWORD processID;
    {
        HWND hWnd = FindWindow("SWarClass", "Brood War");
            #define STARCRAFT_IS_NOT_OPEN hWnd != NULL
            assert(STARCRAFT_IS_NOT_OPEN);
        GetWindowThreadProcessId(hWnd, &processID);
    }

    HMODULE module;
    HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processID);
        #define INSUFFICIENT_PERMISSIONS process != NULL
        assert(INSUFFICIENT_PERMISSIONS);

    {
        char fullPath[ MAX_PATH ];
        assert(GetFullPathName(TRIGDEBUGGER_DLL, MAX_PATH, fullPath, NULL));

        void *ptr = VirtualAllocEx(process, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        assert(ptr != NULL);

        assert(WriteProcessMemory(process, ptr, fullPath, MAX_PATH, NULL));

        HANDLE thread;
        module = getModuleByName(process, fullPath);
        if(module) 
            // eject
            thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, module, 0, NULL);
        else
            // inject
            thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, ptr, 0, NULL);

        assert(thread != NULL);
        WaitForSingleObject(thread, INFINITE);
        DWORD ret;
        assert(GetExitCodeThread(thread, &ret));
            #define MAPCHECK_DLL_NOT_FOUND ret
            assert(MAPCHECK_DLL_NOT_FOUND);
        assert(CloseHandle(thread));

        assert(VirtualFreeEx(process, ptr, 0, MEM_RELEASE));
    }

    assert(CloseHandle(process));

    MessageBox(NULL, module ? "Successfully EJECTED "TRIGDEBUGGER_DLL
                            : "Successfully INJECTED "TRIGDEBUGGER_DLL, "Success", MB_OK);
    return 0;
}

// Captured from MSDN example, "Enumerating All Modules For a Process"
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682621(v=vs.85).aspx
HMODULE getModuleByName (HANDLE hProcess, char *chModName) {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    int i;

    // Get a list of all the modules in this process.
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        char szModName[MAX_PATH];
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {

            // Get the full path to the module's file.
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName)))
            {
                // Print the module name and handle value.
                if (!strcmp(szModName, chModName))
                    return hMods[i];
            }
        }
    }
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>

#ifdef _MSC_VER
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "User32")
#endif

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
	{
		void *tokenHandle;
		OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenHandle );
		TOKEN_PRIVILEGES privilegeToken;
		LookupPrivilegeValue( 0, SE_DEBUG_NAME, &privilegeToken.Privileges[ 0 ].Luid );
		privilegeToken.PrivilegeCount = 1;
		privilegeToken.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges( tokenHandle, 0, &privilegeToken, sizeof( TOKEN_PRIVILEGES ), 0, 0 );
		assert( CloseHandle( tokenHandle ) );
	}

	DWORD processID;
	{
		HWND hWnd = FindWindow( "SWarClass", "Brood War" );
			#define STARCRAFT_IS_NOT_OPEN hWnd != NULL
			assert( STARCRAFT_IS_NOT_OPEN );
		GetWindowThreadProcessId( hWnd, &processID );
	}

	HANDLE process = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processID );
		#define INSUFFICIENT_PERMISSIONS process != NULL
		assert( INSUFFICIENT_PERMISSIONS );

	{
		char fullPath[ MAX_PATH ];
		assert( GetFullPathName( "TrigDebugger.dll", MAX_PATH, fullPath, NULL ) );

		void *ptr = VirtualAllocEx( process, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
		assert( ptr != NULL );

		assert( WriteProcessMemory( process, ptr, fullPath, MAX_PATH, NULL ) );

		HANDLE thread = CreateRemoteThread( process, NULL, 0, ( LPTHREAD_START_ROUTINE )LoadLibrary, ptr, 0, NULL );
		assert( thread != NULL );
		WaitForSingleObject( thread, INFINITE );
		DWORD ret;
		assert( GetExitCodeThread( thread, &ret ) );
			#define MAPCHECK_DLL_NOT_FOUND ret
			assert( MAPCHECK_DLL_NOT_FOUND );
		assert( CloseHandle( thread ) );

		assert( VirtualFreeEx( process, ptr, 0, MEM_RELEASE ) );
	}

	assert( CloseHandle( process ) );

	MessageBox( NULL, "Successfully injected TrigDebugger.dll.", "Success", MB_OK );

	return 0;
}

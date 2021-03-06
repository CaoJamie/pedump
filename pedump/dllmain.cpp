// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "pedump.h"

static VOID(WINAPI* TrueGetSystemTimeAsFileTime)(LPFILETIME lpSystemTimeAsFileTime) = GetSystemTimeAsFileTime;


BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH) {
		DetourRestoreAfterWith();
		init();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)TrueGetSystemTimeAsFileTime, GetSystemTimeAsFileTimeHook);
		DetourTransactionCommit();
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)TrueGetSystemTimeAsFileTime, GetSystemTimeAsFileTimeHook);
		DetourTransactionCommit();
	}
	return TRUE;
}


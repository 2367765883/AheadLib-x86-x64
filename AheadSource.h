#pragma once


WCHAR *g_asmFileHeader = LR"(
;
; created by AheadLib
; github:https://github.com/strivexjun/AheadLib-x86-x64
;
; �� .asm �ļ���ӵ�����һ��
; �Ҽ������ļ�-����-����-
; ������:�Զ������ɹ���
; ���������ų�:��

; Ȼ����������������
; ������: ml64 /Fo $(IntDir)%(fileName).obj /c /Cp %(fileName).asm
; ���: $(IntDir)%(fileName).obj;%(Outputs)
; ���Ӷ���: ��
;


)";

WCHAR *g_fileHeader = LR"(
//
// created by AheadLib
// github:https://github.com/strivexjun/AheadLib-x86-x64
//

#include <windows.h>
#include <Shlwapi.h>

#pragma comment( lib, "Shlwapi.lib")

)";


WCHAR *g_Free = LR"(
static
HMODULE g_OldModule = NULL;

VOID WINAPI Free()
{
	if (g_OldModule)
	{
		FreeLibrary(g_OldModule);
	}
}

)";

WCHAR *g_GetAddress = LR"(
FARPROC WINAPI GetAddress(PCSTR pszProcName)
{
	FARPROC fpAddress;
	CHAR szProcName[64];
	TCHAR tzTemp[MAX_PATH];

	fpAddress = GetProcAddress(g_OldModule, pszProcName);
	if (fpAddress == NULL)
	{
		if (HIWORD(pszProcName) == 0)
		{
			wsprintfA(szProcName, "#%d", pszProcName);
			pszProcName = szProcName;
		}

		wsprintf(tzTemp, TEXT("�޷��ҵ����� %hs,�����޷���������"), pszProcName);
		MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		ExitProcess(-2);
	}
	return fpAddress;
}

)";

WCHAR *g_Load = LR"(
BOOL WINAPI Load()
{
	TCHAR tzPath[MAX_PATH];
	TCHAR tzTemp[MAX_PATH * 2];

	//
	// �����Ƿ��ϵͳĿ¼��ǰĿ¼����ԭʼDLL
	//
	GetSystemDirectory(tzPath, MAX_PATH);
	lstrcat(tzPath, TEXT("\\AHEADLIB_XXXXXX.dll"));

	g_OldModule = LoadLibrary(tzPath);
	if (g_OldModule == NULL)
	{
		wsprintf(tzTemp, TEXT("�޷��ҵ�ģ�� %s,�����޷���������"), tzPath);
		MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
	}

	return (g_OldModule != NULL);

}

)";

WCHAR *g_ThreadProc = LR"(
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter)
{
	HANDLE hProcess;

	PVOID addr1 = reinterpret_cast<PVOID>(0x00401000);
	BYTE data1[] = { 0x90, 0x90, 0x90, 0x90 };

	//
	// �ƹ�VMP3.x ���ڴ汣��
	//
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, GetCurrentProcessId());
	if (hProcess)
	{
		WriteProcessMemory(hProcess, addr1, data1, sizeof(data1), NULL);

		CloseHandle(hProcess);
	}

	return 0;
}

)";

WCHAR *g_Dllmain = LR"(
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		if (Load() && Init())
		{
			TCHAR szAppName[MAX_PATH] = TEXT("MyApp.exe");
			TCHAR szCurName[MAX_PATH];

			GetModuleFileName(NULL, szCurName, MAX_PATH);
			PathStripPath(szCurName);

			//�Ƿ��ж�����������
			if (StrCmpI(szAppName, szAppName) == 0)
			{
				//���������̻߳�����������
				HANDLE hThread = CreateThread(NULL, NULL, ThreadProc, NULL, NULL, NULL);
				if (hThread)
				{
					CloseHandle(hThread);
				}
			}
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		Free();
	}

	return TRUE;
}

)";
#include "pch.h"
#include "Dbg_Tool.h"
#include <Windows.h>



Dbg_Tool::Dbg_Tool()
{
}


Dbg_Tool::~Dbg_Tool()
{
}

BOOL Dbg_Tool::inject(HANDLE hProcess, char * shellcode, unsigned int code_size)
{
	SIZE_T Count = 0;
	LPVOID pMem = VirtualAllocEx(hProcess, NULL, code_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, pMem, shellcode, code_size, &Count);
	HANDLE hTread = CreateRemoteThread(
		hProcess,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)pMem,
		NULL,
		NULL,
		NULL
	);
	if (hTread == INVALID_HANDLE_VALUE || hTread == NULL)
	{
		return FALSE;
	}

	WaitForSingleObject(hTread, -1);
	CloseHandle(hTread);
	CloseHandle(hProcess);

	return TRUE;

	return 0;
}




//void _declspec(naked) shellcode()
//{
//	_asm {
//		pushad;				     // ���滷��
//		mov eax, 0xFFFFFFFF;     // LoadLibrary��ַ
//
//		call Next;				// ��ȡEip
//	Next:
//		pop esi;
//		add esi, 0xF;			// ��ȡDLL·��
//
//		push esi;				// dll·����
//		call eax;				// ����LoadLibrary
//
//		popad;					// �ָ�����
//
//
//		mov eax, 0xFFFFFFFF;	// ���淵��������ĵ�ַ
//		push eax;
//		ret;					// ��ת��������
//
//		// dll·���ַ���
//		_asm __emit(0x11);		_asm __emit(0x11);         _asm __emit(0x11);       _asm __emit(0x11);
//		_asm __emit(0x11);      _asm __emit(0x11);		   _asm __emit(0x11);		_asm __emit(0x11);
//	}
//}
//

DWORD Dbg_Tool::inject1(HANDLE hprocess,DWORD Eip,char *dllPath) {

	DWORD dwSize;
	DWORD dwAddr;

	// 1. Ŀ���������ռ�
	char * Target = (char*)VirtualAllocEx(hprocess,0, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// 2.����shellcode 
	const char *shellcode = "\x60\xB8\xFF\xFF\xFF\xFF\xE8\x00\x00\x00\x00\x5E\x83\xC6\x0F\x56\xFF\xD0\x61\xB8\xFF\xFF\xFF\xFF\x50\xC3\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11";

	// 2.1 ����shellcode
	WriteProcessMemory(hprocess,Target, shellcode, 0x27, &dwSize);

	// 2.2 ����ַ
	// 2.2.1 ��ȡLoadLibraryA������ַ
	HMODULE hmod = LoadLibraryA("kernel32.dll");
	dwAddr = (DWORD)GetProcAddress(hmod, "LoadLibraryA");

	// ���LoadLibraryA������ַ
	WriteProcessMemory(hprocess, Target + 2, &dwAddr, 4, &dwSize);

	// ���dll·��
	WriteProcessMemory(hprocess,Target + 26,dllPath,strlen(dllPath)+1,&dwSize);

	// ��ת��������ַ  eip
	WriteProcessMemory(hprocess, Target + 20, &Eip, 4, &dwSize);

	// Ŀ����̿ռ��ַ
	return (DWORD)Target;

}
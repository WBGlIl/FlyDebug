#pragma once
#include <Windows.h>
#include "Dbg_Context.h"

class Dbg_Tool
{
public:
	Dbg_Tool();
	~Dbg_Tool();

	BOOL inject(HANDLE hProces, char *shellcode, unsigned int code_size);

	// ����Ŀ���������Ŀռ�,  ��Ҫ�޸�eipָ�����Ƿ��صĵ�ַ
	DWORD inject1(HANDLE hprocess, DWORD Eip, char *dllPath);

};


#pragma once
#include <Windows.h>
#include "Capstone/include/capstone.h"
#pragma comment(lib,"capstone/capstone.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"libcmtd.lib\"")

class Dbg_Disasm {
public:

	Dbg_Disasm();
	~Dbg_Disasm();

public:
	// ���ڳ�ʼ�����ڴ����ľ��
	static csh Handle;
	static cs_opt_mem OptMem;

	// ���ڳ�ʼ���ĺ���
	static void Init();

	// ����ִ�з����ĺ���
	static void DisAsm(HANDLE Handle, LPVOID Addr, DWORD Count);

};
#pragma once
#include "Dbg_Context.h"
#include "dbghelp.h"
#pragma comment(lib,"Dbghelp.lib")

class Dbg_Cmd
{
public:
	Dbg_Cmd(TCHAR *Name);
	~Dbg_Cmd();

	// ͨ������
	// ��������ϵ�
	static BOOL Dbg_SetBp(LPVOID lpAddr, BOOL IsForever, Dbg_Context * Conetxt);

	// ���õ����ϵ�
	static BOOL Dbg_SetTF(Dbg_Context * Conetxt);

	// �ָ�����ϵ�
	static BOOL Dbg_FixBp(LPVOID lpAddr, Dbg_Context * Conetxt);

	// ��������ϵ�
	static BOOL Dbg_EnableBp(DWORD i, Dbg_Context * Conetxt);

	// ����EIP
	static BOOL Dbg_SetEip(Dbg_Context * Conetxt, DWORD Eip);

	// ��ȡ��������ͨ����ַ
	static BOOL GetFunctionName(HANDLE hProcess, SIZE_T nAddress, const TCHAR * pBuff);

	// ͨ�����ֻ�ȡ��ַ
	static SIZE_T FindApiAddress(HANDLE hProcess, const TCHAR * pszName);



// ����� һ��
	virtual bool Run(TCHAR * szCmd, Dbg_Context * Conetxt);

// ����� ����
	virtual bool Handler(TCHAR * szCmd,Dbg_Context * Conetxt);

// �������֣�ɾ��ʱ��
	TCHAR m_Name[100];
};


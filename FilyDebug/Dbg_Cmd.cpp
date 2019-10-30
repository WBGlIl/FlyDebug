#include "pch.h"
#include "Dbg_Cmd.h"
#include <tchar.h>




Dbg_Cmd::Dbg_Cmd(TCHAR *Name)
{
	_stprintf_s(m_Name,100, _T("%s"), Name);
}

Dbg_Cmd::~Dbg_Cmd()
{
}


bool Dbg_Cmd::Run(TCHAR * szCmd, Dbg_Context * Conetxt)
{
	//��������
	TCHAR szCmdLine[250];

	// ��������
	_stscanf_s(szCmd, _T("%s"), szCmdLine,250);

	// �����������
	// ����
	if (_tcsicmp(szCmdLine, _T("t")) == 0)
	{
		Dbg_SetTF(Conetxt);
		Conetxt->IsInputFlags = FALSE;
		// ����ֹͣ��־
		Conetxt->SingleStep = TRUE;
		return true;
	}
	// ��������
	else if (_tcsicmp(szCmdLine, _T("g")) == 0)
	{
		Conetxt->IsInputFlags = FALSE;
		return true;
	}
	// �ϵ�
	else if (_tcsicmp(szCmdLine, _T("bp")) == 0)
	{
		Conetxt->IsInputFlags = TRUE;
		// ��ȡ��ַ
		DWORD addr;
		_tscanf_s(_T("%X"),&addr);
		Dbg_SetBp((LPVOID)addr, true, Conetxt);
		return true;
	}
	// api��ַ
	else if (_tcsicmp(szCmdLine, _T("api")) == 0) {
		Conetxt->IsInputFlags = TRUE;
			DWORD addr;
			TCHAR szApiName[250];
			_tscanf_s(_T("%s"),szApiName,250);
			addr = FindApiAddress(Conetxt->CurPsHandle, szApiName);
			_tprintf(_T("%s : [0x%08x]"), szApiName, addr);
			return true;
	}

	else {
		// ��û�д���
		return Handler(szCmd,Conetxt);
	}

	return false;
}

bool Dbg_Cmd::Handler(TCHAR * szCmd,Dbg_Context * Conetxt)
{
	return true;
}



BOOL Dbg_Cmd::Dbg_SetBp(LPVOID lpAddr, BOOL IsForever, Dbg_Context * Conetxt)
{
	// ����һ������ϵ�
	Dbg_BreakPoint bp;
	bp.isForever = IsForever;
	bp.lpAddr = lpAddr;

	//��������ϵ�
	bp.Type = Dbg_BreakPoint::SOFTWARE;

	//����Ƿ����ظ��ϵ�
	for (auto bps : Conetxt->m_BreakList)
	{
		if (bps.lpAddr == lpAddr)
		{
			return FALSE;
		}
	}

	// �޸�Ŀ����̵�ַ
	DWORD OldProtect;
	DWORD Size;
	VirtualProtectEx(Conetxt->CurPsHandle, lpAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect);
	ReadProcessMemory(Conetxt->CurPsHandle, lpAddr, bp.buff, 1, &Size);
	WriteProcessMemory(Conetxt->CurPsHandle, lpAddr, "\xcc", 1, &Size);
	VirtualProtectEx(Conetxt->CurPsHandle, lpAddr, 1, OldProtect, &OldProtect);

	//�����б�
	Conetxt->m_BreakList.push_back(bp);
	return TRUE;
}

BOOL Dbg_Cmd::Dbg_SetTF(Dbg_Context * Conetxt)
{
	// ���߳̾��
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Conetxt->dwTid);

	CONTEXT ct = { CONTEXT_CONTROL };
	// ��ȡ�߳�������
	GetThreadContext(hThread, &ct);
	// ���õ���
	ct.EFlags |= 0x100;
	SetThreadContext(hThread, &ct);

	//�رվ��
	CloseHandle(hThread);
	return TRUE;
}

BOOL Dbg_Cmd::Dbg_FixBp(LPVOID lpAddr, Dbg_Context * Conetxt)
{
	// �����ϵ�
	for (DWORD i = 0; i < Conetxt->m_BreakList.size(); i++)
	{
		// ���������ϵ㣬���д���  ͨ����ַ�ҵ�����Ӧ�Ķϵ�λ��
		if (Conetxt->m_BreakList[i].Type == Dbg_BreakPoint::SOFTWARE
			&& Conetxt->m_BreakList[i].lpAddr == lpAddr)
		{
			// �ָ�ԭʼ�ֽ�
			DWORD OldProtect;
			DWORD Size;
			VirtualProtectEx(Conetxt->CurPsHandle, lpAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect);
			WriteProcessMemory(Conetxt->CurPsHandle, lpAddr, Conetxt->m_BreakList[i].buff, 1, &Size);
			VirtualProtectEx(Conetxt->CurPsHandle, lpAddr, 1, OldProtect, &OldProtect);

			// ͬʱeip -1 
			// ���߳̾��
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Conetxt->dwTid);
			CONTEXT ct = { CONTEXT_FULL };
			// ��ȡ�߳�������
			GetThreadContext(hThread, &ct);
			ct.Eip -= 1;
			SetThreadContext(hThread, &ct);
			//�رվ��
			CloseHandle(hThread);

			// ���ý��������־
			Conetxt->IsInputFlags = TRUE;

			//�ϵ��Ƿ�������
			if (Conetxt->m_BreakList[i].isForever)
			{
				// ���õ���
				Dbg_SetTF(Conetxt);
				// ���� ������ָ�ccʱ���±�
				Conetxt->FixBreakIndex = i;
				// ��������
				Conetxt->TFFixFlags = Dbg_BreakPoint::SOFTWARE;
			}
			else
			{
				// ɾ������ϵ�
				Conetxt->m_BreakList.erase(Conetxt->m_BreakList.begin() + i);
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Dbg_Cmd::Dbg_EnableBp(DWORD i, Dbg_Context * Conetxt)
{
	// ����ʧ��
	if (i<0 || i>Conetxt->m_BreakList.size())
	{
		return FALSE;
	}

	// �޸�Ŀ����̵�ַ
	DWORD OldProtect;
	DWORD Size;
	VirtualProtectEx(Conetxt->CurPsHandle, Conetxt->m_BreakList[i].lpAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect);
	WriteProcessMemory(Conetxt->CurPsHandle, Conetxt->m_BreakList[i].lpAddr, "\xcc", 1, &Size);
	VirtualProtectEx(Conetxt->CurPsHandle, Conetxt->m_BreakList[i].lpAddr, 1, OldProtect, &OldProtect);

	return TRUE;
}


BOOL Dbg_Cmd::Dbg_SetEip(Dbg_Context * Conetxt,DWORD Eip) {

	// ���߳̾��
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Conetxt->dwTid);

	CONTEXT ct = { CONTEXT_ALL };
	// ��ȡ�߳�������
	GetThreadContext(hThread, &ct);
	// ����Eip
	ct.Eip = Eip;
	SetThreadContext(hThread, &ct);

	//�رվ��
	CloseHandle(hThread);
	return TRUE;

}



BOOL Dbg_Cmd::GetFunctionName(HANDLE hProcess, SIZE_T nAddress, const TCHAR  *buff)
{
	DWORD64  dwDisplacement = 0;
	char buffer[sizeof(SYMBOL_INFOW) + MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFOW pSymbol = (PSYMBOL_INFOW)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFOW);
	pSymbol->MaxNameLen = MAX_SYM_NAME;

	if (!SymFromAddrW(hProcess, nAddress, &dwDisplacement, pSymbol))
		return FALSE;
	memcpy(buffer, pSymbol->Name, sizeof(pSymbol->Name));
	return TRUE;

}

// ͨ�����ֻ�ȡ��ַ
SIZE_T Dbg_Cmd::FindApiAddress(HANDLE hProcess, const TCHAR * pszName)
{
	//MultiByteToWideChar(CP_ACP, NULL, pszName, 50, buff, 50);
	DWORD64  dwDisplacement = 0;
	TCHAR buffer[sizeof(SYMBOL_INFOW) + MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFOW pSymbol = (PSYMBOL_INFOW)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFOW);
	pSymbol->MaxNameLen = MAX_SYM_NAME;

	if (!SymFromNameW(hProcess, pszName, pSymbol))
	{
		return 0;
	}
	return (SIZE_T)pSymbol->Address;
}
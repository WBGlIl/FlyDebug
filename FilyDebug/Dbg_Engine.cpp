#include "pch.h"
#include <tchar.h>
#include<cstdio>
#include "Dbg_Engine.h"
#include "Dbg_Disasm.h"
#include "Dbg_Tool.h"
#include "Dbg_Cmd.h"



//1. ����ͷ�ļ�
#ifdef _WIN64 // 64λƽ̨���������Զ����������
#pragma 
comment(lib, "capstone/lib/capstone_x64.lib")
#else
#pragma comment(lib,"capstone/capstone.lib")
#endif // _64



Dbg_Engine::Dbg_Engine()
{
	m_context = new Dbg_Context();
	m_Command = new Dbg_Command();
	// ��ʼ�� �������
	Dbg_Disasm::Init();

	Dbg_Cmd* cmd = new Dbg_Cmd((TCHAR*)_T("default"));
	// ���Ĭ�������
	m_Command->Dbg_AddCmd(cmd);

}

Dbg_Engine::~Dbg_Engine()
{
	if (m_context)
	{
		delete m_context;
		m_context = nullptr;
	}
}

bool Dbg_Engine::Dbg_CreateDebug(TCHAR * szPath)
{
	// �������·��
	lstrcpy(m_szPaht, szPath);

	// ������̴����ɹ������ڽ��ս����̵߳ľ����id
	PROCESS_INFORMATION process_info = { 0 };
	STARTUPINFO startup_info = { sizeof(STARTUPINFO) };

	// ��������
	BOOL result = CreateProcess(
		szPath,
		nullptr, NULL, NULL, FALSE,
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,
		NULL, NULL, &startup_info, &process_info);

	// ������̴����ɹ��ˣ��͹رն�Ӧ�ľ������ֹ���й¶
	if (result == TRUE)
	{
		// ���ݾ����������ر�ʱ�ٽ����ͷ�
		m_context->CurPsHandle = process_info.hProcess;
		m_context->CurTsHandle  = process_info.hThread;
	}




	return result;
}

bool Dbg_Engine::Dbg_ActiveDebug(DWORD dwPid)
{
	m_Method = DBG_ACTIVE;			// ������Է�ʽ

	// ����pid
	m_context->dwPid = dwPid;

	// ���ӽ���
	BOOL result = DebugActiveProcess(dwPid);

	// ���ؽ��
	return result;
}

bool Dbg_Engine::Dbg_Run()
{
	DWORD dwContinueStaus = DBG_CONTINUE;
	DEBUG_EVENT DbgEvent;



	// �ȴ������¼�
	while (WaitForDebugEvent(&DbgEvent, -1))
	{
		// ��������߳�ID
		m_context->dwPid = DbgEvent.dwProcessId;
		m_context->dwTid = DbgEvent.dwThreadId;


		// �ַ������쳣��Ϣ
		Dbg_DispatchDebugEvent(&DbgEvent);
		
		// �������ϵͳ���ص�ǰ�Ĵ�����
		ContinueDebugEvent(
			m_context->dwPid,
			m_context->dwTid,
			m_context->dwContinueStaus);
	}

	return TRUE;
}

DWORD Dbg_Engine::Dbg_DispatchDebugEvent(DEBUG_EVENT * dbg_event)
{
	
	// ���浱ǰ��������
	m_context->DebugType = dbg_event->dwDebugEventCode;

	switch (dbg_event->dwDebugEventCode)
	{
	case EXCEPTION_DEBUG_EVENT:							// �쳣�����¼�
		Dbg_DispatchExcetpion(&dbg_event->u.Exception);	// �ַ������쳣�¼�
		break;
	case CREATE_THREAD_DEBUG_EVENT: // �̴߳����¼�
		printf("CREATE_THREAD_DEBUG_EVENT\n"); break;
	case CREATE_PROCESS_DEBUG_EVENT:// ���̴����¼�

		// ����OEP
		m_context->EntryPointAddr = dbg_event->u.CreateProcessInfo.lpStartAddress;
		printf("CREATE_PROCESS_DEBUG_EVENT\n"); break;
	case EXIT_THREAD_DEBUG_EVENT:   // �˳��߳��¼�
		printf("EXIT_THREAD_DEBUG_EVENT\n"); break;
	case EXIT_PROCESS_DEBUG_EVENT:  // �˳������¼�
		printf("EXIT_PROCESS_DEBUG_EVENT\n"); break;
	case LOAD_DLL_DEBUG_EVENT:      // ӳ��DLL�¼�
		printf("LOAD_DLL_DEBUG_EVENT\n"); break;
	case UNLOAD_DLL_DEBUG_EVENT:    // ж��DLL�¼� 
		printf("UNLOAD_DLL_DEBUG_EVENT\n"); break;
	case OUTPUT_DEBUG_STRING_EVENT: // ��������¼�
		printf("OUTPUT_DEBUG_STRING_EVENT\n"); break;
	case RIP_EVENT:                 // RIP�¼�(�ڲ�����)
		printf("RIP_EVENT\n"); break;
	}
	return TRUE;
}

DWORD Dbg_Engine::Dbg_DispatchExcetpion(EXCEPTION_DEBUG_INFO * pException_Info)
{	
	// �쳣�����¼�
	m_context->dwContinueStaus = DBG_CONTINUE;

	// ��ȡ�쳣����
	m_context->ExceptionCode = pException_Info->ExceptionRecord.ExceptionCode;
	// �쳣��ַ
	m_context->ExceptionAddr = pException_Info->ExceptionRecord.ExceptionAddress;

	
	// ���ݴ�����д���
	switch (m_context->ExceptionCode)
	{
		// �豸�����쳣: ���ڴ�ϵ����
	case EXCEPTION_ACCESS_VIOLATION:
		
		//�����ڴ�ϵ�
			Dbg_OnAccessExceptionHander(&pException_Info->ExceptionRecord);
		break;

		// �ϵ��쳣: int 3(0xCC) �ᴥ�����쳣
	case EXCEPTION_BREAKPOINT:
	{
		// ��������ϵ�
			Dbg_OnSoftwareExceptionHandler(&pException_Info->ExceptionRecord);
		break;
	}

	// Ӳ���ϵ��¼�: TF����  DrN�ϵ�
	case EXCEPTION_SINGLE_STEP:
		   //�������ϵ�
			Dbg_OnSingleStepExceptionHandler(&pException_Info->ExceptionRecord);
		break;
	}

	// �Ƿ������������
	while (m_context->IsInputFlags)
	{
		Dbg_Disasm::DisAsm(m_context->CurPsHandle, m_context->ExceptionAddr, 10);
		TCHAR buff[250];
		// ���������
		_tscanf_s(_T("%s"), buff, 250);
		m_Command->Dbg_RunCmd(buff, m_context);
	}

	return TRUE;
}

DWORD Dbg_Engine::Dbg_OnSoftwareExceptionHandler(EXCEPTION_RECORD * pException_recode)
{

	// ϵͳ�ϵ�
	if (m_context->IsSystemBp)
	{
		m_context->IsSystemBp = FALSE;
		// ��һ��ϵͳ�ϵ�  ���⴦��
		Dbg_Cmd::Dbg_SetBp(m_context->EntryPointAddr, FALSE,m_context);
		// �ָ���ϵͳ
		m_context->dwContinueStaus = DBG_CONTINUE;
		// ����������
		m_context->IsInputFlags = FALSE;

		// ���ط���
		SymInitialize(m_context->CurPsHandle, NULL, TRUE);

		// ע�뷴����
		Dbg_Tool tool;
		DWORD NewEip = tool.inject1(m_context->CurPsHandle, (DWORD)pException_recode->ExceptionAddress + 1, (char*)"C:\\Users\\42140\\Desktop\\HOOKAPI.dll");
		Dbg_Cmd::Dbg_SetEip(m_context, NewEip);

		return TRUE;
	}

	//�����ϵ� ��Ҫ�޸�
	Dbg_Cmd::Dbg_FixBp(pException_recode->ExceptionAddress, m_context);

	return TRUE;
}

DWORD Dbg_Engine::Dbg_OnSingleStepExceptionHandler(EXCEPTION_RECORD * pException_recode)
{
	// �������¼�

	// 1.��������  �жϵ�������  ����ָ�������Ӳ���ָ��������ڴ�ָ�������
	// 2.��ͨ����

	// 1.1 ����ָ�����
	if (m_context->TFFixFlags == Dbg_BreakPoint::SOFTWARE)
	{
		// ��������ϵ�
		Dbg_Cmd::Dbg_EnableBp(m_context->FixBreakIndex, m_context);
		m_context->IsInputFlags = FALSE;
	}
	// 1.2 Ӳ���ָ�����
	else if (m_context->TFFixFlags == Dbg_BreakPoint::HARD)
	{
		// ...
	}
	// 1.3 �ڴ�ָ�����
	else if (m_context->TFFixFlags == Dbg_BreakPoint::MEMORY)
	{
		// ...
	}

	//2.1 ������ͨ���� "t"����
	if (m_context->SingleStep)
	{
		// �رյ���ֹͣ
		m_context->SingleStep = FALSE;
		// ����t��Ҫ����
		m_context->IsInputFlags = TRUE;
	}



	return TRUE;
}

DWORD Dbg_Engine::Dbg_OnAccessExceptionHander(EXCEPTION_RECORD * pException_recode)
{
	return 0;
}

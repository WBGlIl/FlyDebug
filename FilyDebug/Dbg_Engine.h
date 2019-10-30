#pragma once
#include <Windows.h>

#include "Dbg_BreakPoint.h"
#include "Dbg_Command.h"
#include "Dbg_Plugin.h"
#include "Dbg_Context.h"


class Dbg_Engine
{
public:
	Dbg_Engine();
	~Dbg_Engine();

	// �������Դ������̷�ʽ����
	bool Dbg_CreateDebug(TCHAR * szPath);

	// �������Ը��ӷ�ʽ����
	bool Dbg_ActiveDebug(DWORD dwPid);

	// ����ѭ��
	bool Dbg_Run();

	// �����¼�����
	DWORD Dbg_DispatchDebugEvent(DEBUG_EVENT * dbg_event);

	// �쳣�¼�����
	DWORD Dbg_DispatchExcetpion(EXCEPTION_DEBUG_INFO * pException_Info);

	// ����쳣����
	DWORD Dbg_OnSoftwareExceptionHandler(EXCEPTION_RECORD * pException_recode);

	// ������Ӳ�����쳣����
	DWORD Dbg_OnSingleStepExceptionHandler(EXCEPTION_RECORD * pException_recode);

	// �����쳣����
	DWORD Dbg_OnAccessExceptionHander(EXCEPTION_RECORD * pException_recode);



public :
	// Ŀ�����·��
	TCHAR m_szPaht[MAX_PATH];

	// ���Է���ö�ٷ�ʽ
	typedef enum DEBUG_METHOD {
		DBG_CREATE,		// ������ʽ
		DBG_ACTIVE		// ���ӷ�ʽ
	}DEBUG_METHOD;
	// Ŀ����Է�ʽ
	DEBUG_METHOD m_Method;

	// ��ǰ�����������е���Ϣ������������ͨ
	Dbg_Context *m_context;

	// ���������
	Dbg_Command *m_Command; //����  ����ص������ص�����ԭ��("cmd",context);



	// ģ���б�

	// �߳��б�


};


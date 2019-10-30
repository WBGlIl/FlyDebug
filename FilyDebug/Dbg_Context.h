#pragma once
#include <Windows.h>
#include "Dbg_BreakPoint.h"
#include <vector>
using std::vector;


class Dbg_Context
{
public:
	Dbg_Context();
	~Dbg_Context();

	// �쳣��ַ
	LPVOID ExceptionAddr =0;
	// �쳣����
	DWORD DebugType =0;
	// �쳣����
	DWORD ExceptionCode = 0;
	// �����쳣״̬
	DWORD dwContinueStaus = DBG_CONTINUE;

	// �ϵ�ָ���־
	Dbg_BreakPoint::BREAK_POINT_TYPE TFFixFlags;
	// ����ֹͣ��־
	BOOL SingleStep = FALSE;


	// �ϵ�λ�� ����ִ�к� �ָ��ϵ���±�
	int FixBreakIndex =-1;

	// �����־
	BOOL IsInputFlags = TRUE;
	// Ŀ�����PID
	DWORD dwPid = 0 ;
	// Ŀ�����TID
	DWORD dwTid = 0;
	// Ŀ�����ȫ�־��
	HANDLE  CurPsHandle = NULL;
	HANDLE  CurTsHandle = NULL;
	// ϵͳ�ϵ�
	BOOL IsSystemBp = TRUE;
	// ������ڵ�
	LPVOID EntryPointAddr = 0;

	// ����б�
	PVOID  PluginList = nullptr;
	// ģ���б�
	PVOID  ModlueList= nullptr;
	// �߳��б�
	PVOID  ThredList = nullptr;

	// ����ϵ��б�
	vector<Dbg_BreakPoint> m_BreakList;

};


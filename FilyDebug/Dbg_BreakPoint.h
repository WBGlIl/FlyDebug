#pragma once
#include<Windows.h>

class Dbg_BreakPoint
{
public:
	Dbg_BreakPoint();
	~Dbg_BreakPoint();

	// �ϵ�����
	typedef enum {
		SOFTWARE = 1,	// ����ϵ�
		HARD,			// Ӳ���ϵ�
		MEMORY,			// �ڴ�ϵ�
		CONDITION,		// �����ϵ�
	} BREAK_POINT_TYPE;
	BREAK_POINT_TYPE Type;

	// ��ַ
	LPVOID lpAddr;

	// ����
	DWORD Length;

	// �Ƿ����öϵ�
	BOOL isForever;

	// ��Ҫ��������� ����ϵ�ʹ��
	BYTE buff[10];


};


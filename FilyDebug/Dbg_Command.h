#pragma once
#include <Windows.h>
#include "Dbg_Context.h"
#include "Dbg_Cmd.h"
#include <vector>
using std::vector;

class Dbg_Command
{
public:
	Dbg_Command();
	~Dbg_Command();

// �����

	// �������
	bool Dbg_AddCmd(Dbg_Cmd* pCmd);

	// ִ������ 
	bool Dbg_RunCmd(TCHAR * szCmd, Dbg_Context * Conetxt);

	// ɾ������
	bool Dbg_DelCmd(TCHAR * szCmdName);

	// �����б�
	vector<Dbg_Cmd*> m_CmdList;

};


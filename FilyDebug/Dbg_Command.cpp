#include "pch.h"
#include "Dbg_Command.h"
#include <tchar.h>




Dbg_Command::Dbg_Command()
{
}


Dbg_Command::~Dbg_Command()
{
	if (!m_CmdList.empty())
	{
		for (DWORD i=0;i<m_CmdList.size();i++)
		{
			delete m_CmdList[i];
		}
		m_CmdList.clear();
	}
}



bool Dbg_Command::Dbg_AddCmd(Dbg_Cmd * pCmd)
{
	if (pCmd)
	{
		// ��������
		m_CmdList.push_back(pCmd);
	}
	return true;
}

bool Dbg_Command::Dbg_RunCmd(TCHAR * szCmd,Dbg_Context * Conetxt)
{
	// ���������б�
	for (auto cmd : m_CmdList)
	{
		// ��������
		if (cmd->Run(szCmd, Conetxt))
		{
			// ����ɹ�
			return true;
		}
	}
	// û�д���
	return false;
}

bool Dbg_Command::Dbg_DelCmd(TCHAR * szCmdName)
{
	// ���������б�
	for (DWORD i=0;i<m_CmdList.size();i++)
	{	
		if (_tcsicmp(szCmdName, m_CmdList[i]->m_Name) == 0)
		{
			// ɾ������
			m_CmdList.erase(m_CmdList.begin()+i);
			return true;
		}
	}

	return false;
}

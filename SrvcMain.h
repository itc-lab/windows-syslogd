// SrvcMain.h: CSrvcMain クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SRVCMAIN_H__19BEE499_8A72_4A23_B7E7_36819C054CDB__INCLUDED_)
#define AFX_SRVCMAIN_H__19BEE499_8A72_4A23_B7E7_36819C054CDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Cservice.h"

class CSrvcMain : public CService
{
public:
	CSrvcMain(LPCTSTR szName, LPCTSTR szDisplay, DWORD dwType);
	virtual ~CSrvcMain();
	DECLARE_SERVICE(CSrvcMain, SrvcMain)

protected:
	void Init();
	void Run();
	void OnStop();
	void OnShutdown();
	void DeInit();
	HANDLE m_hEventStop;

public:
	char m_currentdir[_MAX_PATH];
	char m_configfile[_MAX_PATH];
};

#endif // !defined(AFX_SRVCMAIN_H__19BEE499_8A72_4A23_B7E7_36819C054CDB__INCLUDED_)

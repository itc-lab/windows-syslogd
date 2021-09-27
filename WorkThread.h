// WorkThread.h: CWorkThread クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKTHREAD_H__DEB09D23_A490_11D3_925F_006097A53C6C__INCLUDED_)
#define AFX_WORKTHREAD_H__DEB09D23_A490_11D3_925F_006097A53C6C__INCLUDED_

#include <vector>
#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorkThread
{
public:
	BOOL IsStop();
	CWorkThread();
	virtual ~CWorkThread();
	virtual void Start(LPVOID pParam);
	virtual void Stop();
	static UINT ThreadMain(LPVOID pParam);
	BOOL bDebug;

protected:
	CWinThread* m_pThread;
	void* m_pMain;
	virtual BOOL Init() { return TRUE; }
	virtual void DeInit() { return; }
	virtual UINT Run(LPVOID pParam);
	HANDLE hEventStart;
	HANDLE hEventStop;
	LPVOID	Params[2];
	class config_data {
	public:
		BYTE mask[24];
		std::string ip;
		std::string action;
	};
	time_t configfile_mtime;
	void read_config();
	std::map<std::string, size_t> start_pos;
	std::map<std::string, size_t> end_pos;
	std::vector<config_data> config_file;
	DWORD str2facility( const std::string &facility ) const;
	BYTE str2priority( const std::string &facility ) const;
};

#endif // !defined(AFX_WORKTHREAD_H__DEB09D23_A490_11D3_925F_006097A53C6C__INCLUDED_)

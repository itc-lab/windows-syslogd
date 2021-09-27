#if	!defined(SERVICE_INSTALL_H)
#define	SERVICE_INSTALL_H

#include <winsvc.h>
#include <process.h>

class CServiceInstall
{
public:
	CServiceInstall( LPCTSTR szServiceName, LPCTSTR szDisplay );
	~CServiceInstall( ) { }

	void Install(	DWORD dwType = SERVICE_WIN32_OWN_PROCESS,
					DWORD dwStart = SERVICE_DEMAND_START,
					LPCTSTR lpDepends = NULL, 
					LPCTSTR lpName = NULL,
					LPCTSTR lpPassword = NULL,
					LPCTSTR lpDescription = NULL
					);
	void Remove( BOOL bForce = FALSE );

	BOOL IsInstalled( );
	DWORD ErrorPrinter( const TCHAR* pszFcn, DWORD dwErr = GetLastError() );

private:
	TCHAR m_service[257];
	TCHAR m_display[257];

};

#endif
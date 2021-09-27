#include "stdafx.h"
#include "resource.h"
#include "CServiceInstall.h"

CServiceInstall::CServiceInstall( LPCTSTR szServiceName, LPCTSTR szDisplay )
{
	strcpy_s( m_service, sizeof( m_service ), szServiceName );
	strcpy_s( m_display, sizeof( m_display ), szDisplay );
}

void CServiceInstall::Install(DWORD dwType, DWORD dwStart,
							   LPCTSTR lpDepends, LPCTSTR lpName,
							   LPCTSTR lpPassword, LPCTSTR lpDescription )
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;

	if( IsInstalled() == TRUE )
		return;

	hSCM = OpenSCManager( NULL,
							NULL,
							SC_MANAGER_CREATE_SERVICE );
	if ( !hSCM ) {
		ErrorPrinter( _TEXT("OpenSCManager") );
		return;
	}

	TCHAR szFilePath[_MAX_PATH];
	::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

	hService = ::CreateService( 
		hSCM,
		m_service,
		m_display,
		SERVICE_ALL_ACCESS,
		dwType,
		dwStart,
		SERVICE_ERROR_NORMAL,
		szFilePath,
		NULL,
		NULL,
		lpDepends,
		lpName,
		lpPassword );

	if ( !hService )  {
		ErrorPrinter( _TEXT("CreateService") );
		::CloseServiceHandle( hSCM );
		return;
	}
	if( lpDescription != NULL ) {
		_tprintf(_TEXT("%s Created\n"), m_service);
		HMODULE hmod = ::LoadLibrary("Advapi32.dll");
		if(hmod != NULL) {
			if(::GetProcAddress(hmod, "ChangeServiceConfig2A") != NULL) {
				SERVICE_DESCRIPTION sd;
				sd.lpDescription = _strdup( lpDescription );
				::ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd);
				free( sd.lpDescription );
			}
			::FreeLibrary(hmod);
		}
	}
	if ( !::StartService( hService, 0, NULL ) ) {
		ErrorPrinter( _TEXT("StartService") );
	}
	::CloseServiceHandle( hService );
	::CloseServiceHandle( hSCM );
	return;
}

void CServiceInstall::Remove( BOOL bForce )
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	BOOL bSuccess = FALSE;

	hSCM = ::OpenSCManager( NULL,
							  NULL,
							  SC_MANAGER_CONNECT );
	if ( !hSCM ) {
		ErrorPrinter( _TEXT("OpenSCManager") );
		return;
	}

	hService = ::OpenService(
		hSCM,
		m_service,
		SERVICE_STOP | DELETE );						
	if ( !hService ) {
		ErrorPrinter( _TEXT("OpenService") );
		if (hService) CloseServiceHandle( hService );
		if (hSCM) CloseServiceHandle( hSCM );
		return;
	}

	//Force the service to stop
	if( bForce == TRUE ) {
		SERVICE_STATUS status;
		::ControlService(hService, SERVICE_CONTROL_STOP, &status);
		_tprintf(_TEXT("%s stopped\n"), m_service);
	}

	bSuccess = ::DeleteService( hService );

	if (bSuccess) {
		_tprintf(_TEXT("%s removed\n"), m_service);
	} else {
		ErrorPrinter( _TEXT("DeleteService") );
	}

	if (hService) CloseServiceHandle( hService );
	if (hSCM) CloseServiceHandle( hSCM );
	return;
}

BOOL CServiceInstall::IsInstalled( )
{
	BOOL bRes = FALSE;

	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	if (hSCM != NULL)
	{
		SC_HANDLE hService = ::OpenService(hSCM, m_service, SERVICE_QUERY_CONFIG);
		if (hService != NULL)
		{
			bRes = TRUE;
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(hSCM);
	}
	return bRes;
}

DWORD CServiceInstall::ErrorPrinter( const TCHAR* psz, DWORD dwErr )
{
	LPVOID lpvMsgBuf;
	if ( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		0, dwErr,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR) &lpvMsgBuf, 0, 0 ) )
	{
		_tprintf( _TEXT("%s failed: Unknown error %x\n"), psz, dwErr );
	}
	else {
		_tprintf( _TEXT("%s failed: %s\n"), psz, (LPTSTR)lpvMsgBuf );
	}

	LocalFree( lpvMsgBuf );
	return dwErr;
}
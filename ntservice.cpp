// ntservice.cpp : コンソール アプリケーション用のエントリ ポイントの定義
//

#include "stdafx.h"
#include "ntservice.h"
#include "SrvcMain.h"
#include "CServiceInstall.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// 唯一のアプリケーション オブジェクト

CWinApp theApp;

static std::string LoadString( HINSTANCE hInstance, UINT uID )
{
	std::string str;
	int sz = 256;
	char* bf = new char[sz];
	while( TRUE ) {
		int ln = ::LoadString( hInstance, uID, bf, sz );
		if( ln == 0 ) {
			delete bf;
			return "";
		}
		if( ( ln + 2 ) < sz ) break;
		delete bf;
		sz += 256;
		bf = new char[sz];
	}
	str = bf;
	delete bf;
	return str;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		// TODO: 必要に応じてエラー コードを変更してください。
		std::cerr << _T("Fatal Error: MFC initialization failed") << std::endl;
		nRetCode = 1;
		return nRetCode;
	}

	BOOL bDebug = FALSE;
	std::string szName = LoadString( ::GetModuleHandle(NULL), IDS_SERVICE_NAME );
	std::string szDisplay = LoadString( ::GetModuleHandle(NULL), IDS_DISPLAY_NAME );

   // Process command line
	TCHAR* pszCmdLine = GetCommandLine();
	CharLowerBuff(pszCmdLine, lstrlen(pszCmdLine));
	if(_tcsstr(pszCmdLine, _T("-install"))) {

		std::string desc = LoadString( ::GetModuleHandle(NULL), IDS_SERVICE_DESCRIPTION );
		CServiceInstall si( szName.c_str(), szDisplay.c_str() );
		si.Install( SERVICE_WIN32_OWN_PROCESS,
				SERVICE_AUTO_START,
				NULL, NULL, NULL, desc.length() == 0 ? NULL : desc.c_str() );
		char path[_MAX_PATH];
		::GetModuleFileName(NULL, path, sizeof(path));
//		CEventLog log( (UINT)IDS_DISPLAY_NAME );
//		log.RegisterLog(path);
		return 0;
	} else if(_tcsstr(pszCmdLine, _T("-remove"))) {
		CServiceInstall si( szName.c_str(), szDisplay.c_str() );
		si.Remove( true );
		return 0;
	} else if(_tcsstr(pszCmdLine, _T("-debug"))) {
		bDebug = TRUE;
	}

	// Create the object instance
	CSrvcMain SrvcMain( szName.c_str(), szDisplay.c_str(), SERVICE_WIN32_OWN_PROCESS);
	if(bDebug) {
		BEGIN_DEBUG_SERVICE(CSrvcMain, SrvcMain, argc, argv)
	}

	// Set up an entry for the one service and go
	BEGIN_SERVICE_MAP
		SERVICE_MAP_ENTRY(CSrvcMain, SrvcMain)
	END_SERVICE_MAP

	return(0);
}

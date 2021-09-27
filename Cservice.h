#if	!defined(SERVICE_H)
#define	SERVICE_H

#define STATE_NO_CHANGE 0xffffffff
#define NUMEVENTS 4
#define MAX_SERVICE_LEN 256

#include <winsvc.h>
#include <process.h>

///////////////////////////////////////////////////////
// CService Class
///////////////////////////////////////////////////////
/* Each service derives from this to do its own
   work. The application must instantiate the derived
   service class one and only one time. */
///////////////////////////////////////////////////////
class CService
{
public:
	BOOL ControlHandler(DWORD dwCtrlType);
	CService(LPCTSTR szName, LPCTSTR szDisplay, DWORD dwType);
	~CService();

	DWORD   GetStatus()      { return m_dwState; }
	DWORD   GetControls()    { return m_dwControlsAccepted; }
	LPCTSTR GetName()        { return m_szName; }
	LPCTSTR GetDisplayName() { return m_szDisplay; }

   // All derived class static ServiceMain functions are delegated to me
   void ServiceMainMember(DWORD argc, LPTSTR* argv,
                        LPHANDLER_FUNCTION pf, LPTHREAD_START_ROUTINE pfnWTP);
   void DebugMainMember(DWORD argc, LPTSTR* argv, LPTHREAD_START_ROUTINE pfnWTP);

   // All derived class static handler functions are delegated to me
   void HandlerMember(DWORD dwControl);

	BOOL bDebug;

protected:
   bool SetupHandlerInside(LPHANDLER_FUNCTION lpHandlerProc);

   // Launches a thread to look for control requests
   virtual void LaunchWatcherThread(LPTHREAD_START_ROUTINE pfnWTP);
   virtual DWORD WatcherThreadMemberProc();

   void SetStatus(DWORD dwNewState,
                  DWORD dwNewCheckpoint = STATE_NO_CHANGE,
                  DWORD dwNewHint       = STATE_NO_CHANGE,
                  DWORD dwNewControls   = STATE_NO_CHANGE,
                  DWORD dwExitCode      = NO_ERROR,
                  DWORD dwSpecificExit  = 0);

   DWORD ErrorHandler(const TCHAR* pszFcn,
                      bool         bPrintEvent     = true,
                      bool         bRaiseException = true,
                      DWORD        dwErr           = GetLastError());

// Overrideables
protected:
   virtual void PreInit();     // If you override, call the base class version
   virtual void Init();
   virtual void DeInit();      // If you override, call the base class version
   virtual void ParseArgs(DWORD argc, LPTSTR* argv);
   virtual void OnPause();
   virtual void OnContinue();
   virtual void OnShutdown();
   virtual void HandleUserDefined(DWORD dwControl);

   virtual void Run() = 0;
   virtual void OnStop() = 0;

// Attributes
protected:
   CRITICAL_SECTION m_cs;

   // Status info
   SERVICE_STATUS_HANDLE m_hServiceStatus;
   DWORD m_dwState;
   DWORD m_dwControlsAccepted;
   DWORD m_dwCheckpoint;
   DWORD m_dwWaitHint;

   // Tracks state currently being worked on in Handler
   DWORD m_dwRequestedControl;

   // Control Events
   HANDLE m_hEvents[NUMEVENTS];
   HANDLE m_hWatcherThread;

   TCHAR m_szName[MAX_SERVICE_LEN + 1];
   TCHAR m_szDisplay[MAX_SERVICE_LEN + 1];
   DWORD m_dwType;

   enum EVENTS { STOP, PAUSE, CONTINUE, SHUTDOWN };
};

// Implements static thread functions for derived classes
#define DECLARE_SERVICE(class_name, service_name) \
public: \
	static class_name##* m_pThis; \
	static void WINAPI service_name##Main(DWORD argc, LPTSTR* argv); \
	static void WINAPI service_name##Handler(DWORD dwControl); \
	static DWORD WINAPI service_name##WatcherThreadProc(LPVOID lpParameter); \
	static BOOL WINAPI service_name##ControlHandler(DWORD dwCtrlType);

#define IMPLEMENT_SERVICE(class_name, service_name) \
class_name##* class_name::m_pThis = NULL; \
void WINAPI class_name::service_name##Main(DWORD argc, LPTSTR* argv) \
{ \
   m_pThis->ServiceMainMember(argc, argv, \
                   (LPHANDLER_FUNCTION)service_name##Handler, \
                   (LPTHREAD_START_ROUTINE)service_name##WatcherThreadProc); \
} \
void WINAPI class_name::service_name##Handler(DWORD dwControl) \
{ \
   m_pThis->HandlerMember(dwControl); \
} \
DWORD WINAPI class_name::service_name##WatcherThreadProc(LPVOID lpParameter) \
{ \
   return m_pThis->WatcherThreadMemberProc(); \
} \
BOOL WINAPI class_name::service_name##ControlHandler( DWORD dwCtrlType ) \
{ \
	return(m_pThis->ControlHandler(dwCtrlType)); \
}

// For implementing a service process
#define BEGIN_SERVICE_MAP \
SERVICE_TABLE_ENTRY svcTable[] = {

#define SERVICE_MAP_ENTRY(class_name, service_name) \
{_T(#service_name), (LPSERVICE_MAIN_FUNCTION)class_name::service_name##Main},

#define END_SERVICE_MAP \
{NULL, NULL}}; \
StartServiceCtrlDispatcher(svcTable);

#ifdef UNICODE
#define BEGIN_DEBUG_SERVICE(class_name, service_name, argc, argv) \
{ \
printf( "Start Console Mode\n" ); \
DWORD dwArgc; \
LPTSTR *lpszArgv; \
lpszArgv = CommandLineToArgvW(GetCommandLineW(), &(dwArgc) ); \
::SetConsoleCtrlHandler(class_name::service_name##ControlHandler, TRUE); \
}

#else

#define BEGIN_DEBUG_SERVICE(class_name, service_name, argc, argv) \
{ \
printf( "Start Console Mode\n" ); \
service_name.bDebug = true; \
DWORD dwArgc; \
LPTSTR *lpszArgv; \
dwArgc   = (DWORD) argc; \
lpszArgv = argv; \
::SetConsoleCtrlHandler(class_name::service_name##ControlHandler, TRUE); \
service_name.DebugMainMember(argc, argv, (LPTHREAD_START_ROUTINE)service_name.service_name##WatcherThreadProc); \
return(0); \
}

#endif
#endif

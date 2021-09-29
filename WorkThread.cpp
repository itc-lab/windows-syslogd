// WorkThread.cpp: CWorkThread クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <regex>
#include <sys/stat.h>
#include "SrvcMain.h"
#include "WorkThread.h"
#include "tools.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CWorkThread::CWorkThread()
{
	hEventStart = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventStop	= ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_pThread = NULL;
	configfile_mtime = 0;
}

CWorkThread::~CWorkThread()
{
	::CloseHandle(hEventStart);
	::CloseHandle(hEventStop);
}

UINT CWorkThread::ThreadMain(LPVOID pParam)
{
	LPVOID* params = (LPVOID*)pParam;
	::WaitForSingleObject(((CWorkThread*)params[0])->hEventStart, INFINITE);
	((CWorkThread*)params[0])->Init();
	UINT rt = ((CWorkThread*)params[0])->Run(params[1]);
	((CWorkThread*)params[0])->DeInit();
	return rt;
}

void CWorkThread::Start(LPVOID pParam)
{
	m_pMain = pParam;
	Params[0] = this;
	Params[1] = pParam;
	m_pThread = AfxBeginThread(ThreadMain, Params);
	::SetEvent(hEventStart);
}

void CWorkThread::read_config() {
	CSrvcMain* pMain = (CSrvcMain*)m_pMain;
	struct stat st;
	stat( pMain->m_configfile, &st );
	if ( configfile_mtime == st.st_mtime ) {
		return;
	}
	configfile_mtime = st.st_mtime;
	FILE* fp = NULL;
	if ( fopen_s( &fp, pMain->m_configfile, "rt" ) != 0 ) {
		return;
	}
	LPTSTR lpEnv = GetEnvironmentStrings();
	std::map<std::string, std::string> envs;
	for ( TCHAR* lpWork = lpEnv; *lpWork ; ) {
		if ( lpWork[0] != '=' ) {
			char* pt = strchr( lpWork + 1, '=' );
			if ( pt ) {
				*pt = '\0';
				envs[lpWork] = pt + 1;
				*pt = '=';
			}
		}
		lpWork += strlen( lpWork ) + 1;
	}
	config_file.clear();
	std::string from_ip = "*";
	while( !feof( fp ) ) {
		std::string line;
		while( !feof( fp ) ) {
			char bf[256];
			if( fgets( bf, sizeof( bf ), fp ) == NULL ) break;
			char* pt = strchr( bf, '\r' );
			if( pt == NULL ) pt = strchr( bf, '\n' );
			if( pt != NULL ) {
				*pt = '\0';
				line += bf;
				line = tools::rtrim( line, "\t " );
				if ( line.size() == 0 ) break;
				if ( line.at( line.size() - 1 ) != '\\' )  {
					break;
				}
				line[line.size() - 1] = ' ';
				line = tools::rtrim( line, "\t " );
			} else {
				line += bf;
			}
		}
		line = tools::trim( line, " " );
		if ( line.size() == 0 || line[0] == '#' ) continue;
		if ( line[0] == '!' ) {		//	IP ?
			std::regex reg( "^\\![\\s\\t]*([^\\s\\t]+).*$" );
//			std::regex reg( "^([0-9\\.]+)" );
			std::match_results<const char *> match;
			bool rt = std::regex_match( line.c_str(), match, reg, std::regex_constants::match_default );
			if ( rt ) {
				end_pos[from_ip] = config_file.size();
				from_ip = match.str(1);
				if ( isalpha( from_ip[0] ) ) {
					struct hostent* hp = gethostbyname( from_ip.c_str() );
					if ( hp ) {
						from_ip = inet_ntoa( *(in_addr*)(hp->h_addr_list[0]) );
					}

				}
				start_pos[from_ip] = config_file.size();
			}
			continue;
		}

		std::match_results<const char *> match;
		bool rt = std::regex_match( line.c_str(), match, std::regex( "^([^\\s\\t]+)[\\s\\t]+\\\"([^\\\"]+)\\\".*$"), std::regex_constants::match_default );
		if ( !rt ) {
			rt = std::regex_match( line.c_str(), match, std::regex( "^([^\\s\\t]+)[\\s\\t]+([^\\s\\t#]+).*$"), std::regex_constants::match_default );
		}
		if ( rt ) {
			std::string act = match.str(2);
			auto item = envs.begin();
			while ( item != envs.end() ) {
				std::regex reg( "%" + (*item).first + "%", std::regex_constants::icase );
				act = std::regex_replace( act, reg, (*item).second, std::regex_constants::match_default );
				item ++;
			}
			BYTE mask[24];
			memset( mask, 0, sizeof( mask ) );
			std::vector<std::string> selecctor;
			tools::explode( match.str(1), ";", selecctor );
			size_t sn;
			for ( sn = 0; sn < selecctor.size(); sn ++ ) {
				size_t pos = selecctor[sn].find( '.' );
				if ( pos == std::string::npos ) continue;
				std::string fac = tools::trim( selecctor[sn].substr( 0, pos ), " " );
				if ( fac.empty() ) fac = "*";
				std::string pri = tools::trim( selecctor[sn].substr( pos + 1 ), " " );
				if ( pri.empty() ) pri = "*";

				BYTE pmask = 0;
				BYTE pbit;
				std::string ope;
				if ( pri == "none" ) {
					ope = "and";
					pmask = 0;
				} else if ( pri.substr( 0, 2 ) == "!=" ) {
					ope = "and";
					pmask = 0xff ^ this->str2priority( pri.substr( 2 ) );
				} else if ( pri.substr( 0, 1 ) == "!" ) {
					ope = "and";
					pbit = this->str2priority( pri.substr( 1 ) );
					pbit >>= 1;
					while( pbit != 0 ) {
						pmask |= pbit;
						pbit >>= 1;
					}
				} else if ( pri.substr( 0, 1 ) == "=" ) {
					ope = "or";
					pmask = this->str2priority( pri.substr( 1 ) );
				} else {
					ope = "or";
					pbit = this->str2priority( pri );
					while( pbit != 0 ) {
						pmask |= pbit;
						pbit <<= 1;
					}
				}

				std::vector<std::string> facilities;
				tools::explode( fac, ",", facilities );
				size_t cn;
				for ( cn = 0; cn < facilities.size(); cn ++ ) {
					DWORD fmask = this->str2facility( facilities[cn] );
					int no = 0;
					for ( no = 0; no < 24; no ++ ) {
						if ( ( fmask & ( 1 << no ) ) != 0 ) {
							if ( ope == "or" ) {
								mask[no] |= pmask;
							} else {
								mask[no] &= pmask;
							}
						}
					}
					if ( ((CSrvcMain*)m_pMain)->bDebug ) {
						printf( "%s.%s\t%s\n", facilities[cn].c_str(), pri.c_str(), act.c_str() );
					}
				}
			}
			config_data dt;
			memcpy( dt.mask, mask, sizeof( dt.mask ) );
			dt.action = act;
			config_file.push_back( dt );
		}
	}
	fclose( fp );
	FreeEnvironmentStrings( lpEnv );
}

void CWorkThread::Stop()
{
	::SetEvent(hEventStop);
	config_file.clear();
}


BOOL CWorkThread::IsStop()
{
	if(m_pThread != NULL) {
		DWORD retWait = ::WaitForSingleObject(m_pThread->m_hThread, 1000);
		if(retWait == WAIT_TIMEOUT)
			return FALSE;
	}
	return TRUE;
}

UINT CWorkThread::Run(LPVOID pParam)
{
	SOCKET socket = ::socket( AF_INET, SOCK_DGRAM, 0 );
	SOCKADDR_IN local;
	::memset( &local, 0, sizeof(local) );
	local.sin_addr.s_addr	= INADDR_ANY;
	local.sin_family		= AF_INET;
	local.sin_port			= htons( 514 );
	::bind(socket, (LPSOCKADDR)&local, sizeof(local));
	DWORD flg = 1;		//Non Blocking
	::ioctlsocket( socket, FIONBIO, &flg );

	read_config();

	char appdata_path[MAX_PATH * 2];
	::SHGetSpecialFolderPath( NULL, appdata_path, CSIDL_COMMON_APPDATA, 0 );

	DWORD last = ::GetTickCount();
	while ( true ) {
		if(::WaitForSingleObject(hEventStop, 0) != WAIT_TIMEOUT) break;
		int fromlen ;
		SOCKADDR_IN from;
		::memset( &from, 0, sizeof(from) );
		int ln;
		char rawdata[2000];
		fromlen = sizeof( from );
		ln = ::recvfrom( socket, (char*)rawdata, sizeof(rawdata), 0,
							(struct sockaddr *)&from, &fromlen );
		if ( ln < 0 ) {
			::Sleep(1);
			DWORD err = ::WSAGetLastError();
			if ( err == WSAEWOULDBLOCK ) {
				DWORD now = ::GetTickCount();
				if( ( now - last ) > 1000L ) {
					last += 1000L;
					read_config();
				}
				continue;
			}
			::closesocket( socket );
			::Sleep( 1000 );
			socket = ::socket( AF_INET, SOCK_DGRAM, 0 );
			::bind(socket, (LPSOCKADDR)&local, sizeof(local));
			continue;
		}
		rawdata[ln] = '\0';
		ln = strcspn(rawdata, "\r\n");
		rawdata[ln] = '\0';
		if (((CSrvcMain*)m_pMain)->bDebug) {
			printf("recv data: '%s'\n", rawdata);
		}

		std::string from_ip = inet_ntoa( from.sin_addr );

		std::regex re(".*\\<([\\d]+)\\>(.+)");
		std::match_results<const char *> match;
		//std::smatch match;
		if ( std::regex_match( rawdata, match, re, std::regex_constants::match_default ) ) {
			int pri = atoi( match.str(1).c_str() );
			int facility = pri / 8;
			int priority = pri % 8;
			std::string msg = match.str(2);
			SYSTEMTIME tm;
			::GetLocalTime( &tm );
			char tmstr[100];
			//_snprintf_s( tmstr, _TRUNCATE, "%02d:%02d:%02d.%03d", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds );
			_snprintf_s( tmstr, _TRUNCATE, "%04d/%02d/%02d %02d:%02d:%02d", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond );

			size_t no;
			size_t st = start_pos.find( from_ip ) != start_pos.end() ? start_pos[from_ip] : 0;
			size_t ed = end_pos.find( from_ip ) != end_pos.end() ? end_pos[from_ip] : config_file.size();
			for ( no = st; no < ed; no ++ ) {
				if ( ( config_file[no].mask[facility] & ( 1 << priority ) ) != 0 ) {
					std::string action = config_file[no].action;
					if ( action[0] == '@' ) {
						action = tools::trim( action.substr(1) );
						SOCKADDR_IN to;
						::memset( &to, 0, sizeof( to ) );
						if ( isalpha( action[0] ) ) {
							struct hostent *hp = gethostbyname( action.c_str() );
							if ( !hp ) continue;
							memcpy( &to.sin_addr, hp->h_addr, hp->h_length );
							to.sin_family = hp->h_addrtype;
						} else {
							to.sin_addr.s_addr	= inet_addr( action.c_str() );
							to.sin_family		= AF_INET;
						}
						to.sin_port			= htons( 514 );
						::sendto( socket, rawdata, ln, 0, (PSOCKADDR)&to, sizeof( to ) );
					} else {
						char date[100];
						_snprintf_s( date, _TRUNCATE, "%04d%02d%02d", tm.wYear, tm.wMonth, tm.wDay );
						action = std::regex_replace( action, std::regex( "%Date%", std::regex_constants::icase ),
														std::string( date ), std::regex_constants::match_default );
						char filename[MAX_PATH * 2];
						////_snprintf_s( filename, _TRUNCATE, "%s\\syslogd\\%s.%4d%02d%02d.log",
						if ( action[1] != ':' || action[2] != '\\' ) {
							_snprintf_s( filename, _TRUNCATE, "%s\\syslogd\\%s", appdata_path, action.c_str() );
						} else {
							_snprintf_s( filename, _TRUNCATE, "%s", action.c_str() );
						}
						tools::mkdir( tools::dir_name( filename ) );
						HANDLE hFile;
						hFile = ::CreateFile( filename,
											GENERIC_WRITE,
											FILE_SHARE_READ,
											NULL,
											OPEN_ALWAYS,
											FILE_ATTRIBUTE_ARCHIVE,
											0 );
						if( hFile != INVALID_HANDLE_VALUE ) {
							::SetFilePointer( hFile, 0, NULL, FILE_END );
							char buf[4000];
							_snprintf_s( buf, _TRUNCATE, "%s\t%s\t%s\n", tmstr, from_ip.c_str(), msg.c_str() );

							DWORD wl;
							::WriteFile( hFile, buf, strlen( buf ), &wl, NULL );
							::CloseHandle(hFile);
							if ( ((CSrvcMain*)m_pMain)->bDebug ) {
								printf( "%s", buf );
							}
						}
					}
				}
			}
		}
	}
	::closesocket( socket );
	return 0;
}

DWORD CWorkThread::str2facility( const std::string &facility ) const {
	if ( facility == "*" ) return 0x00ffffff;
	if ( !_stricmp( facility.c_str(), "kern" ) ) return 1 << 0;
	if ( !_stricmp( facility.c_str(), "user" ) ) return 1 << 1;
	if ( !_stricmp( facility.c_str(), "mail" ) ) return 1 << 2;
	if ( !_stricmp( facility.c_str(), "daemon" ) ) return 1 << 3;
	if ( !_stricmp( facility.c_str(), "auth" ) ) return 1 << 4;
	if ( !_stricmp( facility.c_str(), "syslog" ) ) return 1 << 5;
	if ( !_stricmp( facility.c_str(), "lpr" ) ) return 1 << 6;
	if ( !_stricmp( facility.c_str(), "news" ) ) return 1 << 7;
	if ( !_stricmp( facility.c_str(), "uucp" ) ) return 1 << 8;
	if ( !_stricmp( facility.c_str(), "cron" ) ) return 1 << 9;
	if ( !_stricmp( facility.c_str(), "authpriv" ) ) return 1 << 10;
	//if ( !_stricmp( facility.c_str(), "ftp" ) ) return 1 << 11;
	//if ( !_stricmp( facility.c_str(), "ntp" ) ) return 1 << 12;
	//if ( !_stricmp( facility.c_str(), "log audit" ) ) return 1 << 13;
	//if ( !_stricmp( facility.c_str(), "log alert" ) ) return 1 << 14;
	//if ( !_stricmp( facility.c_str(), "clock" ) ) return 1 << 15;
	if ( !_stricmp( facility.c_str(), "local0" ) ) return 1 << 16;
	if ( !_stricmp( facility.c_str(), "local1" ) ) return 1 << 17;
	if ( !_stricmp( facility.c_str(), "local2" ) ) return 1 << 18;
	if ( !_stricmp( facility.c_str(), "local3" ) ) return 1 << 19;
	if ( !_stricmp( facility.c_str(), "local4" ) ) return 1 << 20;
	if ( !_stricmp( facility.c_str(), "local5" ) ) return 1 << 21;
	if ( !_stricmp( facility.c_str(), "local6" ) ) return 1 << 22;
	if ( !_stricmp( facility.c_str(), "local7" ) ) return 1 << 23;

	if ( !_stricmp( facility.c_str(), "security" ) ) return 1 << 4;
	return 0;
}

BYTE CWorkThread::str2priority( const std::string &priority ) const {
	if ( !_stricmp( priority.c_str(), "*" ) ) return 0xff;
	if ( !_stricmp( priority.c_str(), "emerg" ) ) return 1 << 0;
	if ( !_stricmp( priority.c_str(), "alert" ) ) return 1 << 1;
	if ( !_stricmp( priority.c_str(), "crit" ) ) return 1 << 2;
	if ( !_stricmp( priority.c_str(), "err" ) ) return 1 << 3;
	if ( !_stricmp( priority.c_str(), "warning" ) ) return 1 << 4;
	if ( !_stricmp( priority.c_str(), "notice" ) ) return 1 << 5;
	if ( !_stricmp( priority.c_str(), "info" ) ) return 1 << 6;
	if ( !_stricmp( priority.c_str(), "debug" ) ) return 1 << 7;

	if ( !_stricmp( priority.c_str(), "panic" ) ) return 1 << 0;
	if ( !_stricmp( priority.c_str(), "error" ) ) return 1 << 3;
	if ( !_stricmp( priority.c_str(), "warn" ) ) return 1 << 4;
	return 0;
}

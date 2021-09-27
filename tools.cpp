// tools.cpp: tools クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "tools.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

tools::tools()
{

}

tools::~tools()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int tools::explode( const std::string &str, const std::string &del, std::vector<std::string> &strs )
{
	strs.clear();
	if( str.empty() ) return 0;
	const char* pt = str.c_str();
	char* bf = new char[str.length() + 1];
	int ln = 0;
	int spc = 0;
	int num = 0;
	while( true ) {
		if( strchr( del.c_str(), *pt ) != NULL || *pt == '\0' ) {
			bf[ln] = '\0';
			strs.insert( strs.end(), bf );
			num ++;
			ln = 0;
			//if(*pt != '\0') {
			//	while( *pt && ( strchr( del.c_str(), *pt ) != NULL ) )
			//		pt ++;
			//	continue;
			//}
			if(*pt == '\0') break;
			pt += strspn( pt, del.c_str() );
			spc = 0;
		} else if(*pt != ' ') {
			for(; spc != 0; spc --) bf[ln ++] = ' ';
			bf[ln ++] = *pt;
			pt ++;
		} else {
			spc ++;
			pt ++;
		}
	}
	delete bf;
	return num;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int tools::explode( const std::string &str, char del, std::vector<std::string> &strs )
{
	strs.clear();
	if( str.empty() ) return 0;
	const char* pt = str.c_str();
	char* bf = new char[str.length() + 1];
	int ln = 0;
	int spc = 0;
	int num = 0;
	while( true ) {
		if( del == *pt || *pt == '\0' ) {
			bf[ln] = '\0';
			strs.insert( strs.end(), bf );
			num ++;
			ln = 0;
			if(*pt == '\0') break;
			pt ++;
			spc = 0;
		} else if(*pt != ' ') {
			for(; spc != 0; spc --) bf[ln ++] = ' ';
			bf[ln ++] = *pt;
			pt ++;
		} else {
			spc ++;
			pt ++;
		}
	}
	delete bf;
	return num;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::implode( const std::string &del, const std::vector<std::string> &strs )
{
	std::string str;

	size_t ct;
	for( ct = 0; ct < strs.size(); ct ++ ) {
		if( ct != 0 ) str += del;
		str += strs[ct];
	}

	return str;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::LoadString( HINSTANCE hInstance, UINT uID )
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::urlencode( const std::string &url )
{
	return urlencode( url.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::urlencode( const char *url, const char* space /*="%20"*/ )
{
	const char* pt = url;
	std::string str;
	while( *pt ) {
		char bf[4];
		if( ( 'a' <= *pt && *pt <= 'z' )
				|| ( 'A' <= *pt && *pt <= 'Z' )
				|| ( '0' <= *pt && *pt <= '9' )
				|| *pt == '.'
				|| *pt == '-'
				|| *pt == '_' ) {
			bf[0] = *pt;
			bf[1] = '\0';
		} else if( *pt == ' ' ) {
			strcpy_s( bf, sizeof( bf ), space );
		} else {
			sprintf_s( bf, "%%%02x", (unsigned char)*pt );
		}
		str += std::string( bf );
		pt ++;
	}
	return str;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::urldecode( const std::string &url )
{
	return urldecode( url.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::urldecode( const char* url )
{
	int ln = 0;
	std::string conv;
	const char* pt = url;
	while( *pt ) {
		if( *pt == '%' ) {
			char bf[3];
			bf[0] = pt[1];
			bf[1] = pt[2];
			bf[2] = '\0';
			int ch;
			sscanf_s( bf, "%x", &ch );
			conv += ch;
			pt += 3;
		} else if( *pt == '+' ) {
			conv += ' ';
			pt ++;
		} else {
			conv += *pt;
			pt ++;
		}
		ln ++;
	}
	return conv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::rawurldecode( const char* url )
{
	int ln = 0;
	std::string conv;
	const char* pt = url;
	while( *pt ) {
		if( *pt == '%' ) {
			char bf[3];
			bf[0] = pt[1];
			bf[1] = pt[2];
			bf[2] = '\0';
			int ch;
			sscanf_s( bf, "%x", &ch );
			conv += ch;
			pt += 3;
		} else {
			conv += *pt;
			pt ++;
		}
		ln ++;
	}
	return conv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int tools::urldecode( const char* url, unsigned char* buf, int size /* = -1 */ )
{
	int ln = 0;
	const char* pt = url;
	while( *pt ) {
		if( *pt == '%' ) {
			char bf[3];
			bf[0] = pt[1];
			bf[1] = pt[2];
			bf[2] = '\0';
			int ch;
			sscanf_s( bf, "%x", &ch );
			buf[ln] = ch;
			pt += 3;
		} else if( *pt == '+' ) {
			buf[ln] = ' ';
			pt ++;
		} else {
			buf[ln] = *pt;
			pt ++;
		}
		ln ++;
		if( size > 0 && ln >= size ) break;
	}
	return ln;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::pathencode( const std::string &path )
{
	std::vector<std::string> ar;
	tools::explode( path, "/", ar );
	size_t ct;
	for( ct = 0; ct < ar.size(); ct ++ ) {
		std::string str = ar[ct];
		ar[ct] = tools::urlencode( str );
	}
	return tools::implode( "/", ar );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
std::string tools::GetErrorMessage( DWORD err /*=0*/ )
{
	std::string msg;
	if( err == 0 ) {
		err = ::GetLastError();
	}
	//if( err == ERROR_INTERNET_EXTENDED_ERROR ) {
	//	char bf[256];
	//	DWORD ln = sizeof(bf);
	//	::InternetGetLastResponseInfo( &err, bf, &ln );
	//	msg = bf;
	//}
	//else {
		LPTSTR lpMsgBuf;
		::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER
						| FORMAT_MESSAGE_FROM_SYSTEM
						| FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						err,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
						(LPTSTR)&lpMsgBuf, 0, NULL );
		msg = lpMsgBuf;
		::LocalFree( lpMsgBuf );
	//}
	return msg;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::utf8tosjis( const char* utf )
{
	const unsigned char* src = (const unsigned char*)utf;
	unsigned char* uni = new unsigned char[strlen( (const char*)utf ) * 2 + 2];
	unsigned char* dst = uni;
	while( *src ) {
		if( *src <= 0xc0 ) {			//	1 byte
			dst[1] = 0;
			dst[0] = src[0];
			dst += 2;
			src += 1;
		} else if( *src <= 0xe0 ) {	//	2 bytes
			dst[1] = ( src[0] >> 2 ) & 0x07;
			dst[0] = ( ( src[0] << 6 ) & 0xc0 ) | ( src[1] & 0x3f );
			dst += 2;
			src += 2;
		} else if( *src <= 0xf8 ) {	//	3 bytes
			dst[1] = ( ( src[0] << 4 ) & 0xf0 ) | ( ( src[1] >> 2 ) & 0x0f );
			dst[0] = ( ( src[1] << 6 ) & 0xc0 ) | ( ( src[2] ) & 0x3f );
			if( dst[1] == 0x30 && dst[0] == 0x1c ) {
				dst[1] = 0xff;
				dst[0] = 0x5e;
			}
			dst += 2;
			src += 3;
		} else {						//	4 bytes
			dst[3] = 0;
			dst[2] = ( ( src[0] << 2 ) & 0x1c ) | ( ( src[1] >> 4 ) & 0x03 );
			dst[1] = ( ( src[1] << 4 ) & 0xf0 ) | ( ( src[2] >> 2 ) & 0x0f );
			dst[0] = ( ( src[2] << 6 ) & 0xc0 ) | ( ( src[2] >> 2 ) & 0x0f );
			dst += 4;
			src += 4;
		}
	}
	dst[0] = 0;
	dst[1] = 0;
	char* sjis = new char[strlen( (const char*)utf ) * 2 + 2];
	::WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK,
							(LPCWSTR)uni, -1,
							sjis, strlen( (const char*)utf ) * 2 + 2,
							NULL, NULL);

	delete uni;
	std::string result = sjis;
	delete sjis;
	return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::sjistoutf8( const std::string &sjis )
{
	return tools::sjistoutf8( sjis.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::sjistoutf8( const char* sjis )
{
	unsigned char* uni = new unsigned char[strlen( sjis ) * 2 + 2];
	memset( uni, 0, strlen( sjis ) * 2 + 2 );
	unsigned char* utf = new unsigned char[strlen( sjis ) * 3 + 1];
	::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,
						(LPCTSTR)sjis, -1,
						(LPWSTR)uni, strlen( sjis ) * 2 + 2 );
	unsigned char* src = uni;
	unsigned char* dst = utf;
	while( src[0] || src[1] ) {
		if( src[1] == 0 ) {
			dst[0] = src[0];
			src += 2;
			dst += 1;
		} else {
			if( src[1] == 0xff && src[0] == 0x5e ) {
				src[1] = 0x30;
				src[0] = 0x1c;
			}
			dst[0] = 0xe0 | ( ( src[1] >> 4 ) & 0x0f );
			dst[1] = 0x80 | ( ( src[1] << 2 ) & 0x3c ) | ( ( src[0] >> 6 ) & 0x03 );
			dst[2] = 0x80 | ( ( src[0] ) & 0x3f );
			src += 2;
			dst += 3;
		}
	}
	dst[0] = 0;
	delete uni;
	std::string result;
	result = (const char*)utf;
	delete utf;
	return result;
}

#define	issjis0(x)	(((0x81 <= (unsigned char)(x) && (unsigned char)(x) < 0xA0) || (0xE0 <= (unsigned char)(x) && (unsigned char)(x) <= 0xFC)) ? true : false)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::base_name( const char* path )
{
	char *name = new char[ strlen( path ) + 1 ];
	const char* psep = NULL;
	const char* pt = path;
	while( *pt ) 	{
		if( issjis0( *pt ) ) {
			pt += 2;
		} else {
			if( *pt == '\\' || *pt == '/' ) psep = pt;
			pt ++;
		}
	}
	strcpy_s( name, strlen(path) + 1, psep ? psep + 1 : path );
	std::string result = std::string( name );
	delete name;
	return result;
}

///////////////////////////////////////////////////////////////////////////////
std::string tools::dir_name( const char* path )
{
	char *name = new char[ strlen( path ) + 1 ];
	strcpy_s( name, strlen(path) + 1, path );
	char* psep = NULL;
	char* pt = name;
	while( *pt ) 	{
		if( issjis0( *pt ) ) {
			pt += 2;
		} else {
			if( *pt == '\\' || *pt == '/' ) psep = pt;
			pt ++;
		}
	}
	if( psep ) *psep = '\0';
	std::string result = std::string( name );
	delete name;
	return result;
}

///////////////////////////////////////////////////////////////////////////////
std::string tools::ext_name( const char* path )
{
	char *name = new char[ strlen( path ) + 1 ];
	const char* psep = NULL;
	const char* pt = path;
	while( *pt ) 	{
		if( issjis0( *pt ) ) {
			pt += 2;
		} else {
			if( *pt == '.' ) psep = pt;
			pt ++;
		}
	}
	strcpy_s( name, strlen(path) + 1, psep ? psep + 1 : path );
	std::string result = std::string( name );
	delete name;
	return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string tools::cutPath( const char* path, int length )
{
	std::string str;
	if( strlen( path ) <= (size_t)length ) return path;

	int cuts = strlen( path ) - length + 3;
	LPCTSTR pt = (LPCTSTR)path + 3;
	while( cuts > 0 ) {
		if( issjis0( *pt ) ) {
			pt += 2;
			cuts -= 2;
		} else {
			pt ++;
			cuts --;
		}
	}
	str = std::string( path ).substr( 0, 3 ) + "..." + pt;
	return str;
}

///////////////////////////////////////////////////////////////////////////////
std::string tools::trim( const std::string &str, const std::string &charlist )
{
	std::string result = ltrim( str, charlist );
	return rtrim( result, charlist );
}

///////////////////////////////////////////////////////////////////////////////
std::string tools::ltrim( const std::string &str, const std::string &charlist )
{
	if( str.empty() ) return str;
	char* bf = _strdup( str.c_str() );
	char* st = bf + strspn( bf, charlist.c_str() );
	std::string result = st;
	free( bf );
	return result;
}

///////////////////////////////////////////////////////////////////////////////
std::string tools::rtrim( const std::string &str, const std::string &charlist )
{
	if( str.empty() ) return str;
	char* bf = _strdup( str.c_str() );
	char* ed = bf + strlen( bf ) - 1;
	while( ed != bf ) {
		if( !strchr( charlist.c_str(), *ed ) ) break;
		*ed = 0;
		ed --;
	}
	std::string result = bf;
	free( bf );
	return result;
}

#define EOS 	'\0'

///////////////////////////////////////////////////////////////////////////////
BOOL tools::fnmatch( const char *string, const char *pattern ) {
	const char *stringstart;
	char c, test;

	for( stringstart = string;; ) {
		switch( c = *pattern ++ ) {
		case EOS:
			return *string == EOS ? true : false;
		case '?':
			if( *string == EOS ) {
				return false;
			}
			//if( *string == '.' ) {
			//	return false;
			//}
			string ++;
			break;
		case '*':
			c = *pattern;
			/* Collapse multiple stars. */
			while( c == '*' ) {
				c = *++pattern;
			}

			/* Optimize for pattern with * at end or before /. */
			if( c == EOS ) {
				return true;
			}
			/* General case, use recursion. */
			while( ( test = *string ) != EOS ) {
				if( tools::fnmatch( string, pattern ) ) {
					return true;
				}
				//if( *string == '.' ) {
				//	return false;
				//}
				string ++;
			}
			return false;
		default:
			if( tolower( c ) != tolower( *string ) ) {
				return false;
			}
			string ++;
			break;
		}
	}
	/* NOTREACHED */
}

///////////////////////////////////////////////////////////////////////////////
std::string tools::getFileVersion( HINSTANCE hInstance ) {
	char path[MAX_PATH * 2];
	::GetModuleFileName( hInstance, path, sizeof( path ) );
	DWORD dwHandle;
	DWORD dwSize = ::GetFileVersionInfoSize( path, &dwHandle );
	void* pVerBuf = malloc( dwSize );
	::GetFileVersionInfo( path, dwHandle, dwSize, pVerBuf );
	void* ppBuf;
	UINT puLen;
	::VerQueryValue( pVerBuf, "\\VarFileInfo\\Translation", &ppBuf, &puLen );
	char str[256];
	sprintf_s( str, "\\StringFileInfo\\%.4X%.4X\\FileVersion", ((WORD*)ppBuf)[0], ((WORD*)ppBuf)[1] );
	::VerQueryValue( pVerBuf, str, &ppBuf, &puLen );
	std::string ver = (const char*)ppBuf;
	free( pVerBuf );
	while( true ) {
		std::string::size_type pos = ver.find( " " );
		if( pos == std::string::npos ) break;
		ver.replace( pos, 1, "" );
	}
	return ver;
}

///////////////////////////////////////////////////////////////////////////////
BOOL tools::in_array( const std::string &str, const std::vector<std::string> &strs )
{
	size_t ct;
	for( ct = 0; ct < strs.size(); ct ++ ) {
		if( !str.compare( strs[ct] ) ) return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL tools::is_dir( const std::string &path )
{
	DWORD attr = ::GetFileAttributes( path.c_str() );
	if( attr == -1 ) return FALSE;
	if( ( attr & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) return FALSE;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
DWORD tools::mkdir( const std::string &path )
{
	std::string parent = tools::dir_name( tools::rtrim( path, "\\" ).c_str() );
	if( !tools::is_dir( parent ) ) {
		DWORD result = tools::mkdir( parent );
		if( result != NO_ERROR ) return result;
	}

	if( !::CreateDirectory( path.c_str(), NULL ) ) {
		DWORD err = ::GetLastError();
		if( err != ERROR_ALREADY_EXISTS ) {
			return err;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
std::string tools::clsid2string( const CLSID &clsid )
{
	struct _IID {
		unsigned long x;
		unsigned short s1;
		unsigned short s2;
		unsigned char  c[8];
	} *pt = (struct _IID*)&clsid;

	char buf[40];
	sprintf_s( buf, "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				pt->x, pt->s1, pt->s2,
				pt->c[0], pt->c[1], pt->c[2], pt->c[3], pt->c[4], pt->c[5], pt->c[6], pt->c[7]	);
	return std::string( buf );
}

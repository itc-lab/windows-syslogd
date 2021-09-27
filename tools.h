// tools.h: tools クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLS_H__2F8F69D7_D523_4374_B479_695DC08C436F__INCLUDED_)
#define AFX_TOOLS_H__2F8F69D7_D523_4374_B479_695DC08C436F__INCLUDED_

#pragma warning (disable : 4786)

#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class tools
{
public:
	tools();
	virtual ~tools();

	static int explode( const std::string &str, const std::string &del, std::vector<std::string> &strs );
	static int explode( const std::string &str, char del, std::vector<std::string> &strs );
	static std::string implode( const std::string &del, const std::vector<std::string> &strs );
	static std::string LoadString( HINSTANCE hInstance, UINT uID );
	static std::string urlencode( const std::string &url );
	static std::string urlencode( const char *url, const char* space = "%20" );
	static std::string urldecode( const std::string &url );
	static std::string urldecode( const char *url );
	static std::string rawurldecode( const char *url );
	static int urldecode( const char* url, unsigned char* buf, int size = -1 );
	static std::string pathencode( const std::string &path );
	static std::string GetErrorMessage( DWORD err = 0 );
	static std::string utf8tosjis( const char* utf );
	static std::string sjistoutf8( const std::string &sjis );
	static std::string sjistoutf8( const char* sjis );
	static std::string cutPath( const char* path, int length );
	static std::string dir_name( const char* path );
	static std::string base_name( const char* path );
	static std::string ext_name( const char* path );
	static std::string trim( const std::string &str, const std::string &charlist = " \r\n" );
	static std::string ltrim( const std::string &str, const std::string &charlist = " \r\n" );
	static std::string rtrim( const std::string &str, const std::string &charlist = " \r\n" );
	static BOOL fnmatch( const char* string, const char* pattern );
	static std::string getFileVersion( HINSTANCE hInstance );
	static BOOL in_array( const std::string &str, const std::vector<std::string> &strs );
	static BOOL is_dir( const std::string &path );
	static DWORD mkdir( const std::string &path );
	static std::string clsid2string( const CLSID &clsid );
};

#endif // !defined(AFX_TOOLS_H__2F8F69D7_D523_4374_B479_695DC08C436F__INCLUDED_)

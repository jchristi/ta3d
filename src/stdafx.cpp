/*  TA3D, a remake of Total Annihilation
    Copyright (C) 2006  Roland BROCHARD

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA*/

/*
**  File: stdafx.cpp
** Notes:
**   Cire: stdafx.h and stdafx.cpp will generate a pch file
**         (pre compiled headers). Its goal is to include
**         everything that should be global to the project, as well
**         as platform specific setups and configurations.
*/

#include "stdafx.h"

namespace TA3D
{


String
format(const char* fmt, ...)
{
    if( !fmt )
        return String("");

    int result = -1, length = 256;
    char *buffer = 0;

	while(-1 == result || result > length )
	{
	    va_list args;
		va_start(args, fmt);

		length <<= 1;
		if (buffer)
		    delete [] buffer;
		buffer = new char [length + 1];
        memset(buffer, 0, length + 1);
        #if defined TA3D_PLATFORM_WINDOWS && defined TA3D_PLATFORM_MSVC
		result = _vsnprintf(buffer, length, fmt, args);
        #else
		result = vsnprintf(buffer, length, fmt, args);
        #endif
		va_end(args);
	}
	String s(buffer);
	delete [] buffer;
	return s;
}

sint32
SearchString(const String& s, const String& stringToSearch, const bool ignoreCase)
{
	static const std::basic_string <char>::size_type NotFound = std::string::npos;

	std::basic_string <char>::size_type iFind;
	String sz1, sz2;

	if(ignoreCase)
	{
		sz1 = String::ToUpper(s);
		sz2 = String::ToUpper(stringToSearch);
	}
	else
	{
		sz1 = s;
		sz2 = stringToSearch;
	}
	iFind = sz1.find(sz2);

	return ((NotFound == iFind) ? -1 : (sint32)iFind);
}


String
ReplaceString(const String& s, const String& toSearch, const String& replaceWith, const bool ignoreCase)
{
    String result = s;
	int f = 0;
	while ((f = SearchString(result, toSearch, ignoreCase)) != -1)
		result = result.replace(f, toSearch.size(), replaceWith);
	return result;
}


String
ReplaceChar(const String& s, const char toSearch, const char replaceWith)
{
    String ret(s);
    int l = s.size();
	for( int i = 0 ; i < l; ++i)
    {
		if(toSearch == ret[i])
		    ret[i] = replaceWith;
    }
	return ret;
}


bool
StartsWith(const String& a, const String& b)
{
    String y (a);
	String z (b);
	uint16 ai, bi;

	ai = (uint16)(y.toLower().length());
	bi = (uint16)(z.toLower().length());

	if (ai > bi)
		return  ( (y.compare( 0, bi, z ) == 0 ) ? true : false );
	return ( (z.compare( 0, ai, y ) == 0 ) ? true : false );
}


String
get_path(const String& s)
{
	String path;
	for( int i = s.size() - 1 ; i >= 0 ; i-- )
    {
		if( s[i] == '/' || s[i] == '\\' )
        {
			path = s;
			path.resize(i);
			break;
		}
    }
	return path;
}



#if defined TA3D_PLATFORM_WINDOWS && defined TA3D_PLATFORM_MSVC
void
ExtractPathFile(const String& szFullFileName, String& szFile, String& szDir)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	// Below extracts our path where the application is being run.
	::_splitpath_s( szFullFileName.c_str(), drive, dir, fname, ext );

	szFile = fname;
	szDir = drive;
	szDir += dir;
}
#endif


String
GetClientPath(void)
{
	static bool bName = false;
	static String szName = "";

	if (!bName)
	{
        # if defined TA3D_PLATFORM_WINDOWS && defined TA3D_PLATFORM_MSVC
		char fPath[ MAX_PATH ];
		String Tmp;
		::GetModuleFileNameA( NULL, fPath, MAX_PATH );
		ExtractPathFile( fPath, Tmp, szName );
        # endif
		bName = true;
	}
	return szName;
}

}

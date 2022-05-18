//*************************************************************************
//
// Copyright (C) 2008-2018, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: orsCommon.cpp 2009/9/22 22:14 JWS $
//
// Project: OpenRS
//
// Purpose:
//
// Author: Guo Wei, Wanshou Jiang, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: orsCommon.cpp,v $
//
// Revision 1.1 date: 2009/9/22 by JIANG Wanshou
// modified for linux
//

#include "stdafx.h"

#include "orsCommon.h"
#include "orsBase/orsTypedef.h"
#include "orsBase/orsString.h"

#include "orsBase/orsUtil.h"

#include <iostream>
#include <ctype.h>

/*
#include <stdlib.h>
#include <stdio.h>*/

//����windows�ӿ�������ĳ���ļ����µ��ļ�
//�Ա���win8.1 VC120 x64ƽ̨�¼���       ������ 2014 12 23

#if defined(_WIN64) || defined(WIN64)
#include <wtypes.h>
#include <shtypes.h>
#endif


#if defined(_WIN32) || defined(WIN32)
    #include <io.h>
#else
    #include <errno.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <sys/stat.h>

    //extern "C" {
        #include <dlfcn.h>
    //}

#endif

// Encodes binary data to Base64 code
// Returns size of encoded data.
int orsBase64::encode(const unsigned char* inData,
				   int dataLength,
				   stdString & outCode)
{
	stdString  result;

	// output buffer which holds code during conversation
	int len = getCodeLength( dataLength );
	char* out = new char[ len ];

	// charachers used by Base64
	static const char alph[] =
	{
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'0','1','2','3','4','5','6','7','8','9','*','-'
	};

	// mask - first six bits
	const int mask = 0x3F;

	// used as temp 24-bits buffer
	union
	{
		unsigned char bytes[ 4 ];
		unsigned int block;
	} buffer;

	// coversation is done by taking three bytes at time of input data int temp
	// then four six-bits values are extracted, converted to base64 characters
	// and at the end they are written to output buffer
	for( int i = 0, j = 0, left = dataLength; i < dataLength; i += 3, j += 4, left -= 3 )
	{
		//------------------------
		// filling temp buffer

		// get first byte and puts it at MSB position in temp buffer
		buffer.bytes[ 2 ] = inData[ i ];

		// more data left?
		if( left > 1 )
		{
			// get second byte and puts it at middle position in temp buffer
			buffer.bytes[ 1 ] = inData[ i + 1 ];
			// more data left?
			if( left > 2 )
				// get third byte and puts it at LSB position in temp buffer
				buffer.bytes[ 0 ] = inData[ i + 2 ];
			else
				// zero-padding of input data (last bytes)
				buffer.bytes[ 0 ] = 0;
		}
		else
		{
			// zero-padding of input data (last two bytes)
			buffer.bytes[ 1 ] = 0;
			buffer.bytes[ 0 ] = 0;
		}

		//------------------------
		// constructing code from temp buffer
		// and putting it in output buffer

		// extract first and second six-bit value from temp buffer
		// and convert is to base64 character
		out[ j ] = alph[ ( buffer.block >> 18 ) & mask ];
		out[ j + 1 ] = alph[ ( buffer.block >> 12 ) & mask ];
		// more data left?
		if( left > 1 )
		{
			// extract third six-bit value from temp buffer
			// and convert it to base64 character
			out[ j + 2 ] = alph[ ( buffer.block >> 6 ) & mask ];
			// more data left?
			if( left > 2 )
				// extract forth six-bit value from temp buffer
				// and convert it to base64 character
				out[ j + 3 ] = alph[ buffer.block & mask ];
			else
				// pad output code
				out[ j + 3 ] = '[';
		}
		else
		{
			// pad output code
			out[ j + 2 ] = '[';
			out[ j + 3 ] = '[';
		}
	}

	outCode.erase();
	outCode.append( out, len );
	delete[] out;
	return len;
}

// Decodes Base64 code to binary data
// Returns size of decoded data.
int orsBase64::decode(const stdString  & inCode,
				   int codeLength,
				   unsigned char* outData)
{
	// used as temp 24-bits buffer
	union
	{
		unsigned char bytes[ 4 ];
		unsigned int block;
	} buffer;
	buffer.block = 0;

	// number of decoded bytes
	int j = 0;

	for( int i = 0; i < codeLength; i++ )
	{
		// position in temp buffer
		int m = i % 4;

		char x = inCode[ i ];
		int val = 0;

		// converts base64 character to six-bit value
		if( x >= 'A' && x <= 'Z' )
			val = x - 'A';
		else if( x >= 'a' && x <= 'z' )
			val = x - 'a' + 'Z' - 'A' + 1;
		else if( x >= '0' && x <= '9' )
			val = x - '0' + ( 'Z' - 'A' + 1 ) * 2;
		else if( x == '*' )
			val = 62;
		else if( x == '-' )
			val = 63;

		// padding chars are not decoded and written to output buffer
		if( x != '[' )
			buffer.block |= val << ( 3 - m ) * 6;

		// temp buffer is full or end of code is reached
		// flushing temp buffer
		if( m == 3 || x == '[' )
		{
			// writes byte from temp buffer (combined from two six-bit values) to output buffer
			outData[ j++ ] = buffer.bytes[ 2 ];
			// more data left?
			if( x != '[' || m > 1 )
			{
				// writes byte from temp buffer (combined from two six-bit values) to output buffer
				outData[ j++ ] = buffer.bytes[ 1 ];
				// more data left?
				if( x != '[' || m > 2 )
					// writes byte from temp buffer (combined from two six-bit values) to output buffer
					outData[ j++ ] = buffer.bytes[ 0 ];
			}

			// restarts temp buffer
			buffer.block = 0;
		}

		// when padding char is reached it is the end of code
		if( x == '[' )
			break;
	}

	return j;
}

// Returns maximum size of decoded data based on size of Base64 code.
int orsBase64::getDataLength(int codeLength)
{
	return codeLength - codeLength / 4;
}

// Returns maximum length of Base64 code based on size of uncoded data.
int orsBase64::getCodeLength(int dataLength)
{
	int len = dataLength + dataLength / 3 + (int)( dataLength % 3 != 0 );

	// output code size must be multiple of 4 bytes
	if( len % 4 )
		len += 4 - len % 4;

	return len;
}

#if defined(_WIN32) || defined(WIN32)
bool getSubDirs(orsArray<ors_string> &dirs, const orsChar * path);
bool getDirs(const char * pDirPath, orsArray<ors_string> &dirs)
{
	getSubDirs(dirs, pDirPath);

	// add the main directory.
	dirs.push_back(pDirPath);

	return true;
}

bool getSubDirs(orsArray<ors_string> &dirs, const orsChar * path)
{
	stdString  newPath;

	stdString  searchString;
	searchString = path;

	searchString+= "/*.*";

	try
	{
		//struct _finddata_t  c_file;
		//long fhandle;

		WIN32_FIND_DATA c_file;
		HANDLE fhandle;

		if ((fhandle = FindFirstFile(searchString.c_str() , &c_file)) != INVALID_HANDLE_VALUE
			/*(fhandle=_findfirst( searchString.c_str(), &c_file ))!=-1*/)
		{
			// we only care about subdirs
			if ((c_file.dwFileAttributes & _A_SUBDIR) == _A_SUBDIR/*(c_file.attrib & _A_SUBDIR)==_A_SUBDIR*/)
			{
				// add c_file.name to the string array

				// we'll handle parents on our own or special directory such as ".svn"
				//(strcmp(c_file.name, ".")!=0) && (strcmp(c_file.name, "..")!=0) )

				if( c_file.cFileName[0] != '.' )
				{
					newPath = path;
					newPath+= "/";
					newPath += c_file.cFileName;
					getSubDirs( dirs, newPath.c_str() );
					dirs.push_back(  newPath.c_str() );
				}
			}

			// find the rest of them
			while( FindNextFile(fhandle , &c_file) /*_findnext( fhandle, &c_file ) == 0*/ )
			{
				if ((c_file.dwFileAttributes & _A_SUBDIR) == _A_SUBDIR)
				{
					// we'll handle parents on our own
					if (c_file.cFileName[0] != '.') // if ((strcmp(c_file.name, ".")!=0) && (strcmp(c_file.name, "..")!=0))
					{
						newPath = path;
						newPath+= "/";
						newPath += c_file.cFileName;
						getSubDirs( dirs, newPath.c_str() );
						dirs.push_back( newPath.c_str() );
					}
				}
			}
			FindClose(fhandle);
			//_findclose(fhandle);
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

ors_uint32 findFiles(stdString  dir, stdString  filter, orsArray<ors_string> &files)
{
	char buf0[80];

	strcpy( buf0, filter.c_str() );

	char *buf = buf0;
	char *nextFilter = NULL;

	dir += '/';

	do	{
		// ���ҷֺ�
		nextFilter = strchr( buf, ';' );

		if( nextFilter )
			*nextFilter = 0;

		stdString  fullPath = dir + buf;

		if( nextFilter )
			buf = ++nextFilter;

		//////////////////////////////////////////

		stdString  fileName;

		//struct _finddata_t  c_file;
		//long fhandle;

		WIN32_FIND_DATA c_file;
		HANDLE fhandle;

		try
		{
			//fhandle = _findfirst( fullPath.c_str(), &c_file );
			fhandle = FindFirstFile(fullPath.c_str(), &c_file);
			if (fhandle == INVALID_HANDLE_VALUE) 
				continue;	//return false;

			/*long iErr = -1;
			if( fhandle != -1 ) 
				iErr = 0;*/
			do 
			{
				bool bIsFolder = (c_file.dwFileAttributes & _A_SUBDIR) == _A_SUBDIR;

				bool bAddThisOne = !bIsFolder;

				// skip . and ..
				if (bIsFolder && (strcmp(c_file.cFileName, ".") == 0) || (strcmp(c_file.cFileName, "..") == 0))
				{
					bAddThisOne = false;
				}

				if (bAddThisOne)
				{
					fileName = dir;
					fileName += c_file.cFileName;
					files.push_back(fileName.c_str());
				}
			} while (FindNextFile(fhandle, &c_file));

			/*while (iErr == 0)
			{
				bool bIsFolder = (c_file.dwFileAttributes & _A_SUBDIR)==_A_SUBDIR;
				
				bool bAddThisOne = !bIsFolder;
				
				// skip . and ..
				if (bIsFolder && (strcmp(c_file.cFileName, ".") == 0) || (strcmp(c_file.cFileName, "..") == 0))
				{
					bAddThisOne = false;
				}
				
				if (bAddThisOne)
				{
					fileName = dir;
					fileName += c_file.cFileName;
					files.push_back( fileName.c_str() );
				}
				
				// find the rest of them
				iErr = _findnext( fhandle, &c_file );
			};*/

			FindClose(fhandle);
			//_findclose(fhandle);
		}
		catch (...)
		{
			return false;
		}

	}	while( nextFilter != NULL );


	return true;
}

bool findAllFiles( const orsChar * dir, const orsChar * extend, orsArray<orsString> &files )
{
	orsArray<ors_string> dirs;

	getDirs( dir, dirs );

	for (orsArray<ors_string>::iterator it = dirs.begin();it!=dirs.end(); it++)
	{
		stdString  curDir = (*it).c_str();

		if (curDir.length() == 0 || curDir[0] == '.' )
			continue;

		if( !findFiles(curDir, extend, files) )
		{
			return true;
		}
	}

	return true;
}

#else

void findFiles(orsArray<orsString >  &files, const orsChar *path, const orsChar* extend)
{
	DIR   *dir = NULL;
	char   fullpath[1024],currfile[1024];
	struct   dirent   *s_dir;
	struct   stat   file_stat;
	strcpy(fullpath,path);
	dir=opendir(fullpath);
	if(dir == NULL)
	return ;
	while   ((s_dir=readdir(dir))!=NULL)   {
		if   ((strcmp(s_dir->d_name,".")==0)||(strcmp(s_dir->d_name,"..")==0))   continue;
		sprintf(currfile,"%s/%s",fullpath,s_dir->d_name);
		stat(currfile,&file_stat);
		if   (S_ISDIR(file_stat.st_mode))
			findFiles(files,currfile,extend);
		else{
			char temp[512];
			memset(temp,0,512);
			strcpy(temp,s_dir->d_name);
			char *temp2 = strtok(temp,".");
			if(temp2){
				temp2 = strtok(NULL,".");
				if(temp2 && 0 == strcmp(temp2,extend))
				{
					files.push_back(currfile);
				}
			}
		}
	}
	closedir(dir);
}

bool findAllFiles( const orsChar * dir, const orsChar * extend, orsArray<orsString> &files )
{
	char temp[512];

	memset(temp,0,512);
	strcpy(temp,extend);
	
	char * temp2 = strtok(temp,".");

	if(temp2){
		temp2 = strtok(NULL,".");
		if(temp2){
			char shortextend[32];
			strcpy(shortextend,temp2);
			findFiles(files,dir,shortextend);
		}
	}

	return true;
}

#endif


//////////////////////////////////////////////////////////////////////////

#if defined(_WIN32) || defined(WIN32)

#include "AtlBase.h"
#include "AtlConv.h"
#include "shlobj.h"

class CShellLink
{
public:
	CShellLink();
	virtual ~CShellLink();

	orsString GetPath( const orsString &lnkPath );

private:
	IShellLink	*m_pShellLink;
	IPersistFile *m_pPersisFile;

	orsString m_exeDir;
};


CShellLink::CShellLink()
{
	m_pShellLink = NULL;
	m_pPersisFile = NULL;

	::CoInitialize(NULL);

	HRESULT hr = ::CoCreateInstance(
				CLSID_ShellLink,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_IShellLink,
				(LPVOID *)&m_pShellLink
				);

	if( SUCCEEDED(hr) )	{
		hr = m_pShellLink->QueryInterface( IID_IPersistFile, (LPVOID *) &m_pPersisFile );

		if( SUCCEEDED(hr) ) {
			m_exeDir = getExeDir().c_str();
		}
	}

}


CShellLink::~CShellLink()
{
	if( m_pShellLink )
		m_pShellLink->Release();

	if( m_pPersisFile )
		m_pPersisFile->Release();
}


// ���ݿ�ݷ�ʽ�����û�������
void orsSetEnv( const orsChar *path )
{
	char *pathEnv;
	pathEnv = getenv( "PATH" );

	orsString dir = orsString::getDirFromPath( path );

	// �Ѵ���
	if( NULL != strstr( pathEnv, dir.c_str() ) )
		return ;
		
	orsString  newPathEnv = "PATH=";
	newPathEnv = newPathEnv + pathEnv;
	newPathEnv = newPathEnv + ";";
	newPathEnv = newPathEnv + dir;
	
	putenv( newPathEnv.c_str() );
}

orsString CShellLink::GetPath( const orsString &lnkPath )
{
	char buffer[256];
	orsString filePath;

	if( NULL == m_pShellLink || NULL == m_pPersisFile )
		return filePath;

	USES_CONVERSION;

	HRESULT hr;

	hr = m_pPersisFile->Load( T2COLE( lnkPath.c_str() ), STGM_READ );

	if( SUCCEEDED(hr) )	{
		hr = m_pShellLink->GetPath( buffer, 256, NULL, 1);

		if( SUCCEEDED(hr) )	{
			filePath = buffer;

			//////////////////////////////////////////////////////////////////////////
			// �������ӵĿ�����

			for( int i=0; i<2; i++ )
			{
				HINSTANCE  hInstance;

				// Load the DLL/EXE
				hInstance = LoadLibraryEx( filePath, NULL, LOAD_LIBRARY_AS_DATAFILE );

				if( hInstance ) {
					FreeLibrary( hInstance );

					// �滻�ɹ�����������
					if( i== 1 ) {
						// ��������·��
						m_pShellLink->SetPath( filePath );

						// ��������
						m_pPersisFile->Save( T2COLE(lnkPath.c_str() ), TRUE );
					}

					// ��ӵ�����������
					orsSetEnv( filePath.c_str() );

					break;
				}

				// �滻·��
				const char *pFind = orsString::findSubStr_i( buffer, "OpenRS" );

				if( pFind != NULL ) {
					int index = m_exeDir.find_i( _T("OpenRS" ) );

					if( index > -1 ) {
						filePath = m_exeDir.left( index );
						filePath += pFind;
					}
				}
			}
		}
	}

	return filePath;
}

HMODULE LoadLibraryOrLink( const char *libraryname )
{
	if( orsString::findSubStr_i( libraryname, ".lnk" ) != NULL ) {
		CShellLink shLnk;

		orsString dllPath = shLnk.GetPath( libraryname );

		return LoadLibrary( dllPath.c_str() );
	}
	else
		return LoadLibrary(libraryname);
}


#endif


//////////////////////////////////////////////////////////////////////////

orsPluginHandle loadDynamicLib( const orsChar *libraryname )
{
	orsPluginHandle libraryHandle = NULL;

#if defined(_WIN32) || defined( WIN32)
	libraryHandle = LoadLibraryOrLink( libraryname ); //;
	
	if( NULL == libraryHandle )
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
			);

		_getPlatform()->logPrint( ORS_LOG_ERROR, "Error in load plugin %s : %s", libraryname, lpMsgBuf );

		// Free the buffer.
		LocalFree( lpMsgBuf );
	}

#  else
    dlerror();
	libraryHandle = dlopen(libraryname, RTLD_LAZY);

	if( NULL == libraryHandle )
    {
		char *err;

		err = dlerror();
		_getPlatform()->logPrint( ORS_LOG_ERROR, "Error2 in load plugin %s : %s", libraryname, err );
	}

#  endif


	return libraryHandle;
}

void unloadDynamicLib(orsPluginHandle handle)
{

#  if defined(__WIN32__) || defined(_WIN32)
	FreeLibrary((HMODULE)handle);
#  elif HAVE_DLFCN_H
	dlclose(handle);
#  endif

}

orsSymbolHandle getDynamicSymbol(orsPluginHandle handle, const orsChar * name)
{
	orsSymbolHandle symbolHandle = NULL;

#  if defined(__WIN32__) || defined(_WIN32)
	symbolHandle = (void*)GetProcAddress( (HINSTANCE)handle, name );
#  else
	symbolHandle = dlsym(handle, name );
#  endif

	return symbolHandle;
}

#ifdef WIN32

HMODULE g_hModule = NULL;
BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	g_hModule = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	
	return TRUE;
}

#endif


#if defined(__WIN32__) || defined(_WIN32)
stdString  getExeDir()
{
	char exeDir[512];
	memset(exeDir,0,512);
	char selfDir[512];
	memset(selfDir,0,512);

	HMODULE hModule = g_hModule;

// 	if( NULL == hModule)
// 		hModule = GetModuleHandle( "orsBase.dll");
// 	
// 	if( NULL == hModule)
// 		hModule = GetModuleHandle( "orsBaseD.dll");

	assert( hModule );

	GetModuleFileName( hModule, exeDir, 512);
	
	char* temp_str = strrchr(exeDir,'\\');
	memcpy(selfDir,exeDir,temp_str-exeDir);

	return selfDir;
}
#else
stdString  getExeDir()
{
	stdString nullStr;

	char linkname[64]; /* /proc/<pid>/exe */
	pid_t pid;
	char buf[512];
	size_t size = 512;

	/* Get our PID and build the name of the link in /proc */
	pid = getpid();

	if (snprintf(linkname, sizeof(linkname), "/proc/%i/exe", pid) < 0)
	{
		abort();
	}

	int ret;
	/* Now read the symbolic link */
	ret = readlink(linkname, buf, size);

	/* In case of an error, leave the handling up to the caller */
	if (ret == -1)
		return nullStr;

	/* Report insufficient buffer size */
	if (ret >= size)
	{
		errno = ERANGE;
		return nullStr;
	}

	/* Ensure proper NUL termination */
	buf[ret] = 0;
	char selfDir[512];
	memset(selfDir,0,512);
	char* temp_str = strrchr(buf,'/');
	memcpy(selfDir,buf,temp_str-buf);
	return selfDir;
}
#endif

stdString ToStdString(orsString str)
{
	return str.c_str();
}

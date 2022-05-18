#include "stdafx.h"
#include <algorithm>
#include "orsXUtilityService.h"
#include "orsBase/orsFileFormat.h"

#if defined(WIN32) || defined(_WIN32)
#include <direct.h>
#include <io.h>
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#elif defined(LINUX32) || defined(_LINUX32) || defined(LINUX64) || defined(_LINUX64)
#include <stdarg.h>
#include <sys/stat.h>
#define ACCESS access
#define MKDIR(a) mkdir((a),S_IRWXU|S_IRWXG|S_IRWXO)
#else
#error "Platform not Specified!"
#endif

//启动时被调用
bool orsXUtilityService::startup( orsIPlatform *platform )
{
	m_macroSet = _getPlatform()->createProperty();

	return true;
}

//关闭时被调用
void orsXUtilityService::shutdown()
{

}

//是否启动成功
bool orsXUtilityService::isok()
{
	return true;
}

#include "orsCommon.h"

extern stdString  getExeDir();

bool orsXUtilityService::GetDirectory_EXE( orsString &dir )
{
// 	char exeDir[512];
// 	
// 	memset(exeDir,0,512);
// 	char selfDir[512];
// 	memset(selfDir,0,512);
// 	
// 	HMODULE hOrsBase = GetModuleHandle("orsBase.dll");
// 
// 	if( NULL == hOrsBase )
// 		hOrsBase = GetModuleHandle("orsBaseD.dll");
// 
// 	assert( hOrsBase );
// 
// 	GetModuleFileName( hOrsBase,  exeDir, 512);
// 	
// 	char* temp_str = strrchr(exeDir,'/');
// 	
// 	memcpy(selfDir,exeDir,temp_str-exeDir);
	
//	dir = selfDir;

	dir = getExeDir().c_str();
	
	return true;
}


bool orsXUtilityService::GetDirectory_ETC( orsString &dirPath )
{
	orsString exeDir;
	GetDirectory_EXE( exeDir );
	
	orsIUtilityService *pUtilityService = (orsIUtilityService *)_getPlatform()->getService( ORS_SERVICE_UTILITY);
	
	const orsChar *desktop = orsString::findSubStr_i( exeDir.c_str(), "desktop" );
	if( NULL != desktop )	{
		dirPath = exeDir.left( desktop - exeDir.c_str() - 1);
		dirPath  += "/desktop/etc/";

		if( pUtilityService->CheckFolderExist( dirPath ) )
			return true;
	}
	
	const orsChar *openrs = orsString::findSubStr_i( exeDir.c_str(), "openrs" );
	if( NULL != openrs )	{
		dirPath = exeDir.left( openrs - exeDir.c_str() );
		dirPath  += "/openrs/etc/";
		
		if( pUtilityService->CheckFolderExist( dirPath ) )
			return true;
	}
	
	dirPath = exeDir + "/etc/";
	if( pUtilityService->CheckFolderExist( dirPath ) )
		return true;
	
	dirPath = exeDir + "/../etc/";
	if( pUtilityService->CheckFolderExist( dirPath ) )
		return true;

	return false;
}



bool orsXUtilityService::ShellExecute( const orsChar *cmdLine, bool bWaite)
{
#ifdef ORS_PLATFORM_WINDOWS
 	
	if( !bWaite ) {
		if( WinExec( cmdLine, SW_SHOW ) > 31 )
			return true;
		return false;
	}
	
	STARTUPINFO si; //一些必备参数设置
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi; //必备参数设置结束
	
	if( CreateProcess( NULL, (orsChar *)cmdLine, NULL,NULL,FALSE,0,NULL,NULL, &si, &pi) )
	{
		WaitForSingleObject( pi.hProcess, INFINITE );
		
		return true;
	}

#endif
	
	return false;
}


void orsXUtilityService::AddMarco( const orsChar *macroName, const orsChar *macroValue )
{
	m_macroSet->addAttr( macroName, macroValue );
}

bool orsXUtilityService::GetMarco( const orsChar *macroName, orsString &macroValue )
{
	return m_macroSet->getAttr( macroName, macroValue );
}


// replace MACRO prop in outputProp with inputProp
bool orsXUtilityService::ReplaceMacroString( orsIProperty *outputProp, orsIProperty *inputProp, orsIProperty *paraProp )
{ 
	int count;
	
	do{
		count = 0;

		ors_string name;
		orsVariantType type;
		ors_int32 valuenum;
		orsArray<ors_string> values;
		ors_string filePath;
		
		ors_uint32 nOutFileNum = outputProp->size();

		for (ors_uint32 iFile = 0; iFile<nOutFileNum;iFile++)
		{
			outputProp->getAttributeInfo( iFile, name, type, valuenum);	
			outputProp->getAttr(name, filePath);
			
			int pos0 = filePath.find( "$(" );
			if( pos0 != -1 )	{
				int pos1 = filePath.find( ')', pos0 + 2 );

				orsString macro = filePath.mid( pos0+2, pos1-pos0-2 );

				orsString macroValue;

				if( !GetMarco( macro, macroValue ) )	{
					if( !inputProp->getAttr( macro, macroValue ) )
					{	
						if( NULL != paraProp && !paraProp->getAttr( macro, macroValue ) )
						{		
							if( macroValue.isEmpty() ) {
								_getPlatform()->logPrint( ORS_LOG_ERROR, "Can not replace MACRO %s", macro.c_str() );
								return false;
							}
						}
					}
				}

				if( macroValue.length() > 3 )	{
					int pos2 = macroValue.reverseFind('.');
					if( -1 != pos2 )
						macroValue = macroValue.left( pos2 );
				}

				orsString newValue = filePath.left( pos0  );
				newValue += macroValue;
				newValue += filePath.rightFromPos( pos1+1 );

				outputProp->setAttr( name, newValue );
				count++;
			}	
		}

	}	while( count > 0 );


	return true;
}


orsArray<ors_int32> orsXUtilityService::String2Integers( orsString &strIntegers )
{
	bool Flag = false;

	int strlength = strIntegers.length();

	orsArray<int> commaIndex;
	orsArray<ors_int32> integers;

	orsChar *s = (orsChar *)strIntegers.c_str();

	if( NULL == s )
		return integers;

	commaIndex.push_back( 0 );

	int i = 0;
	while( *s )	{
		if( _T(',') == *s ) {
			commaIndex.push_back( i+1 );

			*s = 0;
		}
		
		i++;	s++;
	};

	//////////////////////////////////////////////////////////////////////////
	int k;
	const orsChar *s0;
	for( k=0; k<commaIndex.size(); k++ )
	{
		i = commaIndex[k];

		s0 = s = (orsChar *)strIntegers.c_str() + i;

		bool bRange = false;
		while( *s )	{
			if( _T(':') == *s || _T('-') == *s  ) {
				*s++ = 0;

				int i0 = atoi(s0);
				int i1 = atoi(s);

				for( i=i0; i<=i1; i++)
					integers.push_back( i );

				bRange = true;
				break;
			}
			s++;
		};
		
		if( !bRange )
			integers.push_back( atoi(s0) );
	};

	return integers;
}

orsArray<ors_float32> orsXUtilityService::String2Float( orsString &strFloats )
{
	bool Flag = false;
	
	int strlength = strFloats.length();
	
	orsArray<int> commaIndex;
	orsArray<ors_float32> floats;
	
	orsChar *s = (orsChar *)strFloats.c_str();
	orsChar tmpNumber[20];
	orsChar* pNumber = tmpNumber ;
	if( NULL == s )
		return floats;
	
	commaIndex.push_back( 0 );
	
	int i = 0;
	while( *s )	{
		if( _T(' ') != *s ) {
			*pNumber = *s;
			s++; pNumber++;
		}else
		{
			floats.push_back(atof(tmpNumber));
			pNumber = tmpNumber ;
		}
		
	};
	
	//////////////////////////////////////////////////////////////////////////
	
	return floats;
}

orsString orsXUtilityService::Integers2String( orsArray<ors_int32> &integers )
{
	int i, j, n;
	char buf[40];

	orsString strIntegers;
	
	n = integers.size();

	for( i=0; i<n; i++ )
	{
		for( j=i+1; j<n; j++)
		{
			if( ( integers[j] - integers[i] ) > j-i )
			{
				if( j-i > 1 )
					sprintf( buf, "%d-%d,", integers[i], integers[j-1] );
				else
					sprintf( buf, "%d,", integers[i] );

				strIntegers += buf;

				i = j;

				break;
			}
		}

		// 结束?
		if( j == n )	{
			if( j-i > 1 )	
				sprintf( buf, "%d-%d", integers[i], integers[j-1] );
			else
				sprintf( buf, "%d,", integers[i] );
			
			strIntegers += buf;	

			break;
		}
	}

	return strIntegers;
}


#if defined(ORS_PLATFORM_WINDOWS )

#include <WinBase.h>
ors_int64 orsXUtilityService::GetAvailableMemory()
{
	MEMORYSTATUS   memory; 
	memory.dwLength   =sizeof(memory);   
	memory.dwLength   =sizeof(memory);   
	
	GlobalMemoryStatus(&memory);
	
	return memory.dwAvailPhys;
}

#elif defined(ORS_PLATFORM_LINUX)

#define MEM_PART "/proc/meminfo"


//用于存储内存信息的结构体
struct mem_info
{
	char MemTotal[20];
	char MemFree[20];
	char MemAvailable[20];
	char Buffers[20];
	char Cached[20];
};

typedef struct mem_info MEM_info, *pMEM_info;

#define FILEBUFFER_LENGTH 1024
//打开fileName指定的文件，从中读取第lineNumber行
//返回值：成功返回1，失败返回0
int get_file_line(char *result, char *fileName, int lineNumber)
{
	FILE *filePointer;
	int i = 0;
	char buffer[FILEBUFFER_LENGTH];
	if ((fileName == NULL) || (result == NULL))
	{
		return 0;
	}

	if (!(filePointer = fopen(fileName, "rb")))
	{
		return 0;
	}


	while ((!feof(filePointer)) && (i<lineNumber))
	{
		if (!fgets(buffer, FILEBUFFER_LENGTH, filePointer))
		{
			return 0;
		}
		i++;//差点又忘记加这一句了
	}

	strcpy(result, buffer);

	if (0 != fclose(filePointer))
	{
		return 0;
	}

	return 1;
}

//按行读取/proc/meminfo中的信息，并保存到mem结构体中
int get_mem_info(pMEM_info mem)
{
	char buffer[300];
	if (NULL == mem)
	{
		printf("\nget_mem_info:param null!\n");
		return 0;
	}
	memset(mem, 0, sizeof(MEM_info));
	if (1 == get_file_line(buffer, MEM_PART, 1))//读取第一行
	{
		sscanf(buffer, "%*s %s", mem->MemTotal);
		if (1 == get_file_line(buffer, MEM_PART, 2))//读取第二行
		{
			sscanf(buffer, "%*s %s", mem->MemFree);
			if (1 == get_file_line(buffer, MEM_PART, 3))//读取第三行
			{
				sscanf(buffer, "%*s %s", mem->MemAvailable);
				if (1 == get_file_line(buffer, MEM_PART, 4))//读取第四行
				{
					sscanf(buffer, "%*s %s", mem->Buffers);
					if (1 == get_file_line(buffer, MEM_PART, 5))//读取第五行
					{
						sscanf(buffer, "%*s %s", mem->Cached);
						return 1;
					}
				}
			}
		}
	}

	return 0;
}
ors_int64 orsXUtilityService::GetAvailableMemory()
{	
	MEM_info mem;
	get_mem_info(&mem);
	return atol(mem.MemAvailable) * 1024;
}

#endif

////////////////Assign Output Data Type 
orsDataTYPE orsXUtilityService::GetOutputDataType(ors_string &strOutputDataType) 
{
	if(strOutputDataType=="Byte") return ORS_DT_BYTE;
	if(strOutputDataType=="UInt16") return ORS_DT_UINT16;
	if(strOutputDataType=="Int16") return ORS_DT_INT16;
	if(strOutputDataType=="UInt32") return ORS_DT_UINT32;
	if(strOutputDataType=="Int32") return ORS_DT_INT32;
	if(strOutputDataType=="Float32") return ORS_DT_FLOAT32;
	if(strOutputDataType=="Float64") return ORS_DT_FLOAT64;
	else return ORS_DT_UnKNOWN;
}


#include <sys/stat.h>
bool orsXUtilityService::CheckFileExist( const orsChar *filePath )
{
	if (NULL == filePath)
		return false;

	// 20181214,yanyuan
#ifdef ORS_PLATFORM_WINDOWS

	DWORD attr;
	attr = GetFileAttributes(filePath);
	return  (attr != (DWORD)(-1)) && !(attr & FILE_ATTRIBUTE_DIRECTORY); //不是目录

	//20181214,yanyuan
#elif defined(ORS_PLATFORM_LINUX)

	struct stat stFileInfo;

	// Attempt to get the file attributes
	if (stat(filePath, &stFileInfo) == 0)
		return stFileInfo.st_mode & S_IFREG;
	else
		return false;

#endif
}


bool orsXUtilityService::CheckFolderExist( const orsChar *folderPath )
{
	if (NULL == folderPath)
		return false;

#ifdef ORS_PLATFORM_WINDOWS

	DWORD attr;
	attr = GetFileAttributes( folderPath );  
	return  ( attr != (DWORD)(-1) )  && ( attr & FILE_ATTRIBUTE_DIRECTORY ); 

//	2018/12/14 yanyuan
#elif defined(ORS_PLATFORM_LINUX)
	//////////////////////////////////////////////////////////////////////////
	//20181130,by whx

	orsString::toUnixPathFormat(folderPath);
	struct stat stFileInfo;

	// Attempt to get the file attributes
	if (stat(folderPath, &stFileInfo) == 0)
		return S_ISDIR(stFileInfo.st_mode);
	else
		return false;

#endif
}



#include <fcntl.h>//file control
#if defined(LINUX32) || defined(_LINUX32) || defined(LINUX64) || defined(_LINUX64)
#include <unistd.h>
#endif

bool orsXUtilityService::CopyFile( const orsChar *filePath,  const orsChar *newFilePath, bool bFailIfExists )
{
#ifdef ORS_PLATFORM_WINDOWS

	return ::CopyFile( filePath, newFilePath, bFailIfExists );

#elif defined(ORS_PLATFORM_LINUX)

//#define MAX_SIZE 1024
//	
//	int in, out;
//	
//	int nRead=0;
//	
//	char buffer[MAX_SIZE];
//	
//	in =open( filePath, O_RDONLY, S_IRUSR);	//lost mode: S_IRUSR
//	
//	out =open( newFilePath, O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IXUSR );
//	
//	while( (nRead =read(in, buffer, MAX_SIZE)) >0 )
//		write(out, buffer, nRead);
//	
//    close(in);
//	close(out);
//
	//by whx,20211008,原来的方法太慢了
	char cmd[2048];
	sprintf(cmd, "cp %s %s", filePath, newFilePath);

	return system(cmd);

	return true;

#endif
	
	assert( false );
	return false;
}



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

/************
* 功能：将源文件夹下所有文件包括子文件夹拷贝至目标文件夹下。若目标文件夹不存在，则创建目标文件夹。
************/

bool orsXUtilityService::CopyFolder( const orsChar *srcDir, const orsChar *dstDir, bool bFailIfExists )
{

	if (!CheckFolderExist(srcDir))
		return false;

	if (orsString(srcDir) == orsString(dstDir))
		return true;

	if (!CheckFolderExist(dstDir))
		CreateFolder(dstDir);

	//////////////////////////////////////////////////////////////////////////
	orsString::toUnixPathFormat(srcDir);
	orsString csSourceDir = srcDir;

	int iLen = strlen(srcDir);
	if (srcDir[iLen - 1] != '/')
	{
		csSourceDir += "/";
	}

#ifdef ORS_PLATFORM_WINDOWS

	//////////////////////////////////////////////////////////////////////////	
	struct _finddata_t  c_file;
	intptr_t fhandle;
	
	try
	{
		orsString csSourceFolderFilter = csSourceDir + "*.*";
		fhandle = _findfirst( csSourceFolderFilter.c_str(), &c_file );
		
		long iErr = -1;
		if( fhandle != -1 ) 
			iErr = 0;
		
		while( iErr == 0 )
		{
			bool bIsFolder = (c_file.attrib & _A_SUBDIR)==_A_SUBDIR;

			// skip . and ..
			if( bIsFolder )
			{
				if( strcmp(c_file.name, ".") !=0 && strcmp( c_file.name, "..") != 0 ) {

					orsString srcSubDir = csSourceDir;
					srcSubDir += c_file.name;

					orsString dstSubDir = dstDir;	dstSubDir += "/";
					dstSubDir += c_file.name;

					CopyFolder( srcSubDir, dstSubDir, bFailIfExists );
				}
			}
			else	{

				orsString srcFile = csSourceDir;
				srcFile += c_file.name;
				
				orsString dstFile = dstDir;	dstFile += "/";	
				dstFile += c_file.name;
			
				// 怕有权限问题
				::SetFileAttributes( srcFile, FILE_ATTRIBUTE_NORMAL);
				
				::CopyFile( srcFile, dstFile, FALSE );
			}
			
			// find the rest of them
			iErr = _findnext( fhandle, &c_file );
		};
		
		_findclose(fhandle);
	}
	catch (...)
	{
		return false;
	}
#elif defined(ORS_PLATFORM_LINUX)

	//try
	//{

	//	DIR *dir;
	//	struct dirent *ptr;

	//	if ((dir = opendir(srcDir)) == NULL)
	//	{
	//		printf("Open Directory or file %s Error：%s\n",
	//			srcDir, strerror(errno));
	//	}
	//	while ((ptr = readdir(dir)) != NULL)
	//	{
	//		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
	//			continue;

	//		if (ptr->d_type & DT_DIR)    //dir 
	//		{
	//			orsString srcSubDir = csSourceDir;
	//			srcSubDir += ptr->d_name;

	//			orsString dstSubDir = dstDir;	dstSubDir += "/";
	//			dstSubDir += ptr->d_name;

	//			CopyFolder(srcSubDir.c_str(), dstSubDir.c_str(), true);
	//		}
	//		else // file
	//		{
	//			orsString srcFile = csSourceDir;
	//			srcFile += ptr->d_name;

	//			orsString dstFile = dstDir;	dstFile += "/";
	//			dstFile += ptr->d_name;
	//			CopyFile(srcFile.c_str(), dstFile.c_str(), true);
	//		}
	//	}
	//	closedir(dir);
	//}
	//catch (const std::exception&)
	//{
	//	printf("Open Directory %s Error：%s\n",
	//		srcDir, strerror(errno));

	//	return false;
	//}

//by whx,20211008,原来的方法太慢了
char cmd[2048];
sprintf(cmd, "cp -r %s %s", csSourceDir.c_str(), dstDir);

return system(cmd);

#endif

	return true;
}


bool orsXUtilityService::DeleteFile( const orsChar *filePath )
{
	orsString::toUnixPathFormat(filePath);

#ifdef ORS_PLATFORM_WINDOWS
	bool delFlag = ::DeleteFile( filePath );

	return delFlag;

#elif defined(ORS_PLATFORM_LINUX)

	if (remove(filePath) != 0)
		return false;
	
	return true;

#endif

	assert(false);
}


bool orsXUtilityService::DeleteFolder( const orsChar *pszDir)
{
	orsString::toUnixPathFormat(pszDir);

#ifdef ORS_PLATFORM_WINDOWS
	try
	{
		EmptyFolder(pszDir);
			
		if( RemoveDirectory(pszDir) == FALSE )
			return	false;
	}
	catch (...)
	{
		return false;
	}
	
#elif defined(ORS_PLATFORM_LINUX)
	
// 	orsChar cmdBuf[2048];
// 
// 	sprintf( cmdBuf, "rm -rf %s",  pszDir );
// 
// 	system(cmdBuf);

	try
	{
		EmptyFolder(pszDir);

		if (remove(pszDir) != 0)
			return	false;
	}
	catch (...)
	{
		return false;
	}
	
#endif
	
	return	true;
}


bool orsXUtilityService::EmptyFolder( const orsChar *pszDir )
{
	if (!CheckFolderExist(pszDir))
		return false;

	orsString::toUnixPathFormat(pszDir);

	orsString csSourceDir = pszDir;

	int iLen = strlen(pszDir);
	if (pszDir[iLen - 1] != '/')
	{
		csSourceDir += "/";
	}

#ifdef ORS_PLATFORM_WINDOWS

	struct _finddata_t  c_file;
	intptr_t fhandle;

	try
	{
		orsString csSourceFolderFilter = csSourceDir + "*.*";
		fhandle = _findfirst( csSourceFolderFilter, &c_file );
		
		long iErr = -1;
		if( fhandle != -1 ) 
			iErr = 0;
		
		while( iErr == 0 )
		{
			bool bIsFolder = (c_file.attrib & _A_SUBDIR)==_A_SUBDIR;
			
			// skip . and ..
			if( bIsFolder )	{
				if( strcmp(c_file.name, ".") !=0 && strcmp( c_file.name, "..") != 0 ) {
					orsString srcSubDir = csSourceDir; 
					srcSubDir += c_file.name;
					
					DeleteFolder( srcSubDir );
				}
			}
			else	{
				orsString srcFile = csSourceDir;
				srcFile += c_file.name;

				::DeleteFile( srcFile );
			}

			// find the rest of them
			iErr = _findnext( fhandle, &c_file );
		}
		_findclose(fhandle);
	}
	catch (...)
	{
		return false;
	}

#elif defined(ORS_PLATFORM_LINUX)

// 	orsChar cmdBuf[2048];
// 
// 	sprintf(cmdBuf, "cd %s \n rm -rf*", pszDir);
// 
// 	system(cmdBuf);
	try
	{

		DIR *dir;
		struct dirent *ptr;

		if ((dir = opendir(pszDir)) == NULL)
		{
			printf("Open Directory or file %s Error：%s\n",
				pszDir, strerror(errno));
		}
		while ((ptr = readdir(dir)) != NULL)
		{
			if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
				continue;

			if (ptr->d_type & DT_DIR)    //dir 
			{
				orsString srcSubDir = csSourceDir;
				srcSubDir += ptr->d_name;

				DeleteFolder(srcSubDir);
	
			}
			else // file
			{
				orsString srcFile = csSourceDir;
				srcFile += ptr->d_name;
				DeleteFile(srcFile);
			}
		}
		closedir(dir);
	}
	catch (const std::exception&)
	{
		printf("Open Directory %s Error：%s\n",
			pszDir, strerror(errno));

		return false;
	}

#endif

	return true;
}

/***********************************************************************************************
*函数功能描述 ：查找指定扩展名的文件, 不区分扩展名大小写
*函数参数 ：
	-pszDir: 文件夹路径
	-filters:  过滤器 ，以*. 开头: *.txt, *.tif,  默认值 *.*
*函数返回值 ：全路径文件名
*创建人： yanyuan
*修改人 ：20181214,yanyuan， 支持Linux 
***********************************************************************************************/
int orsXUtilityService::FindFiles(const orsChar* pszDir, orsArray<orsString>& vFiles, const orsChar* filters)
{
	vFiles.empty();

	if (!CheckFolderExist(pszDir))
		return 0;

	orsString::toUnixPathFormat(pszDir);

	orsString srcFolder = pszDir;
	int iLen = strlen(pszDir);

	if (pszDir[iLen - 1] != '/')
		srcFolder += "/";

#ifdef ORS_PLATFORM_WINDOWS
	try
	{

		struct _finddata_t  c_file;
		intptr_t fhandle;

		orsString srcFilterFolder = srcFolder + "*.*";

		fhandle = _findfirst(srcFilterFolder, &c_file);

		long iErr = -1;
		if (fhandle != -1)
			iErr = 0;

		while (iErr == 0)
		{
			bool bIsFolder = (c_file.attrib & _A_SUBDIR) == _A_SUBDIR;

			// skip . and .. ;//跳过当前目录或父目录
			if (bIsFolder) {
				if (strcmp(c_file.name, ".") != 0 && strcmp(c_file.name, "..") != 0) {
					orsString srcSubDir = srcFolder; 
					srcSubDir += c_file.name;
					//orsArray<orsString>  subFileNames = 
					FindFiles(srcSubDir, vFiles, filters);

// 					int i, n = subFileNames.size();
// 					for( i=0;i<n;i++)
// 						vFiles.push_back( subFileNames[i] );
				}
			}
			else {
				orsString srcFile = srcFolder;
				srcFile += c_file.name;

				char* suffix = (char*) orsString::getSuffix(srcFile);

				if (suffix != NULL) {
					char zsufix[32] = { 0 };
					strncpy(zsufix, suffix, strlen(suffix));

					std::transform(zsufix, zsufix + strlen(zsufix), zsufix, ::toupper);
					char ext[32] = { 0 };
					strncpy(ext, filters + 2, strlen(filters + 2));
					
					std::transform(ext, ext + strlen(ext), ext, ::toupper);

					if (orsString(zsufix) == orsString(ext) || orsString(filters) == "*.*")
						vFiles.push_back(srcFile);
				}
			}

			// find the rest of them
			iErr = _findnext(fhandle, &c_file);
		}
		_findclose(fhandle);
	}
	catch (...)
	{

	}

#elif defined(ORS_PLATFORM_LINUX)
	try
	{

		DIR *dir;
		struct dirent *ptr;

		if ((dir = opendir(pszDir)) == NULL)
		{
			printf("Open Directory or file %s Error：%s\n",
				pszDir, strerror(errno));
		}
		while ((ptr = readdir(dir)) != NULL)
		{
			if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
				continue;

			if (ptr->d_type & DT_REG || ptr->d_type & DT_LNK)    ///file or link file
			{
				orsString srcFile = srcFolder;
				srcFile += ptr->d_name;
				
				char* suffix = (char*)orsString::getSuffix(srcFile);

				if (suffix != NULL) {
					char zsufix[32] = { 0 };
					strncpy(zsufix, suffix, strlen(suffix));

					std::transform(zsufix, zsufix + strlen(zsufix), zsufix, ::toupper);
					char ext[256] = { 0 };
					strncpy(ext, filters + 2, strlen(filters + 2));

					std::transform(ext, ext + strlen(ext), ext, ::toupper);

					if (orsString(zsufix) == orsString(ext) || orsString(filters) == "*.*")
						vFiles.push_back(srcFile);
				}

			}
			else if (ptr->d_type & DT_DIR)    ///dir
			{
				orsString srcSubDir = srcFolder;
				srcSubDir += ptr->d_name;

				//orsArray<orsString>  subFileNames = 
				FindFiles(srcSubDir, vFiles, filters);

// 				for (int i = 0; i <  subFileNames.size(); i++)
// 					vFiles.push_back(subFileNames[i]);
			}
		}
		closedir(dir);

	}
	catch (const std::exception&)
	{
		printf("Open Directory %s Error：%s\n",
			pszDir, strerror(errno));
	}
#endif;

	return vFiles.size();
}


bool orsXUtilityService::MoveFile( const orsChar *filePath,  const orsChar *newFilePath, bool bFailIfExists )
{
	if (0 == strcmp(filePath, newFilePath))
		return true;

	if (!CheckFileExist(filePath))
		return false;

	if ( bFailIfExists && CheckFileExist(newFilePath))
		return false;

#if defined(ORS_PLATFORM_WINDOWS )
	if( CheckFileExist(newFilePath) )
		DeleteFile(newFilePath);

	// 同盘符,同目录，直接改名
	if (orsString::getDirFromPath(filePath) == orsString::getDirFromPath(newFilePath))
	{
		// 相对路径
// 		if (':' != filePath[1]) {
// 			char curDir[1024];
// 			::GetCurrentDirectory(1000, curDir);
// 
// 			orsString oldFullPath = curDir;
// 			orsString newFullPath = curDir;
// 
// 			oldFullPath = oldFullPath + "\\";	oldFullPath = oldFullPath + filePath;
// 			newFullPath = newFullPath + "\\";	newFullPath = newFullPath + newFilePath;
// 
// 			return (0 == rename(oldFullPath, newFullPath));
// 		}

		return (0 == rename(filePath, newFilePath));
	}
	// 能否直接移动
	if( false == ::MoveFile( filePath, newFilePath ) ) {	

		if( !CopyFile( filePath, newFilePath, bFailIfExists ) )
			return false;

		return DeleteFile( filePath );
	}
#elif defined(ORS_PLATFORM_LINUX)
	//return(0 == mv(filePath, newFilePath));

	char cmd[2048];
	sprintf(cmd, "mv %s %s", filePath, newFilePath);

	return system(cmd);

#endif
	
	return true;
}


bool orsXUtilityService::MoveFolder(const orsChar *srcDir, const orsChar *dstDir, bool bFailIfExists)
{
	if (!CheckFolderExist(srcDir))
		return false;

	// 同盘符，直接改名
	if (orsString::getDirFromPath(srcDir) == orsString::getDirFromPath(dstDir))
		return (0 == rename(srcDir, dstDir));


	if( !CopyFolder( srcDir, dstDir, bFailIfExists ) )
		return false;
	
	DeleteFolder( srcDir );
	
	return true;

}

bool orsXUtilityService::IsSupportedImageFile(const orsChar* fileName)
{
	ref_ptr<orsIImageService> imgService =
		ORS_PTR_CAST(orsIImageService, _getPlatform()->getService(ORS_SERVICE_IMAGE));

	// 只将tif,bmp ,jpg等图像格式，dem 等不作为有效图像格式
	orsFileFormatList formatList;// = imgService->getSupportedImageFormats();
	orsFileFormat	f;

	f.name = "TIF Image";	f.ext = "tif";	formatList.push_back(f);
	f.name = "TIFF Image";	f.ext = "tiff";	formatList.push_back(f);
	f.name = "ERDAS Image";	f.ext = "img";	formatList.push_back(f);
	f.name = "BIL Image";	f.ext = "bil";	formatList.push_back(f);
	f.name = "JPEG Image";	f.ext = "jpg";	formatList.push_back(f);
	f.name = "JPEG Image";	f.ext = "jpeg";	formatList.push_back(f);
	f.name = "BMP Image";	f.ext = "bmp";	formatList.push_back(f);

	 const orsChar* suffix = orsString::getSuffix(fileName);
	 orsString ext = suffix;

	 for (int i = 0; i < formatList.size(); i++)
	 {
		 if (formatList[i].ext == ext)
		 {
			 return true;
		 }
	 }
	return false;
}

//////////////////////////////////////////////////////////////////////////

//创建目录
bool orsXUtilityService::CreateFolder(const orsChar *pszDir)
{
	char pDir[1024]="";
	
	ors_int32 i = 0;
	ors_int32 iRet;
	ors_int32 iLen = strlen(pszDir);
	strncpy(pDir, pszDir, iLen);
	//在末尾加/
	if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/')
	{
		pDir[iLen] = '/';
		pDir[iLen + 1] = '\0';
	}
	
	iLen=strlen(pDir);
	// 创建多级目录
	for (i = 0;i < iLen;i ++)
	{
		if (pDir[i] == '\\' || pDir[i] == '/')
		{ 
			if(i==0)	continue;
			
			pDir[i] = '\0';
			
			//如果不存在,创建
			iRet = ACCESS(pDir,0);
			if (iRet != 0)
			{
				printf("To create dir: %s\n", pDir);
				iRet = MKDIR(pDir);
				if (iRet != 0)
				{
					printf("Fail to create dir: %s\n", pDir);
					return false;
				} 
			}
			//支持linux,将所有\换成/
			pDir[i] = '/';
		} 
	}
	
	return true;
}
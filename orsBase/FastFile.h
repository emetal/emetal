/******************************************************************************
 * $Id: FastFile.h, v 1.0 date: 04/26/2007 ������ Exp$
 *
 * Project:  
 * Purpose:  declarations of FastFile
 * Author:   ������, wsjws@163.com
 *
 ******************************************************************************
 * Copyright (c) 2007, ������
 *
 *		δ�õ���Ȩ���𿽱�
 *
 ******************************************************************************
 *
 * $Log: FastFile.h,v $
 *
 * Revision 1.1  04/26/2007  ������
 * �Ľ�����ʹ����ӳ����ļ���һ���֣����ж�̬ӳ��
 *
 * Revision 1.2  10/24/2008  �ž�	
 *	֧��д�ļ���д�ļ�ʱע�⣬д���Ҫ����FinishWriting()�����ļ���С����Ϊʵ�ʴ�С
 *
 * Revision 1.3  05/11/2009  �ž�
 * Ϊʵ���ļ��������Ӻ����ӿڡ���ͬһ���ļ���������ļ�ӳ�䣬�����ļ�
 *
 * Revision 1.4  06/08/2009	�ž�
 * ӳ�䳤�Ȳ��ܳ����ļ����ȣ������޷�ӳ�䡣
 */

#if !defined(AFX_FASTFILE_H__583948C4_4CC3_11D4_BBA6_0050BAAF35F4__INCLUDED_)
#define AFX_FASTFILE_H__583948C4_4CC3_11D4_BBA6_0050BAAF35F4__INCLUDED_

// #if _MSC_VER >= 1000
// #pragma once
// #endif // _MSC_VER >= 1000

static void ErrorMsg()
{
	LPVOID lpMsgBuf;
	char msg[80];
	
	FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(), 
		LANG_NEUTRAL, (LPTSTR)&lpMsgBuf,
		0,
		NULL );	
	
	sprintf( msg, "Error: %s", lpMsgBuf );
	LocalFree( lpMsgBuf );
	
	getPlatform()->logPrint( ORS_LOG_ERROR, msg );
}

const	DWORD	Default_BytesToMap=256*1024*1024;			// 64k�ı���	�ڴ�ӳ��Ŀ��С��256Mb

#define  ModeRead	1
#define  ModeWrite	2
#define	 ModeUpdate	3

class CFastFile
{
private:

	orsString	m_fileName;

	HANDLE	m_inHFile;
	HANDLE  m_hFile;
	HANDLE  m_hFileMapping;

	DWORD	m_accessMode;

	LPBYTE	m_pMappedAdress;	// ��ȡָ��

	__int64 m_accessOffset;		// �ļ���ȡƫ����

	__int64	m_fileSize;			// �ļ���С
	__int64	m_mapOffset;		// ӳ�����

	long 	m_mapSize;			// ��ǰ�ڴ�ӳ���С
	long	m_BytesToMap;		// �趨���ڴ�ӳ���С
	int		m_mode;

	bool	m_bMapPartDirty;	//ӳ����Ƿ񱻸���

private:
	void mapPartOfFile()
	{
		if( m_pMappedAdress )
			UnmapViewOfFile( m_pMappedAdress );
		
		m_pMappedAdress = (LPBYTE)MapViewOfFile( m_hFileMapping, m_accessMode, 
			DWORD(m_mapOffset>>32), DWORD(m_mapOffset & 0xffffffff), m_mapSize );
		
		//////////////////////////////////////////////////////////////////////////
		if( m_pMappedAdress == NULL ) 
		{
			DWORD err=GetLastError();
			ErrorMsg();

			MEMORYSTATUS	mem;
			GlobalMemoryStatus(&mem);
		}

		if(m_mode==ModeUpdate)
		{
			m_bMapPartDirty=false;
		}
	};

	void adjustMapAddress( long bytes)
	{
		if( m_mapSize == 0 )	// map whole file	ӳ�������ļ������õ���ӳ���ַ
			return ;
		
		if( bytes > m_BytesToMap )		//��ȡ���ȴ���ӳ��鳤��
		{
			char msg[80];
			sprintf( msg, "Error : Access size > mapped memory size ( %ld )", m_BytesToMap );
			getPlatform()->logPrint( ORS_LOG_ERROR, msg );
			exit(1);
		}
		
		if( NULL == m_pMappedAdress || m_accessOffset < m_mapOffset		//��дλ�ò���ӳ�����
			|| m_accessOffset+bytes > m_mapOffset + m_mapSize			//bytesΪ���������һ��
			|| m_accessOffset+bytes < m_mapOffset)						//bytesΪ��ֵ����ǰ��һ��
		{
			if(m_mode==ModeWrite)
			{
				//���ļ�ӳ��ǿ�Ƹ��µ�����
				FlushViewOfFile(m_pMappedAdress, m_mapSize);
			}
			if(m_mode==ModeUpdate && m_bMapPartDirty)
			{
				FlushViewOfFile(m_pMappedAdress, m_mapSize);
			}
			
			// ����ʹ�µ�ӳ�����ʹ��ǰ��ȡ��λ��ӳ������
			m_mapOffset = m_accessOffset + bytes - m_BytesToMap/4;
			
			if( m_mapOffset < 0L )
				m_mapOffset = 0L;

			//λ��ֵ������ϵͳ�ķ������ȵı���(����Ϊֹ,Windows������ʵ�ִ���ķ������Ⱦ�Ϊ64KB)
			m_mapOffset = m_mapOffset/ (64*1024)*(64*1024);	

			if( m_fileSize > 0  && m_mapOffset + m_BytesToMap > m_fileSize )
			{
				m_mapSize = long((m_fileSize - m_mapOffset) & 0xffffffff);
			}
			else
			{
				m_mapSize = m_BytesToMap;
			}
			
			
			mapPartOfFile();
		}
	}; 

	//д�ļ�ʱ����չ�ļ���С��������ӳ���ڴ�
	void appendMapView( long bytes)
	{
		if( bytes > m_mapSize )		//д�볤�ȴ���ӳ��鳤��
		{
			char msg[80];
			sprintf( msg, "Error : Access size > mapped memory size ( %ld )", m_mapSize );
			getPlatform()->logPrint( ORS_LOG_ERROR, msg );
			exit(1);
		}
		
		if( NULL == m_pMappedAdress || m_accessOffset < m_mapOffset 
			|| m_accessOffset+bytes > m_mapOffset + m_mapSize ) 
		{
			//���ļ�ӳ��ǿ�Ƹ��µ�����
			FlushViewOfFile(m_pMappedAdress, m_mapSize);
			// ����ʹ�µ�ӳ�����ʹ��ǰ��ȡ��λ��ӳ������
			m_mapOffset = m_accessOffset + (bytes - m_mapSize)/2;
			
			if( m_mapOffset < 0L )
				m_mapOffset = 0L;
			
			//λ��ֵ������ϵͳ�ķ������ȵı���(����Ϊֹ,Windows������ʵ�ִ���ķ������Ⱦ�Ϊ64KB)
			m_mapOffset = m_mapOffset/ (64*1024)*(64*1024);	

		//	DWORD dwFileSizeLo = 0; 
		//	DWORD dwSizeHigh = 0; 
		//	dwFileSizeLo = GetFileSize(m_hFile,&dwSizeHigh); 

		//	DWORD	FilePointer=SetFilePointer (m_hFile, 0, 0, FILE_CURRENT);
			long	DistanceToMoveHigh = long((m_mapOffset+m_mapSize)>>32);
			SetFilePointer (m_hFile, long((m_mapOffset+m_mapSize)&0xffffffff), &DistanceToMoveHigh, FILE_BEGIN);
			SetEndOfFile(m_hFile);
			DWORD dwSizeHigh = 0;
			m_fileSize = GetFileSize(m_hFile,&dwSizeHigh); 
			m_fileSize += dwSizeHigh << 32;

			if(m_mapOffset+m_mapSize>m_fileSize)
			{
				char msg[80];
				sprintf( msg, "Error : map size > file size ( %ld )", m_mapSize );
				getPlatform()->logPrint( ORS_LOG_ERROR, msg);
				exit(1);
			}

			Close();

			m_hFile = CreateFile( m_fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS /*| FILE_FLAG_WRITE_THROUGH*/, 0 );
			m_inHFile = INVALID_HANDLE_VALUE; 
//			err=GetLastError();

			if( m_hFile == INVALID_HANDLE_VALUE || m_hFile == (HANDLE)HFILE_ERROR )
			{
				char msg[256];
				sprintf(msg, "Creating file %s failed!\r\n",m_fileName );
				getPlatform()->logPrint( ORS_LOG_ERROR, msg);
				return;
			}
			m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL );

			mapPartOfFile();
		}
	}; 

public:

	CFastFile  ()
	{
		m_inHFile = INVALID_HANDLE_VALUE;
		m_hFile = INVALID_HANDLE_VALUE;
		
		m_hFileMapping = NULL;
		
		m_pMappedAdress = NULL;
		
		m_accessMode = FILE_MAP_READ;
		m_accessOffset = 0L;
		
		m_mapOffset = 0L;				// ӳ�����
		m_BytesToMap = Default_BytesToMap;
		m_mapSize	= m_BytesToMap;
		m_fileSize  = 0L;
	};

	virtual ~CFastFile  ()
	{
		Close();
	};

	//2009.10.9�����ļ�ӳ���С
	void CFastFile::SetMappingSize(DWORD dwMapSize)
	{
		m_BytesToMap=dwMapSize;
	};
	
	
	bool Create( const char *fileName )
	{
		char msg[256];
		
		Close();
		
		// ���ļ�
		m_hFile = CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
			NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, 0 );

		m_fileName=fileName;
		
		m_inHFile = INVALID_HANDLE_VALUE; 
		
		if( m_hFile == INVALID_HANDLE_VALUE || m_hFile == (HANDLE)HFILE_ERROR )
		{
			sprintf(msg, "Creating file %s failed!\r\n",fileName );
			getPlatform()->logPrint( ORS_LOG_ERROR, msg);
			return FALSE;
		}

		//m_fileSize = 0x4000000;		//64M
		m_mapOffset = 0L;
		m_accessMode = FILE_MAP_WRITE;
		m_accessMode = FILE_MAP_ALL_ACCESS;
		m_accessOffset=0;
		m_fileSize = m_mapSize = m_BytesToMap;	//Ϊ�´������ļ��趨��ʼ��С
		m_mode=ModeWrite;
		
		// ����Ӱ���ڴ�ӳ��handle
		m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 0, m_mapSize, NULL );
		
		mapPartOfFile();
		
		return TRUE;
	};

	bool Open( const orsChar *fileName, const orsChar *mode/*'r','w','u'*/, char *lpMapName = NULL, HANDLE hFile = INVALID_HANDLE_VALUE)
	{
		char msg[256];
		
		Close();

		//�ļ������жϣ�ӳ�������ļ����Ƿֿ�ӳ��
		OFSTRUCT fdOFSTRUCT;
		HFILE hfd = OpenFile(fileName, &fdOFSTRUCT, OF_READ);
		if (hfd == HFILE_ERROR) 
		{
			getPlatform()->logPrint( ORS_LOG_WARNING, "Couldn't open %s!", fileName);
			return false;
		}

		BY_HANDLE_FILE_INFORMATION fdInfo;
		if (!GetFileInformationByHandle((HANDLE)hfd, &fdInfo))
			return false;
	
		DWORD len = fdInfo.nFileSizeLow;
		if (len <= m_BytesToMap) 
		{
			m_mapSize = len;
		} 
		else
		{
			m_mapSize = m_BytesToMap;
		}
		_lclose(hfd);
		
		// ���ļ�
		if( hFile == INVALID_HANDLE_VALUE ) 
		{
			if( strchr( mode, 'w' ) ) 
			{//��д��
				m_hFile = CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING , FILE_FLAG_RANDOM_ACCESS, 0 );
				
				m_mode=ModeWrite;
			}
			else if(strchr( mode, 'u' ))
			{
				m_hFile = CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
					OPEN_EXISTING , FILE_FLAG_RANDOM_ACCESS, 0 );
				
				m_mode=ModeUpdate;
			}
			else 
			{
				m_hFile = CreateFile( fileName, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING , FILE_FLAG_RANDOM_ACCESS, 0 );
				
				m_mode=ModeRead;
			}
			
			
			if( m_hFile == INVALID_HANDLE_VALUE || m_hFile == (HANDLE)HFILE_ERROR )
			{
				DWORD err=GetLastError();
				sprintf(msg, "Opening file failed! %s\r\n",fileName );
				//getPlatform()->logPrint( ORS_LOG_ERROR, msg);
				return FALSE;
			}
			m_inHFile = INVALID_HANDLE_VALUE; 
		}
		else
		{
			m_inHFile = m_hFile = hFile;
		}
		
		// ����Ӱ���ڴ�ӳ��handle
		if( strchr( mode, 'w' ) || strchr( mode, 'u' )) 
		{	
			m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 0, 0, lpMapName );
			if( m_hFileMapping == NULL )
			{
				sprintf(msg, "CreateFileMapping for writing failed!\r\n",fileName );
				getPlatform()->logPrint( ORS_LOG_ERROR, msg);
				return FALSE;
			}
			DWORD dwSizeHigh;
			m_fileSize = GetFileSize( m_hFile, &dwSizeHigh );
			m_fileSize += dwSizeHigh << 32;
			m_accessMode = FILE_MAP_ALL_ACCESS;
		}
		else
		{
			m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, lpMapName );
			if( m_hFileMapping == NULL )
			{
				sprintf(msg, "CreateFileMapping for reading failed!\r\n",fileName );
				getPlatform()->logPrint( ORS_LOG_ERROR, msg);
				return FALSE;
			}
			
			DWORD dwSizeHigh;
			
			m_fileSize = GetFileSize( m_hFile, &dwSizeHigh );
			
			m_fileSize += dwSizeHigh << 32;
			
			if( m_fileSize < m_mapSize )
				m_mapSize = 0;
			
			m_accessMode = FILE_MAP_READ;
		}
		
		m_mapOffset = 0L;
		
		mapPartOfFile();
		
		return TRUE;
	};

	void Close()
	{
		if( m_pMappedAdress )
			UnmapViewOfFile( m_pMappedAdress );
		
		if( m_hFileMapping  ) 
			CloseHandle( m_hFileMapping );
		
		if( m_inHFile == INVALID_HANDLE_VALUE )
			if( m_hFile  ) 
				CloseHandle( m_hFile );
			
			m_pMappedAdress = NULL;
			m_hFileMapping = NULL;
			m_hFile = INVALID_HANDLE_VALUE;

			m_accessOffset=0;
	};

	bool Read( void *buf, long bytes)
	{
		adjustMapAddress( bytes );
		
		if( m_pMappedAdress == NULL )
			return FALSE;
		
		// ����64k��ĩβ���֣�
		if( m_mapSize > 0 &&  m_accessOffset + bytes > m_mapOffset + m_mapSize )	
		{//bug	
			//2008.10.22 �޸�  zj
			// ����ԭӳ�䷽��������ĩβ����64k�ļ�¼��ÿ�ζ���ȡ�����һ����¼
			// �޸�Ϊ���һ����ڴ�ӳ����ʵ��ӳ���С
			DWORD dwCurrentFilePosition=SetFilePointer( 
							m_hFile, // must have GENERIC_READ and/or GENERIC_WRITE 
							0,     // do not move pointer 
							NULL,  // hFile is not large enough to need this pointer 
							FILE_CURRENT);  // provides offset from current position 

			dwCurrentFilePosition=SetFilePointer( m_hFile, -bytes, NULL, FILE_END );
			
			DWORD numberOfBytesRead;
			if( ReadFile( m_hFile, buf, bytes, &numberOfBytesRead, NULL ) == FALSE )
				ErrorMsg();

			assert(false);
		}
		else	
		{//m_accessOffset�������ļ��е�ƫ��ֵ��m_mapOffset��ǰ����ʼ��ƫ��ֵ
			memcpy(buf, m_pMappedAdress + (m_accessOffset-m_mapOffset), bytes);
		}
		m_accessOffset += bytes;
		
		return true;
	}; 

	//���÷�ʽ��Read��ͬ���������ڴ濽����ֱ�ӷ���ָ��
	void* ReadPtr(long bytes)
	{
		adjustMapAddress( bytes );
		void *buf=0;
		
		if( m_pMappedAdress == NULL )
			return 0;
		
		// ����64k��ĩβ���֣�
		if( m_mapSize > 0 &&  m_accessOffset + bytes > m_mapOffset + m_mapSize )	
		{//bug	
			//2008.10.22 �޸�  zj
			// ����ԭӳ�䷽��������ĩβ����64k�ļ�¼��ÿ�ζ���ȡ�����һ����¼
			// �޸�Ϊ���һ����ڴ�ӳ����ʵ��ӳ���С
			DWORD dwCurrentFilePosition=SetFilePointer( 
				m_hFile, // must have GENERIC_READ and/or GENERIC_WRITE 
				0,     // do not move pointer 
				NULL,  // hFile is not large enough to need this pointer 
				FILE_CURRENT);  // provides offset from current position 
			
			dwCurrentFilePosition=SetFilePointer( m_hFile, -bytes, NULL, FILE_END );
			
//			DWORD numberOfBytesRead;
//			if( ReadFile( m_hFile, buf, bytes, &numberOfBytesRead, NULL ) == FALSE )
//				ErrorMsg();
			assert(false);
		}
		else	
		{//m_accessOffset�������ļ��е�ƫ��ֵ��m_mapOffset��ǰ����ʼ��ƫ��ֵ
		
			buf=m_pMappedAdress + (m_accessOffset-m_mapOffset);
		}
		m_accessOffset += bytes;
		
		assert(buf);
		
		return buf;
	};

	bool Write(const void *buf, long bytes)
	{//��create��ʽ���ļ��������ļ��������µ�����
		appendMapView( bytes );
		
		if( m_pMappedAdress == NULL )
			return FALSE;
		
		memcpy(m_pMappedAdress+ (m_accessOffset-m_mapOffset), buf, bytes);
		m_accessOffset += bytes;
		
		return true;
	}; 

	bool Update(void *buf, long bytes)
	{//��rw��ʽ�򿪣�����ָ���ֶ�
		adjustMapAddress( bytes );
		
		if( m_pMappedAdress == NULL )
			return FALSE;
		
		// ����64k��ĩβ���֣�
		if( m_mapSize > 0 &&  m_accessOffset + bytes > m_mapOffset + m_mapSize )	
		{//bug	
			//2008.10.22 �޸�  zj
			// ����ԭӳ�䷽��������ĩβ����64k�ļ�¼��ÿ�ζ���ȡ�����һ����¼
			// �޸�Ϊ���һ����ڴ�ӳ����ʵ��ӳ���С
			DWORD dwCurrentFilePosition=SetFilePointer( 
				m_hFile, // must have GENERIC_READ and/or GENERIC_WRITE 
				0,     // do not move pointer 
				NULL,  // hFile is not large enough to need this pointer 
				FILE_CURRENT);  // provides offset from current position 
			
			dwCurrentFilePosition=SetFilePointer( m_hFile, -bytes, NULL, FILE_END );
			
			DWORD numberOfBytesRead;
			if( WriteFile( m_hFile, buf, bytes, &numberOfBytesRead, NULL ) == FALSE )
				ErrorMsg();
		}
		else	
		{//m_accessOffset�������ļ��е�ƫ��ֵ��m_mapOffset��ǰ����ʼ��ƫ��ֵ
			memcpy(m_pMappedAdress + (m_accessOffset-m_mapOffset), buf, bytes);
		}
		m_accessOffset += bytes;
		m_bMapPartDirty=true;
		return true;
	};
	

	//д�ļ���������ã������ļ���СΪʵ�ʴ�С
	void FinishWriting()
	{
		FlushViewOfFile(m_pMappedAdress, m_mapSize);

		if( m_pMappedAdress )	
		{
			UnmapViewOfFile( m_pMappedAdress );
			m_pMappedAdress = NULL;
		}
		
		if( m_hFileMapping  ) 
		{
			CloseHandle( m_hFileMapping );
			m_hFileMapping = NULL;
		}
		
		long	DistanceToMoveHigh = long(m_accessOffset>>32);
		SetFilePointer (m_hFile, long(m_accessOffset & 0xffffffff), &DistanceToMoveHigh, FILE_BEGIN); 
		
		SetEndOfFile(m_hFile);
		
		if( m_hFile  )
		{
			CloseHandle( m_hFile );
			m_hFile = NULL;
		}
	};

	void ReMapping()
	{//����ӳ�䵱ǰ��, ������ݱ����º����¶�ȡ�ļ��е�����
		mapPartOfFile();
	};

	void FlushViewPage()
	{//����ǰҳ������ǿ�Ƹ��µ��ļ���
		BOOL	flag;
		flag=FlushViewOfFile(m_pMappedAdress, m_mapSize);

		if(flag==0)
		{
			DWORD err=GetLastError();
		}
	};

	void Seek(__int64 offset, int origin )
	{
		switch( origin )
		{
		case SEEK_CUR:
			m_accessOffset += offset;
			break;
		case SEEK_SET:
			m_accessOffset = offset;	
			break;
		case SEEK_END:
			assert(false);
		}
	};

	//�ӵ�ǰλ������offset���ֽ�
	//offset�����ɸ�
	void Skip(long offset)
	{
		adjustMapAddress( offset );
		m_accessOffset += offset;
	};

	void Remove()
	{
		if( m_pMappedAdress )	
		{
			UnmapViewOfFile( m_pMappedAdress );
			m_pMappedAdress = NULL;
		}
		
		if( m_hFileMapping  ) 
		{
			CloseHandle( m_hFileMapping );
			m_hFileMapping = NULL;
		}
		
		SetFilePointer (m_hFile, 0L, NULL, FILE_BEGIN); 
		
		SetEndOfFile(m_hFile);
		
		if( m_hFile  )
		{
			CloseHandle( m_hFile );
			m_hFile = NULL;
		}
	};

	LPBYTE GetMapAddress()
	{//ӳ������ʼ��ַ
		return	m_pMappedAdress;
	}
	
	//ȡ�ļ�ӳ�����
	HANDLE  GetFileMapping()
	{
		return	m_hFileMapping;
	}
};



#endif // !defined(AFX_FASTFILE_H__583948C4_4CC3_11D4_BBA6_0050BAAF35F4__INCLUDED_)


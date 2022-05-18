#ifndef OPENRS_STRING_H_HH
#define OPENRS_STRING_H_HH

#include <string>
#include <string.h>
#include <assert.h>


//#include "orsIPlatform.h"
//xb 2010/3/26 add function "orsBasicString right(int nPos)"
//xb 2010/4/20 debug function "left" when nCount < 0


template <typename char_type>
class orsBasicString
{
	typedef int	size_type;

protected:
	void *(*m_mallocFun)( size_t size );
	void (*m_freeFun)( void *ptr );

    char_type*      m_data;
    size_type       m_capacity, m_length;

	void init()
    {
		m_capacity = 0;
		m_length = 0;
		m_data = NULL;
	}

public:
    orsBasicString() : m_capacity(0), m_length(0), m_data(NULL)
	{
		m_mallocFun = malloc;
		m_freeFun = free;
	}

    orsBasicString( const orsBasicString& str ) : m_capacity(0), m_length(0), m_data(NULL)
    {
		m_mallocFun = malloc;
		m_freeFun = free;

		assign(str.c_str());
    }

    orsBasicString( const char_type* str ) : m_capacity(0), m_length(0), m_data(NULL)
	{
		m_mallocFun = malloc;
		m_freeFun = free;

		assign(str);
    }

	~orsBasicString()  {  dealloc_data();  }

	static orsBasicString combine_strs(const char_type * str1, const char_type* str2)
	{
		int str1_len = 0;
		int str2_len = 0;
		int str_len;

		if (str1 != NULL) 
			str1_len = orsStrLen(str1);

		if (str2 != NULL) 
			str2_len = orsStrLen(str2);

		str_len = str1_len + str2_len;

		char_type *combine_str = new char_type[ str_len +1 ];

		if(str1_len > 0 )
			memcpy( combine_str, str1, str1_len );
		if (str2_len > 0)
			memcpy( combine_str + str1_len, str2, str2_len );

		combine_str[str_len] = 0;

		orsBasicString ret( combine_str );

		delete combine_str;

		return ret;
	}

    const char_type* data() const  {  return m_data;  }

    orsBasicString& operator=( const char_type* str )  {  assign( str );  return *this;}
	orsBasicString& operator=( char_type* str )  {  assign( str );  return *this;}
    orsBasicString& operator=( const orsBasicString& str )  {  assign( str.c_str() );  return *this;}

    orsBasicString& operator+=( const orsBasicString& rhs ) {  return append( rhs.c_str() );  }
    orsBasicString& operator+=( char_type c )           {  return append( c );  }
    orsBasicString& operator+=( const char_type* str )  {  return append( str );  }

    size_type length() const    {  return m_length;  }

    bool isEmpty() const {  return 0 == m_length;	}  

    void clear()  {  dealloc_data(); }	//  init(); 

    const char_type* c_str() const
    {
        return data();
    }

	operator const char_type *()	{
		return data();
	}

	operator const char_type *() const	{
		return data();
	}


    char_type& operator[]( size_type index )
        {  return m_data[index];  }

    const char_type& operator[]( size_type index ) const
        {  return data()[index];  }

	orsBasicString left( int nCount ) const
	{
		orsBasicString strLeft;

		if( nCount > m_length )
			nCount = m_length;

		if (nCount <= 0)
			return strLeft;

		strLeft.alloc_data( nCount );

		memcpy( strLeft.m_data, m_data, nCount );
		strLeft.m_length = nCount;
		strLeft.m_data[nCount] = '\0';

		return strLeft;
	};

	orsBasicString mid( int nPos, int nCount ) const
	{
		orsBasicString strRight;

		if( nPos + nCount > m_length )
			nCount = m_length - nPos;
	
		if (nCount <= 0)
			return strRight;
		
		strRight.alloc_data( nCount );

		if( nCount > 0 ) {
			memcpy(strRight.m_data, m_data+nPos, nCount);
			strRight.m_length = nCount;
			strRight.m_data[nCount] = '\0';
		}

		return strRight;
	};

	orsBasicString right( int nCount ) const
	{
		orsBasicString strRight;

		if (nCount <= 0)
			return strRight;
		
		int nPos = 0;
		if( nCount < m_length )
		{
			nPos = m_length - nCount;
			strRight.alloc_data( nCount+1 );

			memcpy(strRight.m_data, m_data + nPos, nCount );
			strRight.m_length = nCount;
			strRight.m_data[nCount] = '\0';
		}
		
		return strRight;
	};

	orsBasicString rightFromPos( int nPos ) const
	{
		orsBasicString strRight;
		
		int nCount = 0;
		
		if( nPos < m_length )
		{
			nCount = m_length - nPos;

			if (nCount <= 0)
				return strRight;

			strRight.alloc_data( nCount+1 );
			
			memcpy(strRight.m_data, m_data + nPos, nCount );
			strRight.m_length = nCount;
			strRight.m_data[nCount] = '\0';
		}
		
		return strRight;
	};
	
	//////////////////////////////////////////////////////////////////////////

/**
    * this will strip lead and trailing character passed in.
    * So if you want to remove blanks:
    *    orsString temp("       asdfasdf      ");
    *    orsString trimmedString = temp.trim(" \n\t\r");
    *
    *    this will now contain "asdfasdf" without the blanks.
    *
    */
	orsBasicString &trim(const orsBasicString &valueToTrim = " \t\r\n")
	{
		if(this->length() == 0) return *this;
		if(valueToTrim.isEmpty()) return *this;

		int startPos = 0;
		int endPos   = m_length -1;

		// 从前往后
		while( startPos != m_length && valueToTrim.find( m_data[startPos] )  ) 
			++startPos;

		if( startPos == m_length )
		{
			m_data[0] = 0;
			m_length = 0;
			return *this;
		}

		// 从后往前
		while( endPos != startPos && valueToTrim.find( m_data[endPos] ) )
			 --endPos;

		m_length = endPos - startPos;

		int i;
		for( i=0; i<m_length; i++)
			m_data[i] = m_data[ i + startPos];
		m_data[m_length] = 0;

		return *this;
	}

	orsBasicString trim(const orsBasicString& valueToTrim = " \t\r\n") const
	{
		orsBasicString tempString(*this);

		return tempString.trim(valueToTrim);
	}
   /**
    *  Substitutes searchKey string with replacementValue and returns a
    *  string.  Will replace all occurrences found if "replaceAll" is true.
    */
	orsBasicString substitute( orsChar searchKey, orsChar replacementValue) const
	{
		orsBasicString result = *this;

		orsChar *s = result.m_data;
		while( *s )	{
			if( *s == searchKey )
				*s = replacementValue;
			s++;
		}
		return result;
   }

	//////////////////////////////////////////////////////////////////////////
	int find( char_type ch, int nStart = 0 ) const
	{
		if( NULL == m_data )
			return -1;

		const char_type *found = _tcschr( m_data+nStart, ch );
		
		if( found )
			return ( found - m_data );
		else
			return -1;
	}

	int find( const char_type *subStr, int nStart = 0 ) const
	{
		if( NULL == m_data )
			return -1;

		const char_type *found = _tcsstr( m_data+nStart, subStr );
		
		if( found )
			return ( found - m_data );
		else
			return -1;
	}

	int find_i( const char_type *subStr, int nStart = 0  ) const
	{
		if( NULL == m_data )
			return -1;

		const char_type *found = findSubStr_i( m_data+nStart, subStr );
		
		if( found )
			return ( found - m_data );
		else
			return -1;
	}

	int reverseFind( char_type ch ) const
	{
		if( NULL == m_data )
			return -1;

		char_type *p = m_data + m_length;

		while( --p >= m_data )	{
			if( *p == ch )	{
				return p-m_data;
			}
		}
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	
	static void toUnixPathFormat( const orsChar *str0 )
	{
		// 网络路径不能替换
		if( *str0 == '\\' && str0[1] == '\\' )
			return ;

		orsChar *str = (orsChar *)str0;
		
		while( *str )	{
			if( '\\' == *str )
				*str = '/';
			str++;
		}
	}

	static const char_type *reverseFind( const char_type *str, char_type ch )
	{
		if( NULL == str )
			return NULL;

		const char_type *p = str + orsStrLen( str );
		
		while( --p >= str )	{
			if( *p == ch )	{
				return p;
			}
		}
		return NULL;
	}

	static const char_type *getSuffix( const char_type *str )
	{
		const char_type *suf = reverseFind( str, '.');
		if( suf  )
			return suf + 1;
		return 
			NULL;
	}

	// no case find
	static const char_type *findSubStr_i( const char_type *str, const char_type *subStr )
	{
		int i, n1, n2;
		char_type *upStr, *upSub;
		
		if( NULL == str || subStr == NULL )
			return NULL;
		
		n1 = orsStrLen( str );	n2 = orsStrLen( subStr );
		
		if( n1 < n2 )
			return NULL;
		
		upStr = new char_type[ n1 + 1 ];
		upSub = new char_type[ n2 + 1 ];
		
		for( i =0; i<n1; i++ )
			*upStr++ = toupper( *str++ );
		*upStr = 0;	upStr -= n1;	str -= n1;
		
		for( i =0; i<n2; i++ )
			*upSub++ = toupper( *subStr++ );
		*upSub = 0;	 upSub -= n2; subStr -= n2;
		
		char_type *subS = _tcsstr( upStr, upSub );
		
		if( NULL != subS )
			n1 = (subS - upStr);
		
		delete upStr;
		delete upSub;
		
		if( NULL == subS )
			return NULL;
		
		return str + n1;
	}

	static const char_type *getPureFileName( const char_type *filePath )
	{
		const char_type *pureFileName0, *pureFileName1;

		pureFileName0 = reverseFind( filePath, '/' );
		pureFileName1 = reverseFind( filePath, '\\' );

		if (NULL != pureFileName0 || NULL != pureFileName1)
		{
			if (pureFileName0 > pureFileName1)
				filePath = pureFileName0+1;
			else if (NULL != pureFileName1)
				filePath = pureFileName1+1;
		}

		return filePath;
	};

 	static orsBasicString getDirFromPath( const char_type *filePath )
 	{
		const char_type *pureFileName;
 		
 		pureFileName = getPureFileName( filePath );

		orsBasicString dir = filePath;		
		
		if( NULL != pureFileName  ) {
			int pos = pureFileName - filePath;

			if( pos > 1 ) 
				dir = dir.left( pos-1 );
			else if( 1 == pos )
				dir = dir.left(pos);
			else
				dir = "";
		}
		
 		return dir;
 	};

	static orsBasicString getUpOneLevelDirFromDir( const char_type *CurrentDir )//获得当前目录的上一级目录
	{
		const char_type *UpOneLevelDir;
		UpOneLevelDir = CurrentDir;
		orsBasicString tempStr = CurrentDir;
		orsBasicString CurrentDirStr = CurrentDir;
		int pos = 0;
		while(tempStr==UpOneLevelDir)
		{
			UpOneLevelDir = reverseFind( tempStr, '/' );
			if( UpOneLevelDir )
			{
				pos = UpOneLevelDir - tempStr;
				if(CurrentDirStr.left(pos+1)==CurrentDirStr) //CurrentDir最后包含了"/"
				{	
					tempStr = CurrentDirStr.left(pos);
					UpOneLevelDir = tempStr;
					CurrentDirStr = tempStr;
				}
				else
					return CurrentDirStr.left(pos);
			}
			
		}
		tempStr = CurrentDir;
		UpOneLevelDir = CurrentDir;
		while(tempStr==UpOneLevelDir)
		{
			UpOneLevelDir = reverseFind( tempStr, '\\' );
			if( UpOneLevelDir )
			{
				pos = UpOneLevelDir - tempStr;
				if(CurrentDirStr.left(pos+1)==CurrentDirStr) //CurrentDir最后包含了"/"
				{	
					tempStr = CurrentDirStr.left(pos);
					UpOneLevelDir = tempStr;
					CurrentDirStr = tempStr;
				}
			}
		}

	    return CurrentDirStr.left(pos);
	};
	//////////////////////////////////////////////////////////////////////////

protected:

	orsBasicString& append(const char_type *str)
	{
		if(str == NULL)
			return *this;

		orsBasicString combine_str;
		combine_str = combine_strs(c_str(),str);
		assign(combine_str.c_str());

		return *this;
	}

	orsBasicString& append(char_type c)
	{
		char_type temp_str[2];
		temp_str[0] = c;
		temp_str[1] = '\0';

		return append(temp_str);
	}

	void assign(const char_type* str)
	{
		int count = 0;

		if (NULL != str) 
			count = strlen(str);

		if ( NULL!= m_data ) {
			if( 0 == count || m_capacity < count)
				dealloc_data();
		}

		if (0 == count)
			return;

		if (m_capacity < count) 
			alloc_data(count);

		memcpy( m_data, str, count*sizeof(char_type) );

		m_data[ count ] = '\0';
		m_length = count;
	}

	void alloc_data(size_type n)
	{
		if (NULL == m_mallocFun)
		{
			m_mallocFun = malloc;
			m_freeFun = free;
		}

		if (n < 1) {
			dealloc_data();
		}
		else if( m_capacity < n )
		{
			// ensure 64bits aligned
			size_type string_bytes = int((n + 1 + 7) / 8) * 8*sizeof(char_type);

			m_capacity = string_bytes / sizeof(char_type) - 1;

			m_data = (char_type *)m_mallocFun(2*string_bytes);

			m_data[0] = '\0';
			m_length = 0;
		}
	}

    void dealloc_data()
    {
        if( NULL != m_data &&  NULL != m_freeFun ) 
		{
			m_freeFun(m_data);
		}
		init();
    }
};  //end class


template <typename char_type> inline orsBasicString<char_type>
operator+(orsBasicString<char_type> &lhs,orsBasicString<char_type> & rhs )
{
	return orsBasicString<char_type>::combine_strs(lhs.c_str(),rhs.c_str());
}

template <typename char_type> inline orsBasicString<char_type>
operator+( const char_type *lhs, const orsBasicString<char_type> &rhs )
{
	return orsBasicString<char_type>::combine_strs(lhs,rhs.c_str());
}

template <typename char_type> inline orsBasicString<char_type>
operator+( const orsBasicString<char_type> & lhs, const char_type* rhs )
{
	return orsBasicString<char_type>::combine_strs(lhs.c_str(),rhs);
}

template <typename char_type> inline orsBasicString<char_type>
operator+(const orsBasicString<char_type> & lhs, const orsBasicString<char_type> &rhs)
{
	return orsBasicString<char_type>::combine_strs(lhs.c_str(), rhs);
}


template <typename char_type>
inline bool compare_str_is_equ(const char_type* str1, const char_type* str2)
{
	if(str1 == NULL || str2 == NULL)
	{
		if(str1 == str2)
			return true;
		else
			return false;
	}
	return !_tcscmp(str1,str2)?true:false;
}

template <typename char_type>
inline bool operator==(const orsBasicString<char_type> &str1,const orsBasicString<char_type> &str2 )
{
	return compare_str_is_equ(str1.c_str(),str2.c_str());
}

template <typename char_type>
inline bool operator==(const orsBasicString<char_type> &str1,const char_type* str2 )
{
	return compare_str_is_equ(str1.c_str(),str2);
}

template <typename char_type>
inline bool operator==(const char_type* str1,const orsBasicString<char_type> & str2 )
{
	return compare_str_is_equ(str1,str2.c_str());
}

template <typename char_type>
inline bool operator!=(const orsBasicString<char_type> &str1,const orsBasicString<char_type> &str2 )
{
	return !compare_str_is_equ(str1.c_str(),str2.c_str());
}

template <typename char_type>
inline bool operator!=(const orsBasicString<char_type> &str1,const char_type* str2 )
{
	return !compare_str_is_equ(str1.c_str(),str2);
}

template <typename char_type>
inline bool operator!=(const char_type* str1,const orsBasicString<char_type> & str2 )
{
	return !compare_str_is_equ(str1,str2.c_str());
}

// 请注意:
//
//	因为依赖getPlaform(), orsString, ors_string 不能用作全局变量或static变量
//

typedef orsBasicString<orsChar>	orsString;

typedef orsString	ors_string;

typedef orsString orsFileName;


#endif

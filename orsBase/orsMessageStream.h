#ifndef OPENRS_MESSAGE_STREAM_H
#define OPENRS_MESSAGE_STREAM_H

//////////////////////////////////内部类，用于序列化///////////////////////////

enum unitType {
	NT_CHAR = 1,
	NT_BOOL,
	NT_SHORT,
	NT_LONG,
	NT_DOUBLE,
	NT_STRING,
	NT_BLOB,
};

#define DEFAULT_BUF_SIZE	512
class orsMessageStream {
public :
	orsMessageStream(long bufLen):m_buffLen(bufLen){
		m_stream = (char *)malloc (m_buffLen) ;
		m_cursor = m_stream ;
		m_rIndex = m_stream ;
	}
	orsMessageStream( const char * ptrIn, long len){
		m_stream = (char *)malloc(len) ;
		memcpy(m_stream, ptrIn, len) ;
		m_cursor = m_stream + len ;
		m_rIndex = m_stream ;
		m_buffLen = len ;
	}
	orsMessageStream(void):m_buffLen(DEFAULT_BUF_SIZE){
		m_stream = (char *)malloc(DEFAULT_BUF_SIZE) ;
		m_cursor = m_stream ;
		m_rIndex = m_stream ;
	}
	~orsMessageStream(void){
		if(m_stream != NULL)
			free(m_stream) ;
	}

	inline orsMessageStream& operator << (char inData){
		addUnit((char *)&inData,NT_BOOL) ;
		return *this;
	}
	inline orsMessageStream& operator << (bool inData){
		addUnit((char *)&inData,NT_CHAR) ;
		return *this;
	}
	inline orsMessageStream& operator << (short inData){
		addUnit((char *)&inData,NT_SHORT) ;
		return *this;
	}
	inline orsMessageStream& operator << (long inData){
		addUnit((char *)&inData,NT_LONG) ;
		return *this;
	}
	inline orsMessageStream& operator << (double inData){
		addUnit((char *)&inData,NT_DOUBLE) ;
		return *this;
	}

	inline orsMessageStream& operator << (orsString& inData){
		long str_len = inData.length() ;
		addUnit((char *)&str_len, NT_LONG) ;
		addUnit((char *)inData.c_str(),NT_STRING) ;
		return *this;
	}

	inline orsMessageStream& operator >> (char &outData){
		if (m_rIndex < m_cursor) {
			outData = *(char *)m_rIndex ;
			m_rIndex += sizeof(char) ;
		}
		return *this ;
	}
	inline orsMessageStream& operator >> (bool &outData){
		if (m_rIndex < m_cursor) {
			outData = *(bool *)m_rIndex ;
			m_rIndex += sizeof(bool) ;
		}
		return *this ;
	}
	inline orsMessageStream& operator >> (short &outData){
		if (m_rIndex < m_cursor) {
			memcpy((void*)&outData,(void*)m_rIndex,sizeof(short)) ;
			m_rIndex += sizeof(short) ;
		}
		return *this ;
	}
	inline orsMessageStream& operator >> (long &outData){
		if (m_rIndex < m_cursor) {
			memcpy((void*)&outData,(void*)m_rIndex,sizeof(long));
			//outData = *(long *)m_rIndex ;
			m_rIndex += sizeof(long) ;
		}
		return *this ;
	}
	inline orsMessageStream& operator >> (double &outData){
		if (m_rIndex < m_cursor) {
			memcpy((void*)&outData,(void*)m_rIndex,sizeof(double));
			m_rIndex += sizeof(double) ;
		}
		return *this ;
	}
	inline orsMessageStream& operator >> (std::string &outData){
		long str_len ;
		char * str_temp ;
		if (m_rIndex < m_cursor) {
			memcpy((void*)&str_len,(void*)m_rIndex,sizeof(long)) ;
			m_rIndex += sizeof(long) ;
			str_temp = new char [str_len + 1] ;
			memcpy(str_temp, m_rIndex, str_len) ;
			m_rIndex += str_len ;
			str_temp[str_len] = '\0' ;
			outData = str_temp ;
			delete [] str_temp ;
		}
		return *this ;
	}

	inline void writeBlob(char * ptrInBlob, long len){
		long nUnitLen = len ;
		while (m_cursor + nUnitLen > m_stream + m_buffLen) appendStream() ;
		memcpy(m_cursor, ptrInBlob, nUnitLen);
		m_cursor += nUnitLen ;
	}
	inline void readBlob(char * ptrOutBlob, long len){
		if (m_rIndex < m_cursor) {
			if (NULL != ptrOutBlob) {
				memcpy(ptrOutBlob, m_rIndex, len) ;
				m_rIndex += len ;
			}
		}
	}

	inline long length(){return m_cursor - m_stream ;}
	inline  char * rd_ptr (){return m_stream ;}
	inline  char * wr_ptr (){return m_cursor ;}

private :
	inline char * addUnit(char * ptrInData,unitType inDataType){
		int nUnitLen ;
		switch(inDataType) {
		case NT_BOOL:
		case NT_CHAR:
			nUnitLen = 1;
			break;
		case NT_SHORT:
			nUnitLen = 2 ;
			break ;
		case NT_LONG:
			nUnitLen = 4 ;
			break ;
		case NT_DOUBLE:
			nUnitLen = 8 ;
			break ;
		case NT_STRING:
			nUnitLen = strlen(ptrInData) ;
			break ;
		default:
			break ;
		}

		while (m_cursor + nUnitLen > m_stream + m_buffLen) appendStream() ;

		memcpy (m_cursor, ptrInData, nUnitLen) ;
		m_cursor += nUnitLen ;
		return m_cursor;
	}
	inline bool appendStream(){
		long offset = m_cursor - m_stream ;
		if (NULL == (m_stream = (char *)realloc (m_stream,m_buffLen * 2))) {
			// failure memory allocate
			return false ;
		}
		m_buffLen *= 2;
		m_cursor = m_stream + offset ;
		m_rIndex = m_stream ;
		return true ;
	}
private :
	char * m_stream;
	char * m_cursor;
	char * m_rIndex;
	long m_buffLen ;
};


#endif

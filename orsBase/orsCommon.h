#ifndef OPENRS_COMMON_H
#define OPENRS_COMMON_H

//#include "orsArray.h"
#include <string>

#include "orsBase/orsTypedef.h"
#include "orsBase/orsArray.h"

#include "orsBase/orsString.h"

#if defined(_UNICODE) || defined(UNICODE)
typedef std::wstring		stdString;
#else
typedef std::string			stdString;
#endif

// Encoding and decoding Base64 code
class orsBase64
{
public:

	// Encodes binary data to Base64 code
	// Returns size of encoded data.
	static int encode(const unsigned char* inData,
				   int dataLength,
				   stdString & outCode);

	// Decodes Base64 code to binary data
	// Returns size of decoded data.
	static int decode(const stdString &inCode,
				   int codeLength,
				   unsigned char* outData);

	// Returns maximum size of decoded data based on size of Base64 code.
	static int getDataLength(int codeLength);

	// Returns maximum length of Base64 code based on size of uncoded data.
	static int getCodeLength(int dataLength);

};

bool findAllFiles( const orsChar * dir, const orsChar * extend, orsArray <orsString> &fileList );

stdString  ToStdString( const orsString &str);

#ifdef _WIN32
#define PLUGIN_EXTENSION	"*.dll;*.lnk"
#else
#define PLUGIN_EXTENSION	"*.so"
#endif

typedef void*	orsPluginHandle;
typedef void*	orsSymbolHandle;

orsPluginHandle loadDynamicLib( const orsChar *name);
void			unloadDynamicLib(orsPluginHandle handle);
orsSymbolHandle getDynamicSymbol(orsPluginHandle handle, const orsChar * name);

stdString  getExeDir();

#endif

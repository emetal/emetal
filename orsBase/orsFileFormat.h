#ifndef ORS_FILE_FORMAT_DEF_H
#define ORS_FILE_FORMAT_DEF_H


#include "orsString.h"
#include "orsArray.h"

struct orsFileFormat
{
	orsFileName name;
	orsString ext;
};

typedef orsArray<orsFileFormat> orsFileFormatList;

#endif

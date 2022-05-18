// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef _ORS_HEADER_STDAFX_H
#define _ORS_HEADER_STDAFX_H

#ifdef WIN32

#include "warningdisable.h"

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <winsock2.h>

#endif

#include <assert.h>
#include <stdio.h>

#include "orsBase/orsIObject.h"
#include "orsBase/orsIPlatform.h"
#include "orsImage/orsIImageService.h"
orsIPlatform * _getPlatform();

//#include "vld.h"

#endif

// TODO: reference additional headers your program requires here

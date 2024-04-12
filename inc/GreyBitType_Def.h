/*
** ===========================================================================
** File: GreyBitTypeDec.h
** Description: GreyBit font library - type defines
** Copyright (c) 2023
** Portions of this software (c) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2023	me				Upgrade
** 08/08/2023	me				Init
** ===========================================================================
*/

#ifndef GREYBITTYPE_DEF_H_
#define GREYBITTYPE_DEF_H_
/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../GreyBitType.h"
#include "../GreyBitSystem.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

typedef struct _GB_FormatRec GB_FormatRec, *GB_Format;
typedef struct _GB_DecoderRec GB_DecoderRec, *GB_Decoder;
typedef struct _GB_EncoderRec GB_EncoderRec, *GB_Encoder;

typedef struct _GB_LibraryRec
{
	GB_Memory	gbMem;
	GB_Format	gbFormatHeader;
} GB_LibraryRec, *GB_Library;

typedef struct _GB_LoaderRec
{
	GB_Library	gbLibrary;
	GB_Memory	gbMem;
	GB_Stream	gbStream;
	GB_Decoder	gbDecoder;
} GB_LoaderRec, *GB_Loader;

typedef struct _GB_LayoutRec
{
	GB_Library	gbLibrary;
	GB_Memory	gbMem;
	GB_Stream	gbStream;
	GB_Decoder	gbDecoder;
	GB_Bitmap	gbBitmap;
	GB_Bitmap	gbBitmap8;
#ifdef ENABLE_GREYVECTORFILE
	void*		gbRaster;
#endif //ENABLE_GREYVECTORFILE
	GB_UINT32	dwCode;
	GB_BYTE*	gbSwitchBuf;
	GB_INT32	nSwitchBufLen;
	GB_INT16	nWidth;
	GB_INT16	nHeight;
	GB_INT16	nSize;
	GB_INT16	nBitCount;
	GB_INT16	bItalic;
	GB_INT16	bBold;
} GB_LayoutRec, *GB_Layout;

typedef struct _GB_CreatorRec
{
	GB_LibraryRec*	gbLibrary;
	GB_MemoryRec*	gbMem;
	GB_StreamRec*	gbStream;
	GB_EncoderRec*	gbEncoder;
} GB_CreatorRec, *GB_Creator;

typedef struct tagSECTIONOINFO
{
	GB_UINT16	gbSectionOff[146];
} SECTIONOINFO;

typedef GB_Decoder(*GB_DECODER_NEW)(GB_Loader loader, GB_Stream stream);
#ifdef ENABLE_ENCODER
typedef GB_Encoder(*GB_ENCODER_NEW)(GB_Creator loader, GB_Stream stream);
#endif
typedef GB_BOOL(*GB_PROBE)(GB_Stream stream);

struct _GB_FormatRec
{
	GB_Format		next;
	GB_CHAR			tag[4];
	GB_PROBE		probe;
	GB_DECODER_NEW	decodernew;
#ifdef ENABLE_ENCODER
	GB_ENCODER_NEW	encodernew;
#endif //ENABLE_ENCODER
};

typedef GB_INT32(*GB_DECODER_SETCACHE)(GB_Decoder decoder,
									   GB_INT32 nCacheItem);
typedef GB_INT32(*GB_DECODER_SETPARAM)(GB_Decoder decoder,
									   GB_Param nParam,
									   GB_UINT32 dwParam);
typedef GB_INT32(*GB_DECODER_GETCOUNT)(GB_Decoder decoder);
typedef GB_INT32(*GB_DECODER_GETHEIGHT)(GB_Decoder decoder);
typedef GB_INT32(*GB_DECODER_GETWIDTH)(GB_Decoder decoder,
									   GB_UINT32 nCode, GB_INT16 nSize);
typedef GB_INT16(*GB_DECODER_GETADVANCE)(GB_Decoder decoder,
										 GB_UINT32 nCode, GB_INT16 nSize);
typedef GB_INT32(*GB_DECODER_DECODE)(GB_Decoder decoder, GB_UINT32 nCode,
									 GB_Data pData, GB_INT16 nSize);
typedef void(*GB_DECODER_DONE)(GB_Decoder decoder);

struct _GB_DecoderRec
{
	GB_DECODER_SETPARAM		setparam;
	GB_DECODER_GETCOUNT		getcount;
	GB_DECODER_GETHEIGHT	getheight;
	GB_DECODER_GETWIDTH		getwidth;
	GB_DECODER_GETADVANCE	getadvance;
	GB_DECODER_DECODE		decode;
	GB_DECODER_DONE			done;
};

#ifdef ENABLE_ENCODER
typedef GB_INT32(*GB_ENCODER_GETCOUNT)(GB_Encoder encoder);
typedef GB_INT32(*GB_ENCODER_SETPARAM)(GB_Encoder encoder,
									   GB_Param nParam, GB_UINT32 dwParam);
typedef GB_INT32(*GB_ENCODER_DELETE)(GB_Encoder encoder, GB_UINT32 nCode);
typedef GB_INT32(*GB_ENCODER_ENCODE)(GB_Encoder encoder, GB_UINT32 nCode,
									 GB_Data pData);
typedef GB_INT32(*GB_ENCODER_FLUSH)(GB_Encoder encoder);
typedef void(*GB_ENCODER_DONE)(GB_Encoder encoder);

struct _GB_EncoderRec
{
	GB_ENCODER_GETCOUNT	getcount;
	GB_ENCODER_SETPARAM	setparam;
	GB_ENCODER_DELETE	remove;
	GB_ENCODER_ENCODE	encode;
	GB_ENCODER_FLUSH	flush;
	GB_ENCODER_DONE		done;
};
#endif //ENABLE_ENCODER

/*
**---------------------------------------------------------------------------
**  Variable Declarations
**---------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Function(external use only) Declarations
**---------------------------------------------------------------------------
*/

#ifdef __cplusplus
}
#endif

#endif //GREYBITTYPE_DEF_H_
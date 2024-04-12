/*
** ===========================================================================
** File: GreyBitFile.h
** Description: GreyBit font library - Bitmap font file handling
** Copyright (c) 2023
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2023	me				Upgrade
** 08/10/2023	me              Init
** ===========================================================================
*/

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#ifndef GREYBITFILE_H_
#define GREYBITFILE_H_
#include "../GreyBitType.h"
#include "../inc/UnicodeSection.h"
#include "../inc/GreyBitType_Def.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_GREYBITFILE
/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

#define MAX_COUNT					0x10000
#define LEN_MASK					0x80
#define MAX_LEN()					(LEN_MASK-1)
#define GET_LEN(d)					(((d)&(~LEN_MASK))+1)
#define SET_LEN(d)					((d)|LEN_MASK)
#define IS_LEN(d)					((d)&LEN_MASK)
#define RAM_MASK					0x80000000
#define IS_INRAM(d)					((d)&RAM_MASK)
#define SET_RAM(d)					((d)|RAM_MASK)
#define GET_INDEX(d)				((d)&(~RAM_MASK))

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

#pragma pack(1)
typedef struct tagGREYBITFILEHEADER
{
	GB_CHAR	gbfTag[4];
} GREYBITFILEHEADER;
#pragma pack()

#pragma pack(1)
typedef struct tagGREYBITINFOHEADER
{
	GB_UINT32		gbiSize;
	GB_UINT32		gbiCount;
	GB_INT16		gbiBitCount;
	GB_INT16		gbiCompression;
	GB_INT16		gbiWidth;
	GB_INT16		gbiHeight;
	GB_UINT32		gbiWidthTabOff;
	GB_UINT32		gbiHoriOffTabOff;
	GB_UINT32		gbiOffsetTabOff;
	GB_UINT32		gbiOffGreyBits;
	SECTIONOINFO	gbiWidthSection;
	SECTIONOINFO	gbiIndexSection;
} GREYBITINFOHEADER;
#pragma pack()

typedef struct GBF_DecoderRec
{
	GB_DecoderRec		gbDecoder;
	GB_Library			gbLibrary;
	GB_Memory			gbMem;
	GB_Stream			gbStream;
	GB_Bitmap			gbBitmap;
	GB_INT32			nCacheItem;
	GB_INT32			nItemCount;
	GB_BYTE*			pBuff;
	GB_INT32			nBuffSize;
	GB_UINT32			gbOffDataBits;
	GREYBITFILEHEADER	gbFileHeader;
	GREYBITINFOHEADER	gbInfoHeader;
	GB_BYTE*			gbWidthTable;
	GB_INT8*			gbHoriOffTable;
	GB_UINT32*			gbOffsetTable;
	GB_BYTE**			gpGreyBits;
	GB_INT32			nGreyBitsCount;
	GB_INT16*			pnGreySize;
} GBF_DecoderRec, *GBF_Decoder;

#ifdef ENABLE_ENCODER
typedef struct GBF_EncoderRec
{
	GB_EncoderRec		gbEncoder;
	GB_Library			gbLibrary;
	GB_Memory			gbMem;
	GB_Stream			gbStream;
	GB_UINT16			nHeight;
	GB_INT16			nBitCount;
	GB_BOOL				bCompress;
	GB_BOOL				gbInited;
	GB_INT32			nCacheItem;
	GB_INT32			nItemCount;
	GB_UINT32			gbOffDataBits;
	GREYBITFILEHEADER	gbFileHeader;
	GREYBITINFOHEADER	gbInfoHeader;
	GB_BYTE*			gbWidthTable;
	GB_INT8*			gbHoriOffTable;
	GB_UINT32*			gbOffsetTable;
	GB_BYTE**			gpGreyBits;
	GB_UINT16*			pnGreySize;
} GBF_EncoderRec, *GBF_Encoder;
#endif //ENABLE_ENCODER

/*
**----------------------------------------------------------------------------
**  Variable Declarations
**----------------------------------------------------------------------------
*/

/*
**----------------------------------------------------------------------------
**  Function(external use only) Declarations
**----------------------------------------------------------------------------
*/

GB_Format	GreyBitFile_Format_New(GB_Library library);
GB_BOOL		GreyBitFile_Probe(GB_Stream stream);

GB_Decoder	GreyBitFile_Decoder_New(GB_Loader loader, GB_Stream stream);
GB_INT32	GreyBitFile_Decoder_SetParam(GB_Decoder decoder, GB_Param nParam,
										 GB_UINT32 dwParam);
GB_INT32	GreyBitFile_Decoder_GetCount(GB_Decoder decoder);
GB_INT32	GreyBitFile_Decoder_GetWidth(GB_Decoder decoder, GB_UINT32 nCode,
										 GB_INT16 nSize);
GB_INT32	GreyBitFile_Decoder_GetHeight(GB_Decoder decoder);
GB_INT16	GreyBitFile_Decoder_GetAdvance(GB_Decoder decoder,
										   GB_UINT32 nCode, GB_INT16 nSize);
GB_INT32	GreyBitFile_Decoder_Decode(GB_Decoder decoder, GB_UINT32 nCode,
									   GB_Data pData, GB_INT16 nSize);
void		GreyBitFile_Decoder_Done(GB_Decoder decoder);

#ifdef ENABLE_ENCODER
GB_Encoder	GreyBitFile_Encoder_New(GB_Creator creator, GB_Stream stream);
GB_INT32	GreyBitFile_Encoder_SetParam(GB_Encoder encoder, GB_Param nParam,
										 GB_UINT32 dwParam);
GB_INT32	GreyBitFile_Encoder_GetCount(GB_Encoder encoder);
GB_INT32	GreyBitFile_Encoder_Encode(GB_Encoder encoder, GB_UINT32 nCode,
									   GB_Data pData);
void		GreyBitFile_Encoder_Done(GB_Encoder encoder);
#endif //ENABLE_ENCODER
#endif //ENABLE_GREYBITFILE

#ifdef __cplusplus
}
#endif 

#endif //GREYBITFILE_H_
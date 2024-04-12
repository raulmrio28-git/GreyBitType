/*
** ===========================================================================
** File: GreyCombineFile.h
** Description: GreyBit font library - Combine font file handling
** Copyright (c) 2023
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2023	me				Upgrade
** 08/07/2023	me              Init
** ===========================================================================
*/

#pragma once
#ifndef GREYVECTORFILE_H_
#define GREYVECTORFILE_H_
/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../GreyBitType.h"
#include "../inc/GreyBitType_Def.h"
#include "../inc/GreyBitCodec.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_GREYVECTORFILE
/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

#define TAG_MASK				0x1
#define MAX_COUNT				0x10000
#define LEN_MASK				0x80
#define MAX_LEN()				(LEN_MASK-1)
#define GET_LEN(d)				(((d)&(~LEN_MASK))+1)
#define SET_LEN(d)				((d)|LEN_MASK)
#define IS_LEN(d)				((d)&LEN_MASK)
#define RAM_MASK				0x80000000
#define IS_INRAM(d)				((d)&RAM_MASK)
#define SET_RAM(d)				((d)|RAM_MASK)
#define GET_INDEX(d)			((d)&(~RAM_MASK))

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

typedef struct  _GVF_PointRec
{
	GB_BYTE x;
	GB_BYTE y;
} GVF_PointRec, *GVF_Point;

typedef struct  _GVF_OutlineRec
{
	GB_BYTE        n_contours;      /* number of contours in glyph        */
	GB_BYTE        n_points;        /* number of points in the glyph      */
	GVF_Point      points;          /* the outline's points               */
	GB_BYTE       *contours;        /* the contour end points             */
} GVF_OutlineRec, *GVF_Outline;

#pragma pack(1)
typedef struct tagGREYVECTORFILEHEADER
{
	GB_CHAR	gbfTag[4];
} GREYVECTORFILEHEADER;
#pragma pack()

#pragma pack(1)
typedef struct tagGREYVECTORINFOHEADER
{
	GB_UINT32		gbiSize;
	GB_UINT32		gbiCount;
	GB_INT16		gbiMaxPoints;
	GB_INT16		gbiMaxContours;
	GB_INT16		gbiWidth;
	GB_INT16		gbiHeight;
	GB_UINT32		gbiWidthTabOff;
	GB_UINT32		gbiHoriOffTabOff;
	GB_UINT32		gbiOffsetTabOff;
	GB_UINT32		gbiOffGreyBits;
	SECTIONOINFO	gbiWidthSection;
	SECTIONOINFO	gbiIndexSection;
} GREYVECTORINFOHEADER;
#pragma pack()

typedef struct _GVF_DecoderRec
{
	GB_DecoderRec			gbDecoder;
	GB_Library				gbLibrary;
	GB_Memory				gbMem;
	GB_Stream				gbStream;
	GB_Outline				gbOutline;
	GB_INT32				nCacheItem;
	GB_INT32				nItemCount;
	GB_BYTE*				pBuff;
	GB_INT32				nBuffSize;
	GB_UINT32				gbOffDataBits;
	GREYVECTORFILEHEADER	gbFileHeader;
	GREYVECTORINFOHEADER	gbInfoHeader;
	GB_BYTE*				gbWidthTable;
	GB_INT8*				gbHoriOffTable;
	GB_UINT32*				gbOffsetTable;
	GB_Outline*				gpGreyBits;
	GB_INT32				nGreyBitsCount;
} GVF_DecoderRec, *GVF_Decoder;

#ifdef ENABLE_ENCODER
typedef struct _GVF_EncoderRec
{
	GB_EncoderRec			gbEncoder;
	GB_Library				gbLibrary;
	GB_Memory				gbMem;
	GB_Stream				gbStream;
	GB_UINT16				nHeight;
	GB_BOOL					gbInited;
	GB_INT32				nCacheItem;
	GB_INT32				nItemCount;
	GB_UINT32				gbOffDataBits;
	GREYVECTORFILEHEADER	gbFileHeader;
	GREYVECTORINFOHEADER	gbInfoHeader;
	GB_BYTE*				gbWidthTable;
	GB_INT8*				gbHoriOffTable;
	GB_UINT32*				gbOffsetTable;
	GB_Outline*				gpGreyBits;
	GB_UINT16*				pnGreySize;
} GVF_EncoderRec, *GVF_Encoder;
#endif

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

extern GB_Format	GreyVectorFile_Format_New(GB_Library library);
extern GB_BOOL		GreyVectorFile_Probe(GB_Stream stream);

extern GVF_Outline	GreyVector_Outline_New(GB_Library library,
										   GB_INT16 n_contours,
										   GB_INT16 n_points);
extern GVF_Outline	GreyVector_Outline_Clone(GB_Library library,
											 GVF_Outline source);
extern GB_INT32		GreyVector_Outline_GetSizeEx(GB_BYTE n_contours,
												 GB_BYTE n_points);
extern GVF_Outline	GreyVector_Outline_GetData(GVF_Outline outline);
extern GVF_Outline	GreyVector_Outline_FromData(GB_BYTE* pData);
extern void			GreyVector_Outline_Done(GB_Library library,
											GVF_Outline outline);
extern GVF_Outline	GreyVector_Outline_NewByGB(GB_Library library,
											   GB_Outline source);
extern GB_Outline	GreyBitType_Outline_NewByGVF(GB_Library library,
												 GVF_Outline source);
extern GB_Outline	GreyBitType_Outline_UpdateByGVF(GB_Outline outline,
													GVF_Outline source);

extern GB_Decoder	GreyVectorFile_Decoder_New(GB_Loader loader,
											   GB_Stream stream);
extern GB_INT32		GreyVectorFile_Decoder_SetParam(GB_Decoder decoder,
													GB_Param nParam,
													GB_UINT32 dwParam);
extern GB_INT32		GreyVectorFile_Decoder_GetCount(GB_Decoder decoder);
extern GB_INT32		GreyVectorFile_Decoder_GetWidth(GB_Decoder decoder,
													GB_UINT32 nCode,
													GB_INT16 nSize);
extern GB_INT32		GreyVectorFile_Decoder_GetHeight(GB_Decoder decoder);
extern GB_INT16		GreyVectorFile_Decoder_GetAdvance(GB_Decoder decoder,
													  GB_UINT32 nCode,
													  GB_INT16 nSize);
extern GB_INT32		GreyVectorFile_Decoder_Decode(GB_Decoder decoder,
												  GB_UINT32 nCode,
												  GB_Data pData,
												  GB_INT16 nSize);
extern void			GreyVectorFile_Decoder_Done(GB_Decoder decoder);

#ifdef ENABLE_ENCODER
extern GB_Encoder	GreyVectorFile_Encoder_New(GB_Creator creator,
											   GB_Stream stream);
extern GB_INT32		GreyVectorFile_Encoder_SetParam(GB_Encoder encoder,
													GB_Param nParam,
													GB_UINT32 dwParam);
extern GB_INT32		GreyVectorFile_Encoder_GetCount(GB_Encoder encoder);
extern GB_INT32		GreyVectorFile_Encoder_Encode(GB_Encoder encoder,
												  GB_UINT32 nCode,
												  GB_Data pData);
extern void			GreyVectorFile_Encoder_Done(GB_Encoder encoder);
#endif //ENABLE_ENCODER

#endif //ENABLE_GREYVECTORFILE

#ifdef __cplusplus
}
#endif

#endif //GREYVECTORFILE_H_
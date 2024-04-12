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
** 08/11/2023	me              Init
** ===========================================================================
*/

#ifndef GREYCOMBINEFILE_H_
#define GREYCOMBINEFILE_H_
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

#ifdef ENABLE_GREYCOMBINEFILE
/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

#define GCF_ITEM_MAX		5
#define GCF_BUF_SIZE		1024

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

#pragma pack(1)
typedef struct tagGREYCOMBINEITEMINFO
{
	GB_UINT32	gbiHeight;
	GB_UINT32	gbiDataOff;
	GB_UINT32	gbiDataSize;
} GREYCOMBINEITEMINFO;
#pragma pack()

#pragma pack(1)
typedef struct tagGREYCOMBINEFILEHEADER
{
	GB_CHAR				gbfTag[4];
	GREYCOMBINEITEMINFO	gbfInfo[GCF_ITEM_MAX];
} GREYCOMBINEFILEHEADER;
#pragma pack()

typedef struct _GCF_DecoderRec
{
	GB_DecoderRec			gbDecoder;
	GB_Library				gbLibrary;
	GB_Memory				gbMem;
	GB_Stream				gbStream;
	GREYCOMBINEFILEHEADER	gbFileHeader;
	GB_Loader				gbLoader[GCF_ITEM_MAX];
} GCF_DecoderRec, *GCF_Decoder;

#ifdef ENABLE_ENCODER
typedef struct _GCF_EncoderRec
{
	GB_EncoderRec			gbEncoder;
	GB_Library				gbLibrary;
	GB_Memory				gbMem;
	GB_Stream				gbStream;
	GREYCOMBINEFILEHEADER	gbFileHeader;
	GB_Stream				gbCreator[GCF_ITEM_MAX];
} GCF_EncoderRec, *GCF_Encoder;
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

extern GB_Format	GreyCombineFile_Format_New(GB_Library library);
extern GB_BOOL		GreyCombineFile_Probe(GB_Stream stream);

extern GB_Decoder	GreyCombineFile_Decoder_New(GB_Loader loader,
												GB_Stream stream);
extern GB_INT32		GreyCombineFile_Decoder_SetParam(GB_Decoder decoder,
													 GB_Param nParam,
													 GB_UINT32 dwParam);
extern GB_INT32		GreyCombineFile_Decoder_GetCount(GB_Decoder decoder);
extern GB_INT32		GreyCombineFile_Decoder_GetWidth(GB_Decoder decoder,
													 GB_UINT32 nCode,
													 GB_INT16 nSize);
extern GB_INT32		GreyCombineFile_Decoder_GetHeight(GB_Decoder decoder);
extern GB_INT16		GreyCombineFile_Decoder_GetAdvance(GB_Decoder decoder,
													   GB_UINT32 nCode,
													   GB_INT16 nSize);
extern GB_INT32		GreyCombineFile_Decoder_Decode(GB_Decoder decoder,
												   GB_UINT32 nCode,
												   GB_Data pData,
												   GB_INT16 nSize);
extern void			GreyCombineFile_Decoder_Done(GB_Decoder decoder);
	
#ifdef ENABLE_ENCODER
extern GB_Encoder	GreyCombineFile_Encoder_New(GB_Creator creator,
												GB_Stream stream);
extern GB_INT32		GreyCombineFile_Encoder_SetParam(GB_Encoder encoder,
													 GB_Param nParam,
													 GB_UINT32 dwParam);
extern GB_INT32		GreyCombineFile_Encoder_GetCount(GB_Encoder encoder);
extern GB_INT32		GreyCombineFile_Encoder_Encode(GB_Encoder encoder,
												   GB_UINT32 nCode,
												   GB_Data pData);
extern void			GreyCombineFile_Encoder_Done(GB_Encoder encoder);
#endif //ENABLE_ENCODER
#endif //ENABLE_GREYCOMBINEFILE

#ifdef __cplusplus
}
#endif 

#endif //GREYCOMBINEFILE_H_
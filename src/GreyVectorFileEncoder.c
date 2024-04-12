/*
** ===========================================================================
** File: GreyVectorFileEncoder.c
** Description: GreyBit font library - Vector font file encode
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

#include "../inc/UnicodeSection.h"
#include "../GreyBitSystem.h"
#include "../inc/GreyVectorFile.h"

#ifdef ENABLE_GREYVECTORFILE
#ifdef ENABLE_ENCODER
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

/*
**----------------------------------------------------------------------------
**  Global variables
**----------------------------------------------------------------------------
*/

/*
**----------------------------------------------------------------------------
**  Internal variables
**----------------------------------------------------------------------------
*/

/*
**----------------------------------------------------------------------------
**  Function(internal use only) Declarations
**----------------------------------------------------------------------------
*/

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_Init
** Description: Init encoder
** Input: encoder - encoder
** Output: Init'ed encoder
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_Init(GVF_Encoder encoder)
{
	encoder->gbWidthTable = (GB_BYTE *)GreyBit_Malloc(encoder->gbMem,
													  MAX_COUNT);
	encoder->gbHoriOffTable = (GB_INT8 *)GreyBit_Malloc(encoder->gbMem,
														MAX_COUNT);
	encoder->gbOffsetTable = (GB_UINT32 *)GreyBit_Malloc(encoder->gbMem,
														 sizeof(GB_UINT32)
														*MAX_COUNT);
	encoder->gpGreyBits = (GB_Outline *)GreyBit_Malloc(encoder->gbMem,
													   sizeof(GB_Outline)
													   *MAX_COUNT);
	encoder->pnGreySize = (GB_UINT16 *)GreyBit_Malloc(encoder->gbMem,
													  sizeof(GB_UINT16)
													 *MAX_COUNT);
	encoder->nCacheItem = MAX_COUNT;
	GB_MEMSET(encoder->gbWidthTable, 0, MAX_COUNT);
	GB_MEMSET(encoder->gbHoriOffTable, 0, MAX_COUNT);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_InfoInit
** Description: Init info
** Input: encoder - encoder
**        nHeight - height
**        nBitCount - bit count
**		  bCompress - compress?
** Output: Initialized info
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_InfoInit(GVF_Encoder encoder,
											GB_INT16 nHeight)
{
	int i;

	if (encoder->gbInited)
	{
		if (encoder->gbInfoHeader.gbiHeight == nHeight)
			return GB_SUCCESS;
		GB_MEMSET(encoder->gbWidthTable, 0, MAX_COUNT);
		GB_MEMSET(encoder->gbHoriOffTable, 0, MAX_COUNT);
		GB_MEMSET(encoder->gbOffsetTable, 0, sizeof(GB_UINT32) * MAX_COUNT);
		for (i = 0; i < encoder->nCacheItem; ++i)
		{
			if (encoder->gpGreyBits[i])
			{
				GreyBitType_Outline_Done(encoder->gbLibrary,
										 encoder->gpGreyBits[i]);
				encoder->gpGreyBits[i] = 0;
				encoder->pnGreySize[i] = 0;
			}
		}
	}
	encoder->gbInited = 1;
	encoder->gbInfoHeader.gbiHeight = nHeight;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_ClearCache
** Description: Clear cache
** Input: encoder - encoder
** Output: Nuked cache
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyVectorFile_Encoder_ClearCache(GVF_Encoder encoder)
{
	int i;

	if (encoder->gbWidthTable)
		GreyBit_Free(encoder->gbMem, encoder->gbWidthTable);
	if (encoder->gbHoriOffTable)
		GreyBit_Free(encoder->gbMem, encoder->gbHoriOffTable);
	if (encoder->gbOffsetTable)
		GreyBit_Free(encoder->gbMem, encoder->gbOffsetTable);
	if (encoder->pnGreySize)
		GreyBit_Free(encoder->gbMem, encoder->pnGreySize);
	if (encoder->gpGreyBits)
	{
		for (i = 0; i < encoder->nCacheItem; ++i)
		{
			if (encoder->gpGreyBits[i])
				GreyBit_Free(encoder->gbMem, encoder->gpGreyBits[i]);
		}
		GreyBit_Free(encoder->gbMem, encoder->gpGreyBits);
	}
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_BuildAll
** Description: Build everything pre-write
** Input: encoder - encoder
** Output: Initialised header
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_BuildAll(GVF_Encoder encoder)
{
	GB_INT32	nSize;
	GB_UINT32	nCount;
	GB_UINT32	nGreyBitSize;
	GB_UINT32	nOffSetTableSize;
	GB_UINT32	nHoriOffTableSize;
	GB_UINT32	nWidthTableSize;
	GB_UINT16	nCode;
	GB_UINT16	nCodea;
	GB_UINT16	nMinCode;
	GB_UINT16	nMaxCode;
	GB_UINT16	nSectionLen;
	GB_UINT16	nSection;

	nWidthTableSize = 0;
	nOffSetTableSize = 0;
	nHoriOffTableSize = 0;
	nGreyBitSize = 0;
	nCount = 0;
	nCodea = 0;
	for (nSection = 0; nSection < UNICODE_SECTION_NUM; ++nSection)
	{
		UnicodeSection_GetSectionInfo(nSection, &nMinCode, &nMaxCode);
		nSectionLen = nMaxCode - nMinCode + 1;
		for (nCode = nMinCode; nCode <= nMaxCode; nCode++)
		{
			if (encoder->gbWidthTable[nCode])
			{
				encoder->gbInfoHeader.gbiWidthSection.gbSectionOff[nSection]
									= (GB_UINT16)nWidthTableSize + 1;
				encoder->gbInfoHeader.gbiIndexSection.gbSectionOff[nSection]
									= (GB_INT16)(nOffSetTableSize >> 2) + 1;
				nWidthTableSize += nSectionLen;
				nHoriOffTableSize += nSectionLen;
				nOffSetTableSize += sizeof(GB_UINT32) * nSectionLen;
				break;
			}
		}
	}
	while (nCodea < MAX_COUNT)
	{
		nSize = encoder->pnGreySize[nCodea];
		if (nSize)
		{
			encoder->gbOffsetTable[nCodea] = nGreyBitSize;
			nGreyBitSize += nSize + 2;
			++nCount;
		}
		nCodea++;
	}
	encoder->gbInfoHeader.gbiCount = nCount;
	encoder->gbInfoHeader.gbiOffGreyBits = nOffSetTableSize+nHoriOffTableSize
									+ nWidthTableSize;
	encoder->gbInfoHeader.gbiOffsetTabOff = nHoriOffTableSize+nWidthTableSize;
	encoder->gbInfoHeader.gbiHoriOffTabOff = nWidthTableSize;
	encoder->gbInfoHeader.gbiWidthTabOff = 0;
	encoder->gbInfoHeader.gbiSize = sizeof(GREYVECTORINFOHEADER);
	encoder->gbFileHeader.gbfTag[0] = 'g';
	encoder->gbFileHeader.gbfTag[1] = 'v';
	encoder->gbFileHeader.gbfTag[2] = 't';
	encoder->gbFileHeader.gbfTag[3] = 'f';
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_WriteAll
** Description: Flush everything to stream
** Input: encoder - encoder
** Output: Finished product
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_WriteAll(GVF_Encoder encoder)
{
	GVF_Outline	outline;
	GB_INT32	nDataSize;
	GB_BYTE *	pData;
	GB_INT32	nCode;
	GB_UINT16	nMinCode;
	GB_UINT16	nMaxCode;
	GB_INT32	nSectionLen;
	GB_INT32	nSection;

	GreyBit_Stream_Seek(encoder->gbStream, 0);
	GreyBit_Stream_Write(encoder->gbStream, (GB_BYTE*)&encoder->gbFileHeader,
						 sizeof(GREYVECTORFILEHEADER));
	GreyBit_Stream_Write(encoder->gbStream, (GB_BYTE*)&encoder->gbInfoHeader,
						 sizeof(GREYVECTORINFOHEADER));
	for (nSection = 0; nSection < UNICODE_SECTION_NUM; ++nSection)
	{
		UnicodeSection_GetSectionInfo(nSection, &nMinCode, &nMaxCode);
		nSectionLen = nMaxCode - nMinCode + 1;
		if (encoder->gbInfoHeader.gbiWidthSection.gbSectionOff[nSection])
		{
			pData = (GB_BYTE *)&encoder->gbWidthTable[nMinCode];
			nDataSize = (GB_UINT16)nSectionLen;
			GreyBit_Stream_Write(encoder->gbStream, pData, nSectionLen);
		}
	}
	for (nSection = 0; nSection < UNICODE_SECTION_NUM; ++nSection)
	{
		UnicodeSection_GetSectionInfo(nSection, &nMinCode, &nMaxCode);
		nSectionLen = nMaxCode - nMinCode + 1;
		if (encoder->gbInfoHeader.gbiWidthSection.gbSectionOff[nSection])
		{
			pData = (GB_BYTE *)&encoder->gbHoriOffTable[nMinCode];
			nDataSize = nSectionLen;
			GreyBit_Stream_Write(encoder->gbStream, pData, nDataSize);
		}
	}
	for (nSection = 0; nSection < UNICODE_SECTION_NUM; ++nSection)
	{
		UnicodeSection_GetSectionInfo(nSection, &nMinCode, &nMaxCode);
		nSectionLen = nMaxCode - nMinCode + 1;
		if (encoder->gbInfoHeader.gbiIndexSection.gbSectionOff[nSection])
		{
			pData = (GB_BYTE *)&encoder->gbOffsetTable[nMinCode];
			nDataSize = sizeof(GB_UINT32) * (GB_UINT16)nSectionLen;
			GreyBit_Stream_Write(encoder->gbStream, pData, nDataSize);
		}
	}
	for (nCode = 0; nCode < encoder->nCacheItem; ++nCode)
	{
		nDataSize = encoder->pnGreySize[nCode];
		if (nDataSize)
		{
		   outline=(GVF_Outline)GreyVector_Outline_NewByGB(encoder->gbLibrary,
												  encoder->gpGreyBits[nCode]);
		   GreyBit_Stream_Write(encoder->gbStream, (GB_BYTE*)&nDataSize, 2);
		   pData = (GB_CHAR*)GreyVector_Outline_GetData(outline);
		   GreyBit_Stream_Write(encoder->gbStream, pData, nDataSize);
		   GreyVector_Outline_Done(encoder->gbLibrary, outline);
		}
	}
	return GB_SUCCESS;
}

/*
**----------------------------------------------------------------------------
**  Function(internal/external use) Declarations
**----------------------------------------------------------------------------
*/

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_GetCount
** Description: Get character count
** Input: encoder - encoder
** Output: Character count
** Return value: encoder->nItemCount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_GetCount(GB_Encoder encoder)
{
	GVF_Encoder	me = (GVF_Encoder)encoder;

	return me->nItemCount;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_SetParam
** Description: Set encoder param
** Input: encoder - encoder
**        nParam - param type
**        dwParam - param value
** Output: Set param
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_SetParam(GB_Encoder encoder,
	GB_Param nParam,
	GB_UINT32 dwParam)
{
	GVF_Encoder	me = (GVF_Encoder)encoder;

	if (dwParam)
	{
		if (nParam == GB_PARAM_HEIGHT)
			me->nHeight = (GB_UINT16)dwParam;
	}
	GreyVectorFile_Encoder_InfoInit(me, me->nHeight);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_Delete
** Description: Remove character
** Input: encoder - encoder
**        nCode - unicode code
** Output: Removed character
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_Delete(GB_Encoder encoder, GB_UINT32 nCode)
{
	GVF_Encoder	me = (GVF_Encoder)encoder;

	me->gbOffsetTable[nCode] = 0;
	me->gbHoriOffTable[nCode] = GB_HORIOFF_DEFAULT;
	me->gbWidthTable[nCode] = GB_WIDTH_DEFAULT;
	if (me->gpGreyBits[nCode])
	{
		GreyBitType_Outline_Done(me->gbLibrary, me->gpGreyBits[nCode]);
		me->gpGreyBits[nCode] = 0;
		me->pnGreySize[nCode] = 0;
	}
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_Encode
** Description: Encode code to new font
** Input: encoder - encoder
**        nCode - unicode code
**        pData - data buffer
** Output: Added character
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_Encode(GB_Encoder encoder, GB_UINT32 nCode,
										  GB_Data pData)
{
	GB_INT16	nWidth;
	GB_Outline	outline; 
	GB_Outline	source;
	GVF_Encoder	me = (GVF_Encoder)encoder;

	if (!pData || pData->format != GB_FORMAT_OUTLINE)
		return GB_FAILED;
	if (!me->nHeight || !me->gbInited)
		return GB_FAILED;
	nWidth = pData->width;
	source = (GB_Outline)pData->data;
	if (source->n_points > 255)
		return GB_FAILED;
	if (me->gbInfoHeader.gbiWidth < nWidth)
		me->gbInfoHeader.gbiWidth = nWidth;
	if (me->gbInfoHeader.gbiMaxPoints < source->n_points)
		me->gbInfoHeader.gbiMaxPoints = source->n_points;
	if (me->gbInfoHeader.gbiMaxContours < source->n_contours)
		me->gbInfoHeader.gbiMaxContours = source->n_contours;
	outline = GreyBitType_Outline_Clone(me->gbLibrary, source);
	if (me->gpGreyBits[nCode])
		GreyBitType_Outline_Done(me->gbLibrary, me->gpGreyBits[nCode]);
	me->gpGreyBits[nCode] = outline;
	me->pnGreySize[nCode] = (GB_UINT16)GreyVector_Outline_GetSizeEx(
							(GB_BYTE)outline->n_contours,
							(GB_BYTE)outline->n_points);
	me->gbOffsetTable[nCode] = SET_RAM(nCode);
	me->gbWidthTable[nCode] = (GB_BYTE)nWidth;
	me->gbHoriOffTable[nCode] = (GB_INT8)pData->horioff;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_Flush
** Description: Flush encoder
** Input: encoder - encoder
** Output: Flushed encoder
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Encoder_Flush(GB_Encoder encoder)
{
	GVF_Encoder	me = (GVF_Encoder)encoder;

	GreyVectorFile_Encoder_BuildAll(me);
	GreyVectorFile_Encoder_WriteAll(me);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_Done
** Description: Done with encoder? Nuke it!
** Input: encoder - encoder
** Output: Nuked encoder
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyVectorFile_Encoder_Done(GB_Encoder encoder)
{
	GVF_Encoder	me = (GVF_Encoder)encoder;

	if (me->nHeight)
	{
		GreyVectorFile_Encoder_Flush(&me->gbEncoder);
		me->nHeight = 0;
	}
	GreyVectorFile_Encoder_ClearCache(me);
	GreyBit_Free(me->gbMem, encoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Encoder_New
** Description: Create new encoder
** Input: creator - creator
**        stream - stream
** Output: New encoder
** Return value: codec
** ---------------------------------------------------------------------------
*/

GB_Encoder	GreyVectorFile_Encoder_New(GB_Creator creator, GB_Stream stream)
{
	GVF_Encoder	codec;

	codec = (GVF_Encoder)GreyBit_Malloc(creator->gbMem, sizeof(GVF_Encoder));
	if (codec)
	{
		codec->gbEncoder.getcount = GreyVectorFile_Encoder_GetCount;
		codec->gbEncoder.setparam = GreyVectorFile_Encoder_SetParam;
		codec->gbEncoder.remove = GreyVectorFile_Encoder_Delete;
		codec->gbEncoder.encode = GreyVectorFile_Encoder_Encode;
		codec->gbEncoder.flush = GreyVectorFile_Encoder_Flush;
		codec->gbEncoder.done = GreyVectorFile_Encoder_Done;
		codec->gbLibrary = creator->gbLibrary;
		codec->gbMem = creator->gbMem;
		codec->gbStream = stream;
		codec->nCacheItem = 0;
		codec->nItemCount = 0;
		codec->gbOffDataBits = sizeof(GREYVECTORFILEHEADER)
						     + sizeof(GREYVECTORINFOHEADER);
		GreyVectorFile_Encoder_Init(codec);
	}
	return (GB_Encoder)codec;
}
#endif //ENABLE_ENCODER
#endif //ENABLE_GREYVECTORFILE
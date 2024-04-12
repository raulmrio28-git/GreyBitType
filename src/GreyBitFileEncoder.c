/*
** ===========================================================================
** File: GreyBitFileEncoder.c
** Description: GreyBit font library - Bitmap font file encode
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
#include "../inc/GreyBitFile.h"

#ifdef ENABLE_GREYBITFILE
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
** Function: GreyBitFile_Encoder_Init
** Description: Init encoder
** Input: encoder - encoder
** Output: Init'ed encoder
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_Init(GBF_Encoder encoder)
{
	encoder->gbWidthTable = (GB_BYTE *)GreyBit_Malloc(encoder->gbMem,
													  MAX_COUNT);
	encoder->gbHoriOffTable = (GB_INT8 *)GreyBit_Malloc(encoder->gbMem,
														MAX_COUNT);
	encoder->gbOffsetTable = (GB_UINT32 *)GreyBit_Malloc(encoder->gbMem,
														 sizeof(GB_UINT32)
														*MAX_COUNT);
	encoder->gpGreyBits = (GB_BYTE **)GreyBit_Malloc(encoder->gbMem,
													 sizeof(GB_BYTE *)
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
** Function: GreyBitFile_Encoder_InfoInit
** Description: Init info
** Input: encoder - encoder
**        nHeight - height
**        nBitCount - bit count
**		  bCompress - compress?
** Output: Initialized info
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_InfoInit(GBF_Encoder encoder, GB_INT16 nHeight,
										 GB_INT16 nBitCount, GB_INT16 bCompress)
{
	GB_INT32	i;

	if (encoder->gbInited)
	{
		if (encoder->gbInfoHeader.gbiHeight == nHeight
			|| encoder->gbInfoHeader.gbiBitCount == nBitCount
			|| encoder->gbInfoHeader.gbiHeight == nHeight)
			return GB_SUCCESS;
		GB_MEMSET(encoder->gbWidthTable, 0, MAX_COUNT);
		GB_MEMSET(encoder->gbHoriOffTable, 0, MAX_COUNT);
		GB_MEMSET(encoder->gbOffsetTable, 0, sizeof(GB_UINT32) * MAX_COUNT);
		for (i = 0; i < encoder->nCacheItem; ++i)
		{
			if (encoder->gpGreyBits[i])
			{
				GreyBit_Free(encoder->gbMem, encoder->gpGreyBits[i]);
				encoder->gpGreyBits[i] = 0;
				encoder->pnGreySize[i] = 0;
			}
		}
	}
	encoder->gbInited = 1;
	encoder->gbInfoHeader.gbiBitCount = nBitCount;
	encoder->gbInfoHeader.gbiHeight = nHeight;
	if (nBitCount == 8)
		encoder->gbInfoHeader.gbiCompression = bCompress;
	else
		encoder->gbInfoHeader.gbiCompression = 0;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_ClearCache
** Description: Clear cache
** Input: encoder - encoder
** Output: Nuked cache
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitFile_Encoder_ClearCache(GBF_Encoder encoder)
{
	int	i;

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
** Function: GreyBitFile_Encoder_Compress
** Description: Compress character data
** Input: pOutData - output data
**        pnInOutLen - output data length
**        pInData - input data
**        nInDataLen - input data length
** Output: Compressed data
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_Compress(GB_BYTE *pOutData,
										 GB_INT32*pnInOutLen,GB_BYTE *pInData,
										 GB_INT32 nInDataLen)
{
	GB_INT32	nCompressLen;
	GB_INT32	nCompressLena;
	GB_INT32	i;
	GB_INT32	ia;
	GB_BYTE		nNextData; 
	GB_BYTE		nData;
	GB_BYTE		nLen;

	nCompressLen = 0;
	nLen = 0;
	nData = *pInData >> 1;
	if (pOutData)
	{
		for (i = 1; i < nInDataLen; ++i)
		{
			nNextData = pInData[i] >> 1;
			if (nData == nNextData && nLen < MAX_LEN())
			{
				++nLen;
			}
			else if (nLen)
			{
				pOutData[nCompressLen] = SET_LEN(nLen);
				pOutData[nCompressLen+1] = nData;
				nLen = 0;
			}
			else
			{
				pOutData[nCompressLen] = nData;
			}
			nCompressLen += ((nLen > 0) ? 2 : 1);
			nData = nNextData;
		}
		if (nLen)
		{
			pOutData[nCompressLen] = SET_LEN(nLen);
			nCompressLen++;
		}
		pOutData[nCompressLen] = nData;
		nCompressLena = nCompressLen + 1;
	}
	else
	{
		for (ia = 1; ia < nInDataLen; ++ia)
		{
			if (nData == pInData[ia] >> 1 && nLen < MAX_LEN())
			{
				++nLen;
			}
			else if (nLen)
			{
				nCompressLen += 2;
				nLen = 0;
			}
			else
			{
				++nCompressLen;
			}
			nData = pInData[ia] >> 1;
		}
		if (nLen)
			nCompressLena = nCompressLen + 2;
		else
			nCompressLena = nCompressLen + 1;
	}
	*pnInOutLen = nCompressLena;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_BuildAll
** Description: Build everything pre-write
** Input: encoder - encoder
** Output: Initialised header
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_BuildAll(GBF_Encoder encoder)
{
	GB_UINT16	nSize;
	GB_UINT16	nSizea;
	GB_UINT32	nCount;
	GB_UINT32	nGreyBitSize;
	GB_UINT32	nOffSetTableSize;
	GB_UINT32	nHoriOffTableSize;
	GB_UINT32	nWidthTableSize;
	GB_UINT16	nCode;
	GB_UINT16	nCodea;
	GB_UINT16	nCodeb;
	GB_UINT16	nMinCode;
	GB_UINT16	nMaxCode;
	GB_UINT16	nSectionLen;
	GB_UINT16	nSection;

	nWidthTableSize = 0;
	nHoriOffTableSize = 0;
	nOffSetTableSize = 0;
	nGreyBitSize = 0;
	nCount = 0;
	nCodea = 0;
	nCodeb = 0;
	for (nSection = 0; nSection < UNICODE_SECTION_NUM; ++nSection)
	{
		UnicodeSection_GetSectionInfo(nSection, &nMinCode, &nMaxCode);
		nSectionLen = nMaxCode - nMinCode + 1;
		for (nCode = nMinCode; nCode <= nMaxCode; ++nCode)
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
	if (encoder->gbInfoHeader.gbiCompression
	 && encoder->gbInfoHeader.gbiBitCount == 8)
	{
		while (nCodea < MAX_COUNT)
		{
			nSize = (GB_UINT16)encoder->pnGreySize[nCodea];
			if (nSize)
			{
				encoder->gbOffsetTable[nCodea] = nGreyBitSize;
				nGreyBitSize += nSize + 2;
				++nCount;
			}
			nCodea++;
		}
	}
	else
	{
		while (nCodeb < MAX_COUNT)
		{
			nSizea = (GB_UINT16)encoder->pnGreySize[nCodeb];
			if (nSizea)
			{
				encoder->gbOffsetTable[nCodeb] = nGreyBitSize;
				nGreyBitSize += nSizea;
				++nCount;
			}
			nCodeb++;
		}
	}
	encoder->gbInfoHeader.gbiCount = nCount;
	encoder->gbInfoHeader.gbiOffGreyBits =nOffSetTableSize + nHoriOffTableSize
										 + nWidthTableSize;
	encoder->gbInfoHeader.gbiOffsetTabOff=nHoriOffTableSize + nWidthTableSize;
	encoder->gbInfoHeader.gbiHoriOffTabOff = nWidthTableSize;
	encoder->gbInfoHeader.gbiWidthTabOff = 0;
	encoder->gbInfoHeader.gbiSize = sizeof(GREYBITINFOHEADER);
	encoder->gbFileHeader.gbfTag[0] = 'g';
	encoder->gbFileHeader.gbfTag[1] = 'b';
	encoder->gbFileHeader.gbfTag[2] = 't';
	encoder->gbFileHeader.gbfTag[3] = 'f';
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_WriteAll
** Description: Flush everything to stream
** Input: encoder - encoder
** Output: Finished product
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_WriteAll(GBF_Encoder encoder)
{
	GB_INT32	nDataSize;
	GB_BYTE *	pData; 
	GB_INT32	nCode; 
	GB_UINT16	nMinCode; 
	GB_UINT16	nMaxCode;
	GB_INT32	nSectionLen;
	GB_INT32	nSection;

	GreyBit_Stream_Seek(encoder->gbStream, 0);
	GreyBit_Stream_Write(encoder->gbStream, (GB_BYTE*)&encoder->gbFileHeader,
						 sizeof(GREYBITFILEHEADER));
	GreyBit_Stream_Write(encoder->gbStream, (GB_BYTE*)&encoder->gbInfoHeader,
						 sizeof(GREYBITINFOHEADER));
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
	if (encoder->gbInfoHeader.gbiCompression
	 && encoder->gbInfoHeader.gbiBitCount == 8)
	{
		for (nCode = 0; nCode < encoder->nCacheItem; ++nCode)
		{
			nDataSize = encoder->pnGreySize[nCode];
			if (nDataSize)
			{
				GreyBit_Stream_Write(encoder->gbStream, (GB_BYTE*)&nDataSize,
									 sizeof(GB_UINT16));
				pData = encoder->gpGreyBits[nCode];
				GreyBit_Stream_Write(encoder->gbStream, pData, nDataSize);
			}
		}
	}
	else
	{
		for (nCode = 0; nCode < encoder->nCacheItem; ++nCode)
		{
			nDataSize = encoder->pnGreySize[nCode];
			if (nDataSize)
			{
				pData = encoder->gpGreyBits[nCode];
				GreyBit_Stream_Write(encoder->gbStream, pData, nDataSize);
			}
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
** Function: GreyBitFile_Encoder_GetCount
** Description: Get character count
** Input: encoder - encoder
** Output: Character count
** Return value: encoder->nItemCount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_GetCount(GB_Encoder encoder)
{
	GBF_Encoder	me = (GBF_Encoder)encoder;

	return me->nItemCount;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_SetParam
** Description: Set encoder param
** Input: encoder - encoder
**        nParam - param type
**        dwParam - param value
** Output: Set param
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_SetParam(GB_Encoder encoder, GB_Param nParam,
									     GB_UINT32 dwParam)
{
	GBF_Encoder	me = (GBF_Encoder)encoder;

	if (dwParam)
	{
		if (nParam == GB_PARAM_HEIGHT)
			me->nHeight = (GB_UINT16)dwParam;
		if (nParam == GB_PARAM_BITCOUNT)
			me->nBitCount = (GB_INT16)dwParam;
		if (nParam ==GB_PARAM_COMPRESS)
			me->bCompress = (GB_BOOL)dwParam;
	}
	GreyBitFile_Encoder_InfoInit(me,me->nHeight,me->nBitCount,me->bCompress);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_Delete
** Description: Remove character
** Input: encoder - encoder
**        nCode - unicode code
** Output: Removed character
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_Delete(GB_Encoder encoder, GB_UINT32 nCode)
{
	GBF_Encoder	me = (GBF_Encoder)encoder;

	me->gbOffsetTable[nCode] = 0;
	me->gbHoriOffTable[nCode] = GB_HORIOFF_DEFAULT;
	me->gbWidthTable[nCode] = GB_WIDTH_DEFAULT;
	if (me->gpGreyBits[nCode])
	{
		GreyBit_Free(me->gbMem, me->gpGreyBits[nCode]);
		me->gpGreyBits[nCode] = 0;
	}
	me->pnGreySize[nCode] = 0;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_Encode
** Description: Encode code to new font
** Input: encoder - encoder
**        nCode - unicode code
**        pData - data buffer
** Output: Added character
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_Encode(GB_Encoder encoder, GB_UINT32 nCode,
									   GB_Data pData)
{
	GB_BYTE *	pByteData;
	GB_INT32	nOutLen;
	GB_INT32	nInDataLen;
	GB_Bitmap	bitmap;
	GBF_Encoder	me = (GBF_Encoder)encoder;

	if (!pData || pData->format != GB_FORMAT_BITMAP)
		return GB_FAILED;
	if (!me->nHeight || !me->nBitCount || !me->gbInited)
		return GB_FAILED;
	bitmap = (GB_Bitmap)pData->data;
	if (bitmap->bitcount != me->gbInfoHeader.gbiBitCount
		|| bitmap->height != me->gbInfoHeader.gbiHeight
		|| bitmap->width > 3 * bitmap->height)
	{
		return GB_FAILED;
	}
	if (me->gbInfoHeader.gbiWidth < bitmap->width)
		me->gbInfoHeader.gbiWidth = bitmap->width;
	nInDataLen = bitmap->pitch * bitmap->height;
	if (me->gbInfoHeader.gbiCompression)
	{
		GreyBitFile_Encoder_Compress(0, &nOutLen, bitmap->buffer, nInDataLen);
		pByteData = (GB_BYTE *)GreyBit_Malloc(me->gbMem, nOutLen);
		GreyBitFile_Encoder_Compress(pByteData, &nOutLen, bitmap->buffer,
									 nInDataLen);
	}
	else
	{
		nOutLen = nInDataLen;
		pByteData = (GB_BYTE *)GreyBit_Malloc(me->gbMem, nInDataLen);
		GB_MEMCPY(pByteData, bitmap->buffer, nInDataLen);
	}
	if (me->gpGreyBits[nCode])
		GreyBit_Free(me->gbMem, me->gpGreyBits[nCode]);
	me->gpGreyBits[nCode] = pByteData;
	me->pnGreySize[nCode] = (GB_UINT16)nOutLen;
	me->gbOffsetTable[nCode] = SET_RAM(nCode);
	me->gbWidthTable[nCode] = (GB_BYTE)bitmap->width;
	me->gbHoriOffTable[nCode] = (GB_INT8)bitmap->horioff;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_Flush
** Description: Flush encoder
** Input: encoder - encoder
** Output: Flushed encoder
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Encoder_Flush(GB_Encoder encoder)
{
	GBF_Encoder	me = (GBF_Encoder)encoder;

	GreyBitFile_Encoder_BuildAll(me);
	GreyBitFile_Encoder_WriteAll(me);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_Done
** Description: Done with encoder? Nuke it!
** Input: encoder - encoder
** Output: Nuked encoder
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitFile_Encoder_Done(GB_Encoder encoder)
{
	GBF_Encoder	me = (GBF_Encoder)encoder;

	if (me->nHeight && me->nBitCount)
	{
		GreyBitFile_Encoder_Flush(&me->gbEncoder);
		me->nHeight = 0;
		me->nBitCount = 0;
	}
	GreyBitFile_Encoder_ClearCache(me);
	GreyBit_Free(me->gbMem, encoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Encoder_New
** Description: Create new encoder
** Input: creator - creator
**        stream - stream
** Output: New encoder
** Return value: codec
** ---------------------------------------------------------------------------
*/

GB_Encoder	GreyBitFile_Encoder_New(GB_Creator creator, GB_Stream stream)
{
	GBF_Encoder	codec;

	codec = (GBF_Encoder)GreyBit_Malloc(creator->gbMem, sizeof(GBF_Encoder));
	if (codec)
	{
		codec->gbEncoder.getcount = GreyBitFile_Encoder_GetCount;
		codec->gbEncoder.setparam = GreyBitFile_Encoder_SetParam;
		codec->gbEncoder.remove = GreyBitFile_Encoder_Delete;
		codec->gbEncoder.encode = GreyBitFile_Encoder_Encode;
		codec->gbEncoder.flush = GreyBitFile_Encoder_Flush;
		codec->gbEncoder.done = GreyBitFile_Encoder_Done;
		codec->gbLibrary = creator->gbLibrary;
		codec->gbMem = creator->gbMem;
		codec->gbStream = stream;
		codec->nCacheItem = 0;
		codec->nItemCount = 0;
		codec->gbOffDataBits = sizeof(GREYBITFILEHEADER)
			+ sizeof(GREYBITINFOHEADER);
		GreyBitFile_Encoder_Init(codec);
	}
	return (GB_Encoder)codec;
}
#endif //ENABLE_ENCODER
#endif //ENABLE_GREYBITFILE
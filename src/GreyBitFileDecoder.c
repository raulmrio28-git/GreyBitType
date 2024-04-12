/*
** ===========================================================================
** File: GreyBitFileDecoder.c
** Description: GreyBit font library - Bitmap font file decode
** Copyright (c) 2023
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2023	me				Upgrade
** 08/09/2023	me              Init
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
** Function: GreyBitFile_Decoder_InfoInit
** Description: Init info
** Input: decoder - decoder
**        nMaxWidth - max width
**        nHeight - height
**        nBitCount - bit count
** Output: Initialized info
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_InfoInit(GBF_Decoder decoder,
										 GB_INT16 nMaxWidth, GB_INT16 nHeight,
										 GB_INT16 nBitCount)
{
	decoder->gbBitmap = GreyBitType_Bitmap_New(decoder->gbLibrary, nMaxWidth, 
											  nHeight, nBitCount, 0);
	decoder->nBuffSize = decoder->gbBitmap->pitch * decoder->gbBitmap->height;
	decoder->pBuff = (GB_BYTE *)GreyBit_Malloc(decoder->gbMem,
											   decoder->nBuffSize);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_CaheItem
** Description: Cache grey item
** Input: decoder - decoder
**        nCode - code
**        pData - data
**        nDataSize - data size
** Output: Cached data
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_CaheItem(GBF_Decoder decoder, GB_UINT32 nCode,
										 GB_BYTE *pData, GB_INT32 nDataSize)
{
	GB_UINT16	nMinCode;
	GB_UINT16	SectionIndex;
	GB_INT32	UniIndex;

	if (decoder->nGreyBitsCount>=decoder->nCacheItem||!decoder->gbOffsetTable)
		return GB_FAILED;
	UniIndex = UnicodeSection_GetIndex((GB_UINT16)nCode);
	SectionIndex=decoder->gbInfoHeader.gbiIndexSection.gbSectionOff[UniIndex];
	if (UniIndex >= UNICODE_SECTION_NUM)
		return GB_FAILED;
	if (!SectionIndex)
		return GB_FAILED;
	decoder->gpGreyBits[decoder->nGreyBitsCount] =(GB_BYTE *)GreyBit_Malloc
												  (decoder->gbMem, nDataSize);
	decoder->pnGreySize[decoder->nGreyBitsCount] = (GB_INT16)nDataSize;
	GB_MEMCPY(decoder->gpGreyBits[decoder->nGreyBitsCount], pData, nDataSize);
	UnicodeSection_GetSectionInfo(UniIndex, &nMinCode, 0);
	SectionIndex--;
	SectionIndex += (GB_UINT16)nCode - nMinCode;
	decoder->gbOffsetTable[SectionIndex] = SET_RAM(decoder->nGreyBitsCount++);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_ClearCache
** Description: Clear cache
** Input: decoder - decoder
** Output: Nuked cache
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitFile_Decoder_ClearCache(GBF_Decoder decoder)
{
	int	i;

	if (decoder->gbBitmap)
		GreyBitType_Bitmap_Done(decoder->gbLibrary, decoder->gbBitmap);
	if (decoder->pBuff)
		GreyBit_Free(decoder->gbMem, decoder->pBuff);
	if (decoder->gbWidthTable)
		GreyBit_Free(decoder->gbMem, decoder->gbWidthTable);
	if (decoder->gbHoriOffTable)
		GreyBit_Free(decoder->gbMem, decoder->gbHoriOffTable);
	if (decoder->gbOffsetTable)
		GreyBit_Free(decoder->gbMem, decoder->gbOffsetTable);
	if (decoder->pnGreySize)
		GreyBit_Free(decoder->gbMem, decoder->pnGreySize);
	if (decoder->gpGreyBits)
	{
		for (i = 0; i < decoder->nCacheItem; ++i)
		{
			if (decoder->gpGreyBits[i])
				GreyBit_Free(decoder->gbMem, decoder->gpGreyBits[i]);
		}
		GreyBit_Free(decoder->gbMem, decoder->gpGreyBits);
	}
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_Decompress
** Description: Decompress character data
** Input: pOutData - output data
**        pnInOutLen - output data length
**        pInData - input data
**        nInDataLen - input data length
** Output: Decompressed data
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	 GreyBitFile_Decoder_Decompress(GB_BYTE* pOutData,
	GB_INT32* pnInOutLen,
	GB_BYTE* pInData,
	GB_INT32 nInDataLen)
{
	GB_INT32	nDecompressLen;
	GB_INT32	j;
	GB_INT32	ja;
	GB_INT32	i;
	GB_INT32	ia;
	GB_BYTE		nData;
	GB_BYTE		nDataa;
	GB_BYTE		nLen;

	nLen = 0;
	nDecompressLen = 0;
	if (pOutData)
	{
		for (i = 0; i < nInDataLen; ++i)
		{
			nData = pInData[i];
			if (nLen)
			{
				for (j = 0; j < nLen; ++j)
					pOutData[nDecompressLen+j] = (nData << 1) | 1;
				nDecompressLen += nLen;
				nLen = 0;
			}
			else if (IS_LEN(nData))
			{
				nLen = GET_LEN(nData);
			}
			else
			{
				pOutData[nDecompressLen] = (nData << 1) | 1;
				nDecompressLen++;
			}
		}
	}
	else
	{
		for (ia = 0; ia < nInDataLen; ++ia)
		{
			nDataa = pInData[ia];
			if (nLen)
			{
				for (ja = 0; ja < nLen; ++ja)
					++nDecompressLen;
				nLen = 0;
			}
			else if (IS_LEN(nDataa))
			{
				nLen = GET_LEN(nDataa);
			}
			else
			{
				++nDecompressLen;
			}
		}
	}
	if (nLen)
		return GB_FAILED;
	*pnInOutLen = nDecompressLen;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_ReadHeader
** Description: Read header of font data
** Input: decoder - decoder
** Output: Read header
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_ReadHeader(GBF_Decoder decoder)
{
	GreyBit_Stream_Seek(decoder->gbStream, 0);
	if (GreyBit_Stream_Read(decoder->gbStream,
							(GB_BYTE*)&decoder->gbFileHeader,
							sizeof(GREYBITFILEHEADER))
							!= sizeof(GREYBITFILEHEADER))
		return GB_FAILED;
	GreyBit_Stream_Read(decoder->gbStream, (GB_BYTE*)&decoder->gbInfoHeader,
						sizeof(GREYBITINFOHEADER));
	decoder->nItemCount = decoder->gbInfoHeader.gbiCount;
	GreyBitFile_Decoder_InfoInit(decoder, decoder->gbInfoHeader.gbiWidth,
								 decoder->gbInfoHeader.gbiHeight,
								 decoder->gbInfoHeader.gbiBitCount);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_GetDataOffset
** Description: Get code data offset
** Input: decoder - decoder
**        nCode - code
** Output: data offset
** Return value: nOffset
** ---------------------------------------------------------------------------
*/

GB_UINT32	GreyBitFile_Decoder_GetDataOffset(GBF_Decoder decoder,
											  GB_UINT32 nCode)
{
	GB_UINT32	nOffset;
	GB_UINT16	nMinCode;
	GB_UINT16	SectionIndex;
	GB_INT32	UniIndex;

	UniIndex = UnicodeSection_GetIndex((GB_UINT16)nCode);
	if (UniIndex >= UNICODE_SECTION_NUM)
		return 0;
	SectionIndex=decoder->gbInfoHeader.gbiIndexSection.gbSectionOff[UniIndex];
	if (!SectionIndex)
		return 0;
	SectionIndex--;
	if (decoder->gbOffsetTable)
	{
		UnicodeSection_GetSectionInfo(UniIndex, &nMinCode, 0);
		return decoder->gbOffsetTable[nCode - nMinCode + SectionIndex];
	}
	else
	{
		UnicodeSection_GetSectionInfo(UniIndex, &nMinCode, 0);
		SectionIndex += (GB_UINT16)nCode - nMinCode;
		GreyBit_Stream_Seek(decoder->gbStream,
							decoder->gbInfoHeader.gbiOffsetTabOff
						  + decoder->gbOffDataBits + sizeof(GB_UINT32)
						  * SectionIndex);
		GreyBit_Stream_Read(decoder->gbStream, (GB_BYTE *)&nOffset, 4);
		return nOffset;
	}
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_Init
** Description: Extract data for decode process
** Input: decoder - decoder
** Output: Init'ed data
** Return value: success/other error
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_Init(GBF_Decoder decoder)
{
	GB_INT32	nDataSize;
	GB_INT32	nDataSizea;
	GB_INT32	nDataSizeb;
	GB_INT32	nRet;

	nRet = GreyBitFile_Decoder_ReadHeader(decoder);
	if (nRet < 0)
		return nRet;
	nDataSize = decoder->gbInfoHeader.gbiHoriOffTabOff
			  - decoder->gbInfoHeader.gbiWidthTabOff;
	decoder->gbWidthTable=(GB_BYTE *)GreyBit_Malloc(decoder->gbMem,nDataSize);
	GreyBit_Stream_Seek(decoder->gbStream,decoder->gbInfoHeader.gbiWidthTabOff
					  + decoder->gbOffDataBits);
	GreyBit_Stream_Read(decoder->gbStream, decoder->gbWidthTable, nDataSize);
	nDataSizea = decoder->gbInfoHeader.gbiOffsetTabOff
			   - decoder->gbInfoHeader.gbiHoriOffTabOff;
	decoder->gbHoriOffTable = (GB_BYTE *)GreyBit_Malloc(decoder->gbMem,
														nDataSizea);
	GreyBit_Stream_Seek(decoder->gbStream,
						decoder->gbInfoHeader.gbiHoriOffTabOff
					  + decoder->gbOffDataBits);
	GreyBit_Stream_Read(decoder->gbStream,decoder->gbHoriOffTable,nDataSizea);
	nDataSizeb = decoder->gbInfoHeader.gbiOffGreyBits
			   - decoder->gbInfoHeader.gbiOffsetTabOff;
	decoder->gbOffsetTable = (GB_UINT32 *)GreyBit_Malloc(decoder->gbMem,
														 nDataSizeb);
	GreyBit_Stream_Seek(decoder->gbStream,
						decoder->gbInfoHeader.gbiOffsetTabOff
					  + decoder->gbOffDataBits);
	GreyBit_Stream_Read(decoder->gbStream, (GB_BYTE *)decoder->gbOffsetTable,
						nDataSizeb);
	return GB_SUCCESS;
}

/*
**----------------------------------------------------------------------------
**  Function(internal and external use) Declarations
**----------------------------------------------------------------------------
*/

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_New
** Description: Initialize decoder handle
** Input: decoder - decoder
** 	      stream - stream
** Output: Init'ed decoder
** Return value: decoder
** ---------------------------------------------------------------------------
*/

GB_Decoder	GreyBitFile_Decoder_New(GB_Loader loader, GB_Stream stream)
{
	GBF_Decoder	decoder;

	decoder = (GBF_Decoder)GreyBit_Malloc(loader->gbMem,sizeof(GBF_Decoder));
	if (decoder)
	{
		decoder->gbDecoder.setparam = GreyBitFile_Decoder_SetParam;
		decoder->gbDecoder.getcount = GreyBitFile_Decoder_GetCount;
		decoder->gbDecoder.getwidth = GreyBitFile_Decoder_GetWidth;
		decoder->gbDecoder.getheight = GreyBitFile_Decoder_GetHeight;
		decoder->gbDecoder.getadvance = GreyBitFile_Decoder_GetAdvance;
		decoder->gbDecoder.decode = GreyBitFile_Decoder_Decode;
		decoder->gbDecoder.done = GreyBitFile_Decoder_Done;
		decoder->gbLibrary = loader->gbLibrary;
		decoder->gbMem = loader->gbMem;
		decoder->gbStream = stream;
		decoder->nCacheItem = 0;
		decoder->nItemCount = 0;
		decoder->gbOffDataBits = sizeof(GREYBITFILEHEADER)
			+ sizeof(GREYBITINFOHEADER);
		GreyBitFile_Decoder_Init(decoder);
	}
	return (GB_Decoder)decoder;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_SetParam
** Description: Set decoder param
** Input: decoder - decoder
**        nParam - param type
**        dwParam - param value
** Output: Set param
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_SetParam(GB_Decoder decoder, GB_Param nParam,
										 GB_UINT32 dwParam)
{
	GBF_Decoder	me = (GBF_Decoder)decoder;

	if (dwParam)
	{
		if (nParam == GB_PARAM_CACHEITEM)
		{
			if (me->gpGreyBits)
				return GB_FAILED;
			me->nCacheItem = dwParam;
			me->gpGreyBits = (GB_BYTE**)GreyBit_Malloc(me->gbMem,
				4 * me->nCacheItem);
			me->pnGreySize = (GB_UINT16*)GreyBit_Malloc(me->gbMem,
				2 * me->nCacheItem);
			me->nGreyBitsCount = 0;
		}
	}
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_GetCount
** Description: Get character count
** Input: decoder - decoder
** Output: Character count
** Return value: decoder->nItemCount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_GetCount(GB_Decoder decoder)
{
	GBF_Decoder	me = (GBF_Decoder)decoder;

	return me->nItemCount;
}


/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_GetHeight
** Description: Get character height
** Input: decoder - decoder
** Output: Height
** Return value: decoder->gbInfoHeader.gbiHeight
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_GetHeight(GB_Decoder decoder)
{
	GBF_Decoder	me = (GBF_Decoder)decoder;

	return me->gbInfoHeader.gbiHeight;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_GetWidth
** Description: Get character width
** Input: decoder - decoder
**		  nCode - unicode code
**		  nSize - character size
** Output: Height
** Return value: nSize * nWidth / decoder->gbInfoHeader.gbiHeight
**             / decoder->gbWidthTable[nCode - nMinCode + WidthIdx]
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_GetWidth(GB_Decoder decoder, GB_UINT32 nCode,
										 GB_INT16 nSize)
{
	GB_BYTE		nWidth;
	GB_UINT16	nMinCode;
	GB_INT32	UniIndex;
	GB_INT32	WidthIdx;
	GBF_Decoder	me;

	me = (GBF_Decoder)decoder;
	UniIndex = UnicodeSection_GetIndex((GB_UINT16)nCode);
	WidthIdx = me->gbInfoHeader.gbiWidthSection.gbSectionOff[UniIndex];
	if (UniIndex >= UNICODE_SECTION_NUM)
		return 0;
	if (!WidthIdx)
		return 0;
	WidthIdx--;
	if (me->gbWidthTable)
	{
		UnicodeSection_GetSectionInfo(UniIndex, &nMinCode, 0);
		nWidth = me->gbWidthTable[nCode - nMinCode + WidthIdx];
	}
	else
	{
		UnicodeSection_GetSectionInfo(UniIndex, &nMinCode, 0);
		WidthIdx += nCode - nMinCode;
		GreyBit_Stream_Seek(me->gbStream, me->gbInfoHeader.gbiWidthTabOff
			+ me->gbOffDataBits + WidthIdx);
		GreyBit_Stream_Read(me->gbStream, &nWidth, sizeof(GB_BYTE));
	}
	return nSize * nWidth / me->gbInfoHeader.gbiHeight;
}
GB_INT16	GreyBitFile_Decoder_GetHoriOff(GB_Decoder decoder,
										   GB_UINT32 nCode,
										   GB_INT16 nSize)
{
	GB_INT8		nHoriOff;
	GB_UINT16	nMinCode;
	GB_INT32	UniIndex;
	GB_INT32	HoriOffIdx;
	GBF_Decoder	me;

	me = (GBF_Decoder)decoder;
	UniIndex = UnicodeSection_GetIndex((GB_UINT16)nCode);
	HoriOffIdx = me->gbInfoHeader.gbiWidthSection.gbSectionOff[UniIndex];
	if (UniIndex >= UNICODE_SECTION_NUM)
		return 0;
	if (!HoriOffIdx)
		return 0;
	HoriOffIdx--;
	if (me->gbHoriOffTable)
	{
		UnicodeSection_GetSectionInfo(UniIndex, &nMinCode, 0);
		nHoriOff = me->gbHoriOffTable[nCode - nMinCode + HoriOffIdx];
	}
	else
	{
		UnicodeSection_GetSectionInfo(UniIndex, &nMinCode, 0);
		HoriOffIdx += nCode - nMinCode;
		GreyBit_Stream_Seek(me->gbStream, me->gbInfoHeader.gbiHoriOffTabOff
						  + me->gbOffDataBits + HoriOffIdx);
		GreyBit_Stream_Read(me->gbStream, &nHoriOff, sizeof(GB_BYTE));
	}
	return nSize * nHoriOff / me->gbInfoHeader.gbiHeight;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_GetAdvance
** Description: Get character advance
** Input: decoder - decoder
**		  nCode - unicode code
**		  nSize - character size
** Output: Font char advance
** Return value: nAdvance
** ---------------------------------------------------------------------------
*/

GB_INT16	GreyBitFile_Decoder_GetAdvance(GB_Decoder decoder, GB_UINT32 nCode,
										   GB_INT16 nSize)
{
	GB_INT16	nWidth;
	GB_INT16	nAdvance;

	nWidth = (GB_INT16)GreyBitFile_Decoder_GetWidth(decoder, nCode, nSize);
	nAdvance = GreyBitFile_Decoder_GetHoriOff(decoder, nCode, nSize) + nWidth;
	if (nAdvance <= 0)
		return 0;
	else
		return nAdvance;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_Decode
** Description: Decode a character
** Input: decoder - decoder
**		  nCode - unicode code
**	      pData - data buffer
**		  nSize - character size
** Output: Decided character
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitFile_Decoder_Decode(GB_Decoder decoder, GB_UINT32 nCode,
									   GB_Data pData, GB_INT16 nSize)
{
	GB_UINT16	Lenght;
	GB_INT16	nHoriOff;
	GB_INT16	nWidth; 
	GB_INT32	nInDataLen; 
	GB_INT32	nDataLen;
	GB_BYTE *	pByteData; 
	GB_UINT32	Offset;
	GBF_Decoder	me = (GBF_Decoder)decoder;

	Offset = GreyBitFile_Decoder_GetDataOffset(me, nCode);
	nWidth=(GB_INT16)GreyBitFile_Decoder_GetWidth(decoder, nCode,
												  me->gbInfoHeader.gbiHeight);
	nHoriOff = GreyBitFile_Decoder_GetHoriOff(decoder, nCode, nSize);
	if (!nWidth)
		return GB_FAILED;
	me->gbBitmap->width = nWidth;
	me->gbBitmap->pitch = (GB_INT16)(me->gbBitmap->bitcount * 8 * nWidth
						+ 63) >> 6;
	me->gbBitmap->horioff = nHoriOff;
	nDataLen = me->gbBitmap->pitch * me->gbBitmap->height;
	if (!IS_INRAM(Offset))
	{
		GreyBit_Stream_Seek(me->gbStream, me->gbInfoHeader.gbiOffGreyBits
						 + me->gbOffDataBits + Offset);
		if (me->gbInfoHeader.gbiCompression
		 && me->gbInfoHeader.gbiBitCount == 8)
		{
			GreyBit_Stream_Read(me->gbStream, (GB_BYTE*)&Lenght,
								sizeof(GB_UINT16));
			nInDataLen = Lenght;
		}
		else
		{
			nInDataLen = nDataLen;
		}
		GreyBit_Stream_Read(me->gbStream, me->pBuff, nInDataLen);
		pByteData = me->pBuff;
		GreyBitFile_Decoder_CaheItem(me, nCode, pByteData, nInDataLen);
	}
	else
	{
		GET_INDEX(Offset);
		pByteData = me->gpGreyBits[Offset];
		nInDataLen = me->pnGreySize[Offset];
	}
	if (!pByteData)
		return GB_SUCCESS;
	if (me->gbInfoHeader.gbiCompression)
		GreyBitFile_Decoder_Decompress(me->gbBitmap->buffer, &nDataLen,
									   pByteData, nInDataLen);
	else
		GB_MEMCPY(me->gbBitmap->buffer, pByteData, nDataLen);
	if (pData)
	{
		pData->format = GB_FORMAT_BITMAP;
		pData->data = me->gbBitmap;
		pData->width = (GB_INT16)nWidth;
		pData->horioff = nHoriOff;
	}
	return GB_SUCCESS;
}


/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Decoder_Done
** Description: Done with decoder? Nuke it!
** Input: decoder - decoder
** Output: Nuked decoder
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitFile_Decoder_Done(GB_Decoder decoder)
{
	GBF_Decoder	me = (GBF_Decoder)decoder;

	GreyBitFile_Decoder_ClearCache(me);
	GreyBit_Free(me->gbMem, decoder);
}
#endif //ENABLE_GREYBITFILE
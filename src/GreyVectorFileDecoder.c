/*
** ===========================================================================
** File: GreyBitFileDecoder.c
** Description: GreyBit font library - Vector font file decode
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

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../inc/UnicodeSection.h"
#include "../GreyBitSystem.h"
#include "../inc/GreyVectorFile.h"

#ifdef ENABLE_GREYVECTORFILE
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
** Function: GreyVectorFile_Decoder_CaheItem
** Description: Cache grey item
** Input: decoder - decoder
**        nCode - code
**        pData - data
**        nDataSize - data size
** Output: Cached data
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_CaheItem(GVF_Decoder decoder,
											GB_UINT32 nCode,
											GB_Outline outline)
{
	GB_UINT16	nMinCode;
	GB_UINT16	SectionIndex;
	GB_INT32	UniIndex;

	if (decoder->nGreyBitsCount >= decoder->nCacheItem
	|| !decoder->gbOffsetTable)
		return GB_FAILED;
	UniIndex = UnicodeSection_GetIndex((GB_UINT16)nCode);
	if (UniIndex >= UNICODE_SECTION_NUM)
		return GB_FAILED;
	SectionIndex=decoder->gbInfoHeader.gbiIndexSection.gbSectionOff[UniIndex];
	if (!SectionIndex)
		return GB_FAILED;
	decoder->gpGreyBits[decoder->nGreyBitsCount] = GreyBitType_Outline_Clone
										 (decoder->gbLibrary, outline);
	UnicodeSection_GetSectionInfo(UniIndex, &nMinCode, 0);
	SectionIndex--;
	SectionIndex += (GB_UINT16)nCode - nMinCode;
	decoder->gbOffsetTable[SectionIndex] = SET_RAM(decoder->nGreyBitsCount++);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_ClearCache
** Description: Clear cache
** Input: decoder - decoder
** Output: Nuked cache
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyVectorFile_Decoder_ClearCache(GVF_Decoder decoder)
{
	int i;

	if (decoder->gbOutline)
		GreyBitType_Outline_Done(decoder->gbLibrary, decoder->gbOutline);
	if (decoder->pBuff)
		GreyBit_Free(decoder->gbMem, decoder->pBuff);
	if (decoder->gbWidthTable)
		GreyBit_Free(decoder->gbMem, decoder->gbWidthTable);
	if (decoder->gbHoriOffTable)
		GreyBit_Free(decoder->gbMem, decoder->gbHoriOffTable);
	if (decoder->gbOffsetTable)
		GreyBit_Free(decoder->gbMem, decoder->gbOffsetTable);
	if (decoder->gpGreyBits)
	{
		for (i = 0; i < decoder->nCacheItem; ++i)
		{
			if (decoder->gpGreyBits[i])
				GreyBitType_Outline_Done(decoder->gbLibrary,
										 decoder->gpGreyBits[i]);
		}
		GreyBit_Free(decoder->gbMem, decoder->gpGreyBits);
	}
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_InfoInit
** Description: Init info
** Input: decoder - decoder
**        nMaxWidth - max width
**        nHeight - height
**        nBitCount - bit count
** Output: Initialized info
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_InfoInit(GVF_Decoder decoder,
											GB_INT16 nMaxWidth,
											GB_INT16 nHeight,
											GB_INT16 nMaxPoints,
											GB_INT16 nMaxContours)
{
	decoder->gbOutline = GreyBitType_Outline_New(decoder->gbLibrary,
											nMaxContours,
											nMaxPoints);
	decoder->nBuffSize = GreyVector_Outline_GetSizeEx((GB_BYTE)nMaxContours,
												(GB_BYTE)nMaxPoints)
											  + sizeof(GVF_OutlineRec);
	decoder->pBuff = (GB_BYTE *)GreyBit_Malloc(decoder->gbMem,
											   decoder->nBuffSize);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_ReadHeader
** Description: Read header of font data
** Input: decoder - decoder
** Output: Read header
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_ReadHeader(GVF_Decoder decoder)
{
	GreyBit_Stream_Seek(decoder->gbStream, 0);
	if (GreyBit_Stream_Read(decoder->gbStream,
							(GB_BYTE*)&decoder->gbFileHeader,
							sizeof(GREYVECTORFILEHEADER))
						 != sizeof(GREYVECTORFILEHEADER))
		return GB_FAILED;
	GreyBit_Stream_Read(decoder->gbStream, (GB_BYTE*)&decoder->gbInfoHeader,
						sizeof(GREYVECTORINFOHEADER));
	decoder->nItemCount = decoder->gbInfoHeader.gbiCount;
	GreyVectorFile_Decoder_InfoInit(decoder, decoder->gbInfoHeader.gbiWidth,
									decoder->gbInfoHeader.gbiHeight,
									decoder->gbInfoHeader.gbiMaxPoints,
									decoder->gbInfoHeader.gbiMaxContours);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_GetDataOffset
** Description: Get code data offset
** Input: decoder - decoder
**        nCode - code
** Output: data offset
** Return value: nOffset
** ---------------------------------------------------------------------------
*/

GB_UINT32	GreyVectorFile_Decoder_GetDataOffset(GVF_Decoder decoder,
												 GB_UINT32 nCode)
{
	GB_UINT32	nOffset;
	GB_UINT16	nMinCode;
	GB_UINT16	SectionIndex;
	GB_INT32	UniIndex;

	UniIndex = UnicodeSection_GetIndex((GB_UINT16)nCode);
	SectionIndex=decoder->gbInfoHeader.gbiIndexSection.gbSectionOff[UniIndex];
	if (UniIndex >= UNICODE_SECTION_NUM)
		return 0;
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
						  + decoder->gbOffDataBits + 4 * SectionIndex);
		GreyBit_Stream_Read(decoder->gbStream, (GB_BYTE*)&nOffset,
							sizeof(GB_UINT32));
		return nOffset;
	}
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_Init
** Description: Extract data for decode process
** Input: decoder - decoder
** Output: Init'ed data
** Return value: success/other error
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_Init(GVF_Decoder decoder)
{
	int nDataSize;
	int nDataSizea;
	int nDataSizeb;
	int nRet;

	GreyBit_Stream_Seek(decoder->gbStream, 0);
	nRet = GreyVectorFile_Decoder_ReadHeader(decoder);
	if (nRet < 0)
		return nRet;
	nDataSize = decoder->gbInfoHeader.gbiHoriOffTabOff
			  - decoder->gbInfoHeader.gbiWidthTabOff;
	decoder->gbWidthTable =(GB_BYTE*)GreyBit_Malloc(decoder->gbMem,nDataSize);
	GreyBit_Stream_Seek(decoder->gbStream,decoder->gbInfoHeader.gbiWidthTabOff
					  + decoder->gbOffDataBits);
	GreyBit_Stream_Read(decoder->gbStream, decoder->gbWidthTable, nDataSize);
	nDataSizea = decoder->gbInfoHeader.gbiOffsetTabOff
			   - decoder->gbInfoHeader.gbiHoriOffTabOff;
	decoder->gbHoriOffTable =(GB_INT8 *)GreyBit_Malloc
										(decoder->gbMem,nDataSizea);
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
	GreyBit_Stream_Read(decoder->gbStream,
						(GB_BYTE*)decoder->gbOffsetTable,nDataSizeb);
	return GB_SUCCESS;
}

/*
**----------------------------------------------------------------------------
**  Function(internal/external use) Declarations
**----------------------------------------------------------------------------
*/

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_New
** Description: Initialize decoder handle
** Input: decoder - decoder
** 	      stream - stream
** Output: Init'ed decoder
** Return value: decoder
** ---------------------------------------------------------------------------
*/

GB_Decoder	GreyVectorFile_Decoder_New(GB_Loader loader, GB_Stream stream)
{
	GVF_Decoder	decoder;

	decoder = (GVF_Decoder)GreyBit_Malloc(loader->gbMem,
										  sizeof(GVF_DecoderRec));
	if (decoder)
	{
		decoder->gbDecoder.setparam = GreyVectorFile_Decoder_SetParam;
		decoder->gbDecoder.getcount = GreyVectorFile_Decoder_GetCount;
		decoder->gbDecoder.getwidth = GreyVectorFile_Decoder_GetWidth;
		decoder->gbDecoder.getheight = GreyVectorFile_Decoder_GetHeight;
		decoder->gbDecoder.getadvance = GreyVectorFile_Decoder_GetAdvance;
		decoder->gbDecoder.decode = GreyVectorFile_Decoder_Decode;
		decoder->gbDecoder.done = GreyVectorFile_Decoder_Done;
		decoder->gbLibrary = loader->gbLibrary;
		decoder->gbMem = loader->gbMem;
		decoder->gbStream = stream;
		decoder->nCacheItem = 0;
		decoder->nItemCount = 0;
		decoder->gbOffDataBits = sizeof(GREYVECTORFILEHEADER)
							   + sizeof(GREYVECTORINFOHEADER);
		GreyVectorFile_Decoder_Init(decoder);
	}
	return (GB_Decoder)decoder;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_SetParam
** Description: Set decoder param
** Input: decoder - decoder
**        nParam - param type
**        dwParam - param value
** Output: Set param
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_SetParam(GB_Decoder decoder,GB_Param nParam,
											GB_UINT32 dwParam)
{
	GVF_Decoder	me = (GVF_Decoder)decoder;

	if (dwParam)
	{
		if (nParam == GB_PARAM_CACHEITEM)
		{
			if (me->gpGreyBits)
				return GB_FAILED;
			me->nCacheItem = dwParam;
			me->gpGreyBits = (GB_Outline*)GreyBit_Malloc(me->gbMem,
														 4 * me->nCacheItem);
			me->nGreyBitsCount = 0;
		}
	}
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_GetCount
** Description: Get character count
** Input: decoder - decoder
** Output: Character count
** Return value: decoder->nItemCount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_GetCount(GB_Decoder decoder)
{
	GVF_Decoder	me = (GVF_Decoder)decoder;

	return me->nItemCount;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_GetHeight
** Description: Get character height
** Input: decoder - decoder
** Output: Height
** Return value: decoder->gbInfoHeader.gbiHeight
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_GetHeight(GB_Decoder decoder)
{
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_GetWidth
** Description: Get character width
** Input: decoder - decoder
**		  nCode - unicode code
**		  nSize - character size
** Output: Height
** Return value: nSize * nWidth / decoder->gbInfoHeader.gbiHeight
**             / decoder->gbWidthTable[nCode - nMinCode + WidthIdx]
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_GetWidth(GB_Decoder decoder,
											GB_UINT32 nCode, GB_INT16 nSize)
{
	GB_BYTE		nWidth;
	GB_UINT16	nMinCode;
	GB_INT32	UniIndex;
	GB_INT32	WidthIdx;
	GVF_Decoder	me = (GVF_Decoder)decoder;

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

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_GetHoriOff
** Description: Get character horizontal offset
** Input: decoder - decoder
**		  nCode - unicode code
**		  nSize - character size
** Output: Font char horioff
** Return value: nSize * nHoriOff / decoder->gbInfoHeader.gbiHeight
** ---------------------------------------------------------------------------
*/

GB_INT16	GreyVectorFile_Decoder_GetHoriOff(GB_Decoder decoder,
											  GB_UINT32 nCode,
											  GB_INT16 nSize)
{
	GB_INT8		nHoriOff;
	GB_UINT16	nMinCode;
	GB_INT32	UniIndex;
	GB_INT32	HoriOffIdx;
	GVF_Decoder	me;

	me = (GVF_Decoder)decoder;
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
** Function: GreyVectorFile_Decoder_GetAdvance
** Description: Get character advance
** Input: decoder - decoder
**		  nCode - unicode code
**		  nSize - character size
** Output: Font char advance
** Return value: nAdvance
** ---------------------------------------------------------------------------
*/

GB_INT16	GreyVectorFile_Decoder_GetAdvance(GB_Decoder decoder,
											  GB_UINT32 nCode,
										      GB_INT16 nSize)
{
	GB_INT16	nWidth;
	GB_INT16	nAdvance;

	nWidth = (GB_INT16)GreyVectorFile_Decoder_GetWidth(decoder, nCode, nSize);
	nAdvance =GreyVectorFile_Decoder_GetHoriOff(decoder, nCode, nSize)+nWidth;
	if (nAdvance <= 0)
		return 0;
	else
		return nAdvance;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_Decode
** Description: Decode a character
** Input: decoder - decoder
**		  nCode - unicode code
**	      pData - data buffer
**		  nSize - character size
** Output: Decided character
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVectorFile_Decoder_Decode(GB_Decoder decoder, GB_UINT32 nCode,
										  GB_Data pData, GB_INT16 nSize)
{
	GB_UINT16	Lenght; 
	GB_INT32	nWidth;
	GB_INT32	nHoriOff;
	GB_Outline	outline; 
	GB_UINT32	Offset;
	GVF_Decoder	me = (GVF_Decoder)decoder;

	Offset = GreyVectorFile_Decoder_GetDataOffset(me, nCode);
	nWidth = GreyVectorFile_Decoder_GetWidth(decoder, nCode, nSize);
	nHoriOff = GreyVectorFile_Decoder_GetHoriOff(decoder, nCode, nSize);
	if (!nWidth)
		return GB_FAILED;
	if (!IS_INRAM(Offset))
	{
		GreyBit_Stream_Seek(me->gbStream, me->gbInfoHeader.gbiOffGreyBits
						  + me->gbOffDataBits + Offset);
		GreyBit_Stream_Read(me->gbStream, (GB_BYTE*)&Lenght, sizeof(Lenght));
		GreyBit_Stream_Read(me->gbStream, me->pBuff + sizeof(GVF_OutlineRec),
							Lenght);
		outline = GreyBitType_Outline_UpdateByGVF(me->gbOutline,
							GreyVector_Outline_FromData(me->pBuff));
		GreyVectorFile_Decoder_CaheItem(me, nCode, outline);
	}
	else
	{
		GET_INDEX(Offset);
		outline = me->gpGreyBits[Offset];
	}
	if (!outline)
		return GB_FAILED;
	GreyBitType_Outline_Transform(me->gbOutline, outline, nSize,
								  me->gbInfoHeader.gbiHeight);
	if (pData)
	{
		pData->format = GB_FORMAT_OUTLINE;
		pData->data = me->gbOutline;
		pData->width = (GB_INT16)nWidth;
		pData->horioff = (GB_INT16)nHoriOff;
	}
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Decoder_Done
** Description: Done with decoder? Nuke it!
** Input: decoder - decoder
** Output: Nuked decoder
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyVectorFile_Decoder_Done(GB_Decoder decoder)
{
	GVF_Decoder	me = (GVF_Decoder)decoder;

	GreyVectorFile_Decoder_ClearCache(me);
	GreyBit_Free(me->gbMem, decoder);
}

#endif //ENABLE_GREYVECTORFILE
/*
** ===========================================================================
** File: GreyBitFileEncoder.c
** Description: GreyBit font library - Combine font file encode
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

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../inc/GreyCombineFile.h"

#ifdef ENABLE_GREYCOMBINEFILE
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

GB_INT32	nCurrItemCount = 0;	/* Item count used during encode */
GB_UINT32	nOffset = 0;		/* Offset used during encode */

/*
**----------------------------------------------------------------------------
**  Function(internal use only) Declarations
**----------------------------------------------------------------------------
*/

/* externs */
extern GB_BOOL		GreyBitFile_Probe(GB_Stream stream);
extern GB_BOOL		GreyVectorFile_Probe(GB_Stream stream);

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Encoder_GetCount
** Description: Get font count
** Input: encoder - encoder
** Output: Font count
** Return value: nCurrItemCount
** ---------------------------------------------------------------------------
*/

GB_INT32 GreyCombineFile_Encoder_GetCount(GB_Encoder encoder)
{
	return nCurrItemCount;
}

/*
** ---------------------------------------------------------------------------
** Function: GreCombineFile_Encoder_SetParam
** Description: Set encoder param
** Input: encoder - encoder
**        nParam - param type
**        dwParam - param value
** Output: None
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Encoder_SetParam(GB_Encoder encoder,
										     GB_Param nParam,
											 GB_UINT32 dwParam)
{
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreCombineFile_Encoder_Delete
** Description: Remove font
** Input: encoder - encoder
**        nCode - unicode code
** Output: Removed font
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Encoder_Delete(GB_Encoder encoder,GB_UINT32 nCode)
{
	GCF_Encoder	me = (GCF_Encoder)encoder;

	nCurrItemCount--;
	me->gbFileHeader.gbfInfo[nCurrItemCount].gbiHeight = 0;
	me->gbFileHeader.gbfInfo[nCurrItemCount].gbiDataSize = 0;
	me->gbFileHeader.gbfInfo[nCurrItemCount].gbiDataOff = 0;
	me->gbCreator[nCurrItemCount] = 0;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Encoder_Encode
** Description: Add font to combined font
** Input: encoder - encoder
**        nCode - unicode code
**        pData - data buffer
** Output: Added font
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Encoder_Encode(GB_Encoder encoder,GB_UINT32 nCode,
										   GB_Data pData)
{
	GCF_Encoder	me = (GCF_Encoder)encoder;

	if (!pData || pData->format != GB_FORMAT_STREAM || pData->data)
		return GB_FAILED;
	if (!me->gbCreator[nCurrItemCount]->size)
		return GB_FAILED;
	if (GreyBitFile_Probe(me->gbCreator[nCurrItemCount]) != GB_TRUE
	|| GreyVectorFile_Probe(me->gbCreator[nCurrItemCount]) != GB_TRUE)
		return GB_FAILED;
	if (nCurrItemCount >= GCF_ITEM_MAX)
		return GB_FAILED;
	me->gbCreator[nCurrItemCount] = (GB_Stream)pData->data;
	me->gbFileHeader.gbfInfo[nCurrItemCount].gbiDataSize
										= me->gbCreator[nCurrItemCount]->size;
	me->gbFileHeader.gbfInfo[nCurrItemCount].gbiDataOff = nOffset;
	nOffset += me->gbFileHeader.gbfInfo[nCurrItemCount].gbiDataSize;
	nCurrItemCount++;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Encoder_WriteAll
** Description: Flush everything to stream
** Input: encoder - encoder
** Output: Finished product
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Encoder_WriteAll(GCF_Encoder encoder)
{
	GB_INT32	nCurrItem;
	GB_BYTE *	pTmp = 0;

	GreyBit_Stream_Seek(encoder->gbStream, 0);
	GreyBit_Stream_Write(encoder->gbStream, (GB_BYTE*)&encoder->gbFileHeader,
						 sizeof(GREYCOMBINEFILEHEADER));
	for (nCurrItem = 0; nCurrItem < GCF_ITEM_MAX; nCurrItem++)
	{
		if (encoder->gbFileHeader.gbfInfo[nCurrItem].gbiDataSize)
		{
			pTmp = (GB_BYTE*)GreyBit_Malloc(encoder->gbMem,
						encoder->gbFileHeader.gbfInfo[nCurrItem].gbiDataSize);
			GreyBit_Stream_Seek(encoder->gbCreator[nCurrItem], 0);
			GreyBit_Stream_Read(encoder->gbCreator[nCurrItem], pTmp,
						encoder->gbFileHeader.gbfInfo[nCurrItem].gbiDataSize);
			GreyBit_Stream_Write(encoder->gbStream, pTmp,
						encoder->gbFileHeader.gbfInfo[nCurrItem].gbiDataSize);
			GreyBit_Free(encoder->gbMem, pTmp);
		}
	}
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Encoder_BuildAll
** Description: Build everything pre-write
** Input: encoder - encoder
** Output: Initialised header
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Encoder_BuildAll(GCF_Encoder encoder)
{
	GB_INT32	nCurrItem = 0;

	nOffset = sizeof(GREYCOMBINEFILEHEADER);
	encoder->gbFileHeader.gbfTag[0] = 'g';
	encoder->gbFileHeader.gbfTag[1] = 'c';
	encoder->gbFileHeader.gbfTag[2] = 't';
	encoder->gbFileHeader.gbfTag[3] = 'f';
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Encoder_Flush
** Description: Flush encoder
** Input: encoder - encoder
** Output: Flushed encoder
** Return value: success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Encoder_Flush(GB_Encoder encoder)
{
	GCF_Encoder	me = (GCF_Encoder)encoder;

	GreyCombineFile_Encoder_BuildAll(me);
	GreyCombineFile_Encoder_WriteAll(me);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Encoder_Done
** Description: Done with encoder? Nuke it!
** Input: encoder - encoder
** Output: Nuked encoder
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyCombineFile_Encoder_Done(GB_Encoder encoder)
{
	GB_INT32	nCurrItem = 0;
	GCF_Encoder	me = (GCF_Encoder)encoder;

	GreyCombineFile_Encoder_Flush(encoder);
	for (nCurrItem = 0; nCurrItem < GCF_ITEM_MAX; nCurrItem++)
	{
		if (me->gbCreator[nCurrItem])
		{
			GreyBit_Stream_Done(me->gbCreator[nCurrItem]);
			me->gbCreator[nCurrItem] = 0;
		}
	}
	GreyBit_Free(me->gbMem, encoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Encoder_New
** Description: Create new encoder
** Input: creator - creator
**        stream - stream
** Output: New encoder
** Return value: codec
** ---------------------------------------------------------------------------
*/

GB_Encoder	GreyCombineFile_Encoder_New(GB_Creator creator, GB_Stream stream)
{
	GCF_Encoder	codec;

	codec = (GCF_Encoder)GreyBit_Malloc(creator->gbMem, sizeof(GCF_Encoder));
	if (codec)
	{
		codec->gbEncoder.getcount = GreyCombineFile_Encoder_GetCount;
		codec->gbEncoder.setparam = GreyCombineFile_Encoder_SetParam;
		codec->gbEncoder.remove = GreyCombineFile_Encoder_Delete;
		codec->gbEncoder.encode = GreyCombineFile_Encoder_Encode;
		codec->gbEncoder.flush = GreyCombineFile_Encoder_Flush;
		codec->gbEncoder.done = GreyCombineFile_Encoder_Done;
		codec->gbLibrary = creator->gbLibrary;
		codec->gbMem = creator->gbMem;
		codec->gbStream = stream;
		GB_MEMSET(codec->gbCreator, 0, sizeof(GB_Stream)*GCF_ITEM_MAX);
	}
	return (GB_Encoder)codec;
}
#endif //ENABLE_ENCODER
#endif //ENABLE_GREYCOMBINEFILE
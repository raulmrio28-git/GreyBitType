/*
** ===========================================================================
** File: GreyCombineFileDecoder.c
** Description: GreyBit font library - Combined font file decode
** Copyright (c) 2023
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

#include "../inc/UnicodeSection.h"
#include "../GreyBitSystem.h"
#include "../inc/GreyCombineFile.h"

#ifdef ENABLE_GREYCOMBINEFILE
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
**----------------------------------------------------------------------------
**  Function(internal/external use) Declarations
**----------------------------------------------------------------------------
*/

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Decoder_New
** Description: Initialize decoder handle
** Input: decoder - decoder
** 	      stream - stream
** Output: Init'ed decoder
** Return value: decoder
** ---------------------------------------------------------------------------
*/

GB_Decoder GreyCombineFile_Decoder_New(GB_Loader loader, GB_Stream stream)
{
	GCF_Decoder	decoder;
	GB_INT32	i = 0;

	decoder = (GCF_Decoder)GreyBit_Malloc(loader->gbMem,
										  sizeof(GCF_DecoderRec));
	if (decoder)
	{
		decoder->gbDecoder.setparam = GreyCombineFile_Decoder_SetParam;
		decoder->gbDecoder.getcount = GreyCombineFile_Decoder_GetCount;
		decoder->gbDecoder.getwidth = GreyCombineFile_Decoder_GetWidth;
		decoder->gbDecoder.getheight = GreyCombineFile_Decoder_GetHeight;
		decoder->gbDecoder.getadvance = GreyCombineFile_Decoder_GetAdvance;
		decoder->gbDecoder.decode = GreyCombineFile_Decoder_Decode;
		decoder->gbDecoder.done = GreyCombineFile_Decoder_Done;
		decoder->gbLibrary = loader->gbLibrary;
		decoder->gbMem = loader->gbMem;
		decoder->gbStream = stream;
		GB_MEMSET(decoder->gbLoader, 0, sizeof(GB_Loader)*GCF_ITEM_MAX);
		GreyBit_Stream_Seek(decoder->gbStream, 0);
		if (GreyBit_Stream_Read(decoder->gbStream,
								(GB_BYTE*)&decoder->gbFileHeader,
							    sizeof(GREYCOMBINEFILEHEADER))
							 == sizeof(GREYCOMBINEFILEHEADER))
		{
		  for (i = 0; i < GCF_ITEM_MAX; i++)
		  {
		    if (decoder->gbFileHeader.gbfInfo[i].gbiDataSize)
		    {
			  GreyBit_Stream_Offset(decoder->gbStream,
								  decoder->gbFileHeader.gbfInfo[i].gbiDataOff,
								  0);
			  decoder->gbLoader[i] = (GB_Loader)GreyBitType_Loader_New_Stream
												(decoder->gbLibrary,
												 decoder->gbStream,
								decoder->gbFileHeader.gbfInfo[i].gbiDataSize);
		    }
	      }
		}
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

GB_INT32	GreyCombineFile_Decoder_SetParam(GB_Decoder decoder,
											 GB_Param nParam,
											 GB_UINT32 dwParam)
{
	GB_INT32	nCurrItem;
	GB_INT32	nCurrCount = 0;
	GB_Loader	gbCurrLoader;
	GCF_Decoder	me = (GCF_Decoder)decoder;

	for (nCurrItem = 0; nCurrItem < GCF_ITEM_MAX; ++nCurrItem)
	{
		gbCurrLoader = me->gbLoader[nCurrItem];
		if (gbCurrLoader && GreyBit_Decoder_SetParam(gbCurrLoader->gbDecoder,
			nParam, dwParam)
			== GB_SUCCESS)
			return GB_SUCCESS;
	}
	return GB_FAILED;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Decoder_GetCount
** Description: Get character count
** Input: decoder - decoder
** Output: Character count
** Return value: nCurrCount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Decoder_GetCount(GB_Decoder decoder)
{
	GB_INT32	nCurrItem;
	GB_INT32	nCurrCount = 0;
	GB_Loader	gbCurrLoader;
	GCF_Decoder	me = (GCF_Decoder)decoder;

	for (nCurrItem = 0; nCurrItem < GCF_ITEM_MAX; ++nCurrItem)
	{
		gbCurrLoader = me->gbLoader[nCurrItem];
		if (gbCurrLoader)
			nCurrCount += GreyBit_Decoder_GetCount(gbCurrLoader->gbDecoder);
	}
	return nCurrCount;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Decoder_GetHeight
** Description: Get character height
** Input: decoder - decoder
** Output: 0
** Return value: 0, we don't know how here!
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Decoder_GetHeight(GB_Decoder decoder)
{
	return 0;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Decoder_GetWidth
** Description: Get character width
** Input: decoder - decoder
**        nCode - unicode code
**        nSize - character size
** Output: Character width
** Return value: decoder->getWidth
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Decoder_GetWidth(GB_Decoder decoder,
											 GB_UINT32 nCode,
											 GB_INT16 nSize)
{
	GB_INT32	nCurrItem;
	GB_Loader	gbCurrLoader;
	GCF_Decoder	me = (GCF_Decoder)decoder;

	for (nCurrItem = 0; nCurrItem < GCF_ITEM_MAX; ++nCurrItem)
	{
		gbCurrLoader = me->gbLoader[nCurrItem];
		if (gbCurrLoader
			&& GreyBitType_Loader_IsExist(me->gbLoader[nCurrItem], nCode))
			break;
	}
	return GreyBit_Decoder_GetWidth(gbCurrLoader->gbDecoder, nCode, nSize);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Decoder_GetAdvance
** Description: Get character advance
** Input: decoder - decoder
**		  nCode - unicode code
**		  nSize - character size
** Output: Font char advance
** Return value: GreyBit_Decoder_GetAdvance
** ---------------------------------------------------------------------------
*/

GB_INT16	GreyCombineFile_Decoder_GetAdvance(GB_Decoder decoder,
											   GB_UINT32 nCode,
											   GB_INT16 nSize)
{
	GB_INT32	nCurrItem;
	GB_Loader	gbCurrLoader;
	GCF_Decoder	me = (GCF_Decoder)decoder;

	for (nCurrItem = 0; nCurrItem < GCF_ITEM_MAX; ++nCurrItem)
	{
		gbCurrLoader = me->gbLoader[nCurrItem];
		if (gbCurrLoader
			&& GreyBitType_Loader_IsExist(me->gbLoader[nCurrItem],nCode))
			break;
	}
	return GreyBit_Decoder_GetAdvance(gbCurrLoader->gbDecoder, nCode, nSize);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Decoder_Decode
** Description: Decode a character
** Input: decoder - decoder
**		  nCode - unicode code
**	      pData - data buffer
**		  nSize - character size
** Output: Decoded character
** Return value: GreyBit_Decoder_Decode
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyCombineFile_Decoder_Decode(GB_Decoder decoder,GB_UINT32 nCode,
										   GB_Data pData, GB_INT16 nSize)
{
	GB_INT32	nCurrItem;
	GB_Loader	gbCurrLoader;
	GCF_Decoder	me = (GCF_Decoder)decoder;

	for (nCurrItem = 0; nCurrItem < GCF_ITEM_MAX; ++nCurrItem)
	{
		gbCurrLoader = me->gbLoader[nCurrItem];
		if (gbCurrLoader
			&& GreyBitType_Loader_IsExist(me->gbLoader[nCurrItem], nCode))
			break;
	}
	return GreyBit_Decoder_Decode(gbCurrLoader->gbDecoder, nCode, pData, nSize);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Decoder_Done
** Description: Done with decoder? Nuke it!
** Input: decoder - decoder
** Output: Nuked decoder
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyCombineFile_Decoder_Done(GB_Decoder decoder)
{
	GB_INT32	nCurrItem;
	GCF_Decoder	me = (GCF_Decoder)decoder;

	for (nCurrItem = 0; nCurrItem < GCF_ITEM_MAX; ++nCurrItem)
	{
		if (me->gbLoader[nCurrItem])
		{
			GreyBitType_Loader_Done(me->gbLoader[nCurrItem]);
			me->gbLoader[nCurrItem] = 0;
		}
	}
	GreyBit_Free(me->gbMem, decoder);
}
#endif //ENABLE_GREYCOMBINEFILE
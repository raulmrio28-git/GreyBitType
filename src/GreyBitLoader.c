/*
** ===========================================================================
** File: GreyBitLoader.c
** Description: GreyBit font library - Loader
** Copyright (c) 2023
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2023	me				Upgrade
** 08/08/2023	me              Init
** ===========================================================================
*/

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../GreyBitType.h"
#include "../inc/GreyBitType_Def.h"
#include "../inc/GreyBitCodec.h"

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

/* externs */
extern int			GreyBit_Format_Probe(GB_Format format, GB_Stream stream);
extern GB_Decoder	GreyBit_Format_DecoderNew(GB_Format format, 
											  GB_Loader loader,
											  GB_Stream stream);

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_Probe
** Description: Probe to-be-decoded font format
** Input: library - library
**        loader - loader
** Output: New decoder
** Return value: decoder
** ---------------------------------------------------------------------------
*/

GB_Decoder	GreyBitType_Loader_Probe(GB_Library library, GB_Loader loader)
{
	GB_Decoder	decoder;
	GB_Format	format;

	format = library->gbFormatHeader;
	decoder = 0;
	while (format)
	{
		if (GreyBit_Format_Probe(format, loader->gbStream))
			decoder = GreyBit_Format_DecoderNew(format, loader,
												loader->gbStream);
		format = format->next;
	}
	return decoder;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_New
** Description: Initialize loader handle
** Input: library - library
**        filepathname - filename of output font
** Output: New loader
** Return value: loader
** ---------------------------------------------------------------------------
*/


GBHANDLE	GreyBitType_Loader_New(GBHANDLE library,
								   const GB_CHAR * filepathname)
{
	GB_Library	me;
	GB_Loader	loader;

	me = (GB_Library)library;
	loader = (GB_Loader)GreyBit_Malloc(me->gbMem, sizeof(GB_LoaderRec));
	if (!loader)
		return loader;
	loader->gbLibrary = me;
	loader->gbMem = loader->gbLibrary->gbMem;
	loader->gbStream = (GB_Stream)GreyBit_Stream_New(filepathname, 0);
	if (loader->gbStream)
		loader->gbDecoder=GreyBitType_Loader_Probe(loader->gbLibrary,loader);
	if (loader->gbStream && loader->gbDecoder)
		return loader;
	GreyBitType_Loader_Done(loader);
	return 0;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_New_Stream
** Description: Initialize loader handle from stream
** Input: library - library
**        stream - stream
**        size - size
** Output: New loader
** Return value: loader
** ---------------------------------------------------------------------------
*/

GBHANDLE    GreyBitType_Loader_New_Stream(GBHANDLE library, GBHANDLE stream,
										  GB_INT32 size)
{
	GB_Library	me;
	GB_Loader	loader;

	me = (GB_Library)library;
	loader = (GB_Loader)GreyBit_Malloc(me->gbMem, sizeof(GB_LoaderRec));
	if (!loader)
		return loader;
	loader->gbLibrary = me;
	loader->gbMem = loader->gbLibrary->gbMem;
	loader->gbStream = (GB_Stream)GreyBit_Stream_New_Child(stream);
	if (loader->gbStream)
		GreyBit_Stream_Offset(loader->gbStream, 0, size);
		loader->gbDecoder = GreyBitType_Loader_Probe(loader->gbLibrary,
													 loader);
	if (loader->gbStream && loader->gbDecoder)
		return loader;
	GreyBitType_Loader_Done(loader);
	return 0;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_New_Memory
** Description: Initialize loader handle from memory
** Input: library - library
**        pBuf - font in buffer
**        nBufSize - font in buffer size
** Output: New loader
** Return value: loader
** ---------------------------------------------------------------------------
*/

GBHANDLE	GreyBitType_Loader_New_Memory(GBHANDLE library, void *pBuf,
										  GB_INT32 nBufSize)
{
	GB_Library	me;
	GB_Loader	loader;

	me = library;
	loader = (GB_Loader)GreyBit_Malloc(me->gbMem, sizeof(GB_LoaderRec));
	if (!loader)
		return loader;
	loader->gbLibrary = me;
	loader->gbMem = loader->gbLibrary->gbMem;
	loader->gbStream = (GB_Stream)GreyBit_Stream_New_Memory(pBuf, nBufSize);
	if (loader->gbStream)
		loader->gbDecoder = GreyBitType_Loader_Probe(loader->gbLibrary,
													 loader);
	if (loader->gbStream && loader->gbDecoder)
		return loader;
	GreyBitType_Loader_Done(loader);
	return 0;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_SetParam
** Description: Set loader param
** Input: loader - loader
**        nParam - param type
**        dwParam - param value
** Output: Set param
** Return value: GreyBit_Decoder_SetParam
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Loader_SetParam(GBHANDLE loader, GB_Param nParam,
										GB_UINT32 dwParam)
{
	GB_Loader	me = (GB_Loader)loader;

	return GreyBit_Decoder_SetParam(me->gbDecoder, nParam, dwParam);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_GetCoun
** Description: Get character count
** Input: loader - loader
** Output: Set param
** Return value: GreyBit_Decoder_GetCount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitType_Loader_GetCount(GBHANDLE loader)
{
	GB_Loader	me = (GB_Loader)loader;

	return GreyBit_Decoder_GetCount(me->gbDecoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_GetHeight
** Description: Get font height
** Input: loader - loader
** Output: Font height
** Return value: GreyBit_Decoder_GetHeight
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitType_Loader_GetHeight(GBHANDLE loader)
{
	GB_Loader	me = (GB_Loader)loader;

	return GreyBit_Decoder_GetHeight(me->gbDecoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_IsExist
** Description: Does character exist?
** Input: loader - loader	  
**        nCode - character code
** Output: true/false
** Return value: GreyBit_Decoder_GetWidth(loader->gbDecoder, nCode, 100) != 0
** ---------------------------------------------------------------------------
*/

GB_BOOL		GreyBitType_Loader_IsExist(GBHANDLE loader, GB_UINT32 nCode)
{
	GB_Loader	me = (GB_Loader)loader;

	return GreyBit_Decoder_GetWidth(me->gbDecoder, nCode, 100) != 0;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Loader_Done
** Description: Done with loader? Nuke it!
** Input: loader - loader
** Output: Nuked loader
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitType_Loader_Done(GBHANDLE loader)
{
	GB_Loader	me = (GB_Loader)loader;

	if (me->gbDecoder)
		GreyBit_Decoder_Done(me->gbDecoder);
	if (me->gbStream)
		GreyBit_Stream_Done(me->gbStream);
	GreyBit_Free(me->gbMem, loader);
}
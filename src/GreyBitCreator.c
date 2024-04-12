/*
** ===========================================================================
** File: GreyBitCreator.c
** Description: GreyBit font library - Creator
** Copyright (c) 2023
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2023	me				Upgrade
** 08/10/2023	me				Add feature as to compile encoder stuff only 
**								if encoder is supported
** 08/09/2023	me              Init
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

/* externs */
extern	int			GreyBit_Format_Probe(GB_Format format, GB_Stream stream);
extern	GB_Encoder	GreyBit_Format_EncoderNew(GB_Format format,
											  GB_Creator creator,
											  GB_Stream stream);

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Creator_Probe
** Description: Probe to-be-encoded font format
** Input: library - library
**        creator - creator
** Output: New encoder
** Return value: encoder
** ---------------------------------------------------------------------------
*/

GB_Encoder	GreyBitType_Creator_Probe(GB_Library library, GB_Creator creator)
{
	GB_BYTE *	pExt;
	GB_Encoder	encoder;
	GB_Format	format;

	format = library->gbFormatHeader;
	encoder = 0;
	while (format)
	{
		if (GreyBit_Format_Probe(format, creator->gbStream))
			return GreyBit_Format_EncoderNew(format, creator,
											 creator->gbStream);
		pExt = (GB_BYTE *)GB_STRRCHR(creator->gbStream->pfilename, '.');
		if (pExt)
		{
			if (!GB_STRCMP(pExt + 1, format->tag))
			  return (GB_Encoder)GreyBit_Format_EncoderNew(format, creator,
														   creator->gbStream);
		}
		format = format->next;
	}
	return encoder;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Creator_New
** Description: Initialize creator handle
** Input: library - library
**        filepathname - filename of output font
** Output: New creator
** Return value: creator
** ---------------------------------------------------------------------------
*/

GBHANDLE	GreyBitType_Creator_New(GBHANDLE library,
								    const GB_CHAR* filepathname)
{
	GB_Library	me;
	GB_Creator	creator;

	me = (GB_Library)library;
	creator = (GB_Creator)GreyBit_Malloc(me->gbMem, sizeof(GB_Creator));
	if (!creator)
		return creator;
	creator->gbLibrary = me;
	creator->gbMem = me->gbMem;
	creator->gbStream = (GB_Stream)GreyBit_Stream_New(filepathname, 1);
	if (creator->gbStream)
		creator->gbEncoder = GreyBitType_Creator_Probe(me, creator);
	if (creator->gbStream && creator->gbEncoder)
		return creator;
	GreyBitType_Creator_Done(creator);
	return 0;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Creator_New_Memory
** Description: Initialize creator handle from membuffer
** Input: library - library
**        pBuf - font out buffer
**        nBufSize - font out buffer size
** Output: New creator
** Return value: creator
** ---------------------------------------------------------------------------
*/

GBHANDLE	GreyBitType_Creator_New_Memory(GBHANDLE library, void *pBuf,
										   GB_INT32 nBufSize)
{
	GB_Library	me;
	GB_Creator	creator;

	me = (GB_Library)library;
	creator = (GB_Creator)GreyBit_Malloc(me->gbMem, sizeof(GB_Creator));
	if (!creator)
		return creator;
	creator->gbLibrary = me;
	creator->gbMem = me->gbMem;
	creator->gbStream = (GB_Stream)GreyBit_Stream_New_Memory(pBuf, nBufSize);
	if (creator->gbStream)
		creator->gbEncoder = GreyBitType_Creator_Probe(creator->gbLibrary,
													   creator);
	if (creator->gbStream && creator->gbEncoder)
		return creator;
	GreyBitType_Creator_Done(creator);
	return 0;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Creator_SetParam
** Description: Set creator param
** Input: creator - creator
**        nParam - param type
**        dwParam - param value
** Output: Set param
** Return value: GreyBit_Encoder_SetParam
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Creator_SetParam(GBHANDLE creator, GB_Param nParam,
									     GB_UINT32 dwParam)
{
	GB_Creator	me = (GB_Creator)creator;

	return GreyBit_Encoder_SetParam(me->gbEncoder, nParam, dwParam);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Creator_DelChar
** Description: Remove character
** Input: creator - creator
**        nCode - Unicode code
** Output: Removed character
** Return value: GreyBit_Encoder_Delete
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Creator_DelChar(GBHANDLE creator, GB_UINT32 nCode)
{
	GB_Creator	me = (GB_Creator)creator;

	return GreyBit_Encoder_Delete(me->gbEncoder, nCode);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Creator_SaveChar
** Description: Encode character
** Input: creator - creator
**        nCode - Unicode code
**        pData - character data
** Output: Encoded character
** Return value: GreyBit_Encoder_Encode
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Creator_SaveChar(GBHANDLE creator, GB_UINT32 nCode,
									     GB_Data pData)
{
	GB_Creator	me = (GB_Creator)creator;

	return GreyBit_Encoder_Encode(me->gbEncoder, nCode, pData);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Creator_Flush
** Description: Flush encoder
** Input: creator - creator
** Output: Flushed encoder
** Return value: GreyBit_Encoder_Flush
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Creator_Flush(GBHANDLE creator)
{
	GB_Creator	me = (GB_Creator)creator;

	return GreyBit_Encoder_Flush(me->gbEncoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Creator_Done
** Description: Done with creator? Nuke it!
** Input: creator - creator
** Output: Nuked creator
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitType_Creator_Done(GBHANDLE creator)
{
	GB_Creator	me = (GB_Creator)creator;

	if (me->gbEncoder)
		GreyBit_Encoder_Done(me->gbEncoder);
	if (me->gbStream)
		GreyBit_Stream_Done(me->gbStream);
	GreyBit_Free(me->gbMem, creator);
}
#endif //ENABLE_ENCODER
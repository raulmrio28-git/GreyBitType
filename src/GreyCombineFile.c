/*
** ===========================================================================
** File: GreyCombineFileDecoder.c
** Description: GreyBit font library - Combined font decode
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
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Probe
** Description: Check if format is gctf
** Input: stream - stream
** Output: true if valid
** Return value: fileHeader = "gctf"
** ---------------------------------------------------------------------------
*/

GB_BOOL		GreyCombineFile_Probe(GB_Stream stream)
{
	GREYCOMBINEFILEHEADER	fileHeader;

	GreyBit_Stream_Seek(stream, 0);
	GreyBit_Stream_Read(stream, (GB_BYTE *)&fileHeader,
						sizeof(GREYCOMBINEFILEHEADER));
	return fileHeader.gbfTag[0] == 'g' && fileHeader.gbfTag[1] == 'c'
		&& fileHeader.gbfTag[2] == 't' && fileHeader.gbfTag[3] == 'f';
}

/*
** ---------------------------------------------------------------------------
** Function: GreyCombineFile_Format_New
** Description: Create combine format
** Input: library - library
** Output: Init'ed format
** Return value: format
** ---------------------------------------------------------------------------
*/


GB_Format	GreyCombineFile_Format_New(GB_Library library)
{
	GB_Format	format;

	format = (GB_Format)GreyBit_Malloc(library->gbMem, sizeof(GB_FormatRec));
	if (format)
	{
		format->next = 0;
		format->tag[0] = 'g';
		format->tag[1] = 'c';
		format->tag[2] = 'f';
		format->probe = GreyCombineFile_Probe;
		format->decodernew = GreyCombineFile_Decoder_New;
#ifdef ENABLE_ENCODER
		format->encodernew = GreyCombineFile_Encoder_New;
#endif //ENABLE_ENCODER
	}
	return format;
}
#endif //ENABLE_GREYCOMBINEFILE
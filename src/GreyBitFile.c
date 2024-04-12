/*
** ===========================================================================
** File: GreyBitFile.c
** Description: GreyBit font library - Bitmap font file handling
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
** Function: GreyBitFile_Probe
** Description: Check if format is gbtf
** Input: stream - stream
** Output: true if valid
** Return value: fileHeader = "gbtf"
** ---------------------------------------------------------------------------
*/

GB_BOOL		GreyBitFile_Probe(GB_Stream stream)
{
	GREYBITFILEHEADER	fileHeader;

	GreyBit_Stream_Seek(stream, 0);
	GreyBit_Stream_Read(stream, (GB_BYTE *)&fileHeader,
						sizeof(GREYBITFILEHEADER));
	return fileHeader.gbfTag[0] == 'g' && fileHeader.gbfTag[1] == 'b'
		&& fileHeader.gbfTag[2] == 't' && fileHeader.gbfTag[3] == 'f';
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitFile_Format_New
** Description: Create bitmap format
** Input: library - library
** Output: Init'ed format
** Return value: format
** ---------------------------------------------------------------------------
*/

GB_Format	GreyBitFile_Format_New(GB_Library library)
{
	GB_Format	format;

	format = (GB_Format)GreyBit_Malloc(library->gbMem, sizeof(GB_FormatRec));
	if (format)
	{
		format->next = 0;
		format->tag[0] = 'g';
		format->tag[1] = 'b';
		format->tag[2] = 'f';
		format->probe = GreyBitFile_Probe;
		format->decodernew = GreyBitFile_Decoder_New;
#ifdef ENCODER_SUPPORT
		format->encodernew = GreyBitFile_Encoder_New;
#endif //ENCODER_SUPPORT
	}
	return format;
}
#endif //ENABLE_GREYBITFILE
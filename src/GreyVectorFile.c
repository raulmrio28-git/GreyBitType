/*
** ===========================================================================
** File: GreyBitFile.c
** Description: GreyBit font library - Vector font file handling
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
** Function: GreyVectorFile_Probe
** Description: Check if format is gvtf
** Input: stream - stream
** Output: true if valid
** Return value: fileHeader = "gvtf"
** ---------------------------------------------------------------------------
*/

GB_BOOL     GreyVectorFile_Probe(GB_Stream stream)
{
  GREYVECTORFILEHEADER  fileHeader; 
  GreyBit_Stream_Seek(stream, 0);
  GreyBit_Stream_Read(stream, (GB_BYTE *)&fileHeader,
                      sizeof(GREYVECTORFILEHEADER));
  return fileHeader.gbfTag[0] == 'g' && fileHeader.gbfTag[1] == 'v'
      && fileHeader.gbfTag[2] == 't' && fileHeader.gbfTag[3] == 'f';
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVectorFile_Format_New
** Description: Create vector format
** Input: library - library
** Output: Init'ed format
** Return value: format
** ---------------------------------------------------------------------------
*/

GB_Format   GreyVectorFile_Format_New(GB_Library library)
{
  GB_Format format;

  format = (GB_Format)GreyBit_Malloc(library->gbMem,
                                     sizeof(GB_FormatRec));
  if ( format )
  {
    format->next = 0;
    format->tag[0] = 'g';
    format->tag[1] = 'v';
    format->tag[2] = 'f';
    format->probe = GreyVectorFile_Probe;
    format->decodernew = GreyVectorFile_Decoder_New;
#ifdef ENCODER_SUPPORT
	format->encodernew = GreyVectorFile_Encoder_New;
#endif //ENCODER_SUPPORT
  }
  return format;
}
#endif //ENABLE_GREYVECTORFILE
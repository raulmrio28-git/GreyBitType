/*
** ===========================================================================
** File: GreyBitType.c
** Description: GreyBit font library - Type-specific functions
** Copyright (c) 2023-2024
** Portions of this software (c) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 03/27/2024	me              Fix bitmap free fail, so allocate bitmap data
**                              separately
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
#include "../GreyBitSystem.h"
#include "../inc/GreyBitType_Def.h"
#ifdef ENABLE_GREYCOMBINEFILE
#include "../inc/GreyCombineFile.h"
#endif //ENABLE_GREYCOMBINEFILE
#ifdef ENABLE_GREYBITFILE
#include "../inc/GreyBitFile.h"
#endif //ENABLE_GREYBITFILE
#ifdef ENABLE_GREYVECTORFILE
#include "../inc/GreyVectorFile.h"
#endif //ENABLE_GREYVECTORFILE

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
extern void		GreyBit_Close_Sys(GB_IOHandler f);
extern void *	GreyBit_Malloc_Sys(GB_INT32 size);
extern void *	GreyBit_Realloc_Sys(void * p, GB_INT32 newsize);
extern void		GreyBit_Free_Sys(void * p);

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Format_Insert
** Description: Insert format header
** Input: library - library
**        format - format
** Output: Inserted format
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitType_Format_Insert(GB_Library library, GB_Format format)
{
	GB_Format	curr;

	if (format)
	{
		curr = library->gbFormatHeader;
		library->gbFormatHeader = format;
		format->next = curr;
	}
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Format_Init
** Description: Initialize every format possible
** Input: library - library
** Output: Init'ed formats
** Return value: library->gbFormatHeader
** ---------------------------------------------------------------------------
*/

GB_Format	GreyBitType_Format_Init(GB_Library library)
{
	GB_Format	format;

#ifdef ENABLE_GREYCOMBINEFILE
	format = GreyCombineFile_Format_New(library);
	GreyBitType_Format_Insert(library, format);
#endif //ENABLE_GREYCOMBINEFILE
#ifdef ENABLE_GREYBITFILE
	format = GreyBitFile_Format_New(library);
	GreyBitType_Format_Insert(library, format);
#endif //ENABLE_GREYBITFILE
#ifdef ENABLE_GREYVECTORFILE
	format = GreyVectorFile_Format_New(library);
	GreyBitType_Format_Insert(library, format);
#endif //ENABLE_GREYVECTORFILE
	return library->gbFormatHeader;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Format_Done
** Description: Done with format? Nuke it!
** Input: format - format
** Output: Nuked format
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBit_Format_Done(GB_Format format, GB_Memory mem)
{
	GreyBit_Free(mem, format);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Format_Done
** Description: Done with all formats? Nuke them!
** Input: library - library
** Output: Nuked formats
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitType_Format_Done(GB_Library library)
{
	GB_Format	next; 

	while (library->gbFormatHeader)
	{
		next = library->gbFormatHeader->next;
		GreyBit_Format_Done(library->gbFormatHeader, library->gbMem);
		library->gbFormatHeader = next;
	}
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Bitmap_New
** Description: Create bitmap handle
** Input: library - library
**        nWidth - width
**        nHeight - height
**        bitcount - bitcount
**        pInitBuf - buffer
** Output: New bitmap
** Return value: bitmap
** ---------------------------------------------------------------------------
*/

GB_Bitmap	GreyBitType_Bitmap_New(GBHANDLE library, GB_INT16 nWidth,
								  GB_INT16 nHeight, GB_INT16 bitcount,
								  GB_BYTE * pInitBuf)
{
	GB_INT32	nBufSize;
	GB_Bitmap	bitmap;
	GB_INT16	nCurrPitch;
	GB_Library	me = (GB_Library)library;

	nCurrPitch = (bitcount * 8 * nWidth + 63) >> 6;
	nBufSize = nHeight * nCurrPitch;
	bitmap = (GB_Bitmap)GreyBit_Malloc(me->gbMem, sizeof(GB_BitmapRec));
	if (bitmap)
	{
		bitmap->width = nWidth;
		bitmap->height = nHeight;
		bitmap->bitcount = bitcount;
		bitmap->pitch = nCurrPitch;
		bitmap->buffer = (GB_BYTE *)GreyBit_Malloc(me->gbMem, nBufSize);
		if (!bitmap->buffer)
		{
			GreyBit_Free(me->gbMem, bitmap);
			return 0;
		}
		if (pInitBuf)
			GB_MEMCPY(bitmap->buffer, pInitBuf, nBufSize);
	}
	return bitmap;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Bitmap_Done
** Description: Done with bitmap? Nuke it!
** Input: library - library
**        bitmap - bitmap
** Output: Nuked bitmap
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitType_Bitmap_Done(GBHANDLE library, GB_Bitmap bitmap)
{
	GB_Library	me = (GB_Library)library;

	GreyBit_Free(me->gbMem, bitmap->buffer);
	GreyBit_Free(me->gbMem, bitmap);
}

#ifdef ENABLE_GREYVECTORFILE
/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Outline_New
** Description: Create outline handle
** Input: library - library
**        n_contours - contours
**        n_points - points
** Output: New outline
** Return value: outline
** ---------------------------------------------------------------------------
*/

GB_Outline	GreyBitType_Outline_New(GBHANDLE library, GB_INT16 n_contours,
								   GB_INT16 n_points)
{
	GB_Outline	outline; 
	GB_Library	me = (GB_Library)library;

	outline = (GB_Outline)GreyBit_Malloc(me->gbMem, sizeof(GB_OutlineRec)
			+ sizeof(GB_INT16) * n_contours + sizeof(GB_Point) * n_points
			+ n_points);
	if (outline)
	{
		outline->n_contours = n_contours;
		outline->n_points = n_points;
		outline->contours = (GB_INT16*)((GB_BYTE*)outline 
						  + sizeof(GB_OutlineRec));
		outline->points = (GB_Point)(outline->contours + n_contours);
		outline->tags = (GB_BYTE*)(outline->points + n_points);
	}
	return outline;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Outline_Clone
** Description: Clone outlinw
** Input: library - library
**        source - source outline
** Output: Cloned outline
** Return value: outline
** ---------------------------------------------------------------------------
*/

GB_Outline	GreyBitType_Outline_Clone(GBHANDLE library, GB_Outline source)
{
	int			i; 
	int			ia;
	GB_Outline	outline;

	outline = GreyBitType_Outline_New(library, source->n_contours,
									  source->n_points);
	if (outline)
	{
		for (i = 0; i < outline->n_contours; ++i)
			outline->contours[i] = source->contours[i];
		for (ia = 0; ia < outline->n_points; ++ia)
		{
			outline->points[ia] = source->points[ia];
			outline->tags[ia] = source->tags[ia];
		}
	}
	return outline;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Outline_Remove
** Description: Remove an outline
** Input: outline - outline
**        idx - index
** Output: Removed outline
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Outline_Remove(GB_Outline outline, GB_INT16 idx)
{
	GB_INT16	curr; 
	GB_INT16	curra;
	int			idxinc; 
	GB_INT16	inc;
	int			i; 
	int			ia; 

	inc = 0;
	idxinc = 0;
	if (idx > outline->n_points || idx < 0)
		return GB_FAILED;
	for (i = 0; i < outline->n_contours; ++i)
	{
		curr = outline->contours[i];
		if (curr >= idx)
			inc = 1;
		curra = curr - inc;
		if (curra >= 0)
			outline->contours[i - idxinc] = curra;
		else
			++idxinc;
	}
	outline->n_contours -= idxinc;
	--outline->n_points;
	for (ia = idx; ia < outline->n_points; ++ia)
	{
		outline->points[ia] = outline->points[ia + 1];
		outline->tags[ia] = outline->tags[ia + 1];
	}
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Outline_GetSizeEx
** Description: Get size of outline buffer
** Input: n_contours - contours
**        n_points - points
** Output: Size of whole outline buffer
** Return value: sizeof(GB_OutlineRec) + sizeof(GB_INT16) * n_contours
**               + sizeof(GB_Point) * n_points + n_points
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitType_Outline_GetSizeEx(GB_INT16 n_contours,
										  GB_INT16 n_points)
{
	return sizeof(GB_OutlineRec) + sizeof(GB_INT16) * n_contours
		 + sizeof(GB_Point) * n_points + n_points;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Outline_GetSize
** Description: Get size of outline buffer
** Input: outline - outline
** Output: Size of whole outline buffer
** Return value: GreyBitType_Outline_GetSizeEx
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitType_Outline_GetSize(GB_Outline outline)
{
	return GreyBitType_Outline_GetSizeEx(outline->n_contours,
										 outline->n_points);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Outline_Transform
** Description: Modify size of outline
** Input: library - library
**        outline - outline
**        tosize - new size
**        fromsize - old size
** Output: Transformed outline
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Outline_Transform(GB_Outline outline,
										  GB_Outline source, GB_INT16 tosize,
										  GB_INT16 fromsize)
{
	int	i; 
	int	ia; 

	outline->n_contours = source->n_contours;
	outline->n_points = source->n_points;
	for (i = 0; i < outline->n_contours; ++i)
		outline->contours[i] = source->contours[i];
	for (ia = 0; ia < outline->n_points; ++ia)
	{
		outline->points[ia].x = tosize * source->points[ia].x / fromsize;
		outline->points[ia].y = tosize * source->points[ia].y / fromsize;
		outline->tags[ia] = source->tags[ia];
	}
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Outline_Done
** Description: Done with outline? Nuke it!
** Input: library - library
**        outline - outline
** Output: Nuked outline
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitType_Outline_Done(GBHANDLE library, GB_Outline outline)
{
	GB_Library	me = (GB_Library)library;

	GreyBit_Free(me->gbMem, outline);
}
#endif //ENABLE_GREYVECTORFILE

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Format_DecoderNew
** Description: Create new decoder
** Input: format - format
**        loader - loader
**        stream - stream
** Output: New decoder
** Return value: format->decodernew
** ---------------------------------------------------------------------------
*/

GB_Decoder	GreyBit_Format_DecoderNew(GB_Format format, GB_Loader loader,
									  GB_Stream stream)
{
	if (format->decodernew)
		return format->decodernew(loader, stream);
	else
		return 0;
}

#ifdef ENABLE_ENCODER
/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Format_EncoderNew
** Description: Create new decoder
** Input: format - format
**        creator - creator
**        stream - stream
** Output: New encoder
** Return value: format->encodernew
** ---------------------------------------------------------------------------
*/

GB_Encoder	GreyBit_Format_EncoderNew(GB_Format format, GB_Creator creator,
									  GB_Stream stream)
{
	if (format->encodernew)
		return format->encodernew(creator, stream);
	else
		return 0;
}
#endif // ENABLE_ENCODER

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Format_Probe
** Description: Probe format
** Input: format - format
**        stream - stream
** Output: Result of probe
** Return value: format->probe
** ---------------------------------------------------------------------------
*/

int			GreyBit_Format_Probe(GB_Format format, GB_Stream stream)
{
	int	result;

	if (format->probe)
		result = format->probe(stream);
	else
		result = 0;
	return result;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Init
** Description: Initialize library
** Input: none
** Output: Init'ed library
** Return value: gbLib
** ---------------------------------------------------------------------------
*/

GBHANDLE	GreyBitType_Init(void)
{
	GB_Library	gbLib;

	gbLib = (GB_Library)GreyBit_Malloc_Sys(sizeof(GB_LibraryRec));
	if (gbLib)
	{
		gbLib->gbMem = GreyBit_Memory_New();
		gbLib->gbFormatHeader = GreyBitType_Format_Init(gbLib);
	}
	return gbLib;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Done
** Description: Done with using library? Nuke it!
** Input: library - library
** Output: Nuked library
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitType_Done(GBHANDLE library)
{
	GB_Library	me = (GB_Library)library;

	GreyBitType_Format_Done((GB_Library)library);
	GreyBit_Memory_Done(me->gbMem);
	GreyBit_Free_Sys(library);
}
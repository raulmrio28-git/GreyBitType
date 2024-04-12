/*
** ===========================================================================
** File: GreyBitFile.c
** Description: GreyBit font library - Vector font common functions
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

#include "../GreyBitType.h"
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
** Function: GreyVector_Outline_New
** Description: Create outline
** Input: library - library
**        n_countours - contours
**        n_points - points
** Output: Init'ed outline
** Return value: outline
** ---------------------------------------------------------------------------
*/

GVF_Outline	GreyVector_Outline_New(GB_Library library, GB_INT16 n_contours,
								   GB_INT16 n_points)
{
	GVF_Outline	outline;

	outline = (GVF_Outline)GreyBit_Malloc(library->gbMem, n_contours
										+ sizeof(GVF_PointRec) * n_points
										+ sizeof(GVF_OutlineRec) + 2);
	if (outline)
	{
		outline->n_contours = (GB_BYTE)n_contours;
		outline->n_points = (GB_BYTE)n_points;
		outline[1].n_contours = (GB_BYTE)n_contours;
		outline[1].n_points = (GB_BYTE)n_points;
		outline->contours = &outline[1].n_points + 1;
		outline->points = (GVF_Point)(outline->contours + n_contours);
	}
	return outline;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVector_Outline_Clone
** Description: Clone outline
** Input: library - library
**        source - source outline
** Output: Cloned outline
** Return value: outline
** ---------------------------------------------------------------------------
*/

GVF_Outline	GreyVector_Outline_Clone(GB_Library library,
									 GVF_Outline source)
{
	int			i;
	int			ia; 
	GVF_Outline	outline; 

	outline = GreyVector_Outline_New(library, source->n_contours,
									 source->n_points);
	if (outline)
	{
		for (i = 0; i < outline->n_contours; ++i)
			outline->contours[i] = source->contours[i];
		for (ia = 0; ia < outline->n_contours; ++ia)
			outline->points[ia] = source->points[ia];
	}
	return outline;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Outline_GetSizeEx
** Description: Get size of outline buffer
** Input: n_contours - contours
**        n_points - points
** Output: Size of whole outline buffer
** Return value: n_contours + sizeof(GVF_PointRec) * n_points
**               + sizeof(GVF_OutlineRec) + 2
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyVector_Outline_GetSizeEx(GB_BYTE n_contours, GB_BYTE n_points)
{
	return n_contours + sizeof(GVF_PointRec)
		 * n_points + sizeof(GVF_OutlineRec) + 2;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVector_Outline_GetData
** Description: Get outline raw data
** Input: outline - outline
** Output: Outline raw data
** Return value: outline + 1
** ---------------------------------------------------------------------------
*/

GVF_Outline GreyVector_Outline_GetData(GVF_Outline outline)
{
	return outline + 1;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVector_Outline_FromData
** Description: Create outline from raw data
** Input: pData - raw data
** Output: Coverted outline
** Return value: outline
** ---------------------------------------------------------------------------
*/

GVF_Outline GreyVector_Outline_FromData(GB_BYTE * pData)
{
	GVF_Outline	outline = (GVF_Outline)pData;

	outline->n_contours = outline[1].n_contours;
	outline->n_points = outline[1].n_points;
	outline->contours = &outline[1].n_points + sizeof(GB_BYTE);
	outline->points = (GVF_Point)((GB_BYTE*)outline->contours
					+ outline->n_contours);
	return outline;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVector_Outline_Done
** Description: Done with outline? Nuke it!
** Input: library - library
**        outline - outline
** Output: Nuked outline
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyVector_Outline_Done(GB_Library library, GVF_Outline outline)
{
	GreyBit_Free(library->gbMem, outline);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVector_Outline_NewByGB
** Description: Create outline from GB outline source
** Input: library - library
**        source - source outline
** Output: New outline
** Return value: outline
** ---------------------------------------------------------------------------
*/

GVF_Outline	GreyVector_Outline_NewByGB(GB_Library library, GB_Outline source)
{
	int			i; 
	int			ia;
	GVF_Outline	outline; 

	outline = GreyVector_Outline_New(library, source->n_contours,
									 source->n_points);
	if (outline)
	{
		for (i = 0; i < outline->n_contours; ++i)
			outline->contours[i] =  (GB_BYTE)source->contours[i];
		for (ia = 0; ia < outline->n_points; ++ia)
		{
			outline->points[ia].x = (GB_BYTE)source->points[ia].x >> 6;
			outline->points[ia].y = (GB_BYTE)source->points[ia].y >> 6;
			outline->points[ia].x = (GB_BYTE)source->tags[ia] & 1
								  | (2 * outline->points[ia].x);
			outline->points[ia].y = (GB_BYTE)(source->tags[ia] >> 1) & 1
								  | (2 * outline->points[ia].y);
		}
	}
	return outline;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVector_Outline_NewByGVF
** Description: Create outline from GVF outline source
** Input: library - library
**        source - source outline
** Output: New outline
** Return value: outline
** ---------------------------------------------------------------------------
*/

GB_Outline GreyBitType_Outline_NewByGVF(GB_Library library,GVF_Outline source)
{
	int			i; 
	int			ia; 
	GB_Outline	outline;

	outline = GreyBitType_Outline_New(library->gbMem, source->n_contours,
									  source->n_points);
	if (outline)
	{
		for (i = 0; i < outline->n_contours; ++i)
			outline->contours[i] = source->contours[i];
		for (ia = 0; ia < outline->n_points; ++ia)
		{
			outline->points[ia].x = source->points[ia].x >> 1 << 6;
			outline->points[ia].y = source->points[ia].y << 6;
			outline->tags[ia] = source->points[ia].y & 1
							  | source->points[ia].x & 1;
		}
	}
	return outline;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyVector_Outline_UpdateByGVF
** Description: Update outline from GB outline source
** Input: library - library
**        source - source outline
** Output: Updated outline
** Return value: outline
** ---------------------------------------------------------------------------
*/

GB_Outline GreyBitType_Outline_UpdateByGVF(GB_Outline outline,
										   GVF_Outline source)
{
	int	i;
	int	ia;

	outline->n_contours = source->n_contours;
	outline->n_points = source->n_points;
	for (i = 0; i < outline->n_contours; ++i)
		outline->contours[i] = source->contours[i];
	for (ia = 0; ia < outline->n_points; ++ia)
	{
		outline->points[ia].x = source->points[ia].x >> 1 << 6;
		outline->points[ia].y = source->points[ia].y >> 1 << 6;
		outline->tags[ia] = (2 * (source->points[ia].y & 1))
						  | source->points[ia].x & 1;
	}
	return outline;
}
#endif //ENABLE_GREYVECTORFILE
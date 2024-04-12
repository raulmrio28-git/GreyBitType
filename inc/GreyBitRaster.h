/*
** ===========================================================================
** File: GreyBitRaster.h
** Description: GreyBit font library - Raster rendering
** Copyright (c) 2023
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2023	me				Return retypes
** 08/10/2023	me				Compile only if vector font is supported
** 08/09/2023	me              Init
** ===========================================================================
*/

#ifndef GREYBITRASTER_H_
#define GREYBITRASTER_H_
/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../GreyBitType.h"
#include "../inc/GreyBitType_Def.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_GREYVECTORFILE
/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

#define DEFAULT_POOL_SIZE			16384
#define GB_Raster_Span_Func			GB_SpanFunc

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

typedef struct GB_BBox_
{
	GB_Pos  xMin, yMin;
	GB_Pos  xMax, yMax;
} GB_BBox;

typedef struct _RST_Vector
{
	int x;
	int y;
} RST_Vector;

#define GB_Vector GB_PointRec

typedef int(*GB_Outline_MoveToFunc)(const GB_Vector* to, void* user);
#define GB_Outline_MoveTo_Func GB_Outline_MoveToFunc

typedef int(*GB_Outline_LineToFunc)(const GB_Vector* to, void* user);
#define GB_Outline_LineTo_Func GB_Outline_LineToFunc

typedef int(*GB_Outline_ConicToFunc)(const GB_Vector* control,
									 const GB_Vector* to, void* user);
#define GB_Outline_ConicTo_Func GB_Outline_ConicToFunc

typedef int(*GB_Outline_CubicToFunc)(const GB_Vector* control1,
									 const GB_Vector* control2,
									 const GB_Vector* to, void* user);
#define GB_Outline_CubicTo_Func GB_Outline_CubicToFunc

typedef struct  GB_Outline_Funcs_
{
	GB_Outline_MoveToFunc	move_to;
	GB_Outline_LineToFunc   line_to;
	GB_Outline_ConicToFunc  conic_to;
	GB_Outline_CubicToFunc  cubic_to;

	int                     shift;
	GB_Pos                  delta;
} GB_Outline_Funcs;

typedef struct GB_Span_
{
	short           x;
	unsigned short  len;
	unsigned char   coverage;
} GB_Span;

typedef void(*GB_SpanFunc)(int y, int count, const GB_Span*spans,void* user);

/*
**---------------------------------------------------------------------------
**  Variable Declarations
**---------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Function(external use only) Declarations
**---------------------------------------------------------------------------
*/

void*		GreyBit_Raster_New(GB_Library library, int nPoolSize);
int			GreyBit_Raster_Render(void * raster, GB_Bitmap tobitmap,
								  GB_Outline fromoutline);
void		GreyBit_Raster_Done(void * raster);
#endif //ENABLE_GREYVECTORFILE

#ifdef __cplusplus
}
#endif

#endif //GREYBITRASTER_H_
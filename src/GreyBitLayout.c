/*
** ===========================================================================
** File: GreyBitLayout.c
** Description: GreyBit font library - Bitmap layout
** Copyright (c) 2023-2024
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 03/29/2024	me				Make bitmap scale function a single function,
**                              add fixes to 8 to 1 conversion (only for vals
**	                            > BITMAP8TO1_SWITCH_VALUE)
** 09/16/2023	me				Upgrade
** 08/10/2023	me				Add feature as to compile encoder stuff only
**                              if encoder is supported
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
#include "../inc/GreyBitRaster.h"
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

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Bitmap_SwitcBuffer
** Description: Switch bitmap buffer
** Input: bitmap - bitmap
**        pNewBuf - new buffer
** Output: New buffer
** Return value: pNewBuf
** ---------------------------------------------------------------------------
*/

GB_BYTE*	GreyBitType_Bitmap_SwitcBuffer(GB_Bitmap bitmap, void * pNewBuf)
{
	GB_BYTE *	pBuf;

	pBuf = bitmap->buffer;
	bitmap->buffer = (GB_BYTE *)pNewBuf;
	return pNewBuf;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Layout_Bold
** Description: Bold character
** Input: layout - layout
** Output: Bolded character bitmap
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Layout_Bold(GB_Layout layout)
{
	GB_BYTE		bitMove;
	GB_INT16	nOff;
	GB_INT16	xMax;
	GB_INT16	xMaxa;
	GB_INT16	yMax;
	GB_BYTE *	pDst;
	GB_BYTE *	pDsta;
	GB_BYTE *	pSrc;
	GB_INT32	y;
	GB_INT32	ya;
	GB_INT32	x;
	GB_INT32	xa;
	GB_Bitmap	bitmap;

	bitmap = layout->gbBitmap;
	pSrc = bitmap->buffer;
	yMax = bitmap->height;
	nOff = yMax >> 5;
	if (!nOff)
		return GB_FAILED;
	if (nOff > 4)
		nOff = 4;
	if (bitmap->bitcount == 8)
	{
		pDst = &layout->gbSwitchBuf[nOff];
		xMax = bitmap->pitch - nOff;
		GB_MEMCPY(layout->gbSwitchBuf, bitmap->buffer, layout->nSwitchBufLen);
		for (y = 0; y < yMax; ++y)
		{
			for (x = 0; x < xMax; ++x)
			{
				if ((pSrc[x] + pDst[x]) <= 255)
					pDst[x] += pSrc[x];
				else
					pDst[x] = 0xff;
			}
			pSrc += bitmap->pitch;
			pDst += bitmap->pitch;
		}
	}
	else
	{
		if (bitmap->bitcount != 1)
			return GB_FAILED;
		pDsta = layout->gbSwitchBuf;
		xMaxa = bitmap->pitch;
		for (ya = 0; ya < yMax; ++ya)
		{
			bitMove = 0;
			for (xa = 0; xa < xMaxa; ++xa)
			{
				pDsta[xa] = (bitMove << (8 - nOff)) | (pSrc[xa] >> nOff);
				bitMove = (0xff >> (8 - nOff)) & pSrc[xa];
			}
			pSrc += bitmap->pitch;
			pDsta += bitmap->pitch;
		}
	}
	layout->gbSwitchBuf = GreyBitType_Bitmap_SwitcBuffer(bitmap,
														 layout->gbSwitchBuf);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Layout_Italic
** Description: Italicize character
** Input: layout - layout
** Output: Italic character bitmap
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Layout_Italic(GB_Layout layout)
{
	GB_BYTE		bitMove;
	GB_INT16	nOff;
	GB_INT16	nOffa;
	GB_INT16	nHalfOffMax;
	GB_INT16	xMax;
	GB_INT16	xMaxa;
	GB_INT16	xMaxb;
	GB_INT16	yMax;
	GB_BYTE *	pDst;
	GB_BYTE *	pDsta;
	GB_BYTE	*	pSrc;
	GB_INT32	y;
	GB_INT32	ya;
	GB_INT32	x;
	GB_INT32	xa;
	GB_INT32	xb;
	GB_INT32	xc;
	GB_Bitmap	bitmap;

	bitmap = layout->gbBitmap;
	pSrc = bitmap->buffer;
	yMax = bitmap->height;
	nHalfOffMax = yMax >> 3;
	if (!(yMax >> 2))
		return GB_FAILED;
	if (bitmap->bitcount == 8)
	{
		pDst = layout->gbSwitchBuf;
		GB_MEMSET(pDst, 0, layout->nSwitchBufLen);
		for (y = 0; y < yMax; ++y)
		{
			nOff = (GB_INT16)(y >> 2) - nHalfOffMax;
			if (nOff >= 0)
			{
				xMaxa = bitmap->pitch - nOff;
				for (xa = 0; xa < xMaxa; ++xa)
					pDst[nOff + xa] = pSrc[xa];
			}
			else
			{
				xMax = bitmap->pitch;
				for (x = -nOff; x < xMax; ++x)
					pDst[nOff + x] = pSrc[x];
			}
			pSrc += bitmap->pitch;
			pDst += bitmap->pitch;
		}
	}
	else
	{
		if (bitmap->bitcount != 1)
			return GB_FAILED;
		pDsta = layout->gbSwitchBuf;
		xMaxb = bitmap->pitch;
		for (ya = 0; ya < yMax; ++ya)
		{
			nOffa = (GB_INT16)(ya >> 2) - nHalfOffMax;
			bitMove = 0;
			if (nOffa >= 0)
			{
				for (xc = 0; xc < xMaxb; ++xc)
				{
					pDsta[xc] = (bitMove << (8 - nOffa))|(pSrc[xc] >> nOffa);
					bitMove = (255 >> (8 - nOffa)) & pSrc[xc];
				}
			}
			else
			{
				--xMaxb;
				for (xb = 0; xb < xMaxb; ++xb)
					pDsta[xb] = (((255<<(nOffa + 8))&pSrc[xb+1])>>(nOffa+8))
							  | (pSrc[xb] << nOffa);
				pDsta[xb] = pSrc[xb] << nOffa;
			}
			pSrc += bitmap->pitch;
			pDsta += bitmap->pitch;
		}
	}
	layout->gbSwitchBuf = GreyBitType_Bitmap_SwitcBuffer(bitmap,
														 layout->gbSwitchBuf);
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Layout_ScaleBitmap
** Description: Scale bitmaop character
** Input: dst - destination bitmap
**        src - source bitmap
** Output: Scaled bitmap
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Layout_ScaleBitmap(GB_Bitmap dst, GB_Bitmap src)
{
	GB_INT32	i;
	GB_INT32	j;
	GB_INT32	ja;
	GB_BYTE *	pSrc;
	GB_BYTE *	pDst;
	
	pSrc = src->buffer;
	pDst = dst->buffer;
	if (dst->bitcount == src->bitcount)
	{
		if (dst->bitcount == 8)
		{
			dst->width = src->width * dst->height / src->height;
			dst->horioff = src->horioff * dst->height / src->height;
			dst->pitch = dst->width;
			for (i = 0; i < dst->height; ++i)
			{
				for (j = 0; j < dst->width; ++j)
					pDst[j] = pSrc[(((src->height << 10) / dst->height*i)>>10)
								  * src->pitch + (((src->width << 10)
								  / dst->width*j)>>10)];
				pDst += dst->pitch;
			}
		}
		else if (dst->bitcount == 1)
		{
			dst->width = src->width * dst->height / src->height;
			dst->horioff = src->horioff * dst->height / src->height;
			dst->pitch = dst->width >> 3;
			if (!dst->pitch)
				dst->pitch = 1;
			for (i = 0; i < dst->height; ++i)
			{
				for (j = 0; j < dst->pitch; ++j)
					pDst[j] = 0;
				for (ja = 0; ja < dst->width; ++ja)
					pDst[ja >> 3] |= ((pSrc[(((src->height << 10)/dst->height
								  * i) >> 10) * src->pitch + (((src->width
								  << 10) / dst->width * ja) >> 13)] >> (7
								  -(((src->width << 10) / dst->width * ja)
								  >> 10) % 8))&1) << (7 - ja % 8);
				pDst += dst->pitch;
			}
		}
	}
	else if (dst->bitcount == 8 && src->bitcount == 1)
	{
		dst->width = src->width * dst->height / src->height;
		dst->horioff = src->horioff * dst->height / src->height;
		dst->pitch = dst->width;
		if (dst->height == src->height)
		{
			for (i = 0; i < dst->height; ++i)
			{
				for (j = 0; j < dst->width; ++j)
					pDst[j] = -(((pSrc[j >> 3] >> (7-j%8))&1) != 0);
				pDst += dst->pitch;
				pSrc += src->pitch;
			}
		}
		else
		{
			for (i = 0; i < dst->height; ++i)
			{
				for (j = 0; j < dst->width; ++j)
					pDst[j] = -(((pSrc[(((src->height << 10) / dst->height
							* i) >> 10) * src->pitch + ( ((src->width 
							<< 10) / dst->width * j) >> 13)] >> (7
							- (((src->width << 10) / dst->width * j) >> 10)
							% 8))& 1) != 0);
				pDst += dst->pitch;
			}
		}
	}
	else if (dst->bitcount == 1 && src->bitcount == 8)
	{
		dst->width = src->width * dst->height / src->height;
		dst->horioff = src->horioff * dst->height / src->height;
		dst->pitch = dst->width >> 3;
		if (!dst->pitch)
			dst->pitch = 1;
		if (dst->height == src->height)
		{
			for (i = 0; i < dst->height; ++i)
			{
				for (j = 0; j < dst->pitch; ++j)
					pDst[j] = 0;
				for (ja = 0; ja < dst->width; ++ja)
				{
					if (pSrc[ja] > BITMAP8TO1_SWITCH_VALUE)
						pDst[ja >> 3] |= 1 << (7-ja%8);
				}
				pDst += dst->pitch;
				pSrc += src->pitch;
			}
		}
		else
		{
			for (i = 0; i < dst->height; ++i)
			{
				for (j = 0; j < dst->pitch; ++j)
					pDst[j] = 0;
				for (ja = 0; ja < dst->width; ++ja)
				{
					if (pSrc[(((src->height << 10) / dst->height * i) >> 10)
							 * src->pitch + (((src->width << 10) / dst->width
						     * ja) >> 10)] > BITMAP8TO1_SWITCH_VALUE)
						pDst[ja >> 3] |= 1 << (7 - ja % 8);
				}
				pDst += dst->pitch;
			}
		}
	}
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Layout_New
** Description: Create new layout
** Input: loader - loader
**        nSize - size
**        nBitCount - bit count
**        bBold - bold?
**        bItalic - italic?
** Output: New layout
** Return value: layout
** ---------------------------------------------------------------------------
*/

GBHANDLE	GreyBitType_Layout_New(GBHANDLE loader, GB_INT16 nSize,
								   GB_INT16 nBitCount, GB_BOOL bBold,
								   GB_BOOL bItalic)
{
	GB_Layout	layout;
	GB_Loader	me = (GB_Loader)loader;

	layout = (GB_Layout)GreyBit_Malloc(me->gbMem, sizeof(GB_LayoutRec));
	if (layout)
	{
		layout->gbLibrary = me->gbLibrary;
		layout->gbMem = me->gbMem;
		layout->gbStream = me->gbStream;
		layout->gbDecoder = me->gbDecoder;
		layout->dwCode = 0xffff;
		layout->nSize = nSize;
		layout->nBitCount = nBitCount;
		layout->bBold = bBold;
		layout->bItalic = bItalic;
		layout->gbBitmap = GreyBitType_Bitmap_New(layout->gbLibrary,
												  2 *nSize,nSize,nBitCount,0);
		if (nBitCount != 8)
			layout->gbBitmap8 = GreyBitType_Bitmap_New(layout->gbLibrary,
													   2 *nSize, nSize, 8, 0);
#ifdef ENABLE_GREYVECTORFILE
		layout->gbRaster = (void *)GreyBit_Raster_New(layout->gbLibrary, 0);
#endif //ENABLE_GREYVECTORFILE
		layout->nSwitchBufLen = layout->gbBitmap->height
							  * layout->gbBitmap->pitch;
		layout->gbSwitchBuf = (GB_BYTE*)GreyBit_Malloc(layout->gbMem,
													   layout->nSwitchBufLen);
	}
	return layout;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Layout_GetWidth
** Description: Get layout width
** Input: layout - layout
**		  nCode - unicode code
** Output: Width
** Return value: GreyBit_Decoder_GetAdvance
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBitType_Layout_GetWidth(GBHANDLE layout, GB_UINT32 nCode)
{
	GB_Layout	me = (GB_Layout)layout;

	return GreyBit_Decoder_GetAdvance(me->gbDecoder, nCode, me->nSize);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Layout_LoadChar
** Description: Get layout width
** Input: layout - layout
**		  nCode - unicode code
**        pBmp - bitmap
** Output: Loaded character
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

int			GreyBitType_Layout_LoadChar(GBHANDLE layout, GB_UINT32 nCode,
										GB_Bitmap * pBmp)
{
	GB_DataRec	data;
	GB_Bitmap	bitmap;
	GB_Layout	me = (GB_Layout)layout;

	if (!me->gbBitmap)
		return GB_FAILED;
	if (me->dwCode != nCode)
	{
		if (GreyBit_Decoder_Decode(me->gbDecoder, nCode, &data, me->nSize)
		 != GB_SUCCESS)
			return GB_FAILED;
#ifdef ENABLE_GREYVECTORFILE
		if (data.format == GB_FORMAT_BITMAP)
		{
			bitmap = (GB_Bitmap)data.data;
		}
		else
		{
			if (data.format != GB_FORMAT_OUTLINE)
				return GB_FAILED;
			if (me->gbBitmap8)
				bitmap = me->gbBitmap8;
			else
				bitmap = me->gbBitmap;
			bitmap->width = data.width;
			bitmap->pitch = data.width;
			bitmap->horioff = data.horioff;
			GB_MEMSET(bitmap->buffer, 0, bitmap->height * bitmap->pitch);
			GreyBit_Raster_Render(me->gbRaster, bitmap, data.data);
		}
#else
		bitmap = (GB_Bitmap)data.data;
#endif //ENABLE_GREYVECTORFILE
		if (bitmap->bitcount == me->gbBitmap->bitcount
		 && bitmap->height == me->gbBitmap->height)
		{
			me->gbBitmap->pitch = bitmap->pitch;
			me->gbBitmap->width = bitmap->width;
			me->gbBitmap->horioff = bitmap->horioff;
			bitmap->buffer=(GB_BYTE *)GreyBitType_Bitmap_SwitcBuffer
									  (me->gbBitmap, bitmap->buffer);
		}
		else
		{
			GreyBitType_Layout_ScaleBitmap(me->gbBitmap, bitmap);
		}
		if (me->bBold)
			GreyBitType_Layout_Bold(me);
		if (me->bItalic)
			GreyBitType_Layout_Italic(me);
		me->dwCode = nCode;
	}
	if (pBmp)
		*pBmp = me->gbBitmap;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBitType_Layout_Done
** Description: Done with layout? Nuke it!
** Input: layout - layout
** Output: Nuked layout
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBitType_Layout_Done(GBHANDLE layout)
{
	GB_Layout	me = (GB_Layout)layout;

	if (me->gbBitmap)
	{
		if (me->gbSwitchBuf)
			GreyBit_Free(me->gbMem, me->gbSwitchBuf);
		GreyBitType_Bitmap_Done(me->gbLibrary, me->gbBitmap);
	}
	if (me->gbBitmap8)
		GreyBitType_Bitmap_Done(me->gbLibrary, me->gbBitmap8);
#ifdef ENABLE_GREYVECTORFILE
	if (me->gbRaster)
		GreyBit_Raster_Done(me->gbRaster);
#endif //ENABLE_GREYVECTORFILE
	GreyBit_Free(me->gbMem, me);
}
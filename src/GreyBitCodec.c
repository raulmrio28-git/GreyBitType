/*
** ===========================================================================
** File: GreyBitCodec.c
** Description: GreyBit font library - Codec interface
** Copyright (c) 2023
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2024	me				Upgrade
** 08/07/2023	me              Init
** ===========================================================================
*/

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../GreyBitType.h"
#include "../inc/GreyBitType_Def.h"

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
** Function: GreyBit_Decoder_SetParam
** Description: Set decoder param
** Input: decoder - decoder
**        nParam - param type
**        dwParam - param value
** Output: Set param
** Return value: decoder->setparam
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Decoder_SetParam(GB_Decoder decoder, GB_Param nParam,
									 GB_UINT32 dwParam)
{
	return decoder->setparam(decoder, nParam, dwParam);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Decoder_GetCount
** Description: Get characteer count from decoder interface
** Input: decoder - decoder
** Output: Count
** Return value: decoder->getcount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Decoder_GetCount(GB_Decoder decoder)
{
	return decoder->getcount(decoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Decoder_GetWidth
** Description: Get characteer width
** Input: decoder - decoder
**		  nCode - unicode code
**		  nSize - character size
** Output: Width
** Return value: decoder->getwidth
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Decoder_GetWidth(GB_Decoder decoder, GB_UINT32 nCode,
									 GB_INT16 nSize)
{
	return decoder->getwidth(decoder, nCode, nSize);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Decoder_GetHeight
** Description: Get character height
** Input: decoder - decoder
** Output: Height
** Return value: decoder->getheight
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Decoder_GetHeight(GB_Decoder decoder)
{
	return decoder->getheight(decoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Decoder_GetAdvance
** Description: Get characteer advance from decoder interface
** Input: decoder - decoder
**		  nCode - unicode code
**		  nSize - character size
** Output: Font char advance
** Return value: decoder->getadvance
** ---------------------------------------------------------------------------
*/

GB_INT16	GreyBit_Decoder_GetAdvance(GB_Decoder decoder, GB_UINT32 nCode,
									   GB_INT16 nSize)
{
	return decoder->getadvance(decoder, nCode, nSize);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Decoder_Decode
** Description: Decode a character
** Input: decoder - decoder
**		  nCode - unicode code
**	      pData - data buffer
**		  nSize - character size
** Output: Decoded character
** Return value: decoder->decode
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Decoder_Decode(GB_Decoder decoder, GB_UINT32 nCode,
								   GB_Data pData, GB_INT16 nSize)
{
	return decoder->decode(decoder, nCode, pData, nSize);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Decoder_Done
** Description: Done with decoder? Nuke it!
** Input: decoder - decoder
** Output: Nuked decoder
** Return value: decoder->done
** ---------------------------------------------------------------------------
*/

void		GreyBit_Decoder_Done(GB_Decoder decoder)
{
	decoder->done(decoder);
}

#ifdef ENABLE_ENCODER
/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Decoder_GetCount
** Description: Get characteer count from encoder interface
** Input: encoder- encoder
** Output: Count
** Return value: encoder->getcount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Encoder_GetCount(GB_Encoder encoder)
{
	return encoder->getcount(encoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Encoder_SetParam
** Description: Set encoder param
** Input: decoder - decoder
**		  nParam - param type
**        dwParam - param value
** Output: Count
** Return value: encoder->getcount
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Encoder_SetParam(GB_Encoder encoder, GB_Param nParam,
									 GB_UINT32 dwParam)
{
	return encoder->setparam(encoder, nParam, dwParam);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Encoder_Delete
** Description: Remove character
** Input: encoder - encoder
**        nCode - unicode code
** Output: Removed character
** Return value: encoder->remove
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Encoder_Delete(GB_Encoder encoder, GB_UINT32 nCode)
{
	return encoder->remove(encoder, nCode);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Encoder_Encode
** Description: Encode code to new font
** Input: encoder - encoder
**        nCode - unicode code
**        pData - data buffer
** Output: Added character
** Return value: encoder->encode
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Encoder_Encode(GB_Encoder encoder, GB_UINT32 nCode,
								   GB_Data pData)
{
	return encoder->encode(encoder, nCode, pData);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Encoder_Flush
** Description: Flush encoder
** Input: encoder - encoder
** Output: Flushed encoder
** Return value: encoder->flush
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Encoder_Flush(GB_Encoder encoder)
{
	return encoder->flush(encoder);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Encoder_Done
** Description: Done with encoder? Nuke it!
** Input: encoder - encoder
** Output: Nuked encoder
** Return value: encoder->done
** ---------------------------------------------------------------------------
*/

void		GreyBit_Encoder_Done(GB_Encoder encoder)
{
	encoder->done(encoder);
}
#endif //ENABLE_ENCODER
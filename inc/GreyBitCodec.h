/*
** ===========================================================================
** File: GreyBitCodec.h
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

#ifndef GREYBITCODEC_H_
#define GREYBITCODEC_H_
/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../GreyBitType.h"
#include "../GreyBitSystem.h"
#include "../inc/GreyBitType_Def.h"

#ifdef __cplusplus
extern "C" {
#endif
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
**---------------------------------------------------------------------------
**  Variable Declarations
**---------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Function(external use only) Declarations
**---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Decoder_SetParam(GB_Decoder decoder, GB_Param nParam,
									 GB_UINT32 dwParam);
GB_INT32	GreyBit_Decoder_GetCount(GB_Decoder decoder);
GB_INT32	GreyBit_Decoder_GetWidth(GB_Decoder decoder, GB_UINT32 nCode,
									 GB_INT16 nSize);
GB_INT32	GreyBit_Decoder_GetHeight(GB_Decoder decoder);
GB_INT16	GreyBit_Decoder_GetAdvance(GB_Decoder decoder, GB_UINT32 nCode,
									  GB_INT16 nSize);
GB_INT32	GreyBit_Decoder_Decode(GB_Decoder decoder, GB_UINT32 nCode,
								   GB_Data pData, GB_INT16 nSize);
void		GreyBit_Decoder_Done(GB_Decoder decoder);

#ifdef ENABLE_ENCODER
GB_INT32	GreyBit_Encoder_SetParam(GB_Encoder encoder, GB_Param nParam,
									 GB_UINT32 dwParam);
GB_INT32	GreyBit_Encoder_GetCount(GB_Encoder encoder);
GB_INT32	GreyBit_Encoder_Delete(GB_Encoder encoder, GB_UINT32 nCode);
GB_INT32	GreyBit_Encoder_Encode(GB_Encoder encoder, GB_UINT32 nCode,
								   GB_Data pData);
GB_INT32	GreyBit_Encoder_Flush(GB_Encoder encoder);
void		GreyBit_Encoder_Done(GB_Encoder encoder);
#endif //ENABLE_ENCODER

#ifdef __cplusplus
}
#endif 

#endif //GREYBITCODEC_H_
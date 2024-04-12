/*
** ===========================================================================
** File: UnicodeSection.h
** Description: GreyBit font library - Unicode section
** Copyright (c) 2023
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 08/09/2023	me              Init
** ===========================================================================
*/

#ifndef UNICODESECTION_H_
#define UNICODESECTION_H_
/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../GreyBitType.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

#define	UNICODE_SECTION_NUM		146

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

typedef struct tagUNICODESECTION
{
	GB_UINT16	nMinCode;
	GB_UINT16	nMaxCode;
} UNICODESECTION;

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

GB_INT32	UnicodeSection_GetIndex(GB_UINT16 nCode);
GB_INT32	UnicodeSection_GetSectionNum(GB_INT32 index);
void		UnicodeSection_GetSectionInfo(GB_INT32 index,
										  GB_UINT16 * pMinCode,
										  GB_UINT16 * pMaxCode);

#ifdef __cplusplus
}
#endif 

#endif //UNICODESECTION_H_

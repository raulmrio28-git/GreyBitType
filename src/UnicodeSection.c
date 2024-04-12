/*
** ===========================================================================
** File: UnicodeSection.c
** Description: GreyBit font library - Unicode section
** Copyright (c) 2023-2024
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 03/27/2024	me              Fix UnicodeSection_GetIndex function failing
**								for Unicode values >= 0x4E00
** 08/09/2023	me              Init
** ===========================================================================
*/

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../inc/UnicodeSection.h"

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

UNICODESECTION g_Unicode_Section[UNICODE_SECTION_NUM] =
{
    { 0x0020u, 0x007Fu }, //Basic Latin
	{ 0x0080u, 0x00FFu }, //Latin-1 Supplement
	{ 0x0100u, 0x017Fu }, //Latin Extended-A
	{ 0x0180u, 0x024Fu }, //Latin Extended-B
    { 0x0250u, 0x02AFu }, //IPA Extensions
	{ 0x02B0u, 0x02FFu }, //Spacing Modifier Letters
	{ 0x0300u, 0x036Fu }, //Combining Diacritical Marks
	{ 0x0370u, 0x03FFu }, //Greek and Coptic
	{ 0x0400u, 0x04FFu }, //Cyrillic
	{ 0x0500u, 0x052Fu }, //Cyrillic Supplement
	{ 0x0530u, 0x058Fu }, //Armenian
	{ 0x0590u, 0x05FFu }, //Hebrew
	{ 0x0600u, 0x06FFu }, //Arabic
	{ 0x0700u, 0x074Fu }, //Syriac
    { 0x0750u, 0x077Fu }, //Arabic Supplement
    { 0x0780u, 0x07BFu }, //Thaana
    { 0x07C0u, 0x07FFu }, //NKo
	{ 0x0800u, 0x085Fu }, //Samaritan and Mandaic
	{ 0x0860u, 0x087Fu }, //Syriac Supplement
    { 0x0880u, 0x08AFu }, //Arabic Extd
	{ 0x0900u, 0x097Fu }, //Devanagari
	{ 0x0980u, 0x09FFu }, //Bengali
	{ 0x0A00u, 0x0A7Fu }, //Gurmukhi
	{ 0x0A80u, 0x0AFFu }, //Gujarati
    { 0x0B00u, 0x0B7Fu }, //Oriya
	{ 0x0B80u, 0x0BFFu }, //Tamil
	{ 0x0C00u, 0x0C7Fu }, //Telugu
	{ 0x0C80u, 0x0CFFu }, //Kannada
	{ 0x0D00u, 0x0D7Fu }, //Malayalam
	{ 0x0D80u, 0x0DFFu }, //Sinhala
	{ 0x0E00u, 0x0E7Fu }, //Thai
	{ 0x0E80u, 0x0EFFu }, //Lao
	{ 0x0F00u, 0x0FFFu }, //Tibetan
    { 0x1000u, 0x109Fu }, //Myanmar
	{ 0x10A0u, 0x10FFu }, //Georgian
	{ 0x1100u, 0x11FFu }, //Hangul Jamo
	{ 0x1200u, 0x137Fu }, //Ethiopic
	{ 0x1380u, 0x139Fu }, //Ethiopic Supplement
	{ 0x13A0u, 0x13FFu }, //Cherokee
	{ 0x1400u, 0x167Fu }, //Unified Canadian Aboriginal Syllabics
    { 0x1680u, 0x169Fu }, //Ogham
	{ 0x16A0u, 0x16FFu }, //Runic
    { 0x1700u, 0x171Fu }, //Tagalog
    { 0x1720u, 0x173Fu }, //Hanunoo
	{ 0x1740u, 0x175Fu }, //Buhid
	{ 0x1760u, 0x177Fu }, //Tagbanwa
	{ 0x1780u, 0x17FFu }, //Khmer
    { 0x1800u, 0x18AFu }, //Mongolian
	{ 0x18B0u, 0x18FFu }, //Unified Canadian Aboriginal Syllabics Extended
	{ 0x1900u, 0x194Fu }, //Limbu
	{ 0x1950u, 0x197Fu }, //Tai Le
	{ 0x1980u, 0x19DFu }, //New Tai Lue
	{ 0x19E0u, 0x19FFu }, //Khmer Symbols
	{ 0x1A00u, 0x1A1Fu }, //Buginese
	{ 0x1A20u, 0x1A5Fu }, //Tai Tham
	{ 0x1A80u, 0x1AEFu }, //Combining Diacritical Marks Extended
	{ 0x1B00u, 0x1B7Fu }, //Balinese
    { 0x1B80u, 0x1BB0u }, //Sundanese
	{ 0x1BC0u, 0x1BFFu }, //Batak
    { 0x1C00u, 0x1C4Fu }, //Lepcha
    { 0x1C50u, 0x1C7Fu }, //Oi CHiki
	{ 0x1C80u, 0x1CDFu }, //Cyrillic Extended C, Georgian Extended,
						  //Sundanese Supplement, Vedic Ext
	{ 0x1D00u, 0x1D7Fu }, //Phonetic Extensions
	{ 0x1D80u, 0x1DBFu }, //Phonetic Extensions Supplement
	{ 0x1DC0u, 0x1DFFu }, //Combining Diacritical Marks Supplement
	{ 0x1E00u, 0x1EFFu }, //Latin Extended Additional
	{ 0x1F00u, 0x1FFFu }, //Greek Extended
	{ 0x2000u, 0x206Fu }, //General Punctuation
	{ 0x2070u, 0x209Fu }, //Superscripts and Subscripts
	{ 0x20A0u, 0x20CFu }, //Currency Symbols
	{ 0x20D0u, 0x20FFu }, //Combining Diacritical Marks for Symbols
	{ 0x2100u, 0x214Fu }, //Letterlike Symbols
	{ 0x2150u, 0x218Fu }, //Number Forms
	{ 0x2190u, 0x21FFu }, //Arrows
	{ 0x2200u, 0x22FFu }, //Mathematical Operators
	{ 0x2300u, 0x23FFu }, //Miscellaneous Technical
    { 0x2400u, 0x243Fu }, //Ctrl. Pictures
	{ 0x2440u, 0x245Fu }, //Optical Character Recognition
	{ 0x2460u, 0x24FFu }, //Enclosed Alphanumerics
	{ 0x2500u, 0x257Fu }, //Box Drawing
	{ 0x2580u, 0x259Fu }, //Block Elements
	{ 0x25A0u, 0x25FFu }, //Geometric Shapes
	{ 0x2600u, 0x26FFu }, //Miscellaneous Symbols
	{ 0x2700u, 0x27BFu }, //Dingbats
	{ 0x27C0u, 0x27EFu }, //Miscellaneous Mathematical Symbols-A
	{ 0x27F0u, 0x27FFu }, //Supplemental Arrows-A
	{ 0x2800u, 0x28FFu }, //Braille Patterns
	{ 0x2900u, 0x297Fu }, //Supplemental Arrows-B
	{ 0x2980u, 0x29FFu }, //Miscellaneous Mathematical Symbols-B
	{ 0x2A00u, 0x2AFFu }, //Supplemental Mathematical Operators
	{ 0x2B00u, 0x2BFFu }, //Miscellaneous Symbols and Arrows
	{ 0x2C00u, 0x2C5Fu }, //Glagolitic
	{ 0x2C60u, 0x2C7Fu }, //Latin Extended-C
	{ 0x2C80u, 0x2CFFu }, //Coptic
	{ 0x2D00u, 0x2D2Fu }, //Georgian Supplement
	{ 0x2D30u, 0x2D7Fu }, //Tifinagh
	{ 0x2D80u, 0x2DDFu }, //Ethiopic Extended
	{ 0x2E00u, 0x2E7Fu }, //Supplemental Punctuation
	{ 0x2E80u, 0x2EFFu }, //CJK Radicals Supplement
	{ 0x2F00u, 0x2FDFu }, //Kangxi Radicals
    { 0x2FF0u, 0x2FFFu }, //undefined 
	{ 0x3000u, 0x303Fu }, //CJK Symbols and Punctuation
	{ 0x3040u, 0x309Fu }, //Hiragana
	{ 0x30A0u, 0x30FFu }, //Katakana
	{ 0x3100u, 0x312Fu }, //Bopomofo
	{ 0x3130u, 0x318Fu }, //Hangul Compatibility Jamo
	{ 0x3190u, 0x319Fu }, //Kanbun
	{ 0x31A0u, 0x31BFu }, //Bopomofo Extended
	{ 0x31C0u, 0x31EFu }, //CJK Strokes
	{ 0x31F0u, 0x31FFu }, //Katakana Phonetic Extensions
	{ 0x3200u, 0x32FFu }, //Enclosed CJK Letters and Months
	{ 0x3300u, 0x33FFu }, //CJK Compatibility 
    { 0x3400u, 0x4DBFu }, //DJK Unified Ideographs Extension A
	{ 0x4DC0u, 0x4DFFu }, //Yijing Hexagram Symbols
	{ 0x4E00u, 0x9FBFu }, //CJK Unified Ideographs
	{ 0xA000u, 0xA48Fu }, //Yi Sills
    { 0xA490u, 0xA4CFu }, //Yi Rads
    { 0xA500u, 0xA61Fu }, //Vai
	{ 0xA660u, 0xA6FFu }, //Cyrillic Extended-B, Bamum
	{ 0xA700u, 0xA71Fu }, //Modifier Tone Letters
	{ 0xA720u, 0xA7FFu }, //Latin Extended-D
	{ 0xA800u, 0xA82Fu }, //Syloti Nagri
	{ 0xA840u, 0xA87Fu }, //Phags-pa
	{ 0xA880u, 0xA8DFu }, //Saurashtra
	{ 0xA900u, 0xA97Fu }, //Devanagari Extended, Kayah Li, Rejand,
						  //Hangul Jamo Extd A
    { 0xA980u, 0xA9DFu }, //Javanese
	{ 0xAA00u, 0xAA3Fu }, //Cham
    { 0xAA40u, 0xAA6Fu }, //Cham, Myanmar Extended A
	{ 0xAA80u, 0xAADFu }, //Tai Viet
	{ 0xAB00u, 0xAB5Fu }, //Ethiopic Extended-A, Latin Extd-E
	{ 0xAB80u, 0xABA0u }, //Cherokee Suppl
	{ 0xAC00u, 0xD7AFu }, //Hangul Syllables
	{ 0xD800u, 0xDBFFu }, //High Surrogates
	{ 0xDC00u, 0xDFFFu }, //Low Surrogates
    { 0xE000u, 0xF8FFu }, //Private Use Area
	{ 0xF900u, 0xFAFFu }, //CJK Compatibility Ideographs
	{ 0xFB00u, 0xFB4Fu }, //Alphabetic Presentation Forms
	{ 0xFB50u, 0xFDFFu }, //Arabic Presentation Forms
	{ 0xFE00u, 0xFE0Fu }, //Variation Selectors
	{ 0xFE10u, 0xFE1Fu }, //Vertical Forms
	{ 0xFE20u, 0xFE2Fu }, //Combining Half Marks
	{ 0xFE30u, 0xFE4Fu }, //CJK Compatibility Forms
	{ 0xFE50u, 0xFE6Fu }, //Small Form Variants
	{ 0xFE70u, 0xFEFFu }, //Arabic Presentation Forms-B
	{ 0xFF00u, 0xFFEFu }, //Halfwidth and Fullwidth Forms
	{ 0xFFF0u, 0xFFFFu }, //Specials
};

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
** Function: UnicodeSection_GetIndex
** Description: Get Unicode section index from code value
** Input: nCode - code value
** Output: Unicode section index
** Return value: i if in index
**               UNICODE_SECTION_NUM if not in index
** ---------------------------------------------------------------------------
*/

GB_INT32	UnicodeSection_GetIndex(GB_UINT16 nCode)
{
  int i; 

  for ( i = 0; i < UNICODE_SECTION_NUM; ++i )
  {
    if ( g_Unicode_Section[i].nMinCode <= nCode
	  && g_Unicode_Section[i].nMaxCode >= nCode )
      return i;
  }
  return UNICODE_SECTION_NUM;
}

/*
** ---------------------------------------------------------------------------
** Function: UnicodeSection_GetSectionNum
** Description: Get count of characters in Unicode sections
** Input: index - Unicode section index
** Output: Count of characters in Unicode section
** Return value: Count of characters in Unicode section (sect max-sect min+1)
** ---------------------------------------------------------------------------
*/

GB_INT32	UnicodeSection_GetSectionNum(GB_INT32 index)
{
 return g_Unicode_Section[index].nMaxCode-g_Unicode_Section[index].nMinCode+1;
}

/*
** ---------------------------------------------------------------------------
** Function: UnicodeSection_GetSectionInfo
** Description: Get Unicode section info
** Input: index - Unicode section index
** Output: pMinCode - pointer to minimum code
**         pMaxCode - pointer to maximum code
** Return value: g_Unicode_Section[index].nMinCode for pMinCode
**               g_Unicode_Section[index].nMaxCode for pMaxCode
** ---------------------------------------------------------------------------
*/

void		UnicodeSection_GetSectionInfo(GB_INT32 index,GB_UINT16 * pMinCode,
										  GB_UINT16 * pMaxCode)
{
  if ( pMinCode )
    *pMinCode = g_Unicode_Section[index].nMinCode;
  if ( pMaxCode )
    *pMaxCode = g_Unicode_Section[index].nMaxCode;
}
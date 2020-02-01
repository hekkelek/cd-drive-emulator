/*! *******************************************************************************************************
* Copyright (c) 2017-2018 Kristóf Szabolcs Horváth
*
* All rights reserved
*
* \file binconvert.c
*
* \brief Binary to raw CD converter functions
*
* \author Kristóf Sz. Horváth
*
**********************************************************************************************************/

//--------------------------------------------------------------------------------------------------------/
// Include files
//--------------------------------------------------------------------------------------------------------/
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "types.h"
#include "reedsolomon.h"


//--------------------------------------------------------------------------------------------------------/
// Definitions
//--------------------------------------------------------------------------------------------------------/
#define CD_RAW_SECTOR_SIZE    2352u
#define CD_FRAME_SIZE         3234u
#define CD_CHANNEL_FRAME_SIZE 7203u


//--------------------------------------------------------------------------------------------------------/
// Types
//--------------------------------------------------------------------------------------------------------/


//--------------------------------------------------------------------------------------------------------/
// Constants
//--------------------------------------------------------------------------------------------------------/
//! \brief Eight-to-fourteen modulation code table
//! \note  The upper 14 bits are used.
const U32 cau32EFMCodeTable[ 256 ] = { 0x48800000, 0x84000000, 0x90800000, 0x88800000, 0x44000000, 0x04400000, 0x10800000, 0x24000000,
                                       0x49000000, 0x81000000, 0x91000000, 0x89000000, 0x41000000, 0x01000000, 0x11000000, 0x21000000,
                                       0x80800000, 0x82000000, 0x92000000, 0x20800000, 0x42000000, 0x02000000, 0x12000000, 0x22000000,
                                       0x48400000, 0x80400000, 0x90400000, 0x88400000, 0x40400000, 0x08400000, 0x10400000, 0x20400000,
                                       0x00800000, 0x84200000, 0x08800000, 0x24800000, 0x44200000, 0x04200000, 0x40800000, 0x24200000,
                                       0x49200000, 0x81200000, 0x91200000, 0x89200000, 0x41200000, 0x01200000, 0x11200000, 0x21200000,
                                       0x04000000, 0x82200000, 0x92200000, 0x84400000, 0x42200000, 0x02200000, 0x12200000, 0x22200000,
                                       0x48200000, 0x80200000, 0x90200000, 0x88200000, 0x40200000, 0x08200000, 0x10200000, 0x20200000,
                                       0x48900000, 0x84900000, 0x90900000, 0x88900000, 0x44900000, 0x00900000, 0x10900000, 0x24900000,
                                       0x49100000, 0x81100000, 0x91100000, 0x89100000, 0x41100000, 0x01100000, 0x11100000, 0x21100000,
                                       0x80900000, 0x82100000, 0x92100000, 0x20900000, 0x42100000, 0x02100000, 0x12100000, 0x22100000,
                                       0x48100000, 0x80100000, 0x90100000, 0x88100000, 0x40100000, 0x08100000, 0x10100000, 0x20100000,
                                       0x48880000, 0x84880000, 0x90880000, 0x88880000, 0x44880000, 0x00880000, 0x40900000, 0x24880000,
                                       0x49080000, 0x81080000, 0x91080000, 0x89080000, 0x41080000, 0x01080000, 0x11080000, 0x21080000,
                                       0x80880000, 0x82080000, 0x92080000, 0x20880000, 0x42080000, 0x02080000, 0x12080000, 0x22080000,
                                       0x48080000, 0x09200000, 0x90080000, 0x88080000, 0x40080000, 0x08080000, 0x10080000, 0x20080000,
                                       0x48840000, 0x84840000, 0x90840000, 0x88840000, 0x44840000, 0x00840000, 0x10840000, 0x24840000,
                                       0x49040000, 0x81040000, 0x91040000, 0x89040000, 0x41040000, 0x01040000, 0x11040000, 0x21040000,
                                       0x80840000, 0x82040000, 0x92040000, 0x20840000, 0x42040000, 0x02040000, 0x12040000, 0x22040000,
                                       0x48040000, 0x82400000, 0x90040000, 0x88040000, 0x42400000, 0x08040000, 0x10040000, 0x22400000,
                                       0x08840000, 0x84240000, 0x44400000, 0x04840000, 0x44240000, 0x04240000, 0x40840000, 0x24240000,
                                       0x49240000, 0x81240000, 0x91240000, 0x89240000, 0x41240000, 0x01240000, 0x11240000, 0x21240000,
                                       0x04800000, 0x82240000, 0x92240000, 0x24400000, 0x42240000, 0x02240000, 0x12240000, 0x22240000,
                                       0x48240000, 0x80240000, 0x90240000, 0x88240000, 0x40240000, 0x08240000, 0x10240000, 0x20240000,
                                       0x44800000, 0x84440000, 0x92400000, 0x08900000, 0x44440000, 0x04440000, 0x12400000, 0x24440000,
                                       0x09040000, 0x84040000, 0x09100000, 0x09000000, 0x44040000, 0x04040000, 0x02400000, 0x24040000,
                                       0x04900000, 0x82440000, 0x92440000, 0x84800000, 0x42440000, 0x02440000, 0x12440000, 0x22440000,
                                       0x48440000, 0x80440000, 0x90440000, 0x88440000, 0x40440000, 0x08440000, 0x10440000, 0x20440000,
                                       0x44080000, 0x04080000, 0x84480000, 0x24080000, 0x44480000, 0x04480000, 0x40880000, 0x24480000,
                                       0x84080000, 0x84100000, 0x09240000, 0x09080000, 0x44100000, 0x04100000, 0x10880000, 0x24100000,
                                       0x04880000, 0x82480000, 0x92480000, 0x08880000, 0x42480000, 0x02480000, 0x12480000, 0x22480000,
                                       0x48480000, 0x80480000, 0x90480000, 0x88480000, 0x40480000, 0x08480000, 0x10480000, 0x20480000 };

//! \brief Potential merging bits (from MSB)
const U8 gca8MergingBits[ 4u ] = { 0x80, 0x40, 0x20, 0x00 };


//--------------------------------------------------------------------------------------------------------/
// Global variables
//--------------------------------------------------------------------------------------------------------/
U8 gau8CIRCDelayLines1[ 2 ][ 24 ];   // delay line for CIRC encoder
U8 gau8CIRCDelayLines2[ 28 ][ 28 ];  // delay line for CIRC encoder
U8 gau8CIRCDelayLines3[ 32 ];        // delay line for CIRC encoder


//--------------------------------------------------------------------------------------------------------/
// Static function declarations
//--------------------------------------------------------------------------------------------------------/
static void ScrambleSector( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static void MakeF1Sector( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static void CIRCEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static void AddSubcode( U8* pu8CIRCSector );
static void EFMEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static void EncodeNRZI( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static I32 CalculateDSV( U32 u32Word, U8 u8BitCount, BOOL* pbDirection );
static U32 AddMergingBits( U32 u32Word1, U32 u32Word2 );
static void WriteBitsToArray( U32 u32WordToWrite, U32 u32NumberofBits, U32 u32BitIndex, U8* pu8Array );


//--------------------------------------------------------------------------------------------------------/
// Static functions
//--------------------------------------------------------------------------------------------------------/
/*! *******************************************************************
 * \brief  Scrambles the contents of the sector according to ECMA-130 Annex B
 * \param  pu8ArrayToConvert: array of 2352 bytes!
 * \param  pu8ArrayResult: array of 2352 bytes!
 * \return -
 *********************************************************************/
static void ScrambleSector( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
{
  U16 u16Index;
  U8  u8BitIndex;
  U16 u16LFSR = 0x8001;
  U8  u8Temp;

  for( u16Index = 12; u16Index < CD_RAW_SECTOR_SIZE; u16Index++ )  // from byte 12 to 2351
  {
    u8Temp = 0;

    for( u8BitIndex = 0; u8BitIndex < 8; u8BitIndex++ )  //NOTE: there is a faster way: generate a 2340-byte long table using this LFSR, then just XOR all the bytes/words
    {
      u16LFSR = (u16LFSR^(u16LFSR>>1))>>1;
      if( u16LFSR & 1<<1 )
        u16LFSR ^= (1<<15);
      if( u16LFSR & 1<<0 )
        u16LFSR ^= ((1<<15) | (1<<14));
    }
    u8Temp = (pu8ArrayToConvert[ u16Index ]) ^ (u16LFSR&0x00FF);

    pu8ArrayResult[ u16Index ] = u8Temp;
  }
}

/*! *******************************************************************
 * \brief  Remaps the contents of a scrambled sector to an array of F1 frames
 * \param  pu8ArrayToConvert: array of 2352 bytes!
 * \param  pu8ArrayResult: array of 2352 bytes!
 * \return -
 * \note   Supports remapping in place.
 *********************************************************************/
static void MakeF1Sector( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
{
  U32  u32WordIndex;
  U32* pu32Src = (U32*)pu8ArrayToConvert;
  U32* pu32Dst = (U32*)pu8ArrayResult;

  // swap even and odd bytes
  for( u32WordIndex = 0; u32WordIndex < CD_RAW_SECTOR_SIZE/sizeof( U32 ); u32WordIndex++ )
  {
    pu32Dst[ u32WordIndex ] = ( ((pu32Src[ u32WordIndex ])&0x00FF00FF)<<8 ) | ( ((pu32Src[ u32WordIndex ])&0xFF00FF00)>>8 );
  }
}

/*! *******************************************************************
 * \brief  Encodes the F1 frames to F2 frames using CIRC
 * \param  pu8ArrayToConvert: array of 2352 bytes!
 * \param  pu8ArrayResult: array of 3234 bytes!
 * \return -
 *********************************************************************/
static void CIRCEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
{
  static U8 au8TempFrame[ 32 ];  // NOTE: used as a temporary array, probably can be removed after optimization
  U8  u8Index;
  U16 u16FrameNum;
  U8  u8Temp;
  U8  au8C2Parity[ 4 ];

  // Iterate for all F1 frames
  for( u16FrameNum = 0; u16FrameNum < 98; u16FrameNum++ )
  {
    // constructing temporary frame
    // first 12-byte block
    au8TempFrame[0]  = gau8CIRCDelayLines1[ 1 ][ 0 ];
    au8TempFrame[1]  = gau8CIRCDelayLines1[ 1 ][ 1 ];
    au8TempFrame[2]  = gau8CIRCDelayLines1[ 1 ][ 8 ];
    au8TempFrame[3]  = gau8CIRCDelayLines1[ 1 ][ 9 ];
    au8TempFrame[4]  = gau8CIRCDelayLines1[ 1 ][ 16 ];
    au8TempFrame[5]  = gau8CIRCDelayLines1[ 1 ][ 17 ];
    au8TempFrame[6]  = gau8CIRCDelayLines1[ 1 ][ 2 ];
    au8TempFrame[7]  = gau8CIRCDelayLines1[ 1 ][ 3 ];
    au8TempFrame[8]  = gau8CIRCDelayLines1[ 1 ][ 10 ];
    au8TempFrame[9]  = gau8CIRCDelayLines1[ 1 ][ 11 ];
    au8TempFrame[10] = gau8CIRCDelayLines1[ 1 ][ 18 ];
    au8TempFrame[11] = gau8CIRCDelayLines1[ 1 ][ 19 ];
    // second 12-byte block
    au8TempFrame[12] = pu8ArrayToConvert[ (24*u16FrameNum) + 4 ];
    au8TempFrame[13] = pu8ArrayToConvert[ (24*u16FrameNum) + 5 ];
    au8TempFrame[14] = pu8ArrayToConvert[ (24*u16FrameNum) + 12 ];
    au8TempFrame[15] = pu8ArrayToConvert[ (24*u16FrameNum) + 13 ];
    au8TempFrame[16] = pu8ArrayToConvert[ (24*u16FrameNum) + 20 ];
    au8TempFrame[17] = pu8ArrayToConvert[ (24*u16FrameNum) + 21 ];
    au8TempFrame[18] = pu8ArrayToConvert[ (24*u16FrameNum) + 6 ];
    au8TempFrame[19] = pu8ArrayToConvert[ (24*u16FrameNum) + 7 ];
    au8TempFrame[20] = pu8ArrayToConvert[ (24*u16FrameNum) + 14 ];
    au8TempFrame[21] = pu8ArrayToConvert[ (24*u16FrameNum) + 15 ];
    au8TempFrame[22] = pu8ArrayToConvert[ (24*u16FrameNum) + 22 ];
    au8TempFrame[23] = pu8ArrayToConvert[ (24*u16FrameNum) + 23 ];

    // first set of delay lines
    for( u8Index = 0; u8Index < 24; u8Index++ )  // NOTE: this can be optimized (not all elements of the array are used)
    {
      gau8CIRCDelayLines1[ 1 ][ u8Index ] = gau8CIRCDelayLines1[ 0 ][ u8Index ];
      gau8CIRCDelayLines1[ 0 ][ u8Index ] = pu8ArrayToConvert[ (24*u16FrameNum) + u8Index ];
    }

    // C2 encoding: RS(28,24)
    ReedSolomon_AddRS2824( au8TempFrame );
    // reorder: put the parity to the middle
    memcpy( au8C2Parity, &au8TempFrame[24], 4 );
    memmove( &au8TempFrame[16], &au8TempFrame[12], 12 );
    memcpy( &au8TempFrame[12], au8C2Parity, 4 );

    // second set of delay lines
    memcpy( gau8CIRCDelayLines2[ 0 ], au8TempFrame, 28 );  // 0. element: no delay
    for( u8Index = 1; u8Index < 28; u8Index++ )
    {
      memcpy( gau8CIRCDelayLines2[ u8Index ], gau8CIRCDelayLines2[ u8Index-1 ], 28 );
    }
    // constructing temporary frame
    for( u8Index = 0; u8Index < 28; u8Index++ )
    {
      au8TempFrame[ u8Index ] = gau8CIRCDelayLines2[ u8Index ][ u8Index ];
    }

    // C1 encoding: RS(32,28)
    ReedSolomon_AddRS3228( au8TempFrame );

    // third set of delay lines
    for( u8Index = 0; u8Index < 32; u8Index = u8Index+2 )
    {
      // every other bytes are delayed
      u8Temp = au8TempFrame[ u8Index ];
      au8TempFrame[ u8Index ] = gau8CIRCDelayLines3[ u8Index ];
      gau8CIRCDelayLines3[ u8Index ] = u8Temp;
    }

    // invert the parity bytes
    au8TempFrame[ 12 ] = ~au8TempFrame[ 12 ];
    au8TempFrame[ 13 ] = ~au8TempFrame[ 13 ];
    au8TempFrame[ 14 ] = ~au8TempFrame[ 14 ];
    au8TempFrame[ 15 ] = ~au8TempFrame[ 15 ];
    au8TempFrame[ 28 ] = ~au8TempFrame[ 28 ];
    au8TempFrame[ 29 ] = ~au8TempFrame[ 29 ];
    au8TempFrame[ 30 ] = ~au8TempFrame[ 30 ];
    au8TempFrame[ 31 ] = ~au8TempFrame[ 31 ];

    // frame is finished, write out
    memcpy( &(pu8ArrayResult[(33*u16FrameNum)+1]), au8TempFrame, 32 );  //NOTE: omits the place needed by subcode (that is placed at the first byte!)
  }
}

/*! *******************************************************************
 * \brief  Add subcode to CIRC encoded frame
 * \param  pu8CIRCSector: array of 3234 bytes!
 * \return -
 *********************************************************************/
static void AddSubcode( U8* pu8CIRCSector )
{
  U16 u16FrameNum;

  for( u16FrameNum = 0; u16FrameNum < 98; u16FrameNum++ )
  {
    pu8CIRCSector[ u16FrameNum*33 ] = 0;  //TODO: implement
    #warning "TODO: implement"
  }
}

/*! *******************************************************************
 * \brief  EFM encoding of channel frames
 * \param  pu8ArrayToConvert: array of 3234 bytes!
 * \param  pu8ArrayResult: array of 7203 bytes!
 * \return -
 *********************************************************************/
static void EFMEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
{
  U32 u32F3Frame;           // F3 frame number inside sector: 0...97
  U32 u32ByteIndex;         // byte index inside F3 frame: 0...32
  U32 u32BitIndex = 0u;     // range: 0...57623
  U32 u32EFM;               // temporary variable
  U32 u32NextWord;          // temporary variable

  // Each F3 frame (33 bytes) is converted to the following 588 bits:
  // -- Sync header: 24 bits
  // -- Merging bits: 3 bits
  // -- Control byte (first byte of the F3 frame): 14 bits
  // -- Merging bits: 3 bits
  // -- Data bytes (from the second byte of the F3 frame to the end: 32*14 = 544 bits
  for( u32F3Frame = 0u; u32F3Frame < 98u; u32F3Frame++ )
  {
    // Sync header (0x80100200) + merging bits
    u32EFM = AddMergingBits( 0x40080000u, cau32EFMCodeTable[ pu8ArrayToConvert[ u32F3Frame*33u ] ] );
    u32EFM = 0x80100200u | (u32EFM>>10u);
    WriteBitsToArray( u32EFM, 27u, u32BitIndex, pu8ArrayResult );
    u32BitIndex += 27u;
    // Control and data bytes without the last one
    for( u32ByteIndex = 0u; u32ByteIndex < 32u; u32ByteIndex++ )
    {
      // look up the code
      u32EFM = cau32EFMCodeTable[ pu8ArrayToConvert[ u32F3Frame*33u + u32ByteIndex ] ];
      u32NextWord = cau32EFMCodeTable[ pu8ArrayToConvert[ u32F3Frame*33u + u32ByteIndex + 1u ] ];
      // if it is a control word
      if( 0u == u32ByteIndex )
      {
        if( 0u == u32F3Frame )  // First F3 frame
        {
          u32EFM = 0x20040000u;  // SYNC0 code
        }
        else if( 1u == u32F3Frame )  // Second F3 frame
        {
          u32EFM = 0x00480000u;  // SYNC1 code
        }
        else
        {
          // the lookup table is used
        }
      }
      // Adding merging bits --> 17 bit result
      u32EFM = AddMergingBits( u32EFM, u32NextWord );
      // Write output
      WriteBitsToArray( u32EFM, 17u, u32BitIndex, pu8ArrayResult );
      u32BitIndex += 17u;
    }
    // Add last byte of the F3 frame and merging bits
    #warning "TODO: implement"
  }
}

/*! *******************************************************************
 * \brief  NRZI encoding of channel frames
 * \param  pu8ArrayToConvert: array of 7203 bytes!
 * \param  pu8ArrayResult: array of 7203 bytes!
 * \return -
 *********************************************************************/
static void EncodeNRZI( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
{
  static U8  u8State = 1;
  U16 u16Index;
  U8  u8BitIndex;

  for( u16Index = 0; u16Index < CD_CHANNEL_FRAME_SIZE; u16Index++ )
  {
    pu8ArrayResult[ u16Index ] = 0;
    for( u8BitIndex = 0; u8BitIndex < 8; u8BitIndex++ )  //TODO: probably there is a faster way to do this...
    {
      if( 0 != (pu8ArrayToConvert[ u16Index ] & (1<<u8BitIndex)) )
      {
        u8State ^= 1;
      }
      pu8ArrayResult[ u16Index ] |= u8State<<u8BitIndex;
    }
  }
}

/*! *******************************************************************
 * \brief  Calculate the digital sum value (DSV) of the given word
 * \param  u32Word: the word
 * \param  u8BitCount: number of bits used (from MSB)
 * \param  pbDirection: TRUE, if the direction was increment
 * \return Digital sum value of the given word
 *********************************************************************/
static I32 CalculateDSV( U32 u32Word, U8 u8BitCount, BOOL* pbDirection )
{
  I32 i32DSV = 0;
  U8  u8BitIndex;

  for( u8BitIndex = 0; u8BitIndex < u8BitCount; u8BitIndex++ )
  {
    if( 0u != ( u32Word & ( 1u<<(31u-u8BitIndex) ) ) )  // if 1
    {
      *pbDirection = (TRUE == *pbDirection) ? FALSE : TRUE;
    }
    i32DSV += (TRUE == *pbDirection) ? 1 : -1;
  }

  return i32DSV;
}

/*! *******************************************************************
 * \brief  Adds merging bits between two 14-bit wide words
 * \param  u32Word1: first word
 * \param  u32Word2: second word
 * \return The first word with added merging bits (17 bits)
 *********************************************************************/
static U32 AddMergingBits( U32 u32Word1, U32 u32Word2 )
{
  static I32  i32DSV = 0;  // DSV of the whole disk
  static BOOL bDirection = TRUE;
  BOOL        bFinalDirection[ (sizeof( gca8MergingBits )/sizeof( U8 )) ];
  U32  u32Return;
  U32  u32Temp;
  I32  i32Temp;
  U8   u8Index;
  U8   u8LeadingZeros;
  U8   u8Zeros;
  U8   au8PotentialMergingBits[ (sizeof( gca8MergingBits )/sizeof( U8 )) ];
  BOOL abValidMergingBits[ (sizeof( gca8MergingBits )/sizeof( U8 )) ];
  I32  ai32DSVWithMergingBits[ (sizeof( gca8MergingBits )/sizeof( U8 )) ];

  // NOTE: this function could be optimized

  (void)memcpy( au8PotentialMergingBits, gca8MergingBits, sizeof( gca8MergingBits ) );

  // init bool array
  for( u8Index = 0u; u8Index < (sizeof( gca8MergingBits )/sizeof( U8 )); u8Index++ )
  {
    abValidMergingBits[ u8Index ] = TRUE;
  }

  // calculate the leading zeros for u32Word1
  u8LeadingZeros = 0u;
  for( u8Index = 0u; u8Index < 14u; u8Index++ )
  {
    if( 0u == ( u32Word1 & ( 1u<<(31u-u8Index) ) ) )  // if the given bit is zero
    {
      u8LeadingZeros++;
    }
    else  // the given bit is 1
    {
      u8LeadingZeros = 0u;
    }
  }

  // test the potential merging bits
  for( u8Index = 0u; u8Index < (sizeof( gca8MergingBits )/sizeof( U8 )); u8Index++ )
  {
    u8Zeros = u8LeadingZeros;
    if( 0u != ( au8PotentialMergingBits[ u8Index ] & 0x80u ) )  // 1
    {
      if( u8Zeros < 2 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    else  // 0
    {
      u8Zeros++;
      if( u8Zeros > 10 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    //-----------
    if( 0u != ( au8PotentialMergingBits[ u8Index ] & 0x40u ) )  // 1
    {
      if( u8Zeros < 2 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    else  // 0
    {
      u8Zeros++;
      if( u8Zeros > 10 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    //-----------
    if( 0u != ( au8PotentialMergingBits[ u8Index ] & 0x20u ) )  // 1
    {
      if( u8Zeros < 2 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    else  // 0
    {
      u8Zeros++;
      if( u8Zeros > 10 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
  }

  //-------------------------------------------------------------
  // calculate the leading zeros from the back for u32Word2
  u8LeadingZeros = 0u;
  for( u8Index = 0u; u8Index < 14u; u8Index++ )
  {
    if( 0u == ( u32Word2 & ( 1u<<(18u+u8Index) ) ) )  // if the given bit is zero
    {
      u8LeadingZeros++;
    }
    else  // the given bit is 1
    {
      u8LeadingZeros = 0u;
    }
  }

  // test the potential merging bits
  for( u8Index = 0u; u8Index < (sizeof( gca8MergingBits )/sizeof( U8 )); u8Index++ )
  {
    u8Zeros = u8LeadingZeros;
    if( 0u != ( au8PotentialMergingBits[ u8Index ] & 0x20u ) )  // 1
    {
      if( u8Zeros < 2 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    else  // 0
    {
      u8Zeros++;
      if( u8Zeros > 10 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    //-----------
    if( 0u != ( au8PotentialMergingBits[ u8Index ] & 0x40u ) )  // 1
    {
      if( u8Zeros < 2 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    else  // 0
    {
      u8Zeros++;
      if( u8Zeros > 10 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    //-----------
    if( 0u != ( au8PotentialMergingBits[ u8Index ] & 0x80u ) )  // 1
    {
      if( u8Zeros < 2 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
    else  // 0
    {
      u8Zeros++;
      if( u8Zeros > 10 )
      {
        abValidMergingBits[ u8Index ] = FALSE;
      }
    }
  }

  //-------------------------------------------------
  // Calculate DSV for each combination
  for( u8Index = 0u; u8Index < (sizeof( gca8MergingBits )/sizeof( U8 )); u8Index++ )
  {
    u32Temp = u32Word1 | ((U32)au8PotentialMergingBits[ u8Index ]<<10) | (u32Word2>>17);
    if( 0x80100200u == u32Temp )  // if adding the merging bits erroneously results in a sync header
    {
      // invalid combination
      abValidMergingBits[ u8Index ] = FALSE;
    }
    else
    {
      bFinalDirection[ u8Index ] = bDirection;
      ai32DSVWithMergingBits[ u8Index ] = CalculateDSV( u32Temp, 31u, &bFinalDirection[ u8Index ] );
    }
  }

  // Select the one valid combination which results the lowest absolute DSV
  i32Temp = -2147483647;
  for( u8Index = 0u; u8Index < (sizeof( gca8MergingBits )/sizeof( U8 )); u8Index++ )
  {
    if( TRUE == abValidMergingBits[ u8Index ] )
    {
      // If two combinations give the same, lowest DSV, a combination with a transition shall be chosen.
      if( ( abs(i32Temp) == abs(i32DSV + ai32DSVWithMergingBits[ u8Index ]) ) && ( 0u != au8PotentialMergingBits[ u8Index ] ) )
      {
        bDirection = bFinalDirection[ u8Index ];
        u32Return = u32Word1 | ((U32)au8PotentialMergingBits[ u8Index ]<<10);
      }
      else if( abs(i32DSV + ai32DSVWithMergingBits[ u8Index ]) < abs(i32Temp) )
      {
        bDirection = bFinalDirection[ u8Index ];
        i32Temp = ai32DSVWithMergingBits[ u8Index ] + i32DSV;
        u32Return = u32Word1 | ((U32)au8PotentialMergingBits[ u8Index ]<<10);
      }
    }
  }
  i32DSV = i32Temp;
  return u32Return;
}

/*! *******************************************************************
 * \brief  Writes the given bits to an array
 * \param  u32WordToWrite: bits to write
 * \param  u32NumberofBits: number of bits from MSB
 * \param  u32BitIndex: index of the first bit to write in the array
 * \param  *pu8Array: array to write
 * \return -
 *********************************************************************/
static void WriteBitsToArray( U32 u32WordToWrite, U32 u32NumberofBits, U32 u32BitIndex, U8* pu8Array )
{
  U32 u32ArrayIndex = u32BitIndex/8;
  U8  u8ArrayBitOffset = u32BitIndex%8;
  U8  u8ExistingElement = pu8Array[ u32ArrayIndex ];
  U8  u8Index;

  (void)memset( &pu8Array[ u32ArrayIndex ], 0, ceil( (u32NumberofBits + u8ArrayBitOffset) / 8.0f ) );
  pu8Array[ u32ArrayIndex ] = u8ExistingElement & ~(0xFFu>>u8ArrayBitOffset);

  for( u8Index = 0; u8Index < ceil( (u32NumberofBits + u8ArrayBitOffset) / 8.0f ); u8Index++ )
  {
    pu8Array[ u32ArrayIndex + u8Index ] |= u32WordToWrite >> ( 24u + u8ArrayBitOffset - 8u*u8Index );
  }
}

//--------------------------------------------------------------------------------------------------------/
// Public functions
//--------------------------------------------------------------------------------------------------------/
/*! *******************************************************************
 * \brief  Convert CD sector binary to pits and lands
 * \param  pu8ArrayToConvert: array of 2352 bytes!
 * \param  pu8ArrayResult: array of 7203 bytes!
 * \return -
 *********************************************************************/
void BinConvert_CDSector2352( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
{
  static U8 au8EFMEncoded[ CD_CHANNEL_FRAME_SIZE ];   // temporary array; won't needed after optimization
  static U8 au8CIRCEncoded[ CD_FRAME_SIZE ];  // temporary array; won't needed after optimization
  static U8 au8F1Frames[ CD_RAW_SECTOR_SIZE ];

  //NOTE: these two can be merged into one optimized function
  ScrambleSector( pu8ArrayToConvert, pu8ArrayToConvert );  // scramble sector in place
  MakeF1Sector( pu8ArrayToConvert, au8F1Frames );  // make F1 frames from sector

  CIRCEncoder( au8F1Frames, au8CIRCEncoded );
  AddSubcode( au8CIRCEncoded );

  EFMEncoder( au8CIRCEncoded, au8EFMEncoded );
  EncodeNRZI( au8EFMEncoded, pu8ArrayResult );  // NRZI encoding of channel frames
}


/*! *******************************************************************
 * \brief  Init the library
 * \param  -
 * \return -
 *********************************************************************/
void BinConvert_Init( void )
{
  memset( gau8CIRCDelayLines1, 0, sizeof(gau8CIRCDelayLines1) );
  memset( gau8CIRCDelayLines2, 0, sizeof(gau8CIRCDelayLines2) );
#warning "TODO: initialize all static values too"

}

/******************************<EOF>**********************************/

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
#include <string.h>
#include "types.h"


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
//! \brief GF(256) field exponential lookup table
const U8 cau8Alpha[ 256 ] = {   1,   2,   4,   8,  16,  32,  64, 128,  29,  58, 116, 232, 205, 135,  19,  38,
                               76, 152,  45,  90, 180, 117, 234, 201, 143,   3,   6,  12,  24,  48,  96, 192,
                              157,  39,  78, 156,  37,  74, 148,  53, 106, 212, 181, 119, 238, 193, 159,  35,
                               70, 140,   5,  10,  20,  40,  80, 160,  93, 186, 105, 210, 185, 111, 222, 161,
                               95, 190, 97,  194, 153,  47,  94, 188, 101, 202, 137,  15,  30,  60, 120, 240,
                              253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163  ,91, 182, 113, 226,
                              217, 175,  67, 134,  17,  34,  68, 136,  13,  26,  52, 104, 208, 189, 103, 206,
                              129,  31,  62, 124, 248, 237, 199, 147,  59, 118, 236, 197, 151,  51, 102, 204,
                              133,  23,  46,  92, 184, 109, 218, 169,  79, 158,  33,  66, 132,  21,  42,  84,
                              168,  77, 154,  41,  82, 164,  85, 170,  73, 146,  57, 114, 228, 213, 183, 115,
                              230, 209, 191,  99, 198, 145,  63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
                              227, 219, 171,  75, 150,  49,  98, 196, 149,  55, 110, 220, 165,  87, 174,  65,
                              130,  25,  50, 100, 200, 141,   7,  14,  28,  56, 112, 224, 221, 167,  83, 166,
                               81, 162,  89, 178, 121, 242, 249, 239, 195, 155,  43,  86, 172,  69, 138,   9,
                               18,  36,  72, 144,  61, 122, 244, 245, 247, 243, 251, 235, 203, 139,  11,  22,
                               44,  88, 176, 125, 250, 233, 207, 131,  27,  54, 108, 216, 173,  71,  142,  1 };

//! \brief GF(256) field logarithmic lookup table
const U8 cau8IdxofAlpha[ 256 ] = {   0,   0,   1,  25,   2,  50,  26, 198,   3, 223,  51, 238,  27, 104, 199,  75,
                                     4, 100, 224,  14,  52, 141, 239, 129,  28, 193, 105, 248, 200,   8,  76, 113,
                                     5, 138, 101,  47, 225,  36,  15,  33,  53, 147, 142, 218, 240,  18, 130,  69,
                                    29, 181, 194, 125, 106,  39, 249, 185, 201, 154,   9, 120,  77, 228, 114, 166,
                                     6, 191, 139,  98, 102, 221,  48, 253, 226, 152,  37, 179,  16, 145,  34, 136,
                                    54, 208, 148, 206, 143, 150, 219, 189, 241, 210,  19,  92, 131,  56,  70,  64,
                                    30,  66, 182, 163, 195,  72, 126, 110, 107,  58,  40,  84, 250, 133, 186,  61,
                                   202,  94, 155, 159,  10,  21, 121,  43,  78, 212, 229, 172, 115, 243, 167,  87,
                                     7, 112, 192, 247, 140, 128,  99,  13, 103,  74, 222, 237,  49, 197, 254,  24,
                                   227, 165, 153, 119,  38, 184, 180, 124,  17,  68, 146, 217,  35,  32, 137,  46,
                                    55,  63, 209,  91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190,  97,
                                   242,  86, 211, 171,  20,  42,  93, 158, 132,  60,  57,  83,  71, 109,  65, 162,
                                    31,  45,  67, 216, 183, 123, 164, 118, 196,  23,  73, 236, 127,  12, 111, 246,
                                   108, 161,  59,  82,  41, 157,  85, 170, 251,  96, 134, 177, 187, 204,  62,  90,
                                   203,  89,  95, 176, 156, 169, 160,  81,  11, 245,  22, 235, 122, 117,  44, 215,
                                    79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168,  80,  88, 175 };

#warning "ˇˇI'm not sure about these values."
const U8 cau8C2Generator[ 5 ] = { 1, 30, 216, 231, 116 };  //!< Generator polynomial of the C2 encoder
const U8 cau8C1Generator[ 5 ] = { 1, 30, 216, 231, 116 };  //!< Generator polynomial of the C1 encoder


//--------------------------------------------------------------------------------------------------------/
// Global variables
//--------------------------------------------------------------------------------------------------------/
U8 gau8CIRCDelayLines1[ 2 ][ 12 ];   // delay line for CIRC encoder
U8 gau8CIRCDelayLines2[ 28 ][ 28 ];  // delay line for CIRC encoder
U8 gau8CIRCDelayLines3[ 16 ];        // delay line for CIRC encoder


//--------------------------------------------------------------------------------------------------------/
// Static function declarations
//--------------------------------------------------------------------------------------------------------/
static void ScrambleSector( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static void MakeF1Sector( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static inline U8 GF256Mul( U8 u8N, U8 u8M );
static void C2Encoder( U8* pu8TempFrame );
static void C1Encoder( U8* pu8TempFrame );
static void CIRCEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static void AddSubcode( U8* pu8CIRCSector );
static void EFMEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
static void EncodeNRZI( U8* pu8ArrayToConvert, U8* pu8ArrayResult );


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

        for( u8BitIndex = 0; u8BitIndex < 8; u8BitIndex++ )  //TODO: there is a faster way: generate a 2340-byte long table using this LFSR, then just XOR all the bytes/words
        {
            u16LFSR = (u16LFSR^(u16LFSR>>1))>>1;
            if( u16LFSR & 1<<1 ) u16LFSR ^= (1<<15);
            if( u16LFSR & 1<<0 ) u16LFSR ^= ((1<<15) | (1<<14));

            //u8Temp |= ((pu8ArrayToConvert[ u16Index ]&&(0x01<<u8BitIndex))^((u16LFSR&0x8000)>>15))<<u8BitIndex;
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
 *********************************************************************/
static void MakeF1Sector( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
{
    U16 u16FrameIndex;
    U8  u8ByteIndex;
    U8  u8Temp;

    for( u16FrameIndex = 1; u16FrameIndex < CD_RAW_SECTOR_SIZE/24; u16FrameIndex++ )  // 98 F1 frames, first frame is sync, it won't be scrambled
    {
        for( u8ByteIndex = 0; u8ByteIndex < 24; u8ByteIndex += 2 )  // TODO: it would be faster using 32-bit masking and shifting
        {
            u8Temp = pu8ArrayToConvert[ 24*u16FrameIndex + u8ByteIndex+1 ];
            pu8ArrayResult[ (24*u16FrameIndex + (u8ByteIndex))+1 ] = pu8ArrayToConvert[ 24*u16FrameIndex + u8ByteIndex ];
            pu8ArrayResult[ (24*u16FrameIndex + (u8ByteIndex)) ] = u8Temp;
        }
    }
}

/*! *******************************************************************
 * \brief  Multiplication on GF(256) finite field
 * \param  u8N: multiplicand > 0
 * \param  u8M: multiplier > 0
 * \return Result of multiplication
 *********************************************************************/
static inline U8 GF256Mul( U8 u8N, U8 u8M )
{
    // TODO: this function can be optimized by constructing an 65536-element table for looking up ((U16)u8N<<8 + u8M), therefore reducing this function to one lookup
    U8   u8Alpha1 = cau8IdxofAlpha[ u8N ];
    U8   u8Alpha2 = cau8IdxofAlpha[ u8M ];
    U16 u16ResIdx = u8Alpha1 + u8Alpha2;

    if( u16ResIdx >= 255 ) u16ResIdx -= 255;  // note: cau8Alpha[ 255 ] == cau8Alpha[ 0 ] == 1

    return cau8Alpha[ u16ResIdx & 0xFF ];
}

/*! *******************************************************************
 * \brief  RS(28,24) encoder
 * \param  pu8TempFrame: array of 28 bytes.
 * \return -
 *********************************************************************/
static void C2Encoder( U8* pu8TempFrame )
{
    U8 u8Temp[ 28 ];
    U8 u8Multiplier;
    U8 u8Index, u8InnerIdx;

    memcpy( u8Temp, pu8TempFrame, 12 );
    memcpy( &(u8Temp[12]), &(pu8TempFrame[16]), 12 );
    memset( &(u8Temp[24]), 0, 4 );

    // Polynomial division based parity calculation
    for( u8Index = 0; u8Index < 24; u8Index++ )
    {
        u8Multiplier = u8Temp[ u8Index ];

        if( 0 == u8Multiplier )
        {
            continue;
        }

        for( u8InnerIdx = 0; u8InnerIdx < 5; u8InnerIdx++ )
        {
            u8Temp[ u8Index + u8InnerIdx ] ^= GF256Mul( u8Multiplier, cau8C2Generator[ u8InnerIdx ] );
        }
    }

    pu8TempFrame[12] = u8Temp[ 24 ];
    pu8TempFrame[13] = u8Temp[ 25 ];
    pu8TempFrame[14] = u8Temp[ 26 ];
    pu8TempFrame[15] = u8Temp[ 27 ];
}

/*! *******************************************************************
 * \brief  RS(32,28) encoder
 * \param  pu8TempFrame: array of 32 bytes.
 * \return -
 *********************************************************************/
static void C1Encoder( U8* pu8TempFrame )
{
    U8 u8Temp[ 32 ];
    U8 u8Multiplier;
    U8 u8Index, u8InnerIdx;

    memcpy( u8Temp, pu8TempFrame, 24 );
    memset( &(u8Temp[28]), 0, 4 );

    // Polynomial division based parity calculation
    for( u8Index = 0; u8Index < 28; u8Index++ )
    {
        u8Multiplier = u8Temp[ u8Index ];

        if( 0 == u8Multiplier )
        {
            continue;
        }

        for( u8InnerIdx = 0; u8InnerIdx < 5; u8InnerIdx++ )
        {
            u8Temp[ u8Index + u8InnerIdx ] ^= GF256Mul( u8Multiplier, cau8C1Generator[ u8InnerIdx ] );
        }
    }

    pu8TempFrame[28] = u8Temp[ 28 ];
    pu8TempFrame[29] = u8Temp[ 29 ];
    pu8TempFrame[30] = u8Temp[ 30 ];
    pu8TempFrame[31] = u8Temp[ 31 ];
}

/*! *******************************************************************
 * \brief  Encodes the F1 frames to F2 frames using CIRC
 * \param  pu8ArrayToConvert: array of 2352 bytes!
 * \param  pu8ArrayResult: array of 3234 bytes!
 * \return -
 *********************************************************************/
static void CIRCEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult )  //TODO: needs optimization
{
    static U8 au8TempFrame[ 32 ];  // probably can be removed after optimization
    U8  u8Index;
    U8  u8InnerIndex;
    U16 u16FrameNum;
    U8  u8Temp;

    for( u16FrameNum = 0; u16FrameNum < 98; u16FrameNum++ )
    {
        // constructing temporary frame
        au8TempFrame[0]  = gau8CIRCDelayLines1[ 1 ][ 0 ];
        au8TempFrame[1]  = gau8CIRCDelayLines1[ 1 ][ 1 ];
        au8TempFrame[2]  = gau8CIRCDelayLines1[ 1 ][ 4 ];
        au8TempFrame[3]  = gau8CIRCDelayLines1[ 1 ][ 5 ];
        au8TempFrame[4]  = gau8CIRCDelayLines1[ 1 ][ 8 ];
        au8TempFrame[5]  = gau8CIRCDelayLines1[ 1 ][ 9 ];
        au8TempFrame[6]  = gau8CIRCDelayLines1[ 1 ][ 2 ];
        au8TempFrame[7]  = gau8CIRCDelayLines1[ 1 ][ 3 ];
        au8TempFrame[8]  = gau8CIRCDelayLines1[ 1 ][ 6 ];
        au8TempFrame[9]  = gau8CIRCDelayLines1[ 1 ][ 7 ];
        au8TempFrame[10] = gau8CIRCDelayLines1[ 1 ][ 10 ];
        au8TempFrame[11] = gau8CIRCDelayLines1[ 1 ][ 11 ];
        au8TempFrame[12] = 0;  // C2 parity byte
        au8TempFrame[13] = 0;  // C2 parity byte
        au8TempFrame[14] = 0;  // C2 parity byte
        au8TempFrame[15] = 0;  // C2 parity byte
        au8TempFrame[16] = pu8ArrayToConvert[ (24*u16FrameNum) + 4 ];
        au8TempFrame[17] = pu8ArrayToConvert[ (24*u16FrameNum) + 5 ];
        au8TempFrame[18] = pu8ArrayToConvert[ (24*u16FrameNum) + 12 ];
        au8TempFrame[19] = pu8ArrayToConvert[ (24*u16FrameNum) + 13 ];
        au8TempFrame[20] = pu8ArrayToConvert[ (24*u16FrameNum) + 20 ];
        au8TempFrame[21] = pu8ArrayToConvert[ (24*u16FrameNum) + 21 ];
        au8TempFrame[22] = pu8ArrayToConvert[ (24*u16FrameNum) + 6 ];
        au8TempFrame[23] = pu8ArrayToConvert[ (24*u16FrameNum) + 7 ];
        au8TempFrame[24] = pu8ArrayToConvert[ (24*u16FrameNum) + 14 ];
        au8TempFrame[25] = pu8ArrayToConvert[ (24*u16FrameNum) + 15 ];
        au8TempFrame[26] = pu8ArrayToConvert[ (24*u16FrameNum) + 22 ];
        au8TempFrame[27] = pu8ArrayToConvert[ (24*u16FrameNum) + 23 ];

        // C2 encoding: RS(28,24)
        C2Encoder( au8TempFrame );

        // second set of delay lines
        for( u8Index = 0; u8Index < 28; u8Index++ )
        {
            for( u8InnerIndex = 1; u8InnerIndex <= u8Index; u8InnerIndex++ )
            {
                gau8CIRCDelayLines2[ u8Index ][ u8InnerIndex-1 ] = gau8CIRCDelayLines2[ u8Index ][ u8InnerIndex ];
            }
            gau8CIRCDelayLines2[ u8Index ][ u8Index ] = au8TempFrame[ u8Index ];
        }

        // constructing temporary frame
        for( u8Index = 0; u8Index < 28; u8Index++ )  //TODO: could be simplified if gau8CIRCDelayLines2[][] had their array indexes the other way
        {
            au8TempFrame[ u8Index ] = gau8CIRCDelayLines2[ u8Index ][ 0 ];
        }

        // C1 encoding: RS(32,28)
        C1Encoder( au8TempFrame );

        // third set of delay lines
        for( u8Index = 0; u8Index < 16; u8Index++ )
        {
            u8Temp = au8TempFrame[ 2*u8Index ];
            au8TempFrame[ 2*u8Index ] = gau8CIRCDelayLines3[ u8Index ];
            gau8CIRCDelayLines3[ u8Index ] = u8Temp;
        }

        // invert parity bytes
        au8TempFrame[ 12 ] = ~au8TempFrame[ 12 ];
        au8TempFrame[ 13 ] = ~au8TempFrame[ 13 ];
        au8TempFrame[ 14 ] = ~au8TempFrame[ 14 ];
        au8TempFrame[ 15 ] = ~au8TempFrame[ 15 ];
        au8TempFrame[ 28 ] = ~au8TempFrame[ 28 ];
        au8TempFrame[ 29 ] = ~au8TempFrame[ 29 ];
        au8TempFrame[ 30 ] = ~au8TempFrame[ 30 ];
        au8TempFrame[ 31 ] = ~au8TempFrame[ 31 ];

        // first set of delay lines
        for( u8Index = 0; u8Index < 12; u8Index++ )
        {
            gau8CIRCDelayLines1[ 1 ][ u8Index ] = gau8CIRCDelayLines1[ 0 ][ u8Index ];
            gau8CIRCDelayLines1[ 0 ][ u8Index ] = pu8ArrayToConvert[ (24*u16FrameNum) + ((u8Index/4) * 8 ) + (u8Index%4) ];  // magic: u8Index/4 will be rounded downwards
        }

        // frame is finished, write out
        memcpy( &(pu8ArrayResult[(33*u16FrameNum)]), au8TempFrame, 32 );  //NOTE: omits the place needed by subcode
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
        pu8CIRCSector[ u16FrameNum*33 + 32 ] = 0;  //TODO: implement
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
    //FIXME: implement
}

/*! *******************************************************************
 * \brief  NRZI encoding of channel frames
 * \param  pu8ArrayToConvert: array of 7203 bytes!
 * \param  pu8ArrayResult: array of 7203 bytes!
 * \return -
 *********************************************************************/
static void EncodeNRZI( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
{
    U16 u16Index;
    U8  u8BitIndex;
    U8  u8State = 1;

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
    static U8 au8CIRCEncoded[ 3234 ];  // temporary array; won't needed after optimization

    //NOTE: these two can be merged into one optimized function
    ScrambleSector( pu8ArrayToConvert, pu8ArrayToConvert );  // scramble sector in place
    MakeF1Sector( pu8ArrayToConvert, pu8ArrayToConvert );  // make F1 frames from sector in place

    CIRCEncoder( pu8ArrayToConvert, au8CIRCEncoded );

    AddSubcode( au8CIRCEncoded );

    //TODO: EFM encoding + sync header + merging bits

    //EncodeNRZI( pu8ArrayToConvert, pu8ArrayResult );  // NRZI encoding of channel frames
    memcpy( pu8ArrayResult, au8CIRCEncoded, 3234 );  //FIXME: debug, delete
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

}

/******************************<EOF>**********************************/

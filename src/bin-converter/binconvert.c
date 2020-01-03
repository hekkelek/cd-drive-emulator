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
        ReedSolomon_AddRS2824( au8TempFrame );

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
        ReedSolomon_AddRS3228( au8TempFrame );

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

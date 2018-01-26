/*! *******************************************************************************************************
* Copyright (c) 2017 Horváth Kristóf Szabolcs
*
* Minden jog fenntartva
*
* \file binconvert.c
*
* \brief Binary to raw CD converter functions
*
* \author Horváth Kristóf Sz.
*
**********************************************************************************************************/

//--------------------------------------------------------------------------------------------------------/
// Include files
//--------------------------------------------------------------------------------------------------------/
#include <string.h>
#include "types.h"


//--------------------------------------------------------------------------------------------------------/
// Definíciók
//--------------------------------------------------------------------------------------------------------/
#define CD_RAW_SECTOR_SIZE    2352u
#define CD_FRAME_SIZE         3234u
#define CD_CHANNEL_FRAME_SIZE 7203u

//--------------------------------------------------------------------------------------------------------/
// Típusok
//--------------------------------------------------------------------------------------------------------/


//--------------------------------------------------------------------------------------------------------/
// Globális változók
//--------------------------------------------------------------------------------------------------------/
U8 gau8CIRCDelayLines1[ 2 ][ 12 ];   // delay line for CIRC encoder
U8 gau8CIRCDelayLines2[ 28 ][ 28 ];  // delay line for CIRC encoder
U8 gau8CIRCDelayLines3[ 16 ];        // delay line for CIRC encoder


//--------------------------------------------------------------------------------------------------------/
// Belsõ függvények deklarációi
//--------------------------------------------------------------------------------------------------------/
void ScrambleSector( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
void MakeF1Sector( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
void C2Encoder( U8* pu8TempFrame );
void C1Encoder( U8* pu8TempFrame );
void CIRCEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
void EncodeNRZI( U8* pu8ArrayToConvert, U8* pu8ArrayResult );


//--------------------------------------------------------------------------------------------------------/
// Belsõ függvények
//--------------------------------------------------------------------------------------------------------/
/*! *******************************************************************
 * \brief  Scrambles the contents of the sector according to ECMA-130 Annex B
 * \param  pu8ArrayToConvert: array of 2352 bytes!
 * \param  pu8ArrayResult: array of 2352 bytes!
 * \return -
 *********************************************************************/
void ScrambleSector( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
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
void MakeF1Sector( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
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
 * \brief  RS(28,24) encoder
 * \param  pu8TempFrame: array of 28 bytes.
 * \return -
 *********************************************************************/
void C2Encoder( U8* pu8TempFrame )
{
    //FIXME: implement encoder
    pu8TempFrame[12] = 0;
    pu8TempFrame[13] = 0;
    pu8TempFrame[14] = 0;
    pu8TempFrame[15] = 0;
}

/*! *******************************************************************
 * \brief  RS(32,28) encoder
 * \param  pu8TempFrame: array of 32 bytes.
 * \return -
 *********************************************************************/
void C1Encoder( U8* pu8TempFrame )
{
    //FIXME: implement encoder
    pu8TempFrame[28] = 0;
    pu8TempFrame[29] = 0;
    pu8TempFrame[30] = 0;
    pu8TempFrame[31] = 0;
}

/*! *******************************************************************
 * \brief  Encodes the F1 frames to F2 frames using CIRC
 * \param  pu8ArrayToConvert: array of 2352 bytes!
 * \param  pu8ArrayResult: array of 3234 bytes!
 * \return -
 *********************************************************************/
void CIRCEncoder( U8* pu8ArrayToConvert, U8* pu8ArrayResult )  //TODO: needs optimization
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
 * \brief  NRZI encoding of channel frames
 * \param  pu8ArrayToConvert: array of 7203 bytes!
 * \param  pu8ArrayResult: array of 7203 bytes!
 * \return -
 *********************************************************************/
void EncodeNRZI( U8* pu8ArrayToConvert, U8* pu8ArrayResult )
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
// Kívülrõl hívható függvények
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

    //TODO: add control byte
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

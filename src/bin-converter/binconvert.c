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


//--------------------------------------------------------------------------------------------------------/
// Belsõ függvények deklarációi
//--------------------------------------------------------------------------------------------------------/
void ScrambleSector( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
void MakeF1Sector( U8* pu8ArrayToConvert, U8* pu8ArrayResult );
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
    ScrambleSector( pu8ArrayToConvert, pu8ArrayToConvert );  // scramble sector in place
    MakeF1Sector( pu8ArrayToConvert, pu8ArrayToConvert );  // make F1 frames from sector in place

    //TODO: CIRC encoding
    //TODO: add control byte
    //TODO: EFM encoding + sync header + merging bits

    //EncodeNRZI( pu8ArrayToConvert, pu8ArrayResult );  // NRZI encoding of channel frames
    memcpy( pu8ArrayResult, pu8ArrayToConvert, 2352 );  //FIXME: debug, töröld
}


/*! *******************************************************************
 * \brief
 * \param
 * \return
 *********************************************************************/


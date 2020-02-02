#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "binconvert.h"
#include "crc.h"


#define LEAD_IN_SECTORS   500
#define LEAD_OUT_SECTORS  500



FILE* frp, *fwp;

U8 au8Sector[ 2352 ];
U8 au8PitsnLands[ 7203 ];

U8 BCD( U8 u8Data )
{
  return ( (u8Data/10)<<8 ) | ( u8Data % 10 );
}

int main()
{
    U16 u16ReadBytes;
    U32 u32Size;
    U32 u32SectorCounter, u32AbsSectorCounter = 0;
    S_CD_SUBCODE sSubcode;




    // CRC test
/*
    memset( &sSubcode, 0x00u, sizeof( sSubcode ) );
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u4Control = 0x0;  // 2 audio channels without pre-emphasis
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u4Address = 0x1;  // mode 1
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x00;  // track number: 0...99 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // pointer: 0...99
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( u32AbsSectorCounter / 4500u );  // p. minutes: 0...99 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = BCD( u32AbsSectorCounter / 75u );  // p. seconds: 0...59 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = BCD( u32AbsSectorCounter % 75u );  // p. frames: 0...74 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = 0;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );
    CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );
*/
   memset( &sSubcode, 0x00u, sizeof( sSubcode ) );
   ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = 0xFFFF;
   ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );
   CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );





    printf("CD binary converter test\n");

    memset( &sSubcode, 0x00u, sizeof( sSubcode ) );  // non-used subcode bits are 0
    BinConvert_Init();

    frp = fopen( "testsound.bin", "rb" );
    fwp = fopen( "output.bin","wb");

    if( !frp )
    {
        printf("Error: file not found!");
        return -1;
    }

    // get file size
    fseek( frp, 0l, SEEK_END );
    u32Size = ftell( frp );
    rewind( frp );

    //--------------------------------------------------------------------------
    // Add lead-in track
    (void)memset( au8Sector, 0, sizeof( au8Sector ) );
    memset( &sSubcode, 0x00u, sizeof( sSubcode ) );
    for( u32SectorCounter = 0; u32SectorCounter < LEAD_IN_SECTORS; u32SectorCounter++ )
    {
      // Set P subcode
      memset( sSubcode.au96SubcodeP, 0x00, sizeof( sSubcode.au96SubcodeP ) );
      // Set Q-subcodes
      if( u32SectorCounter < 75 )
      {
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u4Control = 0x0;  // 2 audio channels without pre-emphasis
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u4Address = 0x1;  // mode 1
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x00;  // track number: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // pointer: 0...99
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( u32AbsSectorCounter / 4500u );  // p. minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = BCD( u32AbsSectorCounter / 75u );  // p. seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = BCD( u32AbsSectorCounter % 75u );  // p. frames: 0...74 (BCD)
      }
      else if( u32SectorCounter < 150 )
      {
        // Table of Contents
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0xA0;  // 0xA0 == first information track data
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // pointer: 0...99
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = 0x01;  // p. minutes: first track number
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x00;  // p. seconds: 0x00 in this case
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x00;  // p. frames:  0x00 in this case
      }
      else if( u32SectorCounter < 225 )
      {
        // Table of Contents
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0xA1;  // 0xA1 == last information track data
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // pointer: 0...99
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = 0x01;  // p. minutes: last track number
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x00;  // p. seconds: 0x00 in this case
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x00;  // p. frames:  0x00 in this case
      }
      else if( u32SectorCounter < 300 )
      {
        // Table of Contents
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0xA2;  // 0xA0 == beginning of the lead out track
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // pointer: 0...99
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( ((u32Size/2352)+LEAD_IN_SECTORS) / 4500u );  // p. minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = BCD( ((u32Size/2352)+LEAD_IN_SECTORS) / 75u );  // p. seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = BCD( ((u32Size/2352)+LEAD_IN_SECTORS) % 75u );  // p. frames: 0...74 (BCD)
      }
      else
      {
        // Pause
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x01;  // 0x01 == beginning of the first track
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x00;  // 0x00 == pause
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( u32AbsSectorCounter / 4500u );  // abs. minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = BCD( u32AbsSectorCounter / 75u );  // abs. seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = BCD( u32AbsSectorCounter % 75u );  // abs. frames: 0...74 (BCD)
      }
      BinConvert_CDSector2352( au8Sector, au8PitsnLands, &sSubcode );
      fwrite( au8PitsnLands, sizeof( U8 ), sizeof( au8PitsnLands ), fwp );
      u32AbsSectorCounter++;
    }
    //--------------------------------------------------------------------------
    // Add track 1
    u32SectorCounter = 0;
    // Convert sectors
    for( u16ReadBytes = 2352; u16ReadBytes == 2352;  )
    {
        u16ReadBytes = fread( au8Sector, sizeof( U8 ), 2352, frp );
        if( u16ReadBytes < 2352 )
        {
            break;
        }
        // Set P-subcodes
        // Audio: first 2-3 seconds of the track: P=all 1; after then: P=all 0
        if( u32SectorCounter < 150u )  // NOTE: 1 second ==  75 sectors
        {
          #warning "Is the payload actually used if the start flag is set?"
          memset( sSubcode.au96SubcodeP, 0xFF, sizeof( sSubcode.au96SubcodeP ) );  // start flag
        }
        else
        {
          memset( sSubcode.au96SubcodeP, 0x00, sizeof( sSubcode.au96SubcodeP ) );
        }
        // Set Q-subcodes
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u4Control = 0x0;  // 2 audio channels without pre-emphasis
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u4Address = 0x1;  // mode 1
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x01;  // track number: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // index: 0...99
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( u32AbsSectorCounter / 4500u );  // abs. minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = BCD( u32AbsSectorCounter / 75u );  // abs. seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = BCD( u32AbsSectorCounter % 75u );  // abs. frames: 0...74 (BCD)

        // Convert the sector
        printf( "Converting sector %u/%u\n", u32SectorCounter, u32Size/2352u );
        BinConvert_CDSector2352( au8Sector, au8PitsnLands, &sSubcode );
        fwrite( au8PitsnLands, sizeof( U8 ), sizeof( au8PitsnLands ), fwp );
        u32SectorCounter++;
        u32AbsSectorCounter++;
    }
    //--------------------------------------------------------------------------
    // Add lead-out track
    // Audio lead-out: zero sound with 2 Hz P-subcode
    (void)memset( au8Sector, 0, sizeof( au8Sector ) );
    memset( &sSubcode, 0x00u, sizeof( sSubcode ) );
    for( u32SectorCounter = 0; u32SectorCounter < LEAD_OUT_SECTORS; u32SectorCounter++ )
    {
      // Set P subcode
      if( u32SectorCounter%75 < 37 )  // NOTE: 1 second ==  75 sectors
      {
        memset( sSubcode.au96SubcodeP, 0xFF, sizeof( sSubcode.au96SubcodeP ) );
      }
      else
      {
        memset( sSubcode.au96SubcodeP, 0x00, sizeof( sSubcode.au96SubcodeP ) );
      }
      // Set Q-subcodes
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u4Control = 0x0;  // 2 audio channels without pre-emphasis
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u4Address = 0x1;  // mode 1
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0xAA;  // track number: 0...99 (BCD)
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // index: 0...99
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( u32AbsSectorCounter / 4500u );  // abs. minutes: 0...99 (BCD)
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = BCD( u32AbsSectorCounter / 75u );  // abs. seconds: 0...59 (BCD)
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = BCD( u32AbsSectorCounter % 75u );  // abs. frames: 0...74 (BCD)
      BinConvert_CDSector2352( au8Sector, au8PitsnLands, &sSubcode );
      fwrite( au8PitsnLands, sizeof( U8 ), sizeof( au8PitsnLands ), fwp );
      u32AbsSectorCounter++;
    }

    fclose( frp );
    fclose( fwp );

    return 0;
}

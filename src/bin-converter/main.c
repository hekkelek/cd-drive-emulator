#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "binconvert.h"
#include "crc.h"


#define LEAD_IN_SECTORS   4500
#define LEAD_OUT_SECTORS  6750



FILE* frp, *fwp;

U8 au8Sector[ 2352 ];
U8 au8PitsnLands[ 7203 ];

U8 BCD( U8 u8Data )
{
  return ( (u8Data/10)<<4 ) | ( u8Data % 10 );
}

int main()
{
    U16 u16ReadBytes;
    U32 u32Size;
    U32 u32SectorCounter, u32AbsSectorCounter = 0;
    S_CD_SUBCODE sSubcode;
    U16 u16Crc;




    // CRC tests
    memset( &sSubcode, 0x00u, sizeof( sSubcode ) );
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u8ControlMode = 0x01;  // 2 audio channels without pre-emphasis, mode 1
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x01;  // track number: 0...99 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // index: 0...99
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = 0x03;//BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = 0x06;//BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = 0x39;//BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = 0x03;//BCD( u32AbsSectorCounter / 4500u );  // p. minutes: 0...99 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x08;//BCD( u32AbsSectorCounter / 75u );  // p. seconds: 0...59 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x39;//BCD( u32AbsSectorCounter % 75u );  // p. frames: 0...74 (BCD)
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = 0;//0xFFFF;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );  // should result 0xBA38
    CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );

    memset( &sSubcode, 0x00u, sizeof( sSubcode ) );
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u8ControlMode = 0x01;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x01;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = 0x02;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = 0x13;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = 0x25;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = 0x02;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x15;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x25;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = 0x0000;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );  // should be 0x450F
    CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );

    memset( &sSubcode, 0x00u, sizeof( sSubcode ) );
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u8ControlMode = 0x01;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x01;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x02;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = 0x0000;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );  // should be 0x5A28
    CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );

    memset( &sSubcode, 0x00u, sizeof( sSubcode ) );
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u8ControlMode = 0x41;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x01;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = 0x07;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = 0x00;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x02;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x07;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = 0x0000;
    ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );  // should be 0x3F01
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
      memset( sSubcode.au96SubcodeP, 0x00, sizeof( sSubcode.au96SubcodeP ) );  // lead-in track is encoded as audio
      // Set Q-subcodes
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u8ControlMode = 0x01;  // 2 audio channels without pre-emphasis, mode 1
      if( u32SectorCounter > ( LEAD_IN_SECTORS - 150 ) )
      {
        // Pause
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x00;  // track number: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x00;  // pointer: 0 indicating pause
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( u32AbsSectorCounter / 4500u );  // p. minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = BCD( u32AbsSectorCounter / 75u );  // p. seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = BCD( u32AbsSectorCounter % 75u );  // p. frames: 0...74 (BCD)
      }
      else if( ( 2u >= u32SectorCounter % 12u ) )
      {
        // Table of Contents
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x00;
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0x01;  // track number
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = 0x01;  // p. minutes: first track number
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x00;  // p. seconds: 0x00 in this case
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x00;  // p. frames:  0x00 in this case
      }
      else if( ( 5u >= u32SectorCounter % 12u ) )
      {
        // Table of Contents
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x00;
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0xA0;  // 0xA0 == first information track data
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = 0x01;  // p. minutes: first track number
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x00;  // p. seconds: always 0x00 in this case
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x00;  // p. frames:  always 0x00 in this case
      }
      else if( ( 8u >= u32SectorCounter % 12u ) )
      {
        // Table of Contents
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x00;
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0xA1;  // 0xA1 == last information track data
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( ((u32Size/2352)+LEAD_IN_SECTORS) / 4500u );  // p. minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = 0x00;  // p. seconds: always 0x00 in this case
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = 0x00;  // p. frames:  always 0x00 in this case
      }
      else
      {
        // Table of Contents
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 0 ] = 0x00;
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 1 ] = 0xA2;  // 0xA2 == beginning of the lead out track
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 2 ] = BCD( u32SectorCounter / 4500u );  // minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 3 ] = BCD( u32SectorCounter / 75u );  // seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 4 ] = BCD( u32SectorCounter % 75u );  // frames: 0...74 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 5 ] = 0x00;  // zero: should be 0x00
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 6 ] = BCD( ((u32Size/2352)+LEAD_IN_SECTORS+1) / 4500u );  // p. minutes: 0...99 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 7 ] = BCD( ((u32Size/2352)+LEAD_IN_SECTORS+1) / 75u );  // p. seconds: 0...59 (BCD)
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->au8DataQ[ 8 ] = BCD( ((u32Size/2352)+LEAD_IN_SECTORS+1) % 75u );  // p. frames: 0...74 (BCD)
      }
/*      else
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
      }*/
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = 0x0000;
      u16Crc = CRC_QSubcode( ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ) );
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u16Crc = ((u16Crc&0xFF00)>>8) | (((u16Crc&0x00FF)<<8));  //TODO: rewrite so endianness swap won't be necessary
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
        ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u8ControlMode = 0x01;  // 2 audio channels without pre-emphasis, mode 1
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
      ((S_CD_SUBCODE_Q*)sSubcode.au96SubcodeQ)->u8ControlMode = 0x01;  // 2 audio channels without pre-emphasis, mode 1
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

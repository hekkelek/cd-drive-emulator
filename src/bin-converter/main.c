#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "binconvert.h"

FILE* frp, *fwp;

U8 u8Sector[ 2352 ];
U8 u8PitsnLands[ 7203 ];

int main()
{
    U16 u16ReadBytes;
    U32 u32Sector = 0;
    U32 u32Size;

    printf("CD binary converter test\n");

    BinConvert_Init();

    frp = fopen( "testsound.bin", "rb" );
    fwp = fopen( "output.bin","wb");

    if( !frp )
    {
        printf("Error: file not found!");
        return -1;
    }

    fseek( frp, 0l, SEEK_END );
    u32Size = ftell( frp );
    rewind( frp );

    for( u16ReadBytes = 2352; u16ReadBytes == 2352;  )
    {
        u16ReadBytes = fread( u8Sector, sizeof( U8 ), 2352, frp );
        if( u16ReadBytes < 2352 )
        {
            break;
        }
        printf( "Converting sector %u/%u\n", ++u32Sector, u32Size/2352u );
        BinConvert_CDSector2352( u8Sector, u8PitsnLands );
        fwrite( u8PitsnLands, sizeof( U8 ), sizeof( u8PitsnLands ), fwp );
    }

    fclose( frp );
    fclose( fwp );

    return 0;
}

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

    printf("CD binary converter test\n");

    frp = fopen( "padtest.bin", "rb" );
    fwp = fopen( "output.bin","wb");

    if( !frp )
    {
        printf("Error: file not found!");
        return -1;
    }

    for( u16ReadBytes = 2352; u16ReadBytes == 2352;  )
    {
        u16ReadBytes = fread( u8Sector, sizeof( U8 ), 2352, frp );
        if( u16ReadBytes < 2352 )
            continue;
        BinConvert_CDSector2352( u8Sector, u8PitsnLands );
        fwrite( u8PitsnLands, sizeof( U8 ), 3234, fwp );
    }

    fclose( frp );
    fclose( fwp );

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BUFFER_SIZE 1024u  //NOTE: must be power-of-2

FILE* frp, *fwp;

int main( int argc, char *argv[] )
{
  uint8_t  au8Buffer[ BUFFER_SIZE ];
  uint8_t  u8Temp;
  uint16_t u16Index;
  uint32_t u32RemainingBytes;

  printf( "Endianness converter by Hekk_Elek\n" );
  printf( "Build date: %s\n", __DATE__ );
  if( argc == 3 )
  {
    printf( "Input file: %s\n", argv[1] );
    printf( "Output file: %s\n", argv[2] );
  }
  else
  {
    printf( "Usage: endianswap input output\n" );
    return -1;
  }

  frp = fopen( argv[1], "rb" );
  fwp = fopen( argv[2],"wb");

  if( !frp )
  {
    printf( "Error: input file not found!" );
    return -2;
  }

  do
  {
    u32RemainingBytes = fread( au8Buffer, 1u, sizeof( au8Buffer ), frp );
    for( u16Index = 0u; u16Index < sizeof( au8Buffer ); u16Index += 2u )
    {
      u8Temp = au8Buffer[ u16Index ];
      au8Buffer[ u16Index ] = au8Buffer[ u16Index + 1u ];
      au8Buffer[ u16Index + 1u ] = u8Temp;
    }
    fwrite( au8Buffer, 1u, u32RemainingBytes, fwp );
  } while( u32RemainingBytes != 0u );

  fclose( frp );
  fclose( fwp );

  return 0;
}

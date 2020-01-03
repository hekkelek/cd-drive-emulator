#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



int main()
{
  uint32_t u32EFMCodeWord;
  uint8_t  u8BitIdx;
  uint8_t  u8ReadByte;
  uint8_t  u8Line;
  FILE* pFile;

  pFile = fopen("efm.txt", "r");

  if( NULL == pFile )
  {
    return -1;
  }

  u8Line = 0;
  u32EFMCodeWord = 0;
  u8BitIdx = 0;
  while( !feof( pFile ) )
  {
    u8ReadByte = fgetc( pFile );
    if( '0' == u8ReadByte )
    {
      u8BitIdx++;
    }
    else if( '1' == u8ReadByte )
    {
      u32EFMCodeWord ^= 1<<(31-u8BitIdx);
      u8BitIdx++;
    }
    else if( '\n' == u8ReadByte )
    {
      printf( "0x%.8X, ",u32EFMCodeWord );
      u32EFMCodeWord = 0;
      u8BitIdx = 0;
      u8Line++;
      if( 8 == u8Line )
      {
        printf( "\n" );
        u8Line = 0;
      }
    }
  }

  fclose( pFile );
  return 0;
}

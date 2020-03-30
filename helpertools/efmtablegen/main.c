#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



int main()
{
  uint32_t u32EFMCodeWord;
  uint8_t  u8BitIdx;
  uint8_t  u8ReadByte;
  uint8_t  u8Line;
  FILE* pFileInput;
  FILE* pFileCOuput;
  FILE* pFileMATLABOuput;

  pFileInput = fopen( "efm.txt", "r" );
  pFileCOuput = fopen( "efm.h", "w+" );
  pFileMATLABOuput = fopen( "efm.m", "w+" );

  if( ( NULL == pFileInput ) || ( NULL == pFileCOuput ) || ( NULL == pFileMATLABOuput ) )
  {
    return -1;
  }

  fprintf( pFileMATLABOuput, "EFMCodeTable = [\n" );

  u8Line = 0;
  u32EFMCodeWord = 0;
  u8BitIdx = 0;
  while( !feof( pFileInput ) )
  {
    u8ReadByte = fgetc( pFileInput );
    if( '0' == u8ReadByte )
    {
      u8BitIdx++;

      fprintf( pFileMATLABOuput, " 0" );
    }
    else if( '1' == u8ReadByte )
    {
      u32EFMCodeWord ^= 1<<(31-u8BitIdx);
      u8BitIdx++;

      fprintf( pFileMATLABOuput, " 1" );
    }
    else if( '\n' == u8ReadByte )
    {
      printf( "0x%.8X, ",u32EFMCodeWord );
      fprintf( pFileCOuput, "0x%.8X, ",u32EFMCodeWord );
      fprintf( pFileMATLABOuput, ";\n" );

      u32EFMCodeWord = 0;
      u8BitIdx = 0;
      u8Line++;
      if( 8 == u8Line )
      {
        printf( "\n" );
        fprintf( pFileCOuput, "\n" );
        u8Line = 0;
      }
    }
  }

  fprintf( pFileMATLABOuput, "];\n" );

  fclose( pFileInput );
  fclose( pFileCOuput );
  fclose( pFileMATLABOuput );
  return 0;
}

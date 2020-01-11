#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "types.h"
#include "reedsolomon.h"

// Test tables for testing RS encoder
#include "testtables.h"

int main()
{
  uint32_t u32Index;
  uint8_t  au8Msg28[ 28 ];
  BOOL     bIsOK = TRUE;
  //uint8_t  au8Msg32[ 32 ];

  memset( au8Msg28, 0, sizeof( au8Msg28 ) );
  memcpy( au8Msg28, cau8EncodedMessage[ 0 ], 24 );  // don't copy the parity  bytes

  printf( "RS(28,24) test\n\n" );
  printf( "First message with CRC:   %u", cau8EncodedMessage[ 0 ][ 0 ] );
  for( u32Index = 1; u32Index < 28; u32Index++ )
  {
    printf( ", %u", cau8EncodedMessage[ 0 ][ u32Index ] );
  }
  printf("\n");

  ReedSolomon_AddRS2824( au8Msg28 );
  printf( "First calculated message: %u", au8Msg28[ 0 ] );
  for( u32Index = 1; u32Index < 28; u32Index++ )
  {
    printf( ", %u", au8Msg28[ u32Index ] );
  }
  printf("\n\n");

  for( u32Index = 0; u32Index < sizeof( cau8EncodedMessage )/sizeof( cau8EncodedMessage[0] ); u32Index++ )
  {
    memset( au8Msg28, 0, sizeof( au8Msg28 ) );
    memcpy( au8Msg28, cau8EncodedMessage[ u32Index ], 24 );  // don't copy the parity  bytes

    ReedSolomon_AddRS2824( au8Msg28 );
    if( 0 != memcmp( au8Msg28, cau8EncodedMessage[ u32Index ], sizeof( au8Msg28 ) ) )
    {
      bIsOK = FALSE;
    }
  }

  if( TRUE == bIsOK )
  {
    printf("RS(28,24) encoder works!\n");
  }
  else
  {
    printf("Something's wrong with RS(28,24) encoder!\n");
  }

  return 0;
}

/*! *******************************************************************************************************
* Copyright (c) 2018-2019 K. Sz. Horvath
*
* All rights reserved
*
* \file crc.c
*
* \brief CRC algorithms
*
* \author K. Sz. Horvath
*
**********************************************************************************************************/

//--------------------------------------------------------------------------------------------------------/
// Include files
//--------------------------------------------------------------------------------------------------------/
#include "types.h"
#include "binconvert.h"

// Own include
#include "crc.h"


//--------------------------------------------------------------------------------------------------------/
// Definitions
//--------------------------------------------------------------------------------------------------------/


//--------------------------------------------------------------------------------------------------------/
// Types
//--------------------------------------------------------------------------------------------------------/


//--------------------------------------------------------------------------------------------------------/
// Global variables
//--------------------------------------------------------------------------------------------------------/


//--------------------------------------------------------------------------------------------------------/
// Static function declarations
//--------------------------------------------------------------------------------------------------------/


//--------------------------------------------------------------------------------------------------------/
// Static functions
//--------------------------------------------------------------------------------------------------------/
/*! *******************************************************************
 * \brief
 * \param
 * \return
 *********************************************************************/

 /*! *******************************************************************
 * \brief
 * \param
 * \return
 *********************************************************************/


//--------------------------------------------------------------------------------------------------------/
// Interface functions
//--------------------------------------------------------------------------------------------------------/
/*! *******************************************************************
 * \brief  Calculate the 16-bit CRC of the Q-subcode
 * \param  psSubCodeQ: pointer to the Q-subcode structure
 * \return Calculated CRC
 * \note   Generator polynomial: x^16 + x^12 + x^5 + 1 == 0x11021
 *********************************************************************/
U16 CRC_QSubcode( S_CD_SUBCODE_Q* psSubCodeQ )
{
  U16 u16CRC = 0;
  U8  u8ByteIndex, u8BitIndex;

  for( u8ByteIndex = 0; u8ByteIndex < sizeof( S_CD_SUBCODE_Q ) - 2u; u8ByteIndex++ )
  {
    u16CRC ^= ((U8*)psSubCodeQ)[ u8ByteIndex ] << 8u;
    for( u8BitIndex = 0; u8BitIndex < 8; u8BitIndex++ )
    {
      u16CRC = ( u16CRC << 1 ) ^ ( (0u != (u16CRC & 0x8000u) ) ? 0x1021u : 0u );
    }
  }
  return ~u16CRC;
}

 /*! *******************************************************************
 * \brief
 * \param
 * \return
 *********************************************************************/



//-----------------------------------------------< EOF >--------------------------------------------------/

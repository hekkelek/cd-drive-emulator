/*! *******************************************************************************************************
* Copyright (c) 2017-2018 Kristóf Szabolcs Horváth
*
* All rights reserved
*
* \file binconvert.h
*
* \brief Binary to raw CD converter functions
*
* \author Kristóf Sz. Horváth
*
**********************************************************************************************************/

#ifndef BINCONVERT_H_INCLUDED
#define BINCONVERT_H_INCLUDED

//--------------------------------------------------------------------------------------------------------/
// Include files
//--------------------------------------------------------------------------------------------------------/


//--------------------------------------------------------------------------------------------------------/
// Definitions
//--------------------------------------------------------------------------------------------------------/
#if (defined(__GNUC__) && __GNUC__) || defined(__SUNPRO_C)
  #define PACKED_STRUCT   __attribute__((packed))
#elif defined(__ICCARM__)
  #define PACKED_STRUCT   __packed
#else
  #warning No directives known for this compiler.
  #define PACKED_STRUCT
#endif


//--------------------------------------------------------------------------------------------------------/
// Types
//--------------------------------------------------------------------------------------------------------/
//! \brief Structure for containing the subcode
typedef struct
{
  U8 au96SubcodeP[ 12u ];  //!< P subcode
  U8 au96SubcodeQ[ 12u ];  //!< Q subcode
  U8 au96SubcodeR[ 12u ];  //!< R subcode
  U8 au96SubcodeS[ 12u ];  //!< S subcode
  U8 au96SubcodeT[ 12u ];  //!< T subcode
  U8 au96SubcodeU[ 12u ];  //!< U subcode
  U8 au96SubcodeV[ 12u ];  //!< V subcode
  U8 au96SubcodeW[ 12u ];  //!< W subcode
} PACKED_STRUCT S_CD_SUBCODE;

//! \brief Structure for Q subcode
typedef struct
{
  U8  u8ControlMode;  // the control and mode nibbles form a byte
  U8  au8DataQ[ 9 ];
  U16 u16Crc;
} PACKED_STRUCT S_CD_SUBCODE_Q;


//--------------------------------------------------------------------------------------------------------/
// Global functions
//--------------------------------------------------------------------------------------------------------/


//--------------------------------------------------------------------------------------------------------/
// Public functions
//--------------------------------------------------------------------------------------------------------/
void BinConvert_Init( void );
void BinConvert_CDSector2352( U8* pu8ArrayToConvert, U8* pu8ArrayResult, S_CD_SUBCODE* psSubcode );



#endif // BINCONVERT_H_INCLUDED

/*
 * $Id$
 */

/*
 * Harbour Project source code:
 * Windows DLL handling function
 *
 * Copyright 2009-2010 Viktor Szakats (harbour.01 syenar.hu)
 * Based on some original code by:
 * Copyright 2002 Vic McClung <vicmcclung@vicmcclung.com>
 * www - http://www.harbour-project.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA (or visit the web site http://www.gnu.org/).
 *
 * As a special exception, the Harbour Project gives permission for
 * additional uses of the text contained in its release of Harbour.
 *
 * The exception is that, if you link the Harbour libraries with other
 * files to produce an executable, this does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * Your use of that executable is in no way restricted on account of
 * linking the Harbour library code into it.
 *
 * This exception does not however invalidate any other reasons why
 * the executable file might be covered by the GNU General Public License.
 *
 * This exception applies only to the code released by the Harbour
 * Project under the name Harbour.  If you copy code from other
 * Harbour Project or Free Software Foundation releases into a copy of
 * Harbour, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for Harbour, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 *
 */

#include "hbwin.h"
#include "hbapierr.h"
#include "hbapiitm.h"
#include "hbvm.h"

#ifndef HB_WIN_NO_LEGACY
#define HB_WIN_NO_LEGACY
#endif
#undef HB_LEGACY_LEVEL3
#include "hbwin.ch"

/* C raw return types */
#define _RETTYPERAW_INT32       1
#define _RETTYPERAW_INT64       2
#define _RETTYPERAW_DOUBLE      3
#define _RETTYPERAW_FLOAT       4

#define _DLLEXEC_MAXPARAM       15

#if defined( HB_ARCH_64BIT )

typedef struct
{
   void *    hString;
   HB_BOOL   bByRef;
   HB_U64    nValue;
} HB_WINARG;

typedef struct
{
   HB_BOOL     bUNICODE;
   int         iFirst;
   HB_WINARG * pArg;
} HB_WINCALL, * PHB_WINCALL;

static HB_U64 hb_u64par( PHB_WINCALL wcall, int iParam )
{
   PHB_ITEM pParam = hb_param( wcall->iFirst + iParam, HB_IT_ANY );
   HB_U64 r = 0;

   if( pParam )
   {
      switch( HB_ITEM_TYPE( pParam ) )
      {
         case HB_IT_LOGICAL:
            wcall->pArg[ iParam - 1 ].nValue = hb_itemGetL( pParam );
            r = wcall->pArg[ iParam - 1 ].bByRef ? ( HB_PTRUINT ) &wcall->pArg[ iParam - 1 ].nValue : wcall->pArg[ iParam - 1 ].nValue;
            break;

         case HB_IT_INTEGER:
         case HB_IT_LONG:
         case HB_IT_DATE:
            wcall->pArg[ iParam - 1 ].nValue = hb_itemGetNInt( pParam );
            r = wcall->pArg[ iParam - 1 ].bByRef ? ( HB_PTRUINT ) &wcall->pArg[ iParam - 1 ].nValue : wcall->pArg[ iParam - 1 ].nValue;
            break;

         case HB_IT_DOUBLE:
            HB_PUT_LE_DOUBLE( ( BYTE * ) &wcall->pArg[ iParam - 1 ].nValue, hb_itemGetND( pParam ) );
            r = wcall->pArg[ iParam - 1 ].bByRef ? ( HB_PTRUINT ) &wcall->pArg[ iParam - 1 ].nValue : wcall->pArg[ iParam - 1 ].nValue;
            break;

         case HB_IT_STRING:
         case HB_IT_MEMO:
            if( wcall->bUNICODE )
               r = ( HB_PTRUINT ) hb_itemGetStrU16( pParam, HB_CDP_ENDIAN_NATIVE, &wcall->pArg[ iParam - 1 ].hString, NULL );
            else
               r = ( HB_PTRUINT ) hb_itemGetStr( pParam, hb_setGetOSCP(), &wcall->pArg[ iParam - 1 ].hString, NULL );
            wcall->pArg[ iParam - 1 ].nValue = r;
            break;

         case HB_IT_POINTER:
            wcall->pArg[ iParam - 1 ].nValue = ( HB_PTRUINT ) hb_itemGetPtr( pParam );
            r = wcall->pArg[ iParam - 1 ].bByRef ? ( HB_PTRUINT ) &wcall->pArg[ iParam - 1 ].nValue : wcall->pArg[ iParam - 1 ].nValue;
            break;
      }
   }

   return r;
}

typedef HB_U64( * WIN64_00 ) ( void );
typedef HB_U64( * WIN64_01 ) ( HB_U64 );
typedef HB_U64( * WIN64_02 ) ( HB_U64, HB_U64 );
typedef HB_U64( * WIN64_03 ) ( HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_04 ) ( HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_05 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_06 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_07 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_08 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_09 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_10 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_11 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_12 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_13 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_14 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );
typedef HB_U64( * WIN64_15 ) ( HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64, HB_U64 );

#elif defined( HB_ARCH_32BIT )

typedef struct
{
   union
   {
      HB_U32 n32;
      HB_U64 n64;
      double nDB;
      float  nFL;
   } t;
} HB_WINVAL;

typedef struct
{
   void *    hString;
   HB_BOOL   bByRef;
   HB_WINVAL value;
} HB_WINARG;

typedef struct
{
   HB_BOOL     bUNICODE;
   int         iFirst;
   HB_WINARG * pArg;
} HB_WINCALL, * PHB_WINCALL;

static void hb_u32par( PHB_WINCALL wcall, int iParam, HB_U32 * r1, HB_U32 * r2, HB_BOOL * b64 )
{
   PHB_ITEM pParam = hb_param( wcall->iFirst + iParam, HB_IT_ANY );

   *b64 = HB_FALSE;

   if( pParam )
   {
      switch( HB_ITEM_TYPE( pParam ) )
      {
         /* TODO: Add 64-bit integer support */

         case HB_IT_LOGICAL:
            wcall->pArg[ iParam - 1 ].value.t.n32 = hb_itemGetL( pParam );
            *r1 = wcall->pArg[ iParam - 1 ].bByRef ? ( HB_U32 ) &wcall->pArg[ iParam - 1 ].value.t.n32 : wcall->pArg[ iParam - 1 ].value.t.n32;
            break;

         case HB_IT_INTEGER:
         case HB_IT_LONG:
         case HB_IT_DATE:
            wcall->pArg[ iParam - 1 ].value.t.n32 = hb_itemGetNL( pParam );
            *r1 = wcall->pArg[ iParam - 1 ].bByRef ? ( HB_U32 ) &wcall->pArg[ iParam - 1 ].value.t.n32 : wcall->pArg[ iParam - 1 ].value.t.n32;
            break;

         case HB_IT_DOUBLE:
            wcall->pArg[ iParam - 1 ].value.t.nDB = hb_itemGetND( pParam );
            if( wcall->pArg[ iParam - 1 ].bByRef )
               *r1 = ( HB_U32 ) &wcall->pArg[ iParam - 1 ].value.t.nDB;
            else
            {
               *r1 = wcall->pArg[ iParam - 1 ].value.t.n64 & 0xFFFFFFFF;
               *r2 = ( wcall->pArg[ iParam - 1 ].value.t.n64 >> 32 );
               *b64 = HB_TRUE;
            }
            break;

         case HB_IT_STRING:
         case HB_IT_MEMO:
            if( wcall->bUNICODE )
               *r1 = ( HB_U32 ) hb_itemGetStrU16( pParam, HB_CDP_ENDIAN_NATIVE, &wcall->pArg[ iParam - 1 ].hString, NULL );
            else
               *r1 = ( HB_U32 ) hb_itemGetStr( pParam, hb_setGetOSCP(), &wcall->pArg[ iParam - 1 ].hString, NULL );
            wcall->pArg[ iParam - 1 ].value.t.n32 = *r1;
            break;

         case HB_IT_POINTER:
            wcall->pArg[ iParam - 1 ].value.t.n32 = ( HB_U32 ) hb_itemGetPtr( pParam );
            *r1 = wcall->pArg[ iParam - 1 ].bByRef ? ( HB_U32 ) &wcall->pArg[ iParam - 1 ].value.t.n32 : wcall->pArg[ iParam - 1 ].value.t.n32;
            break;
      }
   }
}

typedef HB_U32 ( _stdcall * WIN32_S32P00 )( void );
typedef HB_U32 ( _stdcall * WIN32_S32P01 )( HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P02 )( HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P03 )( HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P04 )( HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P05 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P06 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P07 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P08 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P09 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P10 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P11 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P12 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P13 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P14 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P15 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P16 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P17 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P18 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P19 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P20 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P21 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P22 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P23 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P24 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P25 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P26 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P27 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P28 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P29 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _stdcall * WIN32_S32P30 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P00 )( void );
typedef HB_U64 ( _stdcall * WIN32_S64P01 )( HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P02 )( HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P03 )( HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P04 )( HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P05 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P06 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P07 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P08 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P09 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P10 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P11 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P12 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P13 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P14 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P15 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P16 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P17 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P18 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P19 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P20 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P21 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P22 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P23 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P24 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P25 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P26 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P27 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P28 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P29 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _stdcall * WIN32_S64P30 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP00 )( void );
typedef double ( _stdcall * WIN32_SDBP01 )( HB_U32 );
typedef double ( _stdcall * WIN32_SDBP02 )( HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP03 )( HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP04 )( HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP05 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP06 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP07 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP08 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP09 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP10 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP11 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP12 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP13 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP14 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP15 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP16 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP17 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP18 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP19 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP20 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP21 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP22 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP23 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP24 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP25 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP26 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP27 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP28 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP29 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _stdcall * WIN32_SDBP30 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP00 )( void );
typedef float  ( _stdcall * WIN32_SFLP01 )( HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP02 )( HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP03 )( HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP04 )( HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP05 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP06 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP07 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP08 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP09 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP10 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP11 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP12 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP13 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP14 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP15 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP16 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP17 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP18 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP19 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP20 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP21 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP22 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP23 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP24 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP25 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP26 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP27 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP28 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP29 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _stdcall * WIN32_SFLP30 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P00 )( void );
typedef HB_U32 ( _cdecl   * WIN32_C32P01 )( HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P02 )( HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P03 )( HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P04 )( HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P05 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P06 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P07 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P08 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P09 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P10 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P11 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P12 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P13 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P14 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P15 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P16 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P17 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P18 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P19 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P20 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P21 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P22 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P23 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P24 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P25 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P26 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P27 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P28 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P29 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U32 ( _cdecl   * WIN32_C32P30 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P00 )( void );
typedef HB_U64 ( _cdecl   * WIN32_C64P01 )( HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P02 )( HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P03 )( HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P04 )( HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P05 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P06 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P07 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P08 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P09 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P10 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P11 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P12 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P13 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P14 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P15 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P16 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P17 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P18 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P19 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P20 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P21 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P22 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P23 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P24 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P25 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P26 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P27 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P28 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P29 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef HB_U64 ( _cdecl   * WIN32_C64P30 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP00 )( void );
typedef double ( _cdecl   * WIN32_CDBP01 )( HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP02 )( HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP03 )( HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP04 )( HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP05 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP06 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP07 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP08 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP09 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP10 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP11 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP12 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP13 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP14 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP15 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP16 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP17 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP18 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP19 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP20 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP21 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP22 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP23 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP24 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP25 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP26 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP27 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP28 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP29 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef double ( _cdecl   * WIN32_CDBP30 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP00 )( void );
typedef float  ( _cdecl   * WIN32_CFLP01 )( HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP02 )( HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP03 )( HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP04 )( HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP05 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP06 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP07 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP08 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP09 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP10 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP11 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP12 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP13 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP14 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP15 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP16 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP17 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP18 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP19 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP20 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP21 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP22 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP23 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP24 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP25 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP26 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP27 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP28 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP29 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );
typedef float  ( _cdecl   * WIN32_CFLP30 )( HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32, HB_U32 );

#endif

void hbwin_dllCall( int iCallConv, int iRetType, HB_BOOL bUNICODE, FARPROC lpFunction, int iParams, int iFirst, int * iParTypes )
{
   int tmp;

   HB_SYMBOL_UNUSED( iParTypes );

   if( ! lpFunction )
      return;

#if defined( HB_ARCH_64BIT )
   {
      HB_WINCALL wcall;

      HB_SYMBOL_UNUSED( iCallConv );

      wcall.bUNICODE = bUNICODE;
      wcall.iFirst   = iFirst - 1;

      iParams -= wcall.iFirst;

      if( iParams <= _DLLEXEC_MAXPARAM )
      {
         HB_U64 nRetVal = 0;

         if( iParams )
         {
            wcall.pArg = ( HB_WINARG * ) hb_xgrab( iParams * sizeof( HB_WINARG ) );
            memset( wcall.pArg, 0, iParams * sizeof( HB_WINARG ) );
         }
         else
            wcall.pArg = NULL;

         for( tmp = 0; tmp < iParams; ++tmp )
            wcall.pArg[ tmp ].bByRef = HB_ISBYREF( iFirst + tmp );

         switch( iParams )
         {
            case  0: nRetVal = ( ( WIN64_00 ) *lpFunction )(); break;
            case  1: nRetVal = ( ( WIN64_01 ) *lpFunction )( hb_u64par( &wcall, 1 ) ); break;
            case  2: nRetVal = ( ( WIN64_02 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ) ); break;
            case  3: nRetVal = ( ( WIN64_03 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ) ); break;
            case  4: nRetVal = ( ( WIN64_04 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ) ); break;
            case  5: nRetVal = ( ( WIN64_05 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ) ); break;
            case  6: nRetVal = ( ( WIN64_06 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ) ); break;
            case  7: nRetVal = ( ( WIN64_07 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ) ); break;
            case  8: nRetVal = ( ( WIN64_08 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ), hb_u64par( &wcall, 8 ) ); break;
            case  9: nRetVal = ( ( WIN64_09 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ), hb_u64par( &wcall, 8 ), hb_u64par( &wcall, 9 ) ); break;
            case 10: nRetVal = ( ( WIN64_10 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ), hb_u64par( &wcall, 8 ), hb_u64par( &wcall, 9 ), hb_u64par( &wcall, 10 ) ); break;
            case 11: nRetVal = ( ( WIN64_11 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ), hb_u64par( &wcall, 8 ), hb_u64par( &wcall, 9 ), hb_u64par( &wcall, 10 ), hb_u64par( &wcall, 11 ) ); break;
            case 12: nRetVal = ( ( WIN64_12 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ), hb_u64par( &wcall, 8 ), hb_u64par( &wcall, 9 ), hb_u64par( &wcall, 10 ), hb_u64par( &wcall, 11 ), hb_u64par( &wcall, 12 ) ); break;
            case 13: nRetVal = ( ( WIN64_13 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ), hb_u64par( &wcall, 8 ), hb_u64par( &wcall, 9 ), hb_u64par( &wcall, 10 ), hb_u64par( &wcall, 11 ), hb_u64par( &wcall, 12 ), hb_u64par( &wcall, 13 ) ); break;
            case 14: nRetVal = ( ( WIN64_14 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ), hb_u64par( &wcall, 8 ), hb_u64par( &wcall, 9 ), hb_u64par( &wcall, 10 ), hb_u64par( &wcall, 11 ), hb_u64par( &wcall, 12 ), hb_u64par( &wcall, 13 ), hb_u64par( &wcall, 14 ) ); break;
            case 15: nRetVal = ( ( WIN64_15 ) *lpFunction )( hb_u64par( &wcall, 1 ), hb_u64par( &wcall, 2 ), hb_u64par( &wcall, 3 ), hb_u64par( &wcall, 4 ), hb_u64par( &wcall, 5 ), hb_u64par( &wcall, 6 ), hb_u64par( &wcall, 7 ), hb_u64par( &wcall, 8 ), hb_u64par( &wcall, 9 ), hb_u64par( &wcall, 10 ), hb_u64par( &wcall, 11 ), hb_u64par( &wcall, 12 ), hb_u64par( &wcall, 13 ), hb_u64par( &wcall, 14 ), hb_u64par( &wcall, 15 ) ); break;
         }

         switch( iRetType )
         {
            case HB_WIN_DLL_CTYPE_VOID:
               hb_ret();
               break;

            case HB_WIN_DLL_CTYPE_BOOL:
               hb_retl( nRetVal != 0 );
               break;

            case HB_WIN_DLL_CTYPE_CHAR:
            case HB_WIN_DLL_CTYPE_CHAR_UNSIGNED:
            case HB_WIN_DLL_CTYPE_SHORT:
            case HB_WIN_DLL_CTYPE_SHORT_UNSIGNED:
            case HB_WIN_DLL_CTYPE_INT:
               hb_retni( ( int ) nRetVal );
               break;

            case HB_WIN_DLL_CTYPE_LONG:
               hb_retnl( ( long ) nRetVal );
               break;

            case HB_WIN_DLL_CTYPE_INT_UNSIGNED:
            case HB_WIN_DLL_CTYPE_LONG_UNSIGNED:
               hb_retnint( nRetVal );
               break;

            case HB_WIN_DLL_CTYPE_CHAR_PTR:
            case HB_WIN_DLL_CTYPE_CHAR_UNSIGNED_PTR:
               if( bUNICODE )
                  hb_retstr_u16( HB_CDP_ENDIAN_NATIVE, ( const HB_WCHAR * ) nRetVal );
               else
                  hb_retstr( hb_setGetOSCP(), ( const char * ) nRetVal );
               break;

            case HB_WIN_DLL_CTYPE_INT_PTR:
            case HB_WIN_DLL_CTYPE_SHORT_UNSIGNED_PTR:
            case HB_WIN_DLL_CTYPE_INT_UNSIGNED_PTR:
            case HB_WIN_DLL_CTYPE_STRUCTURE_PTR:
            case HB_WIN_DLL_CTYPE_LONG_PTR:
            case HB_WIN_DLL_CTYPE_LONG_UNSIGNED_PTR:
            case HB_WIN_DLL_CTYPE_VOID_PTR:
            case HB_WIN_DLL_CTYPE_FLOAT_PTR:
            case HB_WIN_DLL_CTYPE_DOUBLE_PTR:
               hb_retptr( ( void * ) nRetVal );
               break;

            case HB_WIN_DLL_CTYPE_FLOAT:
            case HB_WIN_DLL_CTYPE_DOUBLE:
               hb_retnd( HB_GET_LE_DOUBLE( ( HB_BYTE * ) &nRetVal ) );
               break;

            default:
               hb_retnl( ( long ) nRetVal );
         }

         for( tmp = 0; tmp < iParams; ++tmp )
         {
            if( wcall.pArg[ tmp ].bByRef )
            {
               switch( HB_ITEM_TYPE( hb_param( iFirst + tmp, HB_IT_ANY ) ) )
               {
                  case HB_IT_LOGICAL:
                     hb_storl( wcall.pArg[ tmp ].nValue != 0, iFirst + tmp );
                     break;

                  case HB_IT_NIL:
                  case HB_IT_INTEGER:
                  case HB_IT_LONG:
                  case HB_IT_DATE:
                     hb_stornint( wcall.pArg[ tmp ].nValue, iFirst + tmp );
                     break;

                  case HB_IT_DOUBLE:
                     hb_stornd( HB_GET_LE_DOUBLE( ( HB_BYTE * ) &wcall.pArg[ tmp ].nValue ), iFirst + tmp );
                     break;

                  case HB_IT_STRING:
                  case HB_IT_MEMO:
                     if( bUNICODE )
                        hb_storstrlen_u16( HB_CDP_ENDIAN_NATIVE, ( const HB_WCHAR * ) wcall.pArg[ tmp ].nValue, hb_parclen( iFirst + tmp ), iFirst + tmp );
                     else
                        hb_storstrlen( hb_setGetOSCP(), ( const char * ) wcall.pArg[ tmp ].nValue, hb_parclen( iFirst + tmp ), iFirst + tmp );
                     break;

                  case HB_IT_POINTER:
                     hb_storptr( ( void * ) wcall.pArg[ tmp ].nValue, iFirst + tmp );
                     break;
               }
            }
         }

         for( tmp = 0; tmp < iParams; ++tmp )
            hb_strfree( wcall.pArg[ tmp ].hString );

         if( wcall.pArg )
            hb_xfree( wcall.pArg );
      }
      else
         hb_errRT_BASE( EG_ARG, 2010, "A maximum of 15 parameters is supported", HB_ERR_FUNCNAME, HB_ERR_ARGS_BASEPARAMS );
   }
#elif defined( HB_ARCH_32BIT )
   {
      HB_WINCALL wcall;

      wcall.bUNICODE = bUNICODE;
      wcall.iFirst   = iFirst - 1;

      iParams -= wcall.iFirst;

      if( iParams <= _DLLEXEC_MAXPARAM )
      {
         int iRetTypeRaw;
         HB_WINVAL ret;

         int iParamsRaw = 0;
         HB_U32 rawpar[ _DLLEXEC_MAXPARAM * 2 ];

         ret.t.n64 = 0;

         if( iRetType == HB_WIN_DLL_CTYPE_DOUBLE )
            iRetTypeRaw = _RETTYPERAW_DOUBLE;
         if( iRetType == HB_WIN_DLL_CTYPE_FLOAT )
            iRetTypeRaw = _RETTYPERAW_FLOAT;
         else
            iRetTypeRaw = _RETTYPERAW_INT32;

         if( iParams )
         {
            wcall.pArg = ( HB_WINARG * ) hb_xgrab( iParams * sizeof( HB_WINARG ) );
            memset( wcall.pArg, 0, iParams * sizeof( HB_WINARG ) );
         }
         else
            wcall.pArg = NULL;

         for( tmp = 0; tmp < iParams; ++tmp )
         {
            HB_U32 r1;
            HB_U32 r2;
            HB_BOOL b64;

            wcall.pArg[ tmp ].bByRef = HB_ISBYREF( iFirst + tmp );

            hb_u32par( &wcall, tmp + 1, &r1, &r2, &b64 );

            /* TOFIX: Check proper order. */
            rawpar[ iParamsRaw++ ] = r1;
            if( b64 )
               rawpar[ iParamsRaw++ ] = r2;
         }

         if( iCallConv == HB_WIN_DLL_CALLCONV_CDECL )
         {
            switch( iRetTypeRaw )
            {
            case _RETTYPERAW_INT32:
               switch( iParamsRaw )
               {
                  case  0: ret.t.n32 = ( ( WIN32_C32P00 ) *lpFunction )(); break;
                  case  1: ret.t.n32 = ( ( WIN32_C32P01 ) *lpFunction )( rawpar[ 0 ] ); break;
                  case  2: ret.t.n32 = ( ( WIN32_C32P02 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ] ); break;
                  case  3: ret.t.n32 = ( ( WIN32_C32P03 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ] ); break;
                  case  4: ret.t.n32 = ( ( WIN32_C32P04 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ] ); break;
                  case  5: ret.t.n32 = ( ( WIN32_C32P05 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ] ); break;
                  case  6: ret.t.n32 = ( ( WIN32_C32P06 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ] ); break;
                  case  7: ret.t.n32 = ( ( WIN32_C32P07 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ] ); break;
                  case  8: ret.t.n32 = ( ( WIN32_C32P08 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ] ); break;
                  case  9: ret.t.n32 = ( ( WIN32_C32P09 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ] ); break;
                  case 10: ret.t.n32 = ( ( WIN32_C32P10 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ] ); break;
                  case 11: ret.t.n32 = ( ( WIN32_C32P11 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ] ); break;
                  case 12: ret.t.n32 = ( ( WIN32_C32P12 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ] ); break;
                  case 13: ret.t.n32 = ( ( WIN32_C32P13 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ] ); break;
                  case 14: ret.t.n32 = ( ( WIN32_C32P14 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ] ); break;
                  case 15: ret.t.n32 = ( ( WIN32_C32P15 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ] ); break;
                  case 16: ret.t.n32 = ( ( WIN32_C32P16 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ] ); break;
                  case 17: ret.t.n32 = ( ( WIN32_C32P17 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ] ); break;
                  case 18: ret.t.n32 = ( ( WIN32_C32P18 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ] ); break;
                  case 19: ret.t.n32 = ( ( WIN32_C32P19 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ] ); break;
                  case 20: ret.t.n32 = ( ( WIN32_C32P20 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ] ); break;
                  case 21: ret.t.n32 = ( ( WIN32_C32P21 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ] ); break;
                  case 22: ret.t.n32 = ( ( WIN32_C32P22 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ] ); break;
                  case 23: ret.t.n32 = ( ( WIN32_C32P23 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ] ); break;
                  case 24: ret.t.n32 = ( ( WIN32_C32P24 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ] ); break;
                  case 25: ret.t.n32 = ( ( WIN32_C32P25 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ] ); break;
                  case 26: ret.t.n32 = ( ( WIN32_C32P26 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ] ); break;
                  case 27: ret.t.n32 = ( ( WIN32_C32P27 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ] ); break;
                  case 28: ret.t.n32 = ( ( WIN32_C32P28 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ] ); break;
                  case 29: ret.t.n32 = ( ( WIN32_C32P29 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ] ); break;
                  case 30: ret.t.n32 = ( ( WIN32_C32P30 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ], rawpar[ 29 ] ); break;
               }
               break;
            case _RETTYPERAW_INT64:
               switch( iParamsRaw )
               {
                  case  0: ret.t.n64 = ( ( WIN32_C64P00 ) *lpFunction )(); break;
                  case  1: ret.t.n64 = ( ( WIN32_C64P01 ) *lpFunction )( rawpar[ 0 ] ); break;
                  case  2: ret.t.n64 = ( ( WIN32_C64P02 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ] ); break;
                  case  3: ret.t.n64 = ( ( WIN32_C64P03 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ] ); break;
                  case  4: ret.t.n64 = ( ( WIN32_C64P04 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ] ); break;
                  case  5: ret.t.n64 = ( ( WIN32_C64P05 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ] ); break;
                  case  6: ret.t.n64 = ( ( WIN32_C64P06 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ] ); break;
                  case  7: ret.t.n64 = ( ( WIN32_C64P07 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ] ); break;
                  case  8: ret.t.n64 = ( ( WIN32_C64P08 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ] ); break;
                  case  9: ret.t.n64 = ( ( WIN32_C64P09 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ] ); break;
                  case 10: ret.t.n64 = ( ( WIN32_C64P10 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ] ); break;
                  case 11: ret.t.n64 = ( ( WIN32_C64P11 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ] ); break;
                  case 12: ret.t.n64 = ( ( WIN32_C64P12 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ] ); break;
                  case 13: ret.t.n64 = ( ( WIN32_C64P13 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ] ); break;
                  case 14: ret.t.n64 = ( ( WIN32_C64P14 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ] ); break;
                  case 15: ret.t.n64 = ( ( WIN32_C64P15 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ] ); break;
                  case 16: ret.t.n64 = ( ( WIN32_C64P16 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ] ); break;
                  case 17: ret.t.n64 = ( ( WIN32_C64P17 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ] ); break;
                  case 18: ret.t.n64 = ( ( WIN32_C64P18 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ] ); break;
                  case 19: ret.t.n64 = ( ( WIN32_C64P19 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ] ); break;
                  case 20: ret.t.n64 = ( ( WIN32_C64P20 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ] ); break;
                  case 21: ret.t.n64 = ( ( WIN32_C64P21 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ] ); break;
                  case 22: ret.t.n64 = ( ( WIN32_C64P22 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ] ); break;
                  case 23: ret.t.n64 = ( ( WIN32_C64P23 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ] ); break;
                  case 24: ret.t.n64 = ( ( WIN32_C64P24 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ] ); break;
                  case 25: ret.t.n64 = ( ( WIN32_C64P25 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ] ); break;
                  case 26: ret.t.n64 = ( ( WIN32_C64P26 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ] ); break;
                  case 27: ret.t.n64 = ( ( WIN32_C64P27 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ] ); break;
                  case 28: ret.t.n64 = ( ( WIN32_C64P28 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ] ); break;
                  case 29: ret.t.n64 = ( ( WIN32_C64P29 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ] ); break;
                  case 30: ret.t.n64 = ( ( WIN32_C64P30 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ], rawpar[ 29 ] ); break;
               }
               break;
            case _RETTYPERAW_DOUBLE:
               switch( iParamsRaw )
               {
                  case  0: ret.t.nDB = ( ( WIN32_CDBP00 ) *lpFunction )(); break;
                  case  1: ret.t.nDB = ( ( WIN32_CDBP01 ) *lpFunction )( rawpar[ 0 ] ); break;
                  case  2: ret.t.nDB = ( ( WIN32_CDBP02 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ] ); break;
                  case  3: ret.t.nDB = ( ( WIN32_CDBP03 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ] ); break;
                  case  4: ret.t.nDB = ( ( WIN32_CDBP04 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ] ); break;
                  case  5: ret.t.nDB = ( ( WIN32_CDBP05 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ] ); break;
                  case  6: ret.t.nDB = ( ( WIN32_CDBP06 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ] ); break;
                  case  7: ret.t.nDB = ( ( WIN32_CDBP07 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ] ); break;
                  case  8: ret.t.nDB = ( ( WIN32_CDBP08 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ] ); break;
                  case  9: ret.t.nDB = ( ( WIN32_CDBP09 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ] ); break;
                  case 10: ret.t.nDB = ( ( WIN32_CDBP10 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ] ); break;
                  case 11: ret.t.nDB = ( ( WIN32_CDBP11 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ] ); break;
                  case 12: ret.t.nDB = ( ( WIN32_CDBP12 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ] ); break;
                  case 13: ret.t.nDB = ( ( WIN32_CDBP13 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ] ); break;
                  case 14: ret.t.nDB = ( ( WIN32_CDBP14 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ] ); break;
                  case 15: ret.t.nDB = ( ( WIN32_CDBP15 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ] ); break;
                  case 16: ret.t.nDB = ( ( WIN32_CDBP16 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ] ); break;
                  case 17: ret.t.nDB = ( ( WIN32_CDBP17 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ] ); break;
                  case 18: ret.t.nDB = ( ( WIN32_CDBP18 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ] ); break;
                  case 19: ret.t.nDB = ( ( WIN32_CDBP19 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ] ); break;
                  case 20: ret.t.nDB = ( ( WIN32_CDBP20 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ] ); break;
                  case 21: ret.t.nDB = ( ( WIN32_CDBP21 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ] ); break;
                  case 22: ret.t.nDB = ( ( WIN32_CDBP22 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ] ); break;
                  case 23: ret.t.nDB = ( ( WIN32_CDBP23 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ] ); break;
                  case 24: ret.t.nDB = ( ( WIN32_CDBP24 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ] ); break;
                  case 25: ret.t.nDB = ( ( WIN32_CDBP25 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ] ); break;
                  case 26: ret.t.nDB = ( ( WIN32_CDBP26 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ] ); break;
                  case 27: ret.t.nDB = ( ( WIN32_CDBP27 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ] ); break;
                  case 28: ret.t.nDB = ( ( WIN32_CDBP28 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ] ); break;
                  case 29: ret.t.nDB = ( ( WIN32_CDBP29 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ] ); break;
                  case 30: ret.t.nDB = ( ( WIN32_CDBP30 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ], rawpar[ 29 ] ); break;
               }
               break;
            case _RETTYPERAW_FLOAT:
               switch( iParamsRaw )
               {
                  case  0: ret.t.nFL = ( ( WIN32_CFLP00 ) *lpFunction )(); break;
                  case  1: ret.t.nFL = ( ( WIN32_CFLP01 ) *lpFunction )( rawpar[ 0 ] ); break;
                  case  2: ret.t.nFL = ( ( WIN32_CFLP02 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ] ); break;
                  case  3: ret.t.nFL = ( ( WIN32_CFLP03 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ] ); break;
                  case  4: ret.t.nFL = ( ( WIN32_CFLP04 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ] ); break;
                  case  5: ret.t.nFL = ( ( WIN32_CFLP05 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ] ); break;
                  case  6: ret.t.nFL = ( ( WIN32_CFLP06 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ] ); break;
                  case  7: ret.t.nFL = ( ( WIN32_CFLP07 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ] ); break;
                  case  8: ret.t.nFL = ( ( WIN32_CFLP08 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ] ); break;
                  case  9: ret.t.nFL = ( ( WIN32_CFLP09 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ] ); break;
                  case 10: ret.t.nFL = ( ( WIN32_CFLP10 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ] ); break;
                  case 11: ret.t.nFL = ( ( WIN32_CFLP11 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ] ); break;
                  case 12: ret.t.nFL = ( ( WIN32_CFLP12 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ] ); break;
                  case 13: ret.t.nFL = ( ( WIN32_CFLP13 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ] ); break;
                  case 14: ret.t.nFL = ( ( WIN32_CFLP14 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ] ); break;
                  case 15: ret.t.nFL = ( ( WIN32_CFLP15 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ] ); break;
                  case 16: ret.t.nFL = ( ( WIN32_CFLP16 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ] ); break;
                  case 17: ret.t.nFL = ( ( WIN32_CFLP17 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ] ); break;
                  case 18: ret.t.nFL = ( ( WIN32_CFLP18 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ] ); break;
                  case 19: ret.t.nFL = ( ( WIN32_CFLP19 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ] ); break;
                  case 20: ret.t.nFL = ( ( WIN32_CFLP20 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ] ); break;
                  case 21: ret.t.nFL = ( ( WIN32_CFLP21 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ] ); break;
                  case 22: ret.t.nFL = ( ( WIN32_CFLP22 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ] ); break;
                  case 23: ret.t.nFL = ( ( WIN32_CFLP23 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ] ); break;
                  case 24: ret.t.nFL = ( ( WIN32_CFLP24 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ] ); break;
                  case 25: ret.t.nFL = ( ( WIN32_CFLP25 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ] ); break;
                  case 26: ret.t.nFL = ( ( WIN32_CFLP26 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ] ); break;
                  case 27: ret.t.nFL = ( ( WIN32_CFLP27 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ] ); break;
                  case 28: ret.t.nFL = ( ( WIN32_CFLP28 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ] ); break;
                  case 29: ret.t.nFL = ( ( WIN32_CFLP29 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ] ); break;
                  case 30: ret.t.nFL = ( ( WIN32_CFLP30 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ], rawpar[ 29 ] ); break;
               }
               break;
            }
         }
         else /* stdcall */
         {
            switch( iRetTypeRaw )
            {
            case _RETTYPERAW_INT32:
               switch( iParamsRaw )
               {
                  case  0: ret.t.n32 = ( ( WIN32_S32P00 ) *lpFunction )(); break;
                  case  1: ret.t.n32 = ( ( WIN32_S32P01 ) *lpFunction )( rawpar[ 0 ] ); break;
                  case  2: ret.t.n32 = ( ( WIN32_S32P02 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ] ); break;
                  case  3: ret.t.n32 = ( ( WIN32_S32P03 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ] ); break;
                  case  4: ret.t.n32 = ( ( WIN32_S32P04 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ] ); break;
                  case  5: ret.t.n32 = ( ( WIN32_S32P05 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ] ); break;
                  case  6: ret.t.n32 = ( ( WIN32_S32P06 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ] ); break;
                  case  7: ret.t.n32 = ( ( WIN32_S32P07 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ] ); break;
                  case  8: ret.t.n32 = ( ( WIN32_S32P08 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ] ); break;
                  case  9: ret.t.n32 = ( ( WIN32_S32P09 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ] ); break;
                  case 10: ret.t.n32 = ( ( WIN32_S32P10 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ] ); break;
                  case 11: ret.t.n32 = ( ( WIN32_S32P11 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ] ); break;
                  case 12: ret.t.n32 = ( ( WIN32_S32P12 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ] ); break;
                  case 13: ret.t.n32 = ( ( WIN32_S32P13 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ] ); break;
                  case 14: ret.t.n32 = ( ( WIN32_S32P14 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ] ); break;
                  case 15: ret.t.n32 = ( ( WIN32_S32P15 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ] ); break;
                  case 16: ret.t.n32 = ( ( WIN32_S32P16 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ] ); break;
                  case 17: ret.t.n32 = ( ( WIN32_S32P17 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ] ); break;
                  case 18: ret.t.n32 = ( ( WIN32_S32P18 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ] ); break;
                  case 19: ret.t.n32 = ( ( WIN32_S32P19 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ] ); break;
                  case 20: ret.t.n32 = ( ( WIN32_S32P20 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ] ); break;
                  case 21: ret.t.n32 = ( ( WIN32_S32P21 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ] ); break;
                  case 22: ret.t.n32 = ( ( WIN32_S32P22 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ] ); break;
                  case 23: ret.t.n32 = ( ( WIN32_S32P23 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ] ); break;
                  case 24: ret.t.n32 = ( ( WIN32_S32P24 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ] ); break;
                  case 25: ret.t.n32 = ( ( WIN32_S32P25 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ] ); break;
                  case 26: ret.t.n32 = ( ( WIN32_S32P26 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ] ); break;
                  case 27: ret.t.n32 = ( ( WIN32_S32P27 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ] ); break;
                  case 28: ret.t.n32 = ( ( WIN32_S32P28 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ] ); break;
                  case 29: ret.t.n32 = ( ( WIN32_S32P29 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ] ); break;
                  case 30: ret.t.n32 = ( ( WIN32_S32P30 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ], rawpar[ 29 ] ); break;
               }
               break;
            case _RETTYPERAW_INT64:
               switch( iParamsRaw )
               {
                  case  0: ret.t.n64 = ( ( WIN32_S64P00 ) *lpFunction )(); break;
                  case  1: ret.t.n64 = ( ( WIN32_S64P01 ) *lpFunction )( rawpar[ 0 ] ); break;
                  case  2: ret.t.n64 = ( ( WIN32_S64P02 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ] ); break;
                  case  3: ret.t.n64 = ( ( WIN32_S64P03 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ] ); break;
                  case  4: ret.t.n64 = ( ( WIN32_S64P04 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ] ); break;
                  case  5: ret.t.n64 = ( ( WIN32_S64P05 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ] ); break;
                  case  6: ret.t.n64 = ( ( WIN32_S64P06 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ] ); break;
                  case  7: ret.t.n64 = ( ( WIN32_S64P07 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ] ); break;
                  case  8: ret.t.n64 = ( ( WIN32_S64P08 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ] ); break;
                  case  9: ret.t.n64 = ( ( WIN32_S64P09 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ] ); break;
                  case 10: ret.t.n64 = ( ( WIN32_S64P10 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ] ); break;
                  case 11: ret.t.n64 = ( ( WIN32_S64P11 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ] ); break;
                  case 12: ret.t.n64 = ( ( WIN32_S64P12 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ] ); break;
                  case 13: ret.t.n64 = ( ( WIN32_S64P13 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ] ); break;
                  case 14: ret.t.n64 = ( ( WIN32_S64P14 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ] ); break;
                  case 15: ret.t.n64 = ( ( WIN32_S64P15 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ] ); break;
                  case 16: ret.t.n64 = ( ( WIN32_S64P16 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ] ); break;
                  case 17: ret.t.n64 = ( ( WIN32_S64P17 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ] ); break;
                  case 18: ret.t.n64 = ( ( WIN32_S64P18 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ] ); break;
                  case 19: ret.t.n64 = ( ( WIN32_S64P19 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ] ); break;
                  case 20: ret.t.n64 = ( ( WIN32_S64P20 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ] ); break;
                  case 21: ret.t.n64 = ( ( WIN32_S64P21 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ] ); break;
                  case 22: ret.t.n64 = ( ( WIN32_S64P22 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ] ); break;
                  case 23: ret.t.n64 = ( ( WIN32_S64P23 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ] ); break;
                  case 24: ret.t.n64 = ( ( WIN32_S64P24 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ] ); break;
                  case 25: ret.t.n64 = ( ( WIN32_S64P25 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ] ); break;
                  case 26: ret.t.n64 = ( ( WIN32_S64P26 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ] ); break;
                  case 27: ret.t.n64 = ( ( WIN32_S64P27 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ] ); break;
                  case 28: ret.t.n64 = ( ( WIN32_S64P28 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ] ); break;
                  case 29: ret.t.n64 = ( ( WIN32_S64P29 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ] ); break;
                  case 30: ret.t.n64 = ( ( WIN32_S64P30 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ], rawpar[ 29 ] ); break;
               }
               break;
            case _RETTYPERAW_DOUBLE:
               switch( iParamsRaw )
               {
                  case  0: ret.t.nDB = ( ( WIN32_SDBP00 ) *lpFunction )(); break;
                  case  1: ret.t.nDB = ( ( WIN32_SDBP01 ) *lpFunction )( rawpar[ 0 ] ); break;
                  case  2: ret.t.nDB = ( ( WIN32_SDBP02 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ] ); break;
                  case  3: ret.t.nDB = ( ( WIN32_SDBP03 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ] ); break;
                  case  4: ret.t.nDB = ( ( WIN32_SDBP04 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ] ); break;
                  case  5: ret.t.nDB = ( ( WIN32_SDBP05 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ] ); break;
                  case  6: ret.t.nDB = ( ( WIN32_SDBP06 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ] ); break;
                  case  7: ret.t.nDB = ( ( WIN32_SDBP07 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ] ); break;
                  case  8: ret.t.nDB = ( ( WIN32_SDBP08 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ] ); break;
                  case  9: ret.t.nDB = ( ( WIN32_SDBP09 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ] ); break;
                  case 10: ret.t.nDB = ( ( WIN32_SDBP10 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ] ); break;
                  case 11: ret.t.nDB = ( ( WIN32_SDBP11 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ] ); break;
                  case 12: ret.t.nDB = ( ( WIN32_SDBP12 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ] ); break;
                  case 13: ret.t.nDB = ( ( WIN32_SDBP13 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ] ); break;
                  case 14: ret.t.nDB = ( ( WIN32_SDBP14 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ] ); break;
                  case 15: ret.t.nDB = ( ( WIN32_SDBP15 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ] ); break;
                  case 16: ret.t.nDB = ( ( WIN32_SDBP16 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ] ); break;
                  case 17: ret.t.nDB = ( ( WIN32_SDBP17 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ] ); break;
                  case 18: ret.t.nDB = ( ( WIN32_SDBP18 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ] ); break;
                  case 19: ret.t.nDB = ( ( WIN32_SDBP19 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ] ); break;
                  case 20: ret.t.nDB = ( ( WIN32_SDBP20 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ] ); break;
                  case 21: ret.t.nDB = ( ( WIN32_SDBP21 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ] ); break;
                  case 22: ret.t.nDB = ( ( WIN32_SDBP22 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ] ); break;
                  case 23: ret.t.nDB = ( ( WIN32_SDBP23 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ] ); break;
                  case 24: ret.t.nDB = ( ( WIN32_SDBP24 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ] ); break;
                  case 25: ret.t.nDB = ( ( WIN32_SDBP25 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ] ); break;
                  case 26: ret.t.nDB = ( ( WIN32_SDBP26 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ] ); break;
                  case 27: ret.t.nDB = ( ( WIN32_SDBP27 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ] ); break;
                  case 28: ret.t.nDB = ( ( WIN32_SDBP28 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ] ); break;
                  case 29: ret.t.nDB = ( ( WIN32_SDBP29 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ] ); break;
                  case 30: ret.t.nDB = ( ( WIN32_SDBP30 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ], rawpar[ 29 ] ); break;
               }
               break;
            case _RETTYPERAW_FLOAT:
               switch( iParamsRaw )
               {
                  case  0: ret.t.nFL = ( ( WIN32_SFLP00 ) *lpFunction )(); break;
                  case  1: ret.t.nFL = ( ( WIN32_SFLP01 ) *lpFunction )( rawpar[ 0 ] ); break;
                  case  2: ret.t.nFL = ( ( WIN32_SFLP02 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ] ); break;
                  case  3: ret.t.nFL = ( ( WIN32_SFLP03 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ] ); break;
                  case  4: ret.t.nFL = ( ( WIN32_SFLP04 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ] ); break;
                  case  5: ret.t.nFL = ( ( WIN32_SFLP05 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ] ); break;
                  case  6: ret.t.nFL = ( ( WIN32_SFLP06 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ] ); break;
                  case  7: ret.t.nFL = ( ( WIN32_SFLP07 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ] ); break;
                  case  8: ret.t.nFL = ( ( WIN32_SFLP08 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ] ); break;
                  case  9: ret.t.nFL = ( ( WIN32_SFLP09 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ] ); break;
                  case 10: ret.t.nFL = ( ( WIN32_SFLP10 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ] ); break;
                  case 11: ret.t.nFL = ( ( WIN32_SFLP11 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ] ); break;
                  case 12: ret.t.nFL = ( ( WIN32_SFLP12 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ] ); break;
                  case 13: ret.t.nFL = ( ( WIN32_SFLP13 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ] ); break;
                  case 14: ret.t.nFL = ( ( WIN32_SFLP14 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ] ); break;
                  case 15: ret.t.nFL = ( ( WIN32_SFLP15 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ] ); break;
                  case 16: ret.t.nFL = ( ( WIN32_SFLP16 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ] ); break;
                  case 17: ret.t.nFL = ( ( WIN32_SFLP17 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ] ); break;
                  case 18: ret.t.nFL = ( ( WIN32_SFLP18 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ] ); break;
                  case 19: ret.t.nFL = ( ( WIN32_SFLP19 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ] ); break;
                  case 20: ret.t.nFL = ( ( WIN32_SFLP20 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ] ); break;
                  case 21: ret.t.nFL = ( ( WIN32_SFLP21 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ] ); break;
                  case 22: ret.t.nFL = ( ( WIN32_SFLP22 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ] ); break;
                  case 23: ret.t.nFL = ( ( WIN32_SFLP23 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ] ); break;
                  case 24: ret.t.nFL = ( ( WIN32_SFLP24 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ] ); break;
                  case 25: ret.t.nFL = ( ( WIN32_SFLP25 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ] ); break;
                  case 26: ret.t.nFL = ( ( WIN32_SFLP26 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ] ); break;
                  case 27: ret.t.nFL = ( ( WIN32_SFLP27 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ] ); break;
                  case 28: ret.t.nFL = ( ( WIN32_SFLP28 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ] ); break;
                  case 29: ret.t.nFL = ( ( WIN32_SFLP29 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ] ); break;
                  case 30: ret.t.nFL = ( ( WIN32_SFLP30 ) *lpFunction )( rawpar[ 0 ], rawpar[ 1 ], rawpar[ 2 ], rawpar[ 3 ], rawpar[ 4 ], rawpar[ 5 ], rawpar[ 6 ], rawpar[ 7 ], rawpar[ 8 ], rawpar[ 9 ], rawpar[ 10 ], rawpar[ 11 ], rawpar[ 12 ], rawpar[ 13 ], rawpar[ 14 ], rawpar[ 15 ], rawpar[ 16 ], rawpar[ 17 ], rawpar[ 18 ], rawpar[ 19 ], rawpar[ 20 ], rawpar[ 21 ], rawpar[ 22 ], rawpar[ 23 ], rawpar[ 24 ], rawpar[ 25 ], rawpar[ 26 ], rawpar[ 27 ], rawpar[ 28 ], rawpar[ 29 ] ); break;
               }
               break;
            }
         }

         switch( iRetType )
         {
            /* TODO: Add 64-bit integer support */

            case HB_WIN_DLL_CTYPE_VOID:
               hb_ret();
               break;

            case HB_WIN_DLL_CTYPE_BOOL:
               hb_retl( ret.t.n32 != 0 );
               break;

            case HB_WIN_DLL_CTYPE_CHAR:
            case HB_WIN_DLL_CTYPE_CHAR_UNSIGNED:
               hb_retni( ( int ) ( ret.t.n32 & 0xFF ) );
               break;

            case HB_WIN_DLL_CTYPE_SHORT:
            case HB_WIN_DLL_CTYPE_SHORT_UNSIGNED:
            case HB_WIN_DLL_CTYPE_INT:
               hb_retni( ( int ) ret.t.n32 );
               break;

            case HB_WIN_DLL_CTYPE_LONG:
               hb_retnl( ( long ) ret.t.n32 );
               break;

            case HB_WIN_DLL_CTYPE_INT_UNSIGNED:
            case HB_WIN_DLL_CTYPE_LONG_UNSIGNED:
               hb_retnint( ret.t.n32 );
               break;

            case HB_WIN_DLL_CTYPE_CHAR_PTR:
            case HB_WIN_DLL_CTYPE_CHAR_UNSIGNED_PTR:
               if( wcall.bUNICODE )
                  hb_retstr_u16( HB_CDP_ENDIAN_NATIVE, ( const HB_WCHAR * ) ret.t.n32 );
               else
                  hb_retstr( hb_setGetOSCP(), ( const char * ) ret.t.n32 );
               break;

            case HB_WIN_DLL_CTYPE_INT_PTR:
            case HB_WIN_DLL_CTYPE_SHORT_UNSIGNED_PTR:
            case HB_WIN_DLL_CTYPE_INT_UNSIGNED_PTR:
            case HB_WIN_DLL_CTYPE_STRUCTURE_PTR:
            case HB_WIN_DLL_CTYPE_LONG_PTR:
            case HB_WIN_DLL_CTYPE_LONG_UNSIGNED_PTR:
            case HB_WIN_DLL_CTYPE_VOID_PTR:
            case HB_WIN_DLL_CTYPE_FLOAT_PTR:
            case HB_WIN_DLL_CTYPE_DOUBLE_PTR:
               hb_retptr( ( void * ) ret.t.n32 );
               break;

            case HB_WIN_DLL_CTYPE_FLOAT:
               hb_retnd( ret.t.nFL );
               break;

            case HB_WIN_DLL_CTYPE_DOUBLE:
               hb_retnd( ret.t.nDB );
               break;

            default:
               hb_retnl( ( long ) ret.t.n32 );
         }

         for( tmp = 0; tmp < iParams; ++tmp )
         {
            if( wcall.pArg[ tmp ].bByRef )
            {
               switch( HB_ITEM_TYPE( hb_param( iFirst + tmp, HB_IT_ANY ) ) )
               {
                  /* TODO: Add 64-bit integer support */

                  case HB_IT_LOGICAL:
                     hb_storl( wcall.pArg[ tmp ].value.t.n32 != 0, iFirst + tmp );
                     break;

                  case HB_IT_NIL:
                  case HB_IT_INTEGER:
                  case HB_IT_LONG:
                  case HB_IT_DATE:
                     hb_stornint( wcall.pArg[ tmp ].value.t.n32, iFirst + tmp );
                     break;

                  case HB_IT_DOUBLE:
                     hb_stornd( wcall.pArg[ tmp ].value.t.nDB, iFirst + tmp );
                     break;

                  case HB_IT_STRING:
                  case HB_IT_MEMO:
                     if( wcall.bUNICODE )
                        hb_storstrlen_u16( HB_CDP_ENDIAN_NATIVE, ( const HB_WCHAR * ) wcall.pArg[ tmp ].value.t.n32, hb_parclen( iFirst + tmp ), iFirst + tmp );
                     else
                        hb_storstrlen( hb_setGetOSCP(), ( const char * ) wcall.pArg[ tmp ].value.t.n32, hb_parclen( iFirst + tmp ), iFirst + tmp );
                     break;

                  case HB_IT_POINTER:
                     hb_storptr( ( void * ) wcall.pArg[ tmp ].value.t.n32, iFirst + tmp );
                     break;
               }
            }
         }

         for( tmp = 0; tmp < iParams; ++tmp )
            hb_strfree( wcall.pArg[ tmp ].hString );

         if( wcall.pArg )
            hb_xfree( wcall.pArg );
      }
      else
         hb_errRT_BASE( EG_ARG, 2010, "A maximum of 15 parameters is supported", HB_ERR_FUNCNAME, HB_ERR_ARGS_BASEPARAMS );
   }

#endif
}

/* ------------------------------------------------------------------ */

FARPROC hbwin_getprocaddress( HMODULE hDLL, int iParam, HB_BOOL * pbUNICODE )
{
#if defined( HB_OS_WIN_CE )
   void * hStr;
   HB_SIZE nLen;
   LPCWSTR szProc = hb_parstr_u16( iParam, HB_CDP_ENDIAN_NATIVE, &hStr, &nLen );
   FARPROC lpFunction = GetProcAddress( hDLL, szProc ? szProc :
                  ( LPCWSTR ) ( HB_PTRDIFF ) ( hb_parni( iParam ) & 0x0FFFF ) );

   if( ! lpFunction && szProc ) /* try with WIDE suffix? */
   {
      LPWSTR pszProcW = ( LPWSTR ) hb_xgrab( ( nLen + 2 ) * sizeof( WCHAR ) );
      memcpy( pszProcW, szProc, nLen * sizeof( WCHAR ) );
      pszProcW[ nLen++ ] = L'W';
      pszProcW[ nLen++ ] = 0;
      lpFunction = GetProcAddress( hDLL, pszProcW );
      hb_xfree( pszProcW );
   }
   hb_strfree( hStr );

   if( pbUNICODE )
      *pbUNICODE = HB_TRUE;
#else
   const char * szProc = hb_parc( iParam );
   FARPROC lpFunction = GetProcAddress( hDLL, szProc ? szProc :
                  ( LPCSTR ) ( HB_PTRDIFF ) ( hb_parni( iParam ) & 0x0FFFF ) );

   if( pbUNICODE )
      *pbUNICODE = HB_FALSE;

#if defined( UNICODE )
   if( ! lpFunction && szProc ) /* try with WIDE suffix? */
   {
      char * pszFuncName = hb_xstrcpy( NULL, szProc, "W", NULL );
      lpFunction = GetProcAddress( hDLL, pszFuncName );
      hb_xfree( pszFuncName );
      if( pbUNICODE )
         *pbUNICODE = HB_TRUE;
   }
#endif

   if( ! lpFunction && szProc ) /* try with ANSI suffix? */
   {
      char * pszFuncName = hb_xstrcpy( NULL, szProc, "A", NULL );
      lpFunction = GetProcAddress( hDLL, pszFuncName );
      hb_xfree( pszFuncName );
      if( pbUNICODE )
         *pbUNICODE = HB_FALSE;
   }
#endif
   return lpFunction;
}

HB_FUNC( WIN_DLLCALL )
{
   PHB_ITEM pParam = hb_param( 1, HB_IT_ARRAY );
   int iFirst = 1;

   int iCallConv = HB_WIN_DLL_CALLCONV_STDCALL;
   int iRetType = HB_WIN_DLL_CTYPE_DEFAULT;
   HB_BOOL bUNICODE = HB_FALSE;

   if( pParam )
   {
      HB_SIZE nLen = hb_arrayLen( pParam );

      ++iFirst;

      if( nLen >= 1 && HB_IS_NUMERIC( hb_arrayGetItemPtr( pParam, 1 ) ) )
         iCallConv = hb_arrayGetNI( pParam, 1 );
      if( nLen >= 2 && HB_IS_NUMERIC( hb_arrayGetItemPtr( pParam, 2 ) ) )
         iRetType = hb_arrayGetNI( pParam, 2 );
      if( nLen >= 3 && HB_IS_LOGICAL( hb_arrayGetItemPtr( pParam, 3 ) ) )
         bUNICODE = hb_arrayGetL( pParam, 3 );

      /* TODO: Allow to specify parameter types, too */
   }

   hbwin_dllCall( iCallConv, iRetType, bUNICODE, ( FARPROC ) hb_parptr( iFirst ), hb_pcount(), iFirst + 1, NULL );
}

/*
 * $Id$
 */

/*
 * Harbour Project source code:
 * MAPI wrappers
 *
 * Copyright 2009 Toninho (toninhofwi yahoo.com.br)
 * Copyright 2009 Viktor Szakats (harbour.01 syenar.hu)
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

#define HB_OS_WIN_USED

#include "hbapi.h"
#include "hbwinuni.h"

#include <mapi.h>

#if ! defined( MAPI_UNICODE )
   #define MAPI_UNICODE   ( ( ULONG ) 0x80000000 )
#endif

HB_FUNC( WIN_MAPISENDMAIL )
{
   HINSTANCE hMapiDll;

   /* Set default return value */
   hb_retnl( -1 );

   if( ( hMapiDll = LoadLibrary( TEXT( "mapi32.dll" ) ) ) >= ( HINSTANCE ) 32 )
   {
      LPMAPISENDMAIL MAPISendMail = ( LPMAPISENDMAIL ) GetProcAddress( hMapiDll, HBTEXT( "MAPISendMail" ) );

      if( MAPISendMail )
      {
         HB_SIZE nLen, i;

         PHB_ITEM pFrom = hb_param( 8, HB_IT_ARRAY );
         PHB_ITEM pToList = hb_param( 9, HB_IT_ARRAY );
         PHB_ITEM pFileList = hb_param( 10, HB_IT_ARRAY );

         void * hString[ 4 + 2 + 2 * 100 + 2 * 100 ];
         int iString = 0;

         MapiMessage note;
         MapiRecipDesc origin;
         FLAGS flags = MAPI_LOGON_UI;

         ZeroMemory( &note, sizeof( MapiMessage ) );
         ZeroMemory( &origin, sizeof( MapiRecipDesc ) );

         note.lpszSubject      = ( LPSTR ) HB_PARSTR( 1, &hString[ iString++ ], NULL );
         note.lpszNoteText     = ( LPSTR ) HB_PARSTR( 2, &hString[ iString++ ], NULL );
         note.lpszMessageType  = ( LPSTR ) HB_PARSTR( 3, &hString[ iString++ ], NULL );
         note.lpszDateReceived = ( LPSTR ) HB_PARSTRDEF( 4, &hString[ iString++ ], NULL );

         if( hb_parl( 6 ) )
            note.flFlags |= MAPI_RECEIPT_REQUESTED;

         if( hb_parl( 7 ) )
            flags |= MAPI_DIALOG;

         #if defined( UNICODE )
            flags |= MAPI_UNICODE;
         #endif

         if( pFrom && hb_arrayLen( pFrom ) >= 2 )
         {
            origin.lpszName    = ( LPSTR ) HB_ARRAYGETSTR( pFrom, 1, &hString[ iString++ ], NULL );
            origin.lpszAddress = ( LPSTR ) HB_ARRAYGETSTR( pFrom, 2, &hString[ iString++ ], NULL );
            note.lpOriginator  = &origin;
         }

         if( pToList && ( nLen = hb_arrayLen( pToList ) ) > 0 )
         {
            MapiRecipDesc recipList[ 100 ];
            ULONG ulCount = 0;

            ZeroMemory( recipList, sizeof( recipList ) );

            if( nLen >= HB_SIZEOFARRAY( recipList ) )
               nLen = HB_SIZEOFARRAY( recipList );

            for( i = 0; i < nLen; ++i )
            {
               PHB_ITEM pItem = hb_arrayGetItemPtr( pToList, i + 1 );

               if( HB_IS_ARRAY( pItem ) && hb_arrayLen( pItem ) >= 3 )
               {
                  if( hb_arrayGetCLen( pItem, 1 ) > 0 )
                  {
                     recipList[ ulCount ].lpszName = ( LPSTR ) HB_ARRAYGETSTR( pItem, 1, &hString[ iString++ ], NULL );

                     if( hb_arrayGetCLen( pItem, 2 ) > 0 )
                        recipList[ ulCount ].lpszAddress = ( LPSTR ) HB_ARRAYGETSTR( pItem, 2, &hString[ iString++ ], NULL );
                  }
                  else
                     recipList[ ulCount ].lpszName = ( LPSTR ) HB_ARRAYGETSTR( pItem, 2, &hString[ iString++ ], NULL );

                  recipList[ ulCount ].ulRecipClass = ( ULONG ) hb_arrayGetNL( pItem, 3 );

                  ++ulCount;
               }
            }

            note.lpRecips    = recipList;
            note.nRecipCount = ulCount;
         }
         else
            note.nRecipCount = 0;

         if( pFileList && ( nLen = hb_arrayLen( pFileList ) ) > 0 )
         {
            MapiFileDesc filedescList[ 100 ];
            ULONG ulCount = 0;

            ZeroMemory( filedescList, sizeof( filedescList ) );

            if( nLen >= HB_SIZEOFARRAY( filedescList ) )
               nLen = HB_SIZEOFARRAY( filedescList );

            for( i = 0; i < nLen; ++i )
            {
               PHB_ITEM pItem = hb_arrayGetItemPtr( pFileList, i + 1 );

               if( HB_IS_ARRAY( pItem ) && hb_arrayLen( pItem ) >= 1 )
               {
                  filedescList[ ulCount ].ulReserved   = 0;
                  filedescList[ ulCount ].lpszFileName = ( LPSTR ) HB_ARRAYGETSTR( pItem, 1, &hString[ iString++ ], NULL );
                  filedescList[ ulCount ].lpszPathName = ( LPSTR ) HB_ARRAYGETSTR( pItem, 2, &hString[ iString++ ], NULL );
                  filedescList[ ulCount ].nPosition    = ( ULONG ) -1;
                  ++ulCount;
               }
            }

            note.lpFiles    = filedescList;
            note.nFileCount = ulCount;
         }
         else
            note.nFileCount = 0;

         hb_retnint( ( *MAPISendMail )( 0, ( ULONG_PTR ) GetActiveWindow(), &note, flags, 0 ) );

         while( --iString >= 0 )
            hb_strfree( hString[ iString ] );
      }

      FreeLibrary( hMapiDll );
   }
}

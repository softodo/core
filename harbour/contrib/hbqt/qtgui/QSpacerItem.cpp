/*
 * $Id$
 */

/* -------------------------------------------------------------------- */
/* WARNING: Automatically generated source file. DO NOT EDIT!           */
/*          Instead, edit corresponding .qth file,                      */
/*          or the generator tool itself, and run regenarate.           */
/* -------------------------------------------------------------------- */

/*
 * Harbour Project source code:
 * QT wrapper main header
 *
 * Copyright 2009-2010 Pritpal Bedi <pritpal@vouchcac.com>
 *
 * Copyright 2009 Marcos Antonio Gambeta <marcosgambeta at gmail dot com>
 * www - http://harbour-project.org
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
/*----------------------------------------------------------------------*/

#include "../hbqt.h"
#include "hbqtgui_garbage.h"
#include "hbqtcore_garbage.h"

/*----------------------------------------------------------------------*/
#if QT_VERSION >= 0x040500
/*----------------------------------------------------------------------*/

#include <QtCore/QPointer>

#include <QtGui/QSpacerItem>


/* QSpacerItem ( int w, int h, QSizePolicy::Policy hPolicy = QSizePolicy::Minimum, QSizePolicy::Policy vPolicy = QSizePolicy::Minimum )
 *
 */

typedef struct
{
   QSpacerItem * ph;
   bool bNew;
   QT_G_FUNC_PTR func;
   int type;
} QGC_POINTER_QSpacerItem;

QT_G_FUNC( hbqt_gcRelease_QSpacerItem )
{
   QGC_POINTER * p = ( QGC_POINTER * ) Cargo;

   if( p && p->bNew )
   {
      if( p->ph )
      {
         HB_TRACE( HB_TR_DEBUG, ( "ph=%p    _rel_QSpacerItem   /.\\", p->ph ) );
         delete ( ( QSpacerItem * ) p->ph );
         HB_TRACE( HB_TR_DEBUG, ( "ph=%p YES_rel_QSpacerItem   \\./", p->ph ) );
         p->ph = NULL;
      }
      else
      {
         HB_TRACE( HB_TR_DEBUG, ( "ph=%p DEL_rel_QSpacerItem    :     Object already deleted!", p->ph ) );
         p->ph = NULL;
      }
   }
   else
   {
      HB_TRACE( HB_TR_DEBUG, ( "ph=%p PTR_rel_QSpacerItem    :    Object not created with new=true", p->ph ) );
      p->ph = NULL;
   }
}

void * hbqt_gcAllocate_QSpacerItem( void * pObj, bool bNew )
{
   QGC_POINTER * p = ( QGC_POINTER * ) hb_gcAllocate( sizeof( QGC_POINTER ), hbqt_gcFuncs() );

   p->ph = ( QSpacerItem * ) pObj;
   p->bNew = bNew;
   p->func = hbqt_gcRelease_QSpacerItem;
   p->type = HBQT_TYPE_QSpacerItem;

   if( bNew )
   {
      HB_TRACE( HB_TR_DEBUG, ( "ph=%p    _new_QSpacerItem", pObj ) );
   }
   else
   {
      HB_TRACE( HB_TR_DEBUG, ( "ph=%p NOT_new_QSpacerItem", pObj ) );
   }
   return p;
}

HB_FUNC( QT_QSPACERITEM )
{
   QSpacerItem * pObj = NULL;

   if( hb_pcount() >= 2 && HB_ISNUM( 1 ) && HB_ISNUM( 2 ) )
   {
      pObj = new QSpacerItem( hb_parni( 1 ), hb_parni( 2 ),
                  HB_ISNUM( 3 ) ? ( QSizePolicy::Policy ) hb_parni( 3 ) : QSizePolicy::Minimum,
                  HB_ISNUM( 4 ) ? ( QSizePolicy::Policy ) hb_parni( 4 ) : QSizePolicy::Minimum );
   }
   else if( hb_pcount() == 1 && HB_ISPOINTER( 1 ) )
   {
      pObj = new QSpacerItem( *hbqt_par_QSpacerItem( 1 ) ) ;
   }

   hb_retptrGC( hbqt_gcAllocate_QSpacerItem( ( void * ) pObj, true ) );
}

/*
 * void changeSize ( int w, int h, QSizePolicy::Policy hPolicy = QSizePolicy::Minimum, QSizePolicy::Policy vPolicy = QSizePolicy::Minimum )
 */
HB_FUNC( QT_QSPACERITEM_CHANGESIZE )
{
   QSpacerItem * p = hbqt_par_QSpacerItem( 1 );
   if( p )
      ( p )->changeSize( hb_parni( 2 ), hb_parni( 3 ), ( HB_ISNUM( 4 ) ? ( QSizePolicy::Policy ) hb_parni( 4 ) : ( QSizePolicy::Policy ) QSizePolicy::Minimum ), ( HB_ISNUM( 5 ) ? ( QSizePolicy::Policy ) hb_parni( 5 ) : ( QSizePolicy::Policy ) QSizePolicy::Minimum ) );
   else
   {
      HB_TRACE( HB_TR_DEBUG, ( "............................... F=QT_QSPACERITEM_CHANGESIZE FP=( p )->changeSize( hb_parni( 2 ), hb_parni( 3 ), ( HB_ISNUM( 4 ) ? ( QSizePolicy::Policy ) hb_parni( 4 ) : ( QSizePolicy::Policy ) QSizePolicy::Minimum ), ( HB_ISNUM( 5 ) ? ( QSizePolicy::Policy ) hb_parni( 5 ) : ( QSizePolicy::Policy ) QSizePolicy::Minimum ) ); p is NULL" ) );
   }
}

/*
 * virtual bool isEmpty () const
 */
HB_FUNC( QT_QSPACERITEM_ISEMPTY )
{
   QSpacerItem * p = hbqt_par_QSpacerItem( 1 );
   if( p )
      hb_retl( ( p )->isEmpty() );
   else
   {
      HB_TRACE( HB_TR_DEBUG, ( "............................... F=QT_QSPACERITEM_ISEMPTY FP=hb_retl( ( p )->isEmpty() ); p is NULL" ) );
   }
}

/*
 * virtual QSpacerItem * spacerItem ()
 */
HB_FUNC( QT_QSPACERITEM_SPACERITEM )
{
   QSpacerItem * p = hbqt_par_QSpacerItem( 1 );
   if( p )
      hb_retptrGC( hbqt_gcAllocate_QSpacerItem( ( p )->spacerItem(), false ) );
   else
   {
      HB_TRACE( HB_TR_DEBUG, ( "............................... F=QT_QSPACERITEM_SPACERITEM FP=hb_retptrGC( hbqt_gcAllocate_QSpacerItem( ( p )->spacerItem(), false ) ); p is NULL" ) );
   }
}


/*----------------------------------------------------------------------*/
#endif             /* #if QT_VERSION >= 0x040500 */
/*----------------------------------------------------------------------*/

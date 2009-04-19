/*
 * $Id$
 */

/*
 * Harbour Project source code:
 * QT wrapper main header
 *
 * Copyright 2009 Marcos Antonio Gambeta <marcosgambeta at gmail dot com>
 * Copyright 2009 Pritpal Bedi <pritpal@vouchcac.com>
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
/*----------------------------------------------------------------------*/


#include "hbclass.ch"


CREATE CLASS QTextBlock

   VAR     pParent
   VAR     pPtr

   METHOD  New()

   METHOD  blockFormat()                       INLINE  Qt_QTextBlock_blockFormat( ::pPtr )
   METHOD  blockFormatIndex()                  INLINE  Qt_QTextBlock_blockFormatIndex( ::pPtr )
   METHOD  blockNumber()                       INLINE  Qt_QTextBlock_blockNumber( ::pPtr )
   METHOD  charFormat()                        INLINE  Qt_QTextBlock_charFormat( ::pPtr )
   METHOD  charFormatIndex()                   INLINE  Qt_QTextBlock_charFormatIndex( ::pPtr )
   METHOD  clearLayout()                       INLINE  Qt_QTextBlock_clearLayout( ::pPtr )
   METHOD  contains( nPosition )               INLINE  Qt_QTextBlock_contains( ::pPtr, nPosition )
   METHOD  document()                          INLINE  Qt_QTextBlock_document( ::pPtr )
   METHOD  firstLineNumber()                   INLINE  Qt_QTextBlock_firstLineNumber( ::pPtr )
   METHOD  isValid()                           INLINE  Qt_QTextBlock_isValid( ::pPtr )
   METHOD  isVisible()                         INLINE  Qt_QTextBlock_isVisible( ::pPtr )
   METHOD  layout()                            INLINE  Qt_QTextBlock_layout( ::pPtr )
   METHOD  length()                            INLINE  Qt_QTextBlock_length( ::pPtr )
   METHOD  lineCount()                         INLINE  Qt_QTextBlock_lineCount( ::pPtr )
   METHOD  next()                              INLINE  Qt_QTextBlock_next( ::pPtr )
   METHOD  position()                          INLINE  Qt_QTextBlock_position( ::pPtr )
   METHOD  previous()                          INLINE  Qt_QTextBlock_previous( ::pPtr )
   METHOD  revision()                          INLINE  Qt_QTextBlock_revision( ::pPtr )
   METHOD  setLineCount( nCount )              INLINE  Qt_QTextBlock_setLineCount( ::pPtr, nCount )
   METHOD  setRevision( nRev )                 INLINE  Qt_QTextBlock_setRevision( ::pPtr, nRev )
   METHOD  setUserData( pData )                INLINE  Qt_QTextBlock_setUserData( ::pPtr, pData )
   METHOD  setUserState( nState )              INLINE  Qt_QTextBlock_setUserState( ::pPtr, nState )
   METHOD  setVisible( lVisible )              INLINE  Qt_QTextBlock_setVisible( ::pPtr, lVisible )
   METHOD  text()                              INLINE  Qt_QTextBlock_text( ::pPtr )
   METHOD  textList()                          INLINE  Qt_QTextBlock_textList( ::pPtr )
   METHOD  userData()                          INLINE  Qt_QTextBlock_userData( ::pPtr )
   METHOD  userState()                         INLINE  Qt_QTextBlock_userState( ::pPtr )

   ENDCLASS

/*----------------------------------------------------------------------*/

METHOD New( pParent ) CLASS QTextBlock

   ::pParent := pParent

   ::pPtr := Qt_QTextBlock( pParent )

   RETURN Self

/*----------------------------------------------------------------------*/


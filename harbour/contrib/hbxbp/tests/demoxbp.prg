/*
 * $Id$
 */

/*
 * Harbour Project source code:
 *
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

#include "common.ch"
#include "xbp.ch"
#include "appevent.ch"
#include "inkey.ch"

/*----------------------------------------------------------------------*/

PROCEDURE Main()
   LOCAL oDlg, mp1, mp2, oXbp, nEvent, aSize

   /* Create Application Window */
   oDlg := GuiStdDialog( 'Harbour - Xbase++ - QT Dialog [ Press "Q" to Exit ]' )

   /* Obtain desktop dimensions */
   aSize := AppDesktop():currentSize()
   /* Place on the center of desktop */
   oDlg:setPos( { ( aSize[ 1 ] - oDlg:currentSize()[ 1 ] ) / 2, ;
                  ( aSize[ 2 ] - oDlg:currentSize()[ 2 ] ) / 2 } )

   /* Install menu system */
   Build_MenuBar( oDlg )

   /* Make background color of :drawingArea different */
   oDlg:drawingArea:setColorBG( GraMakeRGBColor( { 134,128,164 } ) )

   /* Present the dialog on the screen */
   oDlg:Show()

   /* Enter Xbase++ Event Loop - still with limited functionality but working */
   DO WHILE .t.
      nEvent := AppEvent( @mp1, @mp2, @oXbp )

      IF nEvent == xbeP_Keyboard .and. mp1 == 81
         EXIT
      ENDIF
   ENDDO

   /* Very important - destroy resources */
   oDlg:destroy()

   RETURN

/*----------------------------------------------------------------------*/

STATIC FUNCTION GuiStdDialog( cTitle )
   LOCAL oDlg

   DEFAULT cTitle TO "Standard Dialog Window"

   oDlg          := XbpDialog():new( , , {10,10}, {900,500}, , .f. )

   /* NOTE: method to install the windows icon is bit different than Windows */
   /* So curretly we can only place disk icon file only */
   oDlg:icon     :=  "test"

   /* TODO: still not implemented*/
   oDlg:taskList := .T.

   oDlg:title    := cTitle
   oDlg:create()

   RETURN oDlg

/*----------------------------------------------------------------------*/

STATIC FUNCTION Build_MenuBar( oDlg )
   LOCAL oMenuBar, oSubMenu

   oMenuBar := XbpMenuBar():new( oDlg ):create()

   /* Define submenu in procedural style.
    * The numeric index of the selected menu item
    * is passed to the Callback code block -> mp1
    */
   oSubMenu := XbpMenu():new( oMenuBar ):create()
   //
   oSubMenu:title := "~Procedural"
   oSubMenu:addItem( { "Play Charge ~1",   } )
   oSubMenu:addItem( { "Play Nannyboo ~2", } )
   oSubMenu:itemSelected := {|mp1| MyFunctionXbp( 100+mp1 ) }
   //
   oMenuBar:addItem( { oSubMenu, NIL } )
   //
   oSubMenu:disableItem( 2 )

   /* Define submenu in the functional style:
    * A menu item executes a code block that calls a function
    */
   oSubMenu := XbpMenu():new( oMenuBar ):create()
   oSubMenu:title := "~Functional"
   oSubMenu:addItem( { "Play Opening ~1"+chr(K_TAB)+"Ctrl+U", {|| MyFunctionXbp( 1 ) } } )
   oSubMenu:addItem( { "Play Closing ~2", {|| MyFunctionXbp( 2 ) } } )
   oSubMenu:addItem( { NIL, NIL, XBPMENUBAR_MIS_SEPARATOR, NIL } )
   oSubMenu:addItem( { "new.png|~MessageBox", {|| MyFunctionXbp( 3 ) }, , XBPMENUBAR_MIA_HILITED } )
   //
   oMenuBar:addItem( { oSubMenu, NIL } )
   //
   oSubMenu:insItem( 2, { "This executes MsgBox()", {|| MyFunctionXbp( 103 ) }, , XBPMENUBAR_MIA_CHECKED } )
   //
   oSubMenu:itemMarked := {|mp1| IF( mp1 == 5, MsgBox( "WOW - ::itemMarked - Activated" ), NIL ) }
   //
   oSubMenu:setColorFG( GraMakeRGBColor( { 255,  1,  1 } ) )
   oSubMenu:setColorBG( GraMakeRGBColor( { 134,128,250 } ) )

   Return nil

/*----------------------------------------------------------------------*/

STATIC FUNCTION MyFunctionXbp( nMode )

   DO CASE
   CASE nMode == 1
      MsgBox( "Play Opening ~1" )

   CASE nMode == 2
      MsgBox( "Play Closing ~2" )

   CASE nMode == 3
      MsgBox( "new.png|~MessageBox" )

   CASE nMode == 101
      MsgBox( "101 - Play Charge" )

   CASE nMode == 102
      MsgBox( "102 - Play Nanyboo" )

   CASE nMode == 103
      MsgBox( "This executes MsgBox()" )

   ENDCASE

   RETURN nil

/*----------------------------------------------------------------------*/

PROCEDURE AppSys()
   RETURN

/*----------------------------------------------------------------------*/



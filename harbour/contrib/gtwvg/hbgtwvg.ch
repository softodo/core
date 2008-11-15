/*
 * $Id$
 */

/*
 * Harbour Project source code:
 * Header file for the WVG*Classes
 *
 * Copyright 2004 Pritpal Bedi <pritpal@vouchcac.com>
 * www - http://www.xharbour.org http://www.harbour-project.org
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

#ifndef _HBGTWVG_CH
#define _HBGTWVG_CH

//----------------------------------------------------------------------//
//                 Extended GT Manipulation Constants
//----------------------------------------------------------------------//

#define HB_GTE_MOUSE            6
#define HB_GTE_KEYBOARD         7

#define HB_GTI_SETFONT                   71
#define HB_GTI_PRESPARAMS                72
#define HB_GTI_ENABLE                    73
#define HB_GTI_DISABLE                   74
#define HB_GTI_SETFOCUS                  75
#define HB_GTI_DEFERPAINT                76

/* Presentation Parameters | HB_GTI_PRESPARAMS */
#define HB_GTI_PP_EXSTYLE                 1
#define HB_GTI_PP_STYLE                   2
#define HB_GTI_PP_X                       3
#define HB_GTI_PP_Y                       4
#define HB_GTI_PP_WIDTH                   5
#define HB_GTI_PP_HEIGHT                  6
#define HB_GTI_PP_PARENT                  7
#define HB_GTI_PP_VISIBLE                 8
#define HB_GTI_PP_ROWCOLS                 9

#define HB_GTI_PP_SIZE                    9


#define HB_GTI_SPEC                    1000

/* Window Specifications | HB_GTI_SPEC */
#define HB_GTS_WINDOWHANDLE               1
#define HB_GTS_CENTERWINDOW               2
#define HB_GTS_PROCESSMESSAGES            3
#define HB_GTS_KEYBOARD                   4
#define HB_GTS_RESETWINDOW                5
#define HB_GTS_WNDSTATE                   6
#define HB_GTS_SETTIMER                   7
#define HB_GTS_KILLTIMER                  8
#define HB_GTS_SETPOSITION                9
#define HB_GTS_SHOWWINDOW                10
#define HB_GTS_UPDATEWINDOW              11
#define HB_GTS_SYSTRAYICON               12
#define HB_GTS_FACTOR                    13


/* Window States | HB_GTS_WNDSTATE */
#define HB_GTS_WS_SETONTOP                1
#define HB_GTS_WS_SETASNORMAL             2
#define HB_GTS_WS_MINIMIZED               3
#define HB_GTS_WS_MAXIMIZED               4
#define HB_GTS_WS_HIDDEN                  5
#define HB_GTS_WS_NORMAL                  6

/* SysTrayIcon Types | HB_GTS_SYSTRAYICON */
#define HB_GTS_NIT_FILE                   0
#define HB_GTS_NIT_RESOURCEBYNAME         1
#define HB_GTS_NIT_RESOURCEBYID           2

/* ShowWindow modes | HB_GTS_SHOWWINDOW */
#define HB_GTS_SW_NORMAL                  1
#define HB_GTS_SW_RESTORE                 2
#define HB_GTS_SW_HIDE                    3
#define HB_GTS_SW_MINIMIZED               4
#define HB_GTS_SW_MAXIMIZED               5

#define HB_GTI_GUI                     1001

//----------------------------------------------------------------------//
//                        Class Framework Constants
//----------------------------------------------------------------------//

#define objTypeNone       0
#define objTypeCrt        1
#define objTypeWindow     2
#define objTypeActiveX    3

//----------------------------------------------------------------------//

#endif

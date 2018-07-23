/*
Eva library C++ (www.elxala.de)
(c) Copyright 2018 Alejandro Xalabarder Aulet

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef _ZWIDGETLAYABLE_HEADER
#define _ZWIDGETLAYABLE_HEADER

#include <windows.h>

#include <string>

#include "EvaLayable.h"
#include "traceMacro.h"

using namespace std;

// Specific zWidgetLayable for windows API
//
//    class zWidgetLayable
//       { getPreferredSize(), doHide(), doShowAt(), getName(), layableData, nativeWidgetHandle, fixedDy }
//
class zWidgetLayable
{
   public:
   
   //RECT toRECT (uniRect)
   //{
   //   RECT rec;
   //   return rec;
   //}

   // NOTE about parameter "isComboWithIssue"
   //    LayoutManager set the width and height of the components according to the
   //    layout information (which makes sense) but it seems that the component combo box
   //    by design defines its height as another thing (which makes no sense at all!).
   //
   //    Setting isComboWithIssue to true fixes the issue
   //
   zWidgetLayable (const string & widetName, HWND winHandle, bool isComboWithIssue = false):
      layableData (widetName),
      winApiHandle (winHandle),
      fixedDy (isComboWithIssue ? 220: 0)
   {
      RECT rec;
      GetWindowRect (winHandle, & rec);
      layableData.iniRect = uniRect (rec.left, rec.top, rec.right - rec.left, rec.bottom - rec.top);
   }

   uniRect getPreferredSize ()
   {
      return layableData.iniRect;
   }

   void doHide ()
   {
      ShowWindow (winApiHandle, SW_HIDE);
   }

   void doShowAt (int x, int y, int dx, int dy)
   {
      //:SEC EvaLayoutMechanism  | manager -> n:MoveWindow -> windows | physical positioning
      //
      // physical positioning and show/hide of the widget using the
      // windows methods MoveWindow and ShowWindow, the only needed is
      // the position and the component handle
      //
      
      //printf ("%s WI/doShowAt (%d %d %d %d)\n", getName ().c_str (), x, y, dx, dy);
      MoveWindow (winApiHandle, x, y, dx, fixedDy > 0 ? fixedDy: dy, TRUE);
      ShowWindow (winApiHandle, SW_SHOW);
   }

   string & getName ()
   {
      return layableData.name;
   }

   EvaLayable layableData;
   HWND winApiHandle;
   int fixedDy;
};


#endif

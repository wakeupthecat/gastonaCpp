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

#include <gtk/gtk.h>

#include "EvaLayable.h"
#include "traceMacro.h"

using namespace std;

// Specific zWidgetLayable for linux GTK+2.0
//
//    class zWidgetLayable
//       { getPreferredSize(), doHide(), doShowAt(), getName(), layableData, nativeWidgetHandle, fixedDy }
//
// data structure used by EvaLayout to store precalculated
// information about a component, that can be either a win widget or another EvaLayout
//
// object type to store a win api widget or window handle
// it has the member layableData of type EvaLayable to be used by EvaLayout
//
class zWidgetLayable
{
   public:

   // NOTE about parameter "fixador"
   //    Since we want to positionate the widgets by ours we need
   //    a so called fix component that has to be the unique widget in the main window.
   //    Therefore it has to be created and put into that window and then pass it to all widgets
   //    belonging to the window.
   //
   //
   zWidgetLayable (const string & widgetName, GtkWidget * nativePtr, GtkWidget * fixador):
      layableData        (widgetName),
      nativeWidgetHandle (nativePtr),
      fixedGtk           (fixador) // needed for gtk_fixed_put
   {
      // to get default sizes (it does not work!!)
      //gtk_widget_get_preferred_size (GTK_WIDGET (nativePtr), & layableData.iniRect.dx, & layableData.iniRect.dy);

      //=== This works (compiles) but gives always dx = dy = -1
      //int dx, dy;
      //gtk_widget_get_size_request (GTK_WIDGET (nativePtr), &dx, &dy);
      //layableData.iniRect.dx = dx;
      //layableData.iniRect.dy = dy;
      //TRACE(("widgetLayable %s size %d, %d", widgetName.c_str (), dx, dy ));

      //GTK_WIDGET_GET_CLASS (nativePtr)->get_preferred_width (nativePtr, &mini, &nata);
      //GTK_WIDGET_GET_CLASS (nativePtr)->get_preferred_height (nativePtr, &hmini, &hnata);

      // int width = nativePtr->get_allocation().width;
      // int height = nativePtr->get_allocation().height;

      //gtk_widget_get_preferred_size (GTK_WIDGET (nativePtr))
      layableData.iniRect = uniRect (0, 0, 22*15, 22);

      // put the widget in the position container (only once!)
      gtk_fixed_put(GTK_FIXED(fixedGtk), nativeWidgetHandle, 0, 0);
   }

   uniRect getPreferredSize ()
   {
      return layableData.iniRect;
   }

   void doHide ()
   {
      gtk_widget_hide (nativeWidgetHandle);
   }

   void doShowAt (int x, int y, int dx, int dy)
   {
      //:SEC EvaLayoutMechanism  | manager -> n:MoveWindow -> windows | physical positioning
      //
      // physical positioning and show/hide of the widget using the
      // windows methods MoveWindow and ShowWindow, the only needed is
      // the position and the component handle
      //

      printf ("%s WI/doShowAt (%d %d %d %d)\n", getName ().c_str (), x, y, dx, dy);
      if (nativeWidgetHandle)
      {
         if (x >= 0 && y >= 0 && dx > 0 && dy > 0)
         {
            gtk_fixed_move(GTK_FIXED(fixedGtk), nativeWidgetHandle, x, y);
            gtk_widget_set_size_request(nativeWidgetHandle, dx, dy);
            gtk_widget_show (nativeWidgetHandle);
         }
         else {
            // very important! hide widget to avoid
            // the bizarre exception : "CRITICAL **: murrine_style_draw_focus: assertion 'width >= -1' failed"
            gtk_widget_hide (nativeWidgetHandle);
         }
      }
   }

   string & getName ()
   {
      return layableData.name;
   }

   EvaLayable layableData;
   GtkWidget * nativeWidgetHandle;
   GtkWidget * fixedGtk;
};



#endif

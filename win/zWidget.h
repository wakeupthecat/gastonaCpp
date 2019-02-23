/*
Gastona C++ 
Copyright (C) 2016-2019  Alejandro Xalabarder Aulet

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef zWIDGET_HEADER_H
#define zWIDGET_HEADER_H

#include <windows.h>
#include "commctrl.h"

#include <vector>
#include <string>

#include "Eva.h"
#include "EvaLine.h"
#include "EvaUnit.h"
#include "EvaLayoutManager.h"

using namespace std;

class zWidget
{
protected:

   HWND  hwnd;   // each actual widget will create its own window and assign this handle
   string name;  // name of the widget, it has to be unique in a gastona session
   HMENU unid;   // a unique id assigned automatically in this base class

   // some controls need it ? (button, label but not Table !)
   HFONT getAuxFont ()
   {
      static HFONT auxFont = CreateFont(14, 0, 0, 0, 0, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Arial"));
      return auxFont;
   }

   HFONT getAuxFontBig ()
   {
      static HFONT auxFont = CreateFont(16, 0, 0, 0, 0, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Arial"));
      return auxFont;
   }

   void setFontNormal ()
   {
      SendMessage (hwnd, WM_SETFONT, (WPARAM) getAuxFont (), 0);
   }

   void setFontBig ()
   {
      // SendMessage (hwnd, WM_SETFONT, (WPARAM)getAuxFontBig   (), 0);
   }

   int widthForText (const string & text)
   {
      return widthForText (text.length ());
   }

   int widthForText (int nchars)
   {
      int cxChar = LOWORD (GetDialogBaseUnits ());
      //int cyChar = HIWORD (GetDialogBaseUnits ());
      if (nchars < 0) return -1;
      return cxChar * 1.1 * (nchars > 0 ? nchars: 1);
   }

   int heightForText (float nchars = 0.)
   {
      //int cxChar = LOWORD (GetDialogBaseUnits ());
      int cyChar = HIWORD (GetDialogBaseUnits ());

      return cyChar * (nchars == 0. ? 1.1: nchars);
   }

public:

   // data and control is shared by all widgets!
   // (blackboard principle)
   //
   static EvaUnit & getDataAndControl ()
   {
      static EvaUnit euDataAndCtrlObj = EvaUnit("data&control");
      return euDataAndCtrlObj;
   }

   zWidget (const string & pname):
      hwnd(0),
      name(pname),
      unid(0)
   {
      // assign uniq id for the widget
      static HMENU currentUniqueID = (HMENU) 1000;
      unid = currentUniqueID ++;
   }

   virtual ~zWidget ()
   {
      // DO NOT call destroy () here to allow copies in maps and vectors!!!
      // if the zWidget is passed to GastonaC then GastonaC.destroy will destroy all zWidgets
      // if not (use not intended in gastonaWinter) the zWidget has to be destroyed explicitly

      // NON COPYABLE stuff would complicate too much all
      //    zWidget (const zWidget&) = delete;
      //    zWidget & operator=(const zWidget &) = delete;
   }

   // due to windows message WM_COMMAND to this widget (getControlID ())
   //
   virtual void winMessage    (HWND hwnd, int winHiWord) { };
   virtual bool winKeyMessage (HWND hwnd, UINT message, WPARAM tecla) { return false; };

   static void mergeDataAndControlUnit (EvaUnit & euData)
   {
      printf ("TODO: implement mergeDataAndControlUnit!");
   }

   static void setDataAndControlUnit (EvaUnit & euData)
   {
      getDataAndControl () = euData;
   }

   void destroy ()
   {
      if (hwnd != 0)
         DestroyWindow (hwnd);
   }

   const string & getName      ()   { return name;   }
   HWND           getNativeHandle ()   { return hwnd;   }
   HMENU &        getControlID ()   { return unid;   }
   EvaUnit &      getDataUnit  ()   { return getDataAndControl (); }
   EvaUnit &      getControlUnit () { return getDataAndControl (); }
   bool           isNIL ()          { return name.length() == 0; } // in the desert a zWidget without name (Chicago)

   /**
      gets the Eva variable attribute with name 'attName' from the widget
      the container must exist, if the Eva variable still does not exists a null is returned.
      This is the way a default value
   */
   Eva & getAttribute (const string & attName, bool force = false)
   {
      string attEvaName = name + (attName.length () > 0 ? (" " + attName): "");

      // first try if we have referenced the attribute to another place (<NAME ATT var>)
      //
      Eva & eva1 = getDataAndControl () [string (attEvaName + " var")];
      if (! EvaUnit::isNILEva (eva1))
      {
         // found eva <widname var>
         // not its contain is the name of the new data variable
         string refEvaName = eva1.getValue ();
         //printf ("(%s) data var referenced to (%s)", attEvaName.c_str (), refEvaName.c_str ());
         Eva & eva3 = getDataAndControl () [refEvaName];
         if (! EvaUnit::isNILEva (eva3))
            return eva3;
         if (force)
         {
            getDataAndControl ().setValue (refEvaName, "");
            return getDataAndControl () [refEvaName];
         }
      }

      Eva & eva2 = getDataAndControl () [attEvaName];
      if (EvaUnit::isNILEva (eva2))
      {
         if (force)
         {
            getDataAndControl ().setValue (attEvaName, "");
            return getDataAndControl () [attEvaName];
         }
      }
      return eva2;
   }


   // used by message "data!"
   //
   void setVariablesFromArray (const vector<string> & arrAttVal)
   {
      // arrAttVal = attrib1, val1, attrib2, val2 .. etc
      //
      for (int vv = 0; vv < arrAttVal.size (); vv += 2)
      {
         getAttribute(arrAttVal[vv], true).setValue (arrAttVal[vv + 1]);
      }
   }
};

#endif  // _HEADER_H

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

#ifndef zTEXTAREA_HEADER_H
#define zTEXTAREA_HEADER_H

#include "zWidget.h"
#include "Mensaka.h"

using namespace std;

class zTextArea : public zWidget, public MensakaTarget
{
public:
   enum
   {
      UPDATE_DATA
   };

   zTextArea (HWND hwinParent, const string & pname): zWidget(pname)
   {
      hwnd = CreateWindow (WC_EDIT, "",
                          WS_VISIBLE | WS_CHILD | SS_LEFT | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN,
                          0, 0,
                          widthForText  (20),
                          heightForText (),
                          hwinParent,
                          (HMENU) unid,
                          (HINSTANCE) GetWindowLong (hwnd, GWLP_HINSTANCE),
                          NULL);

      Mensaka::subscribeToMessage (getName () + " data!", UPDATE_DATA, this);
      setFontBig ();
   }

   ~zTextArea ()
   {
   }


   bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      if (mappedMsg == UPDATE_DATA)
      {
         TRACE2 (("mira var[%s] val[%s]\n", params.size () > 1 ? params[0].c_str ():"?", params.size () > 1 ? params[1].c_str ():"?"));
         setVariablesFromArray (params);
         updateData ();
         return true;
      }
      return false;
   }

   void updateData ()
   {
      //string value = getAttribute ("").getValue ();
      //setText (value);
      Eva & etext = getAttribute ("");

      string stext = "";
      for (int ii = 0; ii < etext.rows (); ii ++)
         stext += etext.getValue (ii) + "\n";

      setText (stext);
   }

   void vuelcaData ()
   {
      int len = GetWindowTextLength (getNativeHandle ());
      TCHAR * pVarName = (TCHAR *) calloc (len + 3, sizeof (TCHAR));
      if (pVarName)
         pVarName[len] = 0;

      GetWindowText(getNativeHandle (), pVarName, len+1);

      // set the variable with the current control content
      getAttribute ("", true).setValue (string (pVarName));

      //printf ("currito es [%s]\n", getAttribute("").getValue().c_str ());
      free (pVarName);
   }

   virtual void winMessage (HWND hwin, int winHiWord)
   {
      if (winHiWord == WM_KILLFOCUS || winHiWord == EN_KILLFOCUS)
      {
         // losing focus, so revert current value (text) to data
         vuelcaData ();
      }
   }

   virtual bool winKeyMessage (HWND forHwnd, UINT message, WPARAM tecla)
   {
      if (forHwnd != hwnd) return false;

      return false;
   }


   void setText (const string & value)
   {
      SetWindowTextA  (getNativeHandle (), value.c_str ());
   }
};

#endif //  HEADER_H

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

#ifndef zCHECKBOX_HEADER_H
#define zCHECKBOX_HEADER_H

#include "zWidget.h"
#include "Mensaka.h"

using namespace std;

class zCheckBox : public zWidget, public MensakaTarget
{
private:

public:
   enum
   {
      UPDATE_DATA
   };

   zCheckBox (HWND hwinParent, const string & pname): zWidget(pname)
   {
      hwnd = CreateWindow (WC_BUTTON, "",
                          WS_VISIBLE |  WS_CHILD |  BS_CHECKBOX,
                          0, 0,
                          widthForText  (pname),
                          heightForText (),
                          hwinParent,
                          (HMENU) unid,
                          (HINSTANCE) GetWindowLong (hwnd, GWLP_HINSTANCE),
                          NULL);

      //setText (const string & value);
      Mensaka::subscribeToMessage (getName () + " data!", UPDATE_DATA, this);
      setFontNormal ();
   }

   ~zCheckBox ()
   {
   }


   bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      if (mappedMsg == UPDATE_DATA)
      {
         printf ("mira var[%s] val[%s]\n", params.size () > 1 ? params[0].c_str ():"?", params.size () > 1 ? params[1].c_str ():"?");
         setVariablesFromArray (params);
         updateData ();
         return true;
      }
      return false;
   }

   void updateData ()
   {
      Eva & edata = getAttribute ("");

      if (EvaUnit::isNILEva (edata))
           setText (getName ().substr (1));
      else setText (edata.getValue ());
   }

   virtual void winMessage (HWND hwnd, int winHiWord)
   {
      if (winHiWord == 0) // press
      {
         // set attribute "selected" to "1" or to "0", e.g. "<kMyCheco selected> 1"
         //
         Eva & edata = getAttribute ("selected", true);

         //edata.setValue ((SendMessage (hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED) ? "1": "0");
         BOOL checked = IsDlgButtonChecked(hwnd, (int) unid);
         if (checked)
         {
            CheckDlgButton(hwnd, (int) unid, BST_UNCHECKED);
         }
         else
         {
            CheckDlgButton(hwnd, (int) unid, BST_CHECKED);
         }
         edata.setValue (checked ? "1": "0");
         Mensaka::sendPacket (getName (), getDataAndControl ());
      }
      //else printf ("hombre! ME TOCAS EL %d\n", winHiWord);
   }


   void setText (const string & value)
   {
      SetWindowTextA  (getNativeHandle (), value.c_str ());
   }
};

#endif //  HEADER_H

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

#ifndef zTOOLBOX_HEADER_H
#define zTOOLBOX_HEADER_H

#include "zWidget.h"
#include "Mensaka.h"

using namespace std;

class zToolbox : public zWidget, public MensakaTarget
{
private:

public:
   enum
   {
      UPDATE_DATA
   };

   zToolbox (HWND hwinParent, const string & pname): zWidget(pname)
   {

      hwnd = ::CreateWindowEx(
               WS_EX_PALETTEWINDOW,
               TOOLBARCLASSNAME,
               TEXT(""),
               WS_TOOLBARSTYLE,
               0, 0,
               0, 0,
               hwinParent,
               NULL,
               (HINSTANCE) GetWindowLong (hwnd, GWLP_HINSTANCE),
               0);
      // Send the TB_BUTTONSTRUCTSIZE message, which is required for
      // backward compatibility.
      ::SendMessage(hwnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
      ::SendMessage(hwnd, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_HIDECLIPPEDBUTTONS);

      //setText (const string & value);
      Mensaka::subscribeToMessage (getName () + " data!", UPDATE_DATA, this);

      setFontNormal ();
   }

   ~zToolbox ()
   {
   }


   bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      if (mappedMsg == UPDATE_DATA)
      {
         TRACE2 (("mira var[%s] val[%s]\n", params.size () > 1 ? params[0].c_str ():"?", params.size () > 1 ? params[1].c_str ():"?"));
         setVariablesFromArray (params);
         updateData ();
      }
   }

   void updateData ()
   {
      Eva & edata = getAttribute ("");

      if (EvaUnit::isNILEva (edata))
           setText (getName ().substr (1));
      else setText (edata.getValue ());
   }

   virtual void winMessage (HWND hwin, int winHiWord)
   {
      if (winHiWord == 0) // press
      {
         //printf ("button [%s] pressed!\n", getName ().c_str ());
         Mensaka::sendPacket (getName (), getDataAndControl ());
      }
   }


   void setText (const string & value)
   {
      SetWindowTextA  (getWinHandle (), value.c_str ());
   }
};

#endif //  HEADER_H

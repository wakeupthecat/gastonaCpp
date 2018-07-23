/*
*/

#ifndef zEDITFIELD_HEADER_H
#define zEDITFIELD_HEADER_H

#include "zWidget.h"
#include "Mensaka.h"

using namespace std;

class zEditField : public zWidget, public MensakaTarget
{
public:
   enum
   {
      UPDATE_DATA
   };

   zEditField (HWND hwinParent, const string & pname): zWidget(pname)
   {
      hwnd = CreateWindow (WC_EDIT, "",
                          WS_VISIBLE | WS_CHILD | SS_LEFT | WS_BORDER,
                          0, 0,
                          widthForText  (20),
                          heightForText (1.2),
                          hwinParent,
                          (HMENU) unid,
                          (HINSTANCE) GetWindowLong (hwnd, GWLP_HINSTANCE),
                          NULL);

      Mensaka::subscribeToMessage (getName () + " data!", UPDATE_DATA, this);
      setFontNormal ();
   }

   ~zEditField ()
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
      string value = getAttribute ("").getValue ();
      setText (value);
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

      if (message == WM_KEYDOWN && tecla == VK_RETURN)
      {
         vuelcaData ();
         Mensaka::sendPacket (getName (), getDataAndControl ()); // p.e. --> eSample
         return true;
      }

      return false;
   }


   void setText (const string & value)
   {
      SetWindowTextA  (getNativeHandle (), value.c_str ());
   }
};

#endif //  HEADER_H

/*
*/

#ifndef zLABEL_HEADER_H
#define zLABEL_HEADER_H

#include "zWidget.h"
#include "Mensaka.h"

using namespace std;

class zLabel : public zWidget, public MensakaTarget
{
public:
   enum
   {
      UPDATE_DATA
   };

   zLabel (HWND hwinParent, const string & pname): zWidget(pname)
   {
      hwnd = CreateWindow (WC_STATIC, "",
                          WS_VISIBLE | WS_CHILD | SS_LEFT,
                          0, 0,
                          widthForText  (pname),
                          heightForText (),
                          hwinParent,
                          (HMENU) unid,
                          (HINSTANCE) GetWindowLong (hwnd, GWLP_HINSTANCE),
                          NULL);

      Mensaka::subscribeToMessage (getName () + " data!", UPDATE_DATA, this);
      setFontNormal ();
   }

   ~zLabel ()
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

   virtual void winMessage (HWND hwin, int winHiWord)
   {
   }


   void setText (const string & value)
   {
      SetWindowTextA  (getNativeHandle (), value.c_str ());
   }
};

#endif //  HEADER_H

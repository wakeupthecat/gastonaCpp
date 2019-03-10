/*
*/

#ifndef zLABEL_HEADER_H
#define zLABEL_HEADER_H

#include <gtk/gtk.h>

#include "zWidget.h"
#include "Mensaka.h"
#include "uniRect.h"

using namespace std;

class zLabel : public zWidget, public MensakaTarget
{
private:

public:
   enum
   {
      MSK_UPDATE_DATA
   };

   zLabel (GtkWidget * hwinParent, const string & pname): zWidget(pname)
   {
      hwnd = gtk_label_new (pname.c_str ());
      //setText (const string & value);
      Mensaka::subscribeToMessage (getName () + " data!", MSK_UPDATE_DATA, this);
   }

   ~zLabel ()
   {
   }

   uniRect getDefaultSize ()
   {
      return uniRect (0, 0, widthForText(getName ()), heightForText());
   }


   bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      if (mappedMsg == MSK_UPDATE_DATA)
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
      Eva & edata = getAttribute ("");
      
      setText (EvaUnit::isNILEva (edata) ? getName ().substr (1): edata.getValue ());
      Eva & atip = getAttribute ("tooltip");
      if (not EvaUnit::isNILEva (atip))
           gtk_widget_set_tooltip_text(hwnd, atip.getValue ().c_str ());
      else gtk_widget_set_tooltip_text(hwnd, "");
   }

   void setText (const string & value)
   {
      gtk_label_set_text ((GtkLabel *) hwnd, value.c_str ());
   }
};

#endif //  HEADER_H

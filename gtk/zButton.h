/*
*/

#ifndef zBUTTON_HEADER_H
#define zBUTTON_HEADER_H

#include <gtk/gtk.h>

#include "zWidget.h"
#include "Mensaka.h"
#include "uniRect.h"

using namespace std;

class zButton : public zWidget, public MensakaTarget
{
private:

public:
   enum
   {
      MSK_UPDATE_DATA
   };

   zButton (GtkWidget * hwinParent, const string & pname): zWidget(pname)
   {
      hwnd = gtk_button_new_with_label (pname.c_str ());

      // notify to click of native gtkk widget
      // we have to provide the static fucntion, so we will pass this as parameter
      // to be able to receive it finally in the object
      gtk_signal_connect (GTK_OBJECT (hwnd),
                          "clicked",
                          GTK_SIGNAL_FUNC (zButton::static_clicked_callback),
                          this);

      //gtk_container_add (GTK_CONTAINER (hwinParent), hwnd);

      //setText (const string & value);
      Mensaka::subscribeToMessage (getName () + " data!", MSK_UPDATE_DATA, this);
   }

   ~zButton ()
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
      else gtk_widget_set_tooltip_text(hwnd, "rabanáles!");
   }

   void on_button_clicked ()
   {
      //printf ("button [%s] pressed!\n", getName ().c_str ());
      Mensaka::sendPacket (getName (), getDataAndControl ());
   }

   void setText (const string & value)
   {
      gtk_button_set_label ((GtkButton *) hwnd, value.c_str ());
      //what is better ?
      //
      //g_object_set_data (G_OBJECT(hwnd), "char", (gpointer) value.c_str ());
   }

   // gtk callback, it has to be a static method
   // data will be the pointer to the object that
   // has been actually clicked
   static void static_clicked_callback (GtkWidget * button, gpointer* data)
   {
      reinterpret_cast<zButton *>(data) -> on_button_clicked ();
   }
};

#endif //  HEADER_H

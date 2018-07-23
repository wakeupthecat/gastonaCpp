/*
*/

#ifndef GASTONAGTK_HEADER
#define GASTONAGTK_HEADER


#include <map>

#include "gastonaCBase.h"
// #include "zTable.h"
// #include "zTree.h"
// #include "zLabel.h"
#include "zButton.h"
// #include "zRadioButton.h"
// #include "zCheckBox.h"
// #include "zGroupBox.h"
//
// #include "zEditField.h"
// #include "zTextArea.h"

#include "EvaLayoutManager.h"
#include "EvaFile.h"
#include "Mensaka.h"

using namespace std;

/**
   @name: GastonaGtk.h

   @desc: hace las funciones de javaj como fábrica de widgets

   @methods:

      bool      broadcastKey         (HWND hwnd, UINT message, WPARAM tecla)
      bool      loadGUI              (HWND hwnd, const string & gastFileName)
      bool      loadGUI              (HWND hwnd, EvaUnit & ujavaj, EvaUnit & udata)
      void      addAndControlzWidget (zWidget * zw)
      void      showLayout           (HWND frameHwnd, bool forceInvalidate = false)
      bool      haszWidgetName       (const string & name)
      zWidget * getzWidgetByName     (const string & name)
      void      removezWidget        (HMENU zwID)


*/
class GastonaGtk: public gastonaCBase
{
public:

   typedef gastonaCBase super;

   GastonaGtk (): super ()
   {
   }

public:

   bool broadcastKey (GtkWidget * widget, GdkEventKey *event, gpointer data)
   {
      return false;
   }

   bool loadGUI (GtkWidget * frame, GtkWidget * fixer, const string & gastFileName)
   {
      if (!loadGast (gastFileName))
         return false;
      return loadGUI (frame, fixer, wastEvafile["javaj"], wastEvafile["data"]);
   }

   bool loadGUI (GtkWidget * frame, GtkWidget * fixer, EvaUnit & ujavaj, EvaUnit & udata)
   {
      // assign data unit for all widgets
      //
      zWidget::setDataAndControlUnit (udata);

      //printf ("habemos data:\n%s\n---\n", udata.toString ().c_str ());
      Mensaka::sendPacket (gastonaCGlobals::msgDATA_FIRST_LOAD (), zWidget::getDataAndControl ());
      //printf ("transpuesta data:\n%s\n---\n", zWidget::getDataAndControl ().toString ().c_str ());

      // printf ("javaj :\n%s\n---\n", ujavaj.toString ().c_str ());

      layManager.setLayouts (ujavaj);

      //:SEC
      //
      // All widgets are instanciated and set to the manager
      //
      vector<string> wnames = getAllWidgetNames (ujavaj);

      for (int ww = 0; ww < wnames.size (); ww ++)
      {
         string & winam = wnames[ww];

         char letra = winam[0];

         if (letra == '-' || letra == '+') continue;

         //it is a widget
         zWidget * wiObj = 0;

         switch (letra)
         {
            //case 'a': wiObj = new zTree (hwnd, winam); break;
            //case 'l': wiObj = new zLabel (hwnd, winam); break;
            case 'b': wiObj = new zButton (frame, winam); break;
            //case 'k': wiObj = new zCheckBox (hwnd, winam); break;
            //case 'g': wiObj = new zGroupBox (hwnd, winam); break;
            //case 'r': wiObj = new zRadioButton (hwnd, winam); break;
            //case 'e': wiObj = new zEditField (hwnd, winam); break;
            //case 'i': wiObj = new zTable (hwnd, winam, zTable::ASPECT_LIST); break;
            //case 't': wiObj = new zTable (hwnd, winam, zTable::ASPECT_TABLE); break;
            //case 'x': wiObj = new zTextArea (hwnd, winam); break;
            default:
               winam = "??" + winam;
               wiObj = new zButton (frame, winam); break;
               break;
         }
         if (wiObj != 0)
         {
            //printf ("INSERTAMOS UN COSO!! name [%s]\n", wiObj->getName ().c_str ());
            addAndControlzWidget (wiObj);

            // layManager.addComponent(winam, getzWidgetByName (winam).getWinHandle ());
            zWidgetLayable wila = zWidgetLayable(winam, getzWidgetByName (winam)->getNativeHandle (), fixer);
            layManager.addWidget (wila);

            Mensaka::sendPacket (wiObj->getName () + " data!", zWidget::getDataAndControl ());
         }
      }

      return true;
   }

   void showLayout (GtkWidget * frame)
   {
      int dx, dy;
      gtk_window_get_size (GTK_WINDOW (frame), &dx, &dy);
      showLayout (dx, dy);
      gtk_widget_show_all (frame);
   }

   void showLayout (int dx, int dy)
   {
      gastonaCBase::showLayout (dx, dy);
   }
};

#endif  // _HEADER_H

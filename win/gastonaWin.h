/*
*/

#ifndef GASTONAC_HEADER
#define GASTONAC_HEADER

#include <windows.h>
#include <map>

#include "gastonaCGlobals.h"
#include "gastonaCBase.h"

#include "zTable.h"
#include "zTree.h"
#include "zLabel.h"
#include "zButton.h"
#include "zRadioButton.h"
#include "zCheckBox.h"
#include "zGroupBox.h"

#include "zEditField.h"
#include "zTextArea.h"

#include "EvaLayoutManager.h"
#include "EvaFile.h"
#include "Mensaka.h"

using namespace std;

/**
   @name: GastonaC.h

   @desc: hace las funciones de javaj como fábrica de widgets

   @methods:

      bool      broadcastKey         (HWND hwnd, UINT message, WPARAM tecla)
      bool      loadGUI              (HWND hwnd, const string & gastFileName)
      bool      loadGUI              (HWND hwnd, EvaUnit & ujavaj, EvaUnit & udata)
      void      addAndControlzWidget (zWidget * zw)
      void      showLayout           (HWND frameHwnd, bool forceInvalidate = false)
      bool      haszWidgetID         (HMENU zwID)
      zWidget & getzWidgetByID       (HMENU zwID)
      bool      haszWidgetName       (const string & name)
      zWidget & getzWidgetByName     (const string & name)
      void      removezWidget        (HMENU zwID)


*/
class GastonaC: public gastonaCBase
{
public:

   typedef gastonaCBase super;

   GastonaC (): super ()
   {
   }

protected:

   map<HMENU, zWidget *> mapzWidgetByID;   // map : unique id, widget *
   typedef map<string, HMENU>::iterator  autoName;

   map<string, HMENU>  mapIDByName;      // map : name, unique id
   typedef map<HMENU, zWidget *>::iterator autoID;

public:

   bool broadcastKey (HWND hwnd, UINT message, WPARAM tecla)
   {
      for (autoID itero = mapzWidgetByID.begin();
                 itero != mapzWidgetByID.end();
                 itero ++)
      {
         if ((itero->second)->winKeyMessage (hwnd, message, tecla))
            return true;
      }
      return false;
   }

   bool loadGUI (HWND hwnd, const string & gastFileName)
   {
      if (!loadGast (gastFileName))
         return false;
      return loadGUI (hwnd, wastEvafile["javaj"], wastEvafile["data"]);
   }

   bool loadGUI (HWND hwnd, EvaUnit & ujavaj, EvaUnit & udata)
   {
      // data
      {
         //TODO TOREVIEW
         // if data is very big, here we do a copy :(
         zWidget::setDataAndControlUnit (udata);

         //printf ("habemos data:\n%s\n---\n", udata.toString ().c_str ());
         //printf ("copiada data:\n%s\n---\n", zWidget::getDataAndControl ().toString ().c_str ());
         Mensaka::sendPacket (gastonaCGlobals::msgDATA_FIRST_LOAD (), zWidget::getDataAndControl ());
         //printf ("transpuesta data:\n%s\n---\n", zWidget::getDataAndControl ().toString ().c_str ());
      }

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
            case 'a': wiObj = new zTree (hwnd, winam); break;
            case 'l': wiObj = new zLabel (hwnd, winam); break;
            case 'b': wiObj = new zButton (hwnd, winam); break;
            case 'k': wiObj = new zCheckBox (hwnd, winam); break;
            case 'g': wiObj = new zGroupBox (hwnd, winam); break;
            case 'r': wiObj = new zRadioButton (hwnd, winam); break;
            case 'e': wiObj = new zEditField (hwnd, winam); break;
            case 'i': wiObj = new zTable (hwnd, winam, zTable::ASPECT_LIST); break;
            case 't': wiObj = new zTable (hwnd, winam, zTable::ASPECT_TABLE); break;
            case 'x': wiObj = new zTextArea (hwnd, winam); break;
            default:
               winam = "??" + winam;
               wiObj = new zLabel (hwnd, winam); break;
               break;
         }
         if (wiObj != 0)
         {
            //printf ("INSERTAMOS UN COSO!! name [%s]\n", wiObj->getName ().c_str ());
            addAndControlzWidget (wiObj);

            zWidgetLayable wila = zWidgetLayable(winam, getzWidgetByName (winam)->getNativeHandle ());
            layManager.addWidget (wila);

            Mensaka::sendPacket (wiObj->getName () + " data!", zWidget::getDataAndControl ());
         }
      }

      return true;
   }

   //
   // adds a new zWidget to GastonaC, NOTE: from this moment GastonaC owns the zWidget
   //
   void addAndControlzWidget (zWidget * zw)
   {
      super::addAndControlzWidget (zw);

      if (zw == 0) return;
      // insert it into the two maps
      mapzWidgetByID [zw->getControlID ()] = zw;
      mapIDByName    [zw->getName ()]      = zw -> getControlID ();
   }

   void showLayout (HWND frameHwnd, bool forceInvalidate = false)
   {
      RECT rc;
      GetClientRect(frameHwnd, &rc);

      // ---- EvaLayout: do the real job
      //
      layManager.doShowLayout("main", (int) (rc.right - rc.left), (int) (rc.bottom - rc.top));


      // for example: just invalidate sometimes to avoid flickering
      //              if Invalidate is never called then in some situations
      //              the components are wrong painted
      static int sometimes = 0;
      if (forceInvalidate || sometimes ++ > 40)
      {
         sometimes = 0;
         InvalidateRect(frameHwnd, &rc, true);
      }
   }

   bool haszWidgetID (HMENU zwID)
   {
      return (mapzWidgetByID.find (zwID) != mapzWidgetByID.end());
   }

   zWidget & getzWidgetByID (HMENU zwID)
   {
      autoID itero = mapzWidgetByID.find (zwID);
      if (itero == mapzWidgetByID.end())
      {
         return bobo;
      }
      return * (itero -> second);
   }
};

#endif  // _HEADER_H

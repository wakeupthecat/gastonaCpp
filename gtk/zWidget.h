/*
   docu 
     https://developer.gnome.org/gtk2/stable/api-index-full.html
*/

#ifndef zWIDGET_HEADER_H
#define zWIDGET_HEADER_H

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

   GtkWidget * hwnd;   // each actual widget will create its own window and assign this handle
   string name;  // name of the widget, it has to be unique in a gastona session
   int unid;   // a unique id assigned automatically in this base class

   int widthForText (const string & text)
   {
      return widthForText (text.length ());
   }

   int widthForText (int nchars)
   {
      int cxChar = 22;
      if (nchars < 0) return -1;
      return cxChar * 1.1 * (nchars > 0 ? nchars: 1);
   }

   int heightForText (float nchars = 0.)
   {
      int cyChar = 22;

      return cyChar * (nchars == 0. ? 1.1: nchars);
   }

public:

   // data and control is shared by all widgets!
   // (blackboard principle)
   //
   static EvaUnit & getDataAndControl ()
   {
      static EvaUnit dummy("data&control");
      static EvaUnit * euDataAndCtrlObj = & dummy;
      
      // ensure pointer is always valid!
      if (euDataAndCtrlObj == 0)
         euDataAndCtrlObj = & dummy;
      
      return *euDataAndCtrlObj;
   }

   zWidget (const string & pname):
      hwnd (0),
      name (pname),
      unid (0)
   {
      // assign uniq id for the widget
      static int currentUniqueID = 1000;
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
   // virtual void winMessage    (HWND hwnd, int winHiWord) { };
   // virtual bool winKeyMessage (HWND hwnd, UINT message, WPARAM tecla) { return false; };

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
      // cannot do this
      //gtk_widget_destroy (hwnd);
      
      //because of assertion      
      //Gtk-CRITICAL **: IA__gtk_widget_destroy: assertion 'GTK_IS_WIDGET (widget)' failed
      
      // don't really understand it 
      // for example
      // zButton creates the handle with
      //     hwnd = gtk_button_new_with_label (pname.c_str ());
      // where is the problem?
   }

   const string & getName      ()    { return name;   }
   GtkWidget *    getNativeHandle () { return hwnd;   }
   int &          getControlID ()    { return unid;   }
   EvaUnit &      getDataUnit  ()    { return getDataAndControl (); }
   EvaUnit &      getControlUnit ()  { return getDataAndControl (); }
   bool           isNIL ()           { return name.length() == 0; } // in the desert a zWidget without name (Chicago)

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


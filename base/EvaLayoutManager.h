/*
Eva library C++ (www.elxala.de)
(c) Copyright 2006,20194 Alejandro Xalabarder Aulet

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef _CELTA_LANG_LAYOUT_EVALAYOUTMANAGERV5_HEADER_
#define _CELTA_LANG_LAYOUT_EVALAYOUTMANAGERV5_HEADER_

#include "EvaFile.h"
#include "EvaLayable.h"
#include "zWidgetLayable.h"

using namespace std;

// main class: EvaLayoutManager
//
//
//    An EvaLayoutManager object basically parse an eva structure, usually the unit #javaj#,
//    in the method "setLayouts()" finding layout structures, currently only of type EvaLayout.
//
//    Note though that it does not recognize any widget, this knowledge has to be provided by
//    another component. A widget fabric reads the javaj unit again, find the widgets, build,
//    pack in the structure zWidgetLayable and add them to the manager using the method "addWidget()"
//
//    This way the manager collect an array of wigets that can position, show or hide just by using
//    its handle (see method zWidgetLayable::doShowAt ()), other widget information like input and output messages
//    is unknown to the manager, it simply does not need it.
//
// Structures and classes
//
//    struct uniqueTextStack
//       {  textStack, clear(), checkPushable(), push(), pop() }
//
//    class EvaLayoutManager
//       { class EvaLayout {}, setLayouts(), addWidget(), doShowLayout(), maskElement(), clearAllMasks(), ... }
//
struct uniqueTextStack
{
   vector<string> textStack;

   void clear ()
   {
      textStack.clear ();
   }

   void push (const string & text)
   {
      textStack.push_back (text);
   }

   bool checkPushable (const string & text)
   {
      if (textStack.size () >= 100) return false; // limit maximum depth for security
      for (int kk = 0; kk < textStack.size (); kk ++)
      {
         if (textStack[kk] == text)
         {
            return false;
         }
      }
      return true;
   }

   void pop (const string & lastValue)
   {
      long lastIndx = textStack.size ()-1;
      if (lastIndx >= 0)
      {
         if (textStack[lastIndx] != lastValue)
            TRACE_ERR (("uniqueTextStack error broken show stack at %s!!\n", lastValue.c_str ()));
         else
            textStack.resize (lastIndx);
      }
      //else ERROR on pop
   }
};

// Higher object that stores all zWidgets as well as EvaLayouts
// Its nested class EvaLayout do the actual job of layouting, EvaLayout objects
// get all the widgets and even its own layout information from its parent object
// EvaLayoutManager
//
class EvaLayoutManager
{
public:

   typedef map<string, string>::iterator autoMap;

   enum layableType
   {
      IS_WIDGET=0,
      IS_EVALAYOUT,
      IS_OTHER,
   };

   EvaLayoutManager ();

   // Nested class from EvaLayoutManager
   // It is just a layout calculator but do not hold the object structure
   // of widgets and other layouts which is done by its parent object EvaLayoutManager
   //
   // How the composition or nested layouts works:
   //
   //    From the layout info EvaLayout only work with names, it does not
   //    know even which type of component is working with, if it is an actual widget
   //    or even another layout. Finally it delegates to the manager to paint the
   //    final object and the manager find the proper object and call its method
   //    doShowAt (see EvaLayoutManager::doShowByNameAt) which can derive into
   //    a recursive call
   //
   class EvaLayout
   {
      public:
      enum
      {
         HEADER_ADAPT = 'A',
         HEADER_EXPAND = 'X',
         EXPAND_HORIZONTAL = '-',
         EXPAND_VERTICAL = '+',
         EXPAND_UNDEF = ' ',
         EXPAND_HORIZONTAL_TOEND = '>',   // --
         EXPAND_VERTICAL_TOEND   = 'v',   // ++
      };

      EvaLayout (EvaLayoutManager & manager, const string & layoutName):
        layManager      (manager),
        layableData     (layoutName),
        layoutInfo      (layManager.euLayoutInfo[string ("layout of ") + layoutName]),
        isPrecalculated (false),
        Hmargin         (0),  // left and right margin
        Vmargin         (0),  // top and bottom margin
        Hgap            (0),  // distance between columns
        Vgap            (0),  // distance between rows
        fijoH           (0),  // intern precalculated not variable width
        fijoV           (0)   // intern precalculated not variable height
      {
      }

      string & getName ()
      {
         return layableData.name;
      }

      uniRect getPreferredSize ()
      {
         precalculateAll ();
         return layableData.iniRect;
      }

      void doShowAt(int x0, int y0, int totWidth, int totHeight);

      /** returns the name at nrow, ncol of the grid
      */
      string & widgetAt(int nrow, int ncol)
      {
         return layoutInfo[nrow + 2][ncol + 1];
      }

      // return character if expansion '-' or '+', otherwise undefined (anything else)
      //
      char expansionAt (int nrow, int ncol)
      {
         string & nam = widgetAt(nrow, ncol);
         if (nam == "--") return EXPAND_HORIZONTAL_TOEND;
         if (nam == "++") return EXPAND_VERTICAL_TOEND;
         if (nam.length () == 0)
            return EXPAND_UNDEF;
         return nam[0];
      }

      void invalidate ()
      {
         isPrecalculated = false;
      }

      EvaLayable layableData;

      /// precalculate the layout
      void precalculateAll ();

   private:
      enum RType
      {
         FIXLEN = 0,
         MAXMIN,
         EXPAND,
      };

      struct renglon
      {
         renglon (const string & rtype = "");

         RType type;
         int length;
         float extraProp;
      };

      EvaLayoutManager & layManager;
      Eva & layoutInfo;

      // variables for pre-calculated layout
      //
      bool isPrecalculated;

      int Hmargin;  // left and right margin
      int Vmargin;  // top and bottom margin
      int Hgap;     // distance between columns
      int Vgap;     // distance between rows
      int fijoH;    // intern precalculated not variable width
      int fijoV;    // intern precalculated not variable height


      vector<renglon> Hdims;
      vector<renglon> Vdims;

      // header of the column 'ncol'. Either "A", "X", "" or a number
      string & columnHeader (int ncol);

      // header of the row 'nrow'. Either "A", "X", "" or a number
      string & rowHeader    (int nrow);

   };  // subclass EvaLayout

   // Provide all the layouts that can be set and/or
   // are needed to build some layout in case of nested layouts.
   //
   // An EvaUnit contain several Eva variables each one
   // might contain one evalayout, the name of the variable is the
   // name of the layout that can be used in other layout
   //
   //
   void setLayouts (EvaUnit & allLayouts);

   // Adds a winApi widget to be laid out by EvaLayoutManager
   //
   void addWidget (const zWidgetLayable & widlay);

   // Show the layout named "main" if exists
   //
   //void doShowLayout (int dx, int dy)
   //{
   //   doShowByNameAt ("main", 0, 0, dx, dy);
   //}

   // Show a layout by its name
   //
   void doShowLayout  (const string & cname, int dx, int dy);


   uniRect getPreferredSizeOf (const string & cname)
   {
      string rename = doYouMean (cname);

      int indx = getWidgetIndex (rename);
      if (indx >= 0)
      {
         return arrLayWidgets[indx].getPreferredSize ();
      }
      return uniRect ();
   }


   // ??
   // ?? int getLayoutCount () { return arrPtrLayouts.size (); }
   // ?? EvaLayout * getPtrLayoutByIndex (int indx) { return arrPtrLayouts[indx]; }

   // Provides an extra mechanism to change the layouts dynamically
   // through a set of aliases (masks). The elements to mask or be masked
   // can be both widget names and layouts, allowing all masking combinations
   // (i.e. a layout masked by a button, a button masked by a list etc)
   //
   // example:
   //
   //    manager.maskElement ("a", "b");
   //
   // When an element "a" is masked by other element "b" it means that
   // whenever "a" appears in a layout "b" has to be taken instead.
   // This can be imagined as if we put "b" over all occurrences of "a"
   // thus being "a" masked by "b". In this situation "a" will not be
   // shown in any case.
   //
   void maskElement (const string & elToMask, const string & maskElement);

   // Removes the mask of an element
   void unmaskElement (const string & elToMask)
   {
      maskElement (elToMask, "");
      changeInWidgets = true;
   }

   void clearAllMasks ()
   {
      masks.clear ();
      changeInWidgets = true;
   }

protected:

   // returns the same name or a mask if the element
   // is masked (see maskElement, unmaskElement)
   //
   string doYouMean (const string & namewanted);

   int getWidgetIndex (const string & lname);
   int getLayoutIndex (const string & lname);

   EvaLayable & getLayableByName (const string & lname, layableType & type);

   EvaLayoutManager & self ()
   {
      return *this;
   }

   void doShowByNameAt (const string & cname, int x0, int y0, int dx, int dy);

   void doHideByName (const string & cname)
   {
      // not necessary, all widgets are hidden when the main layout change
      // then they are shown on demand
   }

   bool                 changeInWidgets;
   map<string, string>  masks;
   uniqueTextStack      showingStack;
   string               currentShownLayout;
   EvaUnit              euLayoutInfo;
   vector<zWidgetLayable> arrLayWidgets;
   vector<EvaLayout *>  arrPtrLayouts; // pointer (*) because cannot do default constructor with reference to parent

}; // class EvaLayoutManager

#endif

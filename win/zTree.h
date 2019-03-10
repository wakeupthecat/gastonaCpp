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

/*
   https://www.codeproject.com/Articles/3448/Using-TreeControl-TreeView-under-Win32-API

   http://www.songho.ca/misc/treeview/treeview.html
*/

#ifndef zTREE_HEADER_H
#define zTREE_HEADER_H

#include <windows.h>
#include <commctrl.h>

#include "zWidget.h"
#include "Mensaka.h"

#include "EBSTree.h"
#include "utilTree.h"

using namespace std;

class zTree : public zWidget, public MensakaTarget
{
private:

public:
   enum
   {
      MSK_UPDATE_DATA
   };

   zTree (HWND hwinParent, const string & pname): zWidget(pname)
   {
      int basicStyle = WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT | WS_VSCROLL;

      // LVS_EDITLABELS
      hwnd = CreateWindow(WC_TREEVIEW, "",
                          basicStyle,
                          0, 0,
                          widthForText  (35),
                          heightForText (7),
                          hwinParent,
                          (HMENU) unid,
                          (HINSTANCE) GetWindowLong (hwnd, GWLP_HINSTANCE),
                          NULL);

      //setText (const string & value);
      Mensaka::subscribeToMessage (getName () + " data!", MSK_UPDATE_DATA, this);

      setFontNormal ();
   }

   ~zTree ()
   {
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
      EBSTree modelo = EBSTree (getName (), getDataAndControl ());

      // TreeView_DeleteAllItems(hwnd);
      eva2TreeStruct asserrao = utilTree::sierra (modelo);
      pushNodes (asserrao, modelo.getIsShortPath (), modelo.getSeparator ());
   }

   virtual void winMessage (HWND hwin, int winHiWord)
   {
      if (winHiWord == 0) // press
      {
         //printf ("button [%s] pressed!\n", getName ().c_str ());
         Mensaka::sendPacket (getName (), getDataAndControl ());
      }
   }

   HTREEITEM insertTo(HTREEITEM parent, TCHAR *itemStr, int imgIndex)
   {
      TV_INSERTSTRUCT tvinsert;
      tvinsert.hParent = parent;
      tvinsert.hInsertAfter = parent?TVI_LAST:TVI_ROOT;
      tvinsert.item.mask = TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
      tvinsert.item.pszText = itemStr;
      tvinsert.item.iImage = imgIndex;
      tvinsert.item.iSelectedImage = 0;
      return (HTREEITEM)::SendMessage (hwnd, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
   }

   void pushNodes (eva2TreeStruct & aserro, bool shortpath, const string & separador)
   {
      vector<HTREEITEM> elevos;

      //root node
      HTREEITEM root = insertTo(NULL, TEXT("root"), 0); // title or whatever
      elevos.push_back (root);
      TRACE2 (("Cargamos arbols de %d nods\n", aserro.eva.rows ()));

      for (int rr = 0; rr < aserro.eva.rows (); rr ++)
      {
         TRACE2 (("  lina rr %d cols %d\n", rr, aserro.eva[rr].cols ()));
         int cc = 0;
         while (++cc < aserro.eva[rr].cols ())
         {
            if (aserro.eva[rr][cc].length () == 0) continue; // no node content at this depth

            //resize stack of open nodes (adjust to current depth)
            if (elevos.size () > cc)
               elevos.resize (cc);

            // if (aserro.eva[rr].cols () > cc + 2)
            // {
            //    // possible short
            //    // examine if next has less than cc blanks (or there is no next) then it is short!!
            //    // join all cc until last -1 with separator as node text
            //
            //    //Example
            //    //       spain, barcelona, centre, gotic
            //    //            ,          , eixample, clinic
            //    //            ,          ,         , casp
            //    //            ,          , horta   , guinardo, garriga i roca
            //    //            ,          , sants   , badal
            //    //            ,          ,         , torrasa, mercat          , ateneu, pis1
            //    //            ,          ,         ,        , hospi           , etc
            //    //    ----------------------------------------------------------------------
            //    //         0  ,    1     ,   2     ,   3    , 4               , 5
            //
            //    // here there are two short paths:
            //    //          horta/guinardo    (elems = 3;  pos"sants"-pos"horta" = 0   ;   3-0-1 = 2 elements to join
            //    //          mercat/ateneu     (elems = 4;  pos"hospi"-pos"torrasa" = 1 ;   4-1-1 = 2 elements to join
            //
            // }

            HTREEITEM parent = elevos[elevos.size ()-1];

            string nodostr = aserro.eva[rr][cc];

            if (shortpath)
            {
               // short until end
               //     short = xxx2/xxx3/xxx4/... except the last node = leaf if
               //
               //          ..., xxx0, xxx1, xxx2, xxx3, xxx4, ...
               //             ,     ,     , yyy1, yyy2
               //
               while (cc+2 < aserro.eva[rr].cols () &&
                      (rr + 1 == aserro.eva.rows () ||
                       cc >= aserro.eva[rr+1].cols () ||
                       aserro.eva[rr+1][cc].length () > 0))
               {
                   TRACE2 (("  acorto at cc %d \"%s\"\n", (cc+1), aserro.eva[rr][1+cc].c_str ()));
                   nodostr += separador;
                   nodostr += aserro.eva[rr][++cc];
               }

               // short start or middle
               //     short = xxx0/xxx1/xxx2 if
               //
               //          ..., xxx0, xxx1, xxx2, xxx3, xxx4, ...
               //             ,     ,     ,     , yyy2
               //             ,     ,     ,     , yyy3
               //             , yyy4, yyy5,
               //
            }

            elevos.push_back (insertTo(parent, TEXT(nodostr.c_str ()), 0));
            TRACE2 (("Insert node \"%s\" at depth %d\n",  nodostr.c_str (), elevos.size ()));
         }
      }
   }
};

#endif //  HEADER_H

//
//   void setData_FAKE (const Eva & value)
//   {
//      if (!hwnd) return;
//
//      HTREEITEM Parent;           // Tree item handle
//      HTREEITEM Before;           // .......
//      HTREEITEM Root;
//
//      Parent = insertTo(NULL, TEXT("MAOCS30 Command"), 0);
//      Root = Parent;
//      Before = Parent;
//
//      Parent = insertTo (Parent, TEXT("Native command"), 0);
//      insertTo(Parent, TEXT("Power On PETAAARDO!!!"), 0);
//      insertTo(Parent, TEXT("Power off"), 0);
//      insertTo(Parent, TEXT("Entrant"), 0);
//      insertTo(Parent, TEXT("Sortant"), 0);
//
//      Parent = insertTo(Before, TEXT("Macro"), 0);
//      insertTo(Parent, TEXT("ChangeCode"), 0);
//      insertTo(Parent, TEXT("CipherData"), 0);
//
//      insertTo(NULL, TEXT("Bla bla bla bla..."), 0);
//   }
//
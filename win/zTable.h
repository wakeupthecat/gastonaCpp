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

#ifndef zTABLE_HEADER_H
#define zTABLE_HEADER_H

#include "zWidget.h"
#include "Mensaka.h"

using namespace std;

class zTable : public zWidget, public MensakaTarget
{
public:
   enum eAspect
   {
      ASPECT_TABLE,
      ASPECT_LIST,
   };

   enum
   {
      UPDATE_DATA,
      SELECT_ROWS,
   };


   zTable (HWND hwinParent, const string & pname, eAspect aspecto): zWidget(pname)
   {
      PII_createWidgetsWindow (hwinParent, aspecto);
   }

   ~zTable ()
   {
   }

protected:

   eAspect myAspect;
   int firstVisibleColumnIndx;

   bool isList () { return myAspect == ASPECT_LIST; }

   void PII_createWidgetsWindow (HWND hwinParent, eAspect aspecto)
   {
      firstVisibleColumnIndx = 0;
      myAspect = aspecto;
      int basicStyle = WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT | WS_VSCROLL | LVS_SORTASCENDING;
      if (isList ())
         basicStyle |= LVS_NOCOLUMNHEADER;

      // LVS_EDITLABELS
      hwnd = CreateWindow(WC_LISTVIEW, "",
                          basicStyle,
                          0, 0,
                          widthForText  (isList () ? 15: 35),
                          heightForText (7),
                          hwinParent,
                          (HMENU) unid,
                          (HINSTANCE) GetWindowLong (hwnd, GWLP_HINSTANCE),
                          NULL);

      // don't know which compiler defines have to be set to get
      //
      //#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)
      //#define LVS_EX_GRIDLINES        0x00000001
      //#define LVS_EX_SUBITEMIMAGES    0x00000002
      //#define LVS_EX_CHECKBOXES       0x00000004
      //#define LVS_EX_TRACKSELECT      0x00000008
      //#define LVS_EX_HEADERDRAGDROP   0x00000010
      //#define LVS_EX_FULLROWSELECT    0x00000020
      //#define LVS_EX_ONECLICKACTIVATE 0x00000040
      //#define LVS_EX_TWOCLICKACTIVATE 0x00000080
      //
      // etc so use it directly
      SendMessage (hwnd,
                   (LVM_FIRST + 54) /* LVM_SETEXTENDEDLISTVIEWSTYLE */,
                   0,
                   0x00000020 | 0x00000008 | (isList () ? 0x0 : 0x00000001 | 0x00000010)); /* LVS_EX_FULLROWSELECT | LVS_EX_TRACKSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP */
      // TODO
      //    Todavía hay efectos indeseados en la selección de ListView
      //    1) solo posicionar el cursor se marca toda la linea, esto NO la selecciona (OK) pero DES-SELECCIONA TODAS
      //       las que estaban seleccionadas!!
      //    2) si hay una multiselección al cambiar el focus a otra tabla o lista y seleccionar en esta
      //       DESAPARECE VISUALMENTE LA SELECCION DE LA PRIMERA TABLA!!! aunque cuando se vuelve ésta vuelve !?!?!?!
      Mensaka::subscribeToMessage (getName () + " data!", UPDATE_DATA, this);
      Mensaka::subscribeToMessage (getName () + " select!", SELECT_ROWS, this);
   }

   void updateData ()
   {
      //printf ("la cosa global es:---\n%s\n---\n", zWidget::getDataAndControl ().toString ().c_str ());
      Eva & evaData = getAttribute ("", true);
      //printf ("zTable::updateData atribute data =---\n%s\n------\n", evaData.toString ().c_str ());

      while (ListView_DeleteColumn (hwnd, 0));
      ListView_DeleteAllItems (hwnd);

      //calculate width column
      vector<int> widths;
      {
         Eva & evaVisibleColumns = getAttribute ("visibleColumns");
         int WIDTH_SAMPLES = 20;
         for (int cc = 0; cc < evaData[0].getColumns ().size (); cc ++)
         {
            int maxw = 3; // 3 char is minimum
            if (! EvaUnit::isNILEva (evaVisibleColumns) && evaVisibleColumns[0].indexOf (evaData[0][cc]) == -1)
            {
               // printf ("[%s] es no visible\n", evaData[0][cc].c_str ());
               maxw = -1; // not visible
            }
            else
            {
               // visible
               for (int rr = 1; rr < WIDTH_SAMPLES && rr < evaData.rows (); rr ++)
               {
                  int len = evaData[rr][cc].length ();
                  maxw = maxw < len ? len: maxw;
               }
               //printf ("[%s] visible en tanto %d\n", evaData[0][cc].c_str (), maxw);
            }
            widths.push_back (maxw);
         }
      }

      setColumns (evaData[0].getColumns (), widths);
      for (int rr = 1; rr < evaData.rows (); rr ++)
         addRow (evaData[rr].getColumns ());
   }

   bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      //printf ("zTable::takePacket received mappedMsg %d\n", mappedMsg);
      if (mappedMsg == UPDATE_DATA)
      {
         setVariablesFromArray (params);
         updateData ();
      }
      if (mappedMsg == SELECT_ROWS)
      {
         printf ("TODO: implement this SELECT_ROWS\n");
      }
      return true;
   }

   // void setDataAndControl (EvaUnit & data, EvaUnit & control)
   // {
   // }zTable

   virtual void winMessage (HWND hwin, int winCode)
   {
      //LVN_BEGINLABELEDIT
      //LVN_ENDLABELEDIT
      //LVN_BEGINLABELEDIT

      if (winCode == NM_DBLCLK || winCode == NM_CLICK)
      {
         actionSelection ();

         //printf ("hemos seleccionado algo %d\n", winCode);
         //send mensaka message for selection
         string msg = getName () + (winCode == NM_DBLCLK ? " 2": "");
         Mensaka::sendPacket (getName (), getDataAndControl ()); // p.e. --> tTable
      }
      //else
      //   printf ("QUE ES ETO!! %d\n", winCode);
   }


   void actionSelection ()
   {
      // Fill the attributes selected.columnX .. and subTableSelection
      //

      Eva & evaData = getAttribute ("", true);
      Eva & evaSelMulti = getAttribute ("subTableSelection", true);

      int row = 0;
      int indx = -1;
      int WHAT = LVNI_SELECTED; // LVNI_FOCUSED
      indx = ListView_GetNextItem (hwnd, indx, WHAT);
      while (indx >= 0)
      {
         int evaIndx = indx + 1;
         // printf ("seleccionado indx %d\n", indx);
         for (int cc = 0; cc < evaData.cols (); cc ++)
         {
            if (row == 0)
            {
               string & colname = evaData[0][cc];
               //printf (" colname (%s) column %d\n", colname.c_str(), cc);
               // attributes selected.colname etc only for the first selected item
               //
               Eva & evaSelCol = getAttribute ("selected." + colname, true); // i.e. selected.name
               evaSelCol.setValue (evaData[evaIndx][cc]);
               evaSelMulti.setValue (colname, 0, cc);
            }
            evaSelMulti.setValue (evaData[evaIndx][cc], indx + 1, cc);
         }
         row ++;
         indx = ListView_GetNextItem (hwnd, indx, WHAT);
      }
   }


//   ListView_SetItemState (hWndListView,         // handle to listview
//                          iWhichItem,         // index to listview item
//                          LVIS_FOCUSED | LVIS_SELECTED, // item state
//                          0x000F);                      // mask


   virtual void setColumns (vector<string> & colNames, vector<int> width)
   {
      LVCOLUMN col;

      for (int cc = 0; cc < colNames.size (); cc ++)
      {
         col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
         col.fmt = LVCFMT_LEFT;
         col.cx = width [cc] < 1 ? -1: widthForText (width [cc]);
         col.pszText = (LPSTR) colNames[cc].c_str ();
         col.iSubItem = cc;
         printf ("colname (%s) de width %d\n", colNames[cc].c_str(), col.cx);
         ListView_InsertColumn (hwnd, cc, & col);
         if (isList () && width[cc] > 0)
         {
            firstVisibleColumnIndx = cc;
            break; // show only one column!
         }
      }
   }

   virtual void addRow (vector<string> & colValues)
   {
      if (colValues.size () < 1) return;
      int reto = 0;
      LVITEM ite = { 0 };

      int maxCol = isList () ? firstVisibleColumnIndx: colValues.size () - 1;

      ite.mask = LVIF_TEXT;
      ite.pszText = (LPSTR) colValues[0].c_str ();
      reto = ListView_InsertItem (hwnd, &ite);
      if (reto >= 0)
         for (int cc = 1; cc <= maxCol; cc ++)
         {
            ListView_SetItemText (hwnd, reto, cc, (LPSTR) colValues[cc].c_str ());
         }
   }
};

#endif //  _HEADER_H

/*
from https://www.codeguru.com/cpp/controls/controls/scrollbars/article.php/c2323/Handling-The-ScrollBar.htm


  SCROLLINFO ScrollInfo;
  ScrollInfo.cbSize = sizeof(ScrollInfo);     // size of this structure
  ScrollInfo.fMask = SIF_ALL;                 // parameters to set
  ScrollInfo.nMin = 0;                        // minimum scrolling position
  ScrollInfo.nMax = 100;                      // maximum scrolling position
  ScrollInfo.nPage = 40;                      // the page size of the scroll box
  ScrollInfo.nPos = 50;                       // initial position of the scroll box
  ScrollInfo.nTrackPos = 0;                   // immediate position of a scroll box that the user is dragging
  m_MyScrollBar.SetScrollInfo(&ScrollInfo);


   case SB_BOTTOM:         //Scrolls to the lower right.
      break;
    case SB_ENDSCROLL:      //Ends scroll.
      break;
    case SB_LINEDOWN:       //Scrolls one line down.
      SetScrollPos(GetScrollPos() + 1);
      break;
    case SB_LINEUP:         //Scrolls one line up.
      SetScrollPos(GetScrollPos() - 1);
      break;
    case SB_PAGEDOWN:       //Scrolls one page down.
      SetScrollPos(GetScrollPos() + ScrollInfo.nPage);
      break;
    case SB_PAGEUP:         //Scrolls one page up.
      SetScrollPos(GetScrollPos() - ScrollInfo.nPage);
      break;
    case SB_THUMBPOSITION:  //The user has dragged the scroll box (thumb) and released the mouse button. The nPos parameter indicates the position of the scroll box at the end of the drag operation.
      break;
    case SB_THUMBTRACK:     //The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nPos parameter indicates the position that the scroll box has been dragged to.
      SetScrollPos(nPos);
      break;
    case SB_TOP:            //Scrolls to the upper left.
      break;
*/

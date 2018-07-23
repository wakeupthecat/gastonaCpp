/*
*/

#ifndef zTREE_HEADER_H
#define zTREE_HEADER_H

#include "zWidget.h"
#include "Mensaka.h"

#include <windows.h>
#include <commctrl.h>

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
           setData (Eva ());
      else setData (edata);
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

   void setData (const Eva & value)
   {
      if (!hwnd) return;

      HTREEITEM Parent;           // Tree item handle
      HTREEITEM Before;           // .......
      HTREEITEM Root;

      Parent = insertTo(NULL, TEXT("MAOCS30 Command"), 0);
      Root = Parent;
      Before = Parent;

      Parent = insertTo (Parent, TEXT("Native command"), 0);
      insertTo(Parent, TEXT("Power On PETAAARDO!!!"), 0);
      insertTo(Parent, TEXT("Power off"), 0);
      insertTo(Parent, TEXT("Entrant"), 0);
      insertTo(Parent, TEXT("Sortant"), 0);

      Parent = insertTo(Before, TEXT("Macro"), 0);
      insertTo(Parent, TEXT("ChangeCode"), 0);
      insertTo(Parent, TEXT("CipherData"), 0);

      insertTo(NULL, TEXT("Bla bla bla bla..."), 0);
   }
};

#endif //  HEADER_H

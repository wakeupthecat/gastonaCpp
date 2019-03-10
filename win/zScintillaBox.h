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

#ifndef zSCINTILLABOX_HEADER_H
#define zSCINTILLABOX_HEADER_H

#include <richedit.h> // for TEXTRANGE structure (?)

#include "zWidget.h"
#include "Mensaka.h"
#include "Scintilla.h"

using namespace std;


class zScintillaBox : public zWidget, public MensakaTarget
{
public:
   enum
   {
      UPDATE_DATA,
      MAX_BUFF_SIZE = 4*1024,
   };

   const COLORREF C_BLACK = RGB(0,0,0);
   const COLORREF C_WHITE = RGB(0xff,0xff,0xff);
   
   bool mIsDirty;

   zScintillaBox (HWND hwinParent, const string & pname): zWidget(pname), mIsDirty (false)
   {
      static HINSTANCE scin = LoadLibrary("SciLexer.dll");
      if (scin == NULL)
      {
         TRACE (("ERROR: The Scintilla DLL could not be loaded."));
         return;
      }
      else
      {
         TRACE (("SciLexer.dll loaded successfuly!"));
      }

      hwnd = CreateWindow (TEXT("Scintilla"), TEXT("Scintilla"),
                          WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_CLIPCHILDREN,
                          0, 0,
                          widthForText  (20),
                          heightForText (),
                          hwinParent,
                          (HMENU) unid,
                          (HINSTANCE) GetWindowLong (hwnd, GWLP_HINSTANCE),
                          NULL);
      if (hwnd == NULL)
      {
         TRACE (("ERROR: on CreateWindow Scintilla!"));
         return;
      }

      Mensaka::subscribeToMessage (getName () + " data!", UPDATE_DATA, this);

      SendEditor (SCI_CREATEDOCUMENT);
      SendEditor (SCI_SETTABWIDTH, 3);
      SendEditor (SCI_SETUSETABS, 0);

      SetAStyle(STYLE_DEFAULT, C_BLACK, C_WHITE, 11, "Consolas");
      SendEditor(SCI_STYLECLEARALL);	// Copies global style to all others
      // setFontBig ();
      updateData ();
   }

   ~zScintillaBox ()
   {
   }

	LRESULT SendEditor(UINT Msg, WPARAM wParam=0, LPARAM lParam=0)
   {
      return SendMessage(hwnd, Msg, wParam, lParam);
	}

   void SetAStyle (int style, COLORREF fore, COLORREF back, int size = 0, const char *face = 0)
   {
      SendEditor(SCI_STYLESETFORE, style, fore);
      SendEditor(SCI_STYLESETBACK, style, back);
      if (size >= 1)
         SendEditor(SCI_STYLESETSIZE, style, size);
      if (face)
         SendEditor(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
   }


   bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      if (mappedMsg == UPDATE_DATA)
      {
         TRACE (("mira var[%s] val[%s]", params.size () > 1 ? params[0].c_str ():"?", params.size () > 1 ? params[1].c_str ():"?"));
         setVariablesFromArray (params);
         updateData ();
         return true;
      }
      return false;
   }

   void updateData ()
   {
      if (hasAttribute ("fileName"))
      {
         TRACE (("FILENAME: [%s]", getAttribute ("fileName").toString ().c_str ()));
         loadFile (getAttributeAsText ("fileName").c_str ());
      }
      else
      {
         setText (getAttributeAsText (""));
      }
   }

   void resetEditor ()
   {
      SendEditor(SCI_CLEARALL);
      SendEditor(EM_EMPTYUNDOBUFFER);
      mIsDirty = false;
      SendEditor(SCI_SETSAVEPOINT);
   }

   void loadFile (const char * fileName)
   {
      TRACE (("loading file %s ...", fileName));
      resetEditor ();
      SendEditor(SCI_CANCEL);
      SendEditor(SCI_SETUNDOCOLLECTION, 0);

      FILE * fp = fopen(fileName, "rb");
      if (fp != NULL)
      {
         char data[MAX_BUFF_SIZE+1];
         int len = 0;
         do {
            len = fread (data, 1, sizeof(data), fp);
            TRACE (("%creading %d bytes of data ...", 10, len));
            SendEditor(SCI_ADDTEXT, len, reinterpret_cast<LPARAM>(static_cast<char *>(data)));
         } while (len > 0);
         fclose(fp);
      }
      else
      {
         TRACE (("Could not open file [\"%s\"]", fileName));
      }
      SendEditor(SCI_SETUNDOCOLLECTION, 1);
      // ::SetFocus(hwnd);
      SendEditor(EM_EMPTYUNDOBUFFER);
      SendEditor(SCI_SETSAVEPOINT);
      SendEditor(SCI_GOTOPOS, 0);
   }

   void getRangeText (int start, int end, char *text)
   {
      TEXTRANGE tr;
      tr.chrg.cpMin = start;
      tr.chrg.cpMax = end;
      tr.lpstrText = text;
      SendEditor(EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
   }

   void saveFile (const char *fileName)
   {
      FILE *fp = fopen(fileName, "wb");
      if (fp) 
      {
         char data[MAX_BUFF_SIZE + 1];
         int lengthDoc = SendEditor(SCI_GETLENGTH);
         for (int ii = 0; ii < lengthDoc; ii += MAX_BUFF_SIZE) 
         {
            int grabSize = lengthDoc - ii;
            if (grabSize > MAX_BUFF_SIZE)
               grabSize = MAX_BUFF_SIZE;
            getRangeText (ii, ii + grabSize, data);
            fwrite(data, grabSize, 1, fp);
         }
         fclose(fp);
         SendEditor(SCI_SETSAVEPOINT);
      } 
      else 
      {
         TRACE (("Could not save file [\"%s\"]", fileName));
      }
   }
   
   void vuelcaData ()
   {
      // int len = GetWindowTextLength (getNativeHandle ());
      // TCHAR * pVarName = (TCHAR *) calloc (len + 3, sizeof (TCHAR));
      // if (pVarName)
      //    pVarName[len] = 0;
      //
      // GetWindowText(getNativeHandle (), pVarName, len+1);
      //
      // // set the variable with the current control content
      // getAttribute ("", true).setValue (string (pVarName));
      //
      // //printf ("currito es [%s]\n", getAttribute("").getValue().c_str ());
      // free (pVarName);
   }

   virtual void winMessage (HWND hwin, int winHiWord)
   {
      switch (winHiWord)
      {
         case WM_KILLFOCUS:
         case EN_KILLFOCUS:
            // losing focus, so revert current value (text) to data
            vuelcaData ();
            break;

         case SCN_SAVEPOINTREACHED:
            mIsDirty = false;
            break;

         case SCN_SAVEPOINTLEFT:
            mIsDirty = true;
            break;

         case SCN_UPDATEUI:
            {
               //:seq zwidgets zScintillaBox scintilla << selected text changed
               //
               int selectSize = SendEditor (SCI_GETSELECTIONEND) - SendEditor (SCI_GETSELECTIONSTART);
               if (selectSize <= MAX_BUFF_SIZE)
               {
                  char selBuffer[MAX_BUFF_SIZE+1];
                  // getset the variable with the current control content
                  SendEditor (SCI_GETSELTEXT, 0, selBuffer);

                  // set the variable with the current control content
                  getAttribute ("selected", true).setValue (string (selBuffer));
                  Mensaka::sendPacket (getName (), getDataAndControl ());
               }
            }
            break;

         default:
            break;
      }
   }

   virtual bool winKeyMessage (HWND forHwnd, UINT message, WPARAM tecla)
   {
      if (forHwnd != hwnd) return false;

      return false;
   }


   void setText (const string & value)
   {
      SendEditor (SCI_SETTEXT, 0, value.c_str ());
   }
};

#endif //  HEADER_H

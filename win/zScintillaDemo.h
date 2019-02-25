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

#ifndef zSCINTILLADEMO_HEADER_H
#define zSCINTILLADEMO_HEADER_H

#include "zWidget.h"
#include "Mensaka.h"
#include "Scintilla.h"
#include "SciLexer.h"

using namespace std;


const char htmlKeyWords[] =
   "a abbr acronym address applet area b base basefont "
   "bdo big blockquote body br button caption center "
   "cite code col colgroup dd del dfn dir div dl dt em "
   "fieldset font form frame frameset h1 h2 h3 h4 h5 h6 "
   "head hr html i iframe img input ins isindex kbd label "
   "legend li link map menu meta noframes noscript "
   "object ol optgroup option p param pre q s samp "
   "script select small span strike strong style sub sup "
   "table tbody td textarea tfoot th thead title tr tt u ul "
   "var xmlns "
   "abbr accept-charset accept accesskey action align alink "
   "alt archive axis background bgcolor border "
   "cellpadding cellspacing char charoff charset checked cite "
   "class classid clear codebase codetype color cols colspan "
   "compact content coords "
   "data datafld dataformatas datapagesize datasrc datetime "
   "declare defer dir disabled enctype "
   "face for frame frameborder "
   "headers height href hreflang hspace http-equiv "
   "id ismap label lang language link longdesc "
   "marginwidth marginheight maxlength media method multiple "
   "name nohref noresize noshade nowrap "
   "object onblur onchange onclick ondblclick onfocus "
   "onkeydown onkeypress onkeyup onload onmousedown "
   "onmousemove onmouseover onmouseout onmouseup "
   "onreset onselect onsubmit onunload "
   "profile prompt readonly rel rev rows rowspan rules "
   "scheme scope shape size span src standby start style "
   "summary tabindex target text title type usemap "
   "valign value valuetype version vlink vspace width "
   "text password checkbox radio submit reset "
   "file hidden image "
   "public !doctype xml";

const char jsKeyWords[] =
   "break case catch continue default "
   "do else for function if return throw try var while";

const char vbsKeyWords[] =
   "and as byref byval case call const "
   "continue dim do each else elseif end error exit false for function global "
   "goto if in loop me new next not nothing on optional or private public "
   "redim rem resume select set sub then to true type while with "
   "boolean byte currency date double integer long object single string type "
   "variant";

class zScintillaDemo : public zWidget, public MensakaTarget
{
public:
   enum
   {
      UPDATE_DATA,
      MAX_SEL_SIZE = 1024,
   };

   zScintillaDemo (HWND hwinParent, const string & pname): zWidget(pname)
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
      InitialiseEditor ();
      // setFontBig ();
   }

   ~zScintillaDemo ()
   {
   }

   const COLORREF C_BLACK = RGB(0,0,0);
   const COLORREF C_WHITE = RGB(0xff,0xff,0xff);

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


   void InitialiseEditor() 
   {
      SendEditor(SCI_SETLEXER, SCLEX_HTML);
      //SendEditor(SCI_SETSTYLEBITS, 7);
   
      SendEditor(SCI_SETKEYWORDS, 0, reinterpret_cast<LPARAM>(htmlKeyWords));
      SendEditor(SCI_SETKEYWORDS, 1, reinterpret_cast<LPARAM>(jsKeyWords));
      SendEditor(SCI_SETKEYWORDS, 2, reinterpret_cast<LPARAM>(vbsKeyWords));
   
      // Set up the global default style. These attributes are used wherever no explicit choices are made.
      SetAStyle(STYLE_DEFAULT, C_WHITE, C_BLACK, 11, "Consolas");
      SendEditor(SCI_STYLECLEARALL);	// Copies global style to all others
   
      const COLORREF red = RGB(0xFF, 0, 0);
      const COLORREF offWhite = RGB(0xFF, 0xFB, 0xF0);
      const COLORREF darkGreen = RGB(0, 0x80, 0);
      const COLORREF darkBlue = RGB(0, 0, 0x80);
   
      // Hypertext default is used for all the document's text
      SetAStyle(SCE_H_DEFAULT, C_WHITE, C_BLACK, 11, "Consolas");
   
      // SendEditor(SCI_STYLESETBOLD, SCE_HB_WORD, 1);
   
      /// // This light blue is found in the windows system palette so is safe to use even in 256 colour modes.
      /// const COLORREF lightBlue = RGB(0xA6, 0xCA, 0xF0);
      /// // Show the whole section of VBScript with light blue background
      /// for (int bstyle=SCE_HB_DEFAULT; bstyle<=SCE_HB_STRINGEOL; bstyle++) {
      ///    SendEditor(SCI_STYLESETFONT, bstyle,
      ///       reinterpret_cast<LPARAM>("Georgia"));
      ///    SendEditor(SCI_STYLESETBACK, bstyle, lightBlue);
      ///    // This call extends the backround colour of the last style on the line to the edge of the window
      ///    SendEditor(SCI_STYLESETEOLFILLED, bstyle, 1);
      /// }
      /// SendEditor(SCI_STYLESETBACK, SCE_HB_STRINGEOL, RGB(0x7F,0x7F,0xFF));
      /// SendEditor(SCI_STYLESETFONT, SCE_HB_COMMENTLINE,
      ///    reinterpret_cast<LPARAM>("Comic Sans MS"));
      /// 
      /// SendEditor(SCI_STYLESETBOLD, SCE_HBA_WORD, 1);
      /// 
      /// // Show the whole section of ASP VBScript with bright yellow background
      /// for (int bastyle=SCE_HBA_DEFAULT; bastyle<=SCE_HBA_STRINGEOL; bastyle++) {
      ///    SendEditor(SCI_STYLESETFONT, bastyle,
      ///       reinterpret_cast<LPARAM>("Georgia"));
      ///    SendEditor(SCI_STYLESETBACK, bastyle, RGB(0xFF, 0xFF, 0));
      ///    // This call extends the backround colour of the last style on the line to the edge of the window
      ///    SendEditor(SCI_STYLESETEOLFILLED, bastyle, 1);
      /// }
      /// SendEditor(SCI_STYLESETBACK, SCE_HBA_STRINGEOL, RGB(0xCF,0xCF,0x7F));
      /// SendEditor(SCI_STYLESETFONT, SCE_HBA_COMMENTLINE,
      ///    reinterpret_cast<LPARAM>("Comic Sans MS"));
      /// 
      /// // Show the whole section of Javascript with off white background
      /// for (int jstyle=SCE_HJ_DEFAULT; jstyle<=SCE_HJ_SYMBOLS; jstyle++) {
      ///    SendEditor(SCI_STYLESETFONT, jstyle,
      ///       reinterpret_cast<LPARAM>("Lucida Sans Unicode"));
      ///    SendEditor(SCI_STYLESETBACK, jstyle, offWhite);
      ///    SendEditor(SCI_STYLESETEOLFILLED, jstyle, 1);
      /// }
      /// SendEditor(SCI_STYLESETBACK, SCE_HJ_STRINGEOL, RGB(0xDF, 0xDF, 0x7F));
      /// SendEditor(SCI_STYLESETEOLFILLED, SCE_HJ_STRINGEOL, 1);
      /// 
      /// // Show the whole section of Javascript with brown background
      /// for (int jastyle=SCE_HJA_DEFAULT; jastyle<=SCE_HJA_SYMBOLS; jastyle++) {
      ///    SendEditor(SCI_STYLESETFONT, jastyle,
      ///       reinterpret_cast<LPARAM>("Lucida Sans Unicode"));
      ///    SendEditor(SCI_STYLESETBACK, jastyle, RGB(0xDF, 0xDF, 0x7F));
      ///    SendEditor(SCI_STYLESETEOLFILLED, jastyle, 1);
      /// }
      /// SendEditor(SCI_STYLESETBACK, SCE_HJA_STRINGEOL, RGB(0x0,0xAF,0x5F));
      /// SendEditor(SCI_STYLESETEOLFILLED, SCE_HJA_STRINGEOL, 1);
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
      //string value = getAttribute ("").getValue ();
      //setText (value);
      Eva & etext = getAttribute ("");

      string stext = "";
      for (int ii = 0; ii < etext.rows (); ii ++)
         stext += etext.getValue (ii) + "\n";

      setText (stext);
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
      if (winHiWord == WM_KILLFOCUS || winHiWord == EN_KILLFOCUS)
      {
         // losing focus, so revert current value (text) to data
         vuelcaData ();
      }
      if (winHiWord == SCN_UPDATEUI)
      {
         int selectSize = SendEditor (SCI_GETSELECTIONEND) - SendEditor (SCI_GETSELECTIONSTART);
         if (selectSize <= MAX_SEL_SIZE)
         {
            // getset the variable with the current control content
            SendEditor (SCI_GETSELTEXT, 0, mSelBuffer);

            // set the variable with the current control content
            getAttribute ("selected", true).setValue (string (mSelBuffer));
            Mensaka::sendPacket (getName (), getDataAndControl ());
         }
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

   char mSelBuffer[MAX_SEL_SIZE];
};

#endif //  HEADER_H

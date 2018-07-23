/*
Eva library C++ (see java package de.elxala.Eva and EvaFormat.PDF)
Copyright (C) 2005-2016  Alejandro Xalabarder Aulet

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

/**
   mainEvaLayoutV5.cpp

   Copyright (c) 2006-2017 Alejandro Xalabarder


   This is a demo application of EvaLayoutManager.
   The layout information will be load from the file ConfigDemoSwitchAndMask.eva
   Read comments in this file

   All is done in MainDlgProc

      on WM_INITDIALOG :
         we load the layouts or harcode them and set the layout named "main" into the layout manager.
         also we give the manager the all the physical components that it has to handle

      on WM_SIZE :
         we call ::doLayout to ensure the components are dimensioned and placed as defined in the 
         layout info 

      on WM_COMMAND / ID_BUTTON_1 and IDM_OTRO (menu Switch layout) :

         we switch the layout

      on WM_COMMAND / ID_BUTTON_2, ID_BUTTON_3, ID_BUTTON_4 :
      
         we demonstrate the action of masking/unmasking a component
         
*/

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>  // for  _T macro

#include "EvaLayoutManager.h"
#include "EvaFile.h"

#define MAX_LOADSTRING 100


// EVA Layout --------------------------------
//
//  variables for this sample
//
EvaLayoutManager gaston;  // the EvaLayoutManager object
EvaUnit layoutArray;
string currentLayoutName = "main";
int currentLayIndx = 0;             // # of current layout

void prepareLayout (HWND hDlg);
void linkLayout    (HWND hDlg);
void showLayout    (HWND hDlg, bool forceInvalidate = false);
void switchLayout  (HWND hDlg);


// Global Variables:
HINSTANCE hInst;                       // current instance
TCHAR szTitle[MAX_LOADSTRING];         // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];   // The title bar text


// Foward declarations of functions included in this code module:
BOOL           InitInstance(HINSTANCE, int);
LRESULT CALLBACK  About(HWND, UINT, WPARAM, LPARAM);


BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE /* hPrevInstance */,
                     LPSTR     /* lpCmdLine */,
                     int       nCmdShow)
{
   MSG msg;
   HACCEL hAccelTable;

   // Initialize global strings
   LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadString(hInstance, IDC_DEMO, szWindowClass, MAX_LOADSTRING);

   // Perform application initialization:
   if (!InitInstance (hInstance, nCmdShow))
   {
      printf ("WinMain: cannot InitInstance, return!\n");
      return FALSE;
   }

   hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DEMO);

   // Main message loop:
   while (GetMessage(&msg, NULL, 0, 0))
   {
      if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
     }
   }

   return msg.wParam;
}


// --------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateDialog(hInstance,
      MAKEINTRESOURCE(IDD_DIALOG_MAIN),
      NULL, // handle to owner window
      (DLGPROC)MainDlgProc);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// --------------------------------------------------------
// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM /* lParam */)
{
   switch (message)
   {
      case WM_INITDIALOG:
            return TRUE;

      case WM_COMMAND:
         if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
         {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
         }
         break;
   }
    return FALSE;
}


// --------------------------------------------------------
// A dialog is used instead of a window for this example
BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
         {
            prepareLayout (hDlg);
            linkLayout (hDlg);

            // add some data to the combo box to check the fix if its weird behavior
            SendMessage(GetDlgItem(hDlg, ID_COMBO),CB_ADDSTRING,0, (LPARAM) _T("Item1"));
            SendMessage(GetDlgItem(hDlg, ID_COMBO),CB_ADDSTRING,0, (LPARAM) _T("Segon"));
            SendMessage(GetDlgItem(hDlg, ID_COMBO),CB_ADDSTRING,0, (LPARAM) _T("Tercer"));
            SendMessage(GetDlgItem(hDlg, ID_COMBO),CB_ADDSTRING,0, (LPARAM) _T("etc.. etc.."));

            showLayout(hDlg, true);
         }
         return TRUE;

      case WM_SIZE:
         if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
         {
            // EVA Layout --------------------------------
            //
            // on each window resize we give the job to the layout manager
            //
            showLayout (hDlg);
         }
         break;

      case WM_COMMAND:
         {
            WORD cmd = LOWORD(wParam);
            switch(cmd)
            {
               case ID_BUTTON_1:
               {
                  // DEMO action : switch whole layout
                  //
                  switchLayout (hDlg);
                  return TRUE;
               }

               case ID_BUTTON_2:
               {
                  // DEMO action : mask a component
                  // mask (substitute) layout "form1" with the layout "form2"
                  gaston.maskElement ("form1", "form2");
                  showLayout (hDlg, true);
                  return TRUE;
               }

               case ID_BUTTON_3:
               {
                  // DEMO action : mask a component
                  // mask (substitute) layout "form1" with the component "memo"
                  gaston.maskElement("form1", "memo");
                  showLayout(hDlg, true);
                  return TRUE;
               }

               case ID_BUTTON_4:
               {
                  // DEMO action : revert any mask on a component
                  // unmask layout "form1" (revert any mask on it)
                  gaston.unmaskElement("form1");
                  showLayout(hDlg, true);
                  return TRUE;
               }

               case IDM_EXIT:
               case IDOK:
               case IDCANCEL:
               {
                  PostQuitMessage(0);
                  EndDialog(hDlg, LOWORD(wParam));
               }
               return TRUE;

               case IDM_OTRO:
               {
                  // DEMO action : switch whole layout
                  //
                  switchLayout (hDlg);
               }
               return TRUE;

               case IDM_ABOUT:
               {
                  DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hDlg, (DLGPROC)About);
               }
               return TRUE;

               default:
                  break;
            }
         }
         break;

   }
    return FALSE;
}

void prepareLayout (HWND hDlg)
{
   // EVA Layout --------------------------------
   // Load layoutInfo in two different ways :
   //
   //    If the file config file is found (ConfigDemoSwitchAndMask.eva) then the layouts
   //    contained in the unit #data# are loaded. Otherwise we build here two harcoded Eva layouts.
   //

   EvaFile efi ("ConfigDemoSwitchAndMask.eva");
   layoutArray = efi["data"];
   
   if (layoutArray.size () == 0)
   {
      //MessageBox(hDlg, (LPCWSTR) "Working with two harcoded layoutInfo's\nbecause file ConfigLayouts.eva is not found", "Warning", MB_OK);

      // create two Eva's on this EvaUnit
      //
      Eva e1 = Eva ("main");
      Eva e2 = Eva ("second");
      layoutArray.add (e1);
      layoutArray.add (e2);

      // get the references to the Eva's to fill them
      // (note that eunit[0] and eunit[1] could be used as well)
      //
      Eva & layI1 = layoutArray["main"];

      layI1.addLine (EvaLine ("EvaLayout, 10, 10, 5, 5"));
      layI1.addLine (EvaLine (" zzz,         ,   X      ,  A     ,"));
      layI1.addLine (EvaLine ("    , label1  , edit1    , -      ,"));
      layI1.addLine (EvaLine ("    , label2  , edit2    , -      ,"));
      layI1.addLine (EvaLine ("    , combo   , memo     , boton1 ,"));
      layI1.addLine (EvaLine ("  X ,         ,   +      ,        ,"));

      Eva & layI2 = layoutArray["second"];

      layI2.addLine (EvaLine ("EvaLayout, 10, 10, 5, 5"));
      layI2.addLine (EvaLine ("xxx,    A     ,    A    , X       ,  "));
      layI2.addLine (EvaLine ("  A, boton1   , label1  , edit1   ,  "));
      layI2.addLine (EvaLine ("  A,          , label2  , edit2   ,  "));
      layI2.addLine (EvaLine ("  A,          , label3  , edit3   ,  "));
      layI2.addLine (EvaLine ("  A, memo     ,   -     ,    -    ,  "));
      layI2.addLine (EvaLine ("  X,  +       ,         ,         ,  "));

      // to export the harcoded EvaUnit ...
      //
      // efi.saveSingleUnit (eunit, "hardcoded.eva");
   }

   if (layoutArray.size () == 0)
   {
      //MessageBox(hDlg, "Error there are no layouts loaded!", "Error", MB_OK);
      PostQuitMessage(0);
      //EndDialog (hDlg, LOWORD(wParam));
   }

   gaston.setLayouts (layoutArray);
   currentLayoutName = "main";
}

void linkLayout (HWND hDlg)
{
   // ---- EvaLayout: Associate all components with its logic names given in layout info(s)
   //                 It is enough to it only once but all components from all possible layouts has to be declared here
   //                 to allow EvaLayoutManager to handle them in future switch layout or mask operation.
   //
   gaston.addWinApiWidget (winApiWidget ("lista",   GetDlgItem(hDlg, ID_LIST)));
   gaston.addWinApiWidget (winApiWidget ("memo",    GetDlgItem(hDlg, ID_MEMO)));

   // Important!
   //   for combos we have to call winApiWidget with the third parameter = true !!
   //
   gaston.addWinApiWidget (winApiWidget ("combo",   GetDlgItem(hDlg, ID_COMBO), true));

   gaston.addWinApiWidget (winApiWidget ("boton1",  GetDlgItem(hDlg, ID_BUTTON_1)));
   gaston.addWinApiWidget (winApiWidget ("boton2",  GetDlgItem(hDlg, ID_BUTTON_2)));
   gaston.addWinApiWidget (winApiWidget ("boton3",  GetDlgItem(hDlg, ID_BUTTON_3)));
   gaston.addWinApiWidget (winApiWidget ("boton4",  GetDlgItem(hDlg, ID_BUTTON_4)));

   gaston.addWinApiWidget (winApiWidget ("edit1",   GetDlgItem(hDlg, ID_EDIT_1)));
   gaston.addWinApiWidget (winApiWidget ("edit2",   GetDlgItem(hDlg, ID_EDIT_2)));
   gaston.addWinApiWidget (winApiWidget ("edit3",   GetDlgItem(hDlg, ID_EDIT_3)));

   gaston.addWinApiWidget (winApiWidget ("label1",  GetDlgItem(hDlg, ID_LABEL_1)));
   gaston.addWinApiWidget (winApiWidget ("label2",  GetDlgItem(hDlg, ID_LABEL_2)));
   gaston.addWinApiWidget (winApiWidget ("label3",  GetDlgItem(hDlg, ID_LABEL_3)));
   gaston.addWinApiWidget (winApiWidget ("label4",  GetDlgItem(hDlg, ID_LABEL_4)));
   // ================================================================
}

/**
   // ---- EvaLayout: method for MFC to recalculate the layout
   //
   call this function after setting a new layout
   or resizing the window
*/
void showLayout (HWND hDlg, bool forceInvalidate)
{
   RECT rc;
   GetClientRect(hDlg, &rc);

   // ---- EvaLayout: do the real job
   //
   gaston.doShowLayout(currentLayoutName, (int) (rc.right - rc.left), (int) (rc.bottom - rc.top));

   // for example: just invalidate sometimes to avoid flickering
   //              if Invalidate is never called then in some situations
   //              the components are wrong painted
   static int sometimes = 0;
   if (forceInvalidate || sometimes ++ > 40)
   {
      sometimes = 0;
      InvalidateRect(hDlg, &rc, true);
   }
}

void switchLayout (HWND hDlg)
{
   // ---- EvaLayout: here we switch between layout "main" and layout "second" if they are given

   if (layoutArray.containsEva("main") && layoutArray.containsEva("second"))
   {
      //printf ("setLayout %d [%s]\n", currentLayIndx, currentLayoutName.c_str ());
      currentLayoutName = (currentLayoutName == "main") ? "second" : "main";
      showLayout(hDlg, true);
   }
}

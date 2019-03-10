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

   WinMain and WndProc which is the central windows message handler.

   The main frame or window is created and also a instance of GastonaC
   which is used in two scenarios:


      - when a message of type key is received it is forwarded to gastona

            gaston.broadcastKey (msg.hwnd, msg.message, msg.wParam);

      - and the handling on window messages

         case WM_CREATE:
            gaston.loadGUI (hwnd, gastonaMainGastFile.c_str ());
            gaston.showLayout (hwnd, true);

         case WM_SIZE:
            ...
            gaston.showLayout (hwnd);

         case WM_NOTIFY
         case WM_COMMAND
            ...
            .. if the message is for a specific gastona widget "wi" ..
            wi.winMessage (hwnd, HIWORD (wParam));


*/

// #include "GastonaWin.h"
// #include "EvaLayoutManager.cpp"
// #include "LogicController.cpp"


using namespace std;

// handle of all zWidgets
string gastonaMainGastFile = "autoGastWinter.gast";
vector <string> globalMainArguments;

// instance of GastonaC
//
GastonaC gaston;


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);


// static method for separ command line arguments
//
void separaParametros (PSTR cmdline, vector<string> & main_parameters)
{
   // hacer el trabajo del idiota
   //
   bool inComilla = false;
   string current;

   main_parameters.clear ();

   int ii = -1;
   while (++ii < strlen(cmdline))
   {
      switch (cmdline[ii])
      {
         case '\"':  // ["]
            inComilla = ! inComilla;
            if (inComilla) break;
            ii ++;
            // now it has to be a blank!!
            // NO break;!

         case ' ':   // [ ]
            if (inComilla)
               // just add the blank to the current parameter
               current += cmdline[ii];
            else
            {
               // push the current parameter since we have achived the end
               main_parameters.push_back (current);
               current = "";
            }
            break;

         default:
            current += cmdline[ii];
            break;
      }
   }
   if (current.size () != 0)
      main_parameters.push_back (current);
}


int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    PSTR szCmdLine,
                    int iCmdShow)
{
   static TCHAR szAppName[] = TEXT ("gastonaWinter");
   HWND         hwnd;

   MSG          msg;
   WNDCLASS     wndclass;

   wndclass.style         = CS_DBLCLKS; //CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc   = WndProc;
   wndclass.cbClsExtra    = 0;
   wndclass.cbWndExtra    = 0;
   wndclass.hInstance     = hInstance;
   wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
   wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);

   wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);

   wndclass.lpszMenuName  = NULL;
   wndclass.lpszClassName = szAppName;

   separaParametros (szCmdLine, globalMainArguments);
   for (int pp = 0; pp < globalMainArguments.size (); pp++)
   {
      TRACE (("arg[%d] = [%s]\n", pp, globalMainArguments[pp].c_str ()));
   }
   if (globalMainArguments.size () > 0)
      gastonaMainGastFile = globalMainArguments[0];

   if (! RegisterClass (& wndclass))
   {
      return 0;
   }

   hwnd = CreateWindow (szAppName, TEXT ("gastonaWinter"),
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, hInstance, NULL);

   ShowWindow (hwnd, iCmdShow);
   UpdateWindow (hwnd);

   while (GetMessage (&msg, NULL, 0, 0))
   {
      bool capturado = false;
      TranslateMessage (&msg);
      if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN)
         capturado = gaston.broadcastKey (msg.hwnd, msg.message, msg.wParam);

       if (!capturado)
         DispatchMessage (&msg);
   }
   return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
      case EN_KILLFOCUS:
         {
            zWidget & wi = gaston.getzWidgetByID (HMENU (LOWORD (wParam)));
            if (!wi.isNIL ())
            {
               wi.winMessage (hwnd, EN_KILLFOCUS);
            }
         }
         break;

      case WM_CREATE:
         {
            TRACE (("gastona start stamp: %lld", traceCrono.getMillisecondsFrom1970 () ));
            if (gaston.loadGUI (hwnd, gastonaMainGastFile.c_str ()))
            {
               // main frame, note that it can be named different from "main"!
               gastonaCBase::frameWindow haupt = gaston.getMainFrame ();
               MoveWindow (hwnd, haupt.posx, haupt.posy, haupt.dx, haupt.dy, TRUE);
            }

            gaston.showLayout (hwnd, true);
            uniRect mango = gaston.getPreferredSizeOfMain ();
            TRACE (("preferred size main %d, %d", mango.dx, mango.dy ));
          }
         break;

      case WM_SIZE:
         if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
         {
            // EVA Layout --------------------------------
            //
            // on each size change
            //
            gaston.showLayout (hwnd);
         }
         break;

      case WM_NOTIFY:
         {
            zWidget & wi = gaston.getzWidgetByID (HMENU(((LPNMHDR) lParam)->idFrom));
            if (!wi.isNIL ())
            {
               wi.winMessage (hwnd, ((LPNMHDR)lParam)->code);
            }
         }
         break;

      case WM_COMMAND:
         {
            // mainly button clicks ?
            //
            zWidget & wi = gaston.getzWidgetByID (HMENU (LOWORD (wParam)));
            if (!wi.isNIL ())
            {
               wi.winMessage (hwnd, HIWORD (wParam));
            }
         }
         break;

      case WM_DESTROY :
         PostQuitMessage (0);
         break;

      default:
         return DefWindowProc (hwnd, message, wParam, lParam);
   }
   //return DefWindowProc (hwnd, message, wParam, lParam);
   return 0;
}

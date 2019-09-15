/*
project gastonaCpp
Copyright (C) 2018 Alejandro Xalabarder Aulet

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <iostream>

using namespace std;

#include "Eva.h"

int main (int nn, char ** aa)
{
   // creating a new eva
   //
   Eva nova("my first eva");

   // adding EvaLine's
   //
   nova.addLine (EvaLine("one,two,three"));
   nova.addLine (EvaLine(",,,"));

   // seting values
   //
   nova[1][2] = "tres";
   nova[1][5] = "wrong, will not work";
   nova.setValue ("this is ok", 1, 6); // setValue makes place
   nova.ensureDim (3, 3);
   nova[2][2] = "fin";

   // accesing all contents
   //
   cout << "--- accessing one by one:" << endl;
   cout << "eva named =" << nova.getName() << "= contains :" << endl;
   for (int rr = 0; rr < nova.rows(); rr ++)
   {
      cout << rr << ") ";
      for (int cc = 0; cc < nova.cols(rr); cc ++)
         cout << "[" << nova[rr][cc].c_str() << "] ";
      cout << endl;
   }
   cout << endl;

   // deserializing (to save into a file or for debug)
   //
   cout << "--- using toString method:" << endl;
   cout << nova.toString ().c_str ()  << endl;
}

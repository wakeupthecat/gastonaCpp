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

#include "EvaUnit.h"

int main (int nn, char ** aa)
{
   // creating a new evanit
   //
   EvaUnit eunit("my first unit");

   // adding eva's
   //
   eunit.add (Eva("unaEva"));
   eunit.add (Eva("secondEva"));

   // accesing all its Eva's
   //
   cout << "unit " << eunit.getName() << " contains the evas : ";
   for (int ii = 0; ii < eunit.size(); ii ++)
      cout << eunit[ii].getName() << " ";
   cout << endl;

   // accesing Eva per name
   //
   Eva & ptEva1 = eunit.getEva("unaEva");
   Eva & ptEva2 = eunit["unaEva"];

   ptEva1.setValue ("initial line");
   ptEva2.setValue ("second line", 1, 0);
   ptEva2.setValue ("second column", 1, 1);

   // deserializing (to save into a file or for debug)
   cout << eunit.toString ().c_str () << endl;
}

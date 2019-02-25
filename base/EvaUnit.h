/*
Eva library C++ (www.elxala.de)
Copyright (C) 2005-2017  Alejandro Xalabarder Aulet

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

#ifndef _CELTA_EVAUNIT_H_
#define _CELTA_EVAUNIT_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include "Eva.h"

/**
   @class  EvaUnit
   @author Alejandro Xalabarder
   @date   03.07.2003

   @memo
   A EvaUnit is a structure that has a name (almost any text including white spaces)
   and a set of Eva's. Usually will be stored in a text file of format Eva with more
   EvaUnit's

   @todo General review, document it in english and make it conform with Eva specification 1.1

   @example
   <pre>

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

         // accesing Eva per name
         //
         Eva & ptEva1 = eunit.getEva("unaEva");
         Eva & ptEva2 = eunit["unaEva"];

         // deserializing (to save into a file or for debug)
         cout << eunit.toString ().c_str ();
   </pre>
*/
class EvaUnit
{
   public:

   string      Nombre;

   map<string, Eva> mapUnit;
   typedef map<string, Eva>::iterator autoMap;

   vector<string> evasCatalog;

   EvaUnit (const string & nom = ""): Nombre(nom)
   {
   }

   const string & getName () const { return Nombre; }
   int size ()   { return (int) mapUnit.size ();   }
   void setName (const string & name) { Nombre = name; }
   bool containsEva (const string & name)
   {
      autoMap it = mapUnit.find (name);
      return mapUnit.find (name) != mapUnit.end ();
   }

   vector<string> & getEvaCatalog ()
   {
      return evasCatalog;
   }

   // return false if eva was already in the unit
   //
   bool addEva (const Eva & nova)
   {
      if (! containsEva (nova.getName ()))
      {
         mapUnit [nova.Nombre] = nova;
         evasCatalog.push_back (nova.Nombre);
         return true;
      }
      return false;
   }

   // alias from addEva
   bool add (const Eva & nova)
   {
      return addEva (nova);
   }

   void clear ()
   {
      mapUnit.clear ();
      evasCatalog.clear ();
      Nombre = "";
   }

   string toString ()
   {
      string str ("#" + getName () + "#\n");

      // order by name, NOT interested !
      //for (autoMap it = mapUnit.begin (); it != mapUnit.end (); it ++)

      for (int ii = 0; ii < evasCatalog.size (); ii ++)
         str += "\n" + getEva(evasCatalog[ii]).toString ();

      return str;
   }

   /// Returns the Eva with name "nameEva" or NIL_Eva () if it does not exist
   Eva & getEva (const string & nameEva)
   {
      autoMap it = mapUnit.find (nameEva);
      return it != mapUnit.end () ? it->second: NIL_Eva ();
   }

   // ensures the Eva 'nameEva' exists, if not then it will be created
   void ensureEva (const string & nameEva)
   {
      autoMap it = mapUnit.find (nameEva);
      if (it == mapUnit.end ())
      {
         mapUnit[nameEva] = Eva (nameEva);
         evasCatalog.push_back (nameEva);
      }
   }

   /**
      gets the eva named 'evaName' if exists in the EvaUnit if not then it is
      created.
   */
   Eva & getSomeHowEva (const string & evaName)
   {
      ensureEva (evaName);
      return getEva (evaName);
   }

   string & getValue (const string & nameEva, int nrow = 0, int ncol = 0)
   {
      return getEva (nameEva) [nrow] [ncol];
   }

   /// sets the value 'value' at 'nrow' 'col' in the Eva with name 'evaName'
   /// if the eva is not present it will be created and if it is not big enough redimensioned as well
   void setValue (const string & evaName, const char * value, int row = 0, int col = 0)
   {
      getSomeHowEva (evaName).setValue (value, row, col);
   }

   //// return current index of the eva named "evaname"
   //int fixIndexOf (const string & evaName)
   //{
   //   autoMap itx = mapUnit.find (evaName);
   //   if (itx == mapUnit.end ()) return -1;
   //   int indx = 0;
   //   for (autoMap it = mapUnit.begin ();
   //        it != mapUnit.end ();
   //        it ++, indx ++)
   //       if (it == itx) return indx;
   //
   //   return -1; // strange!
   //}

   Eva & operator [] (const int indxEva)
   {
      autoMap it;
      int indx = 0;
      for (it = mapUnit.begin (), indx = 0;
           it != mapUnit.end () && indx < indxEva;
           it ++, indx ++);

      return (it != mapUnit.end ()) ? it->second: NIL_Eva ();
   }

   Eva & operator [] (const string & evaName)
   {
      autoMap it = mapUnit.find (evaName);
      return (it != mapUnit.end ()) ? it->second: NIL_Eva ();
   }

   static bool isNILEva (Eva & eva) { return &eva == &NIL_Eva(); }

protected:

   static Eva & NIL_Eva ()
   {
      static Eva inner;
      inner.clear (); // ensure it is clear on each call
      return inner;
   }
};

#endif // _CELTA_EVAUNIT_H_

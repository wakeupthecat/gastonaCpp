/*
Eva library C++ (www.elxala.de)
Copyright (C) 2005-2019 Alejandro Xalabarder Aulet

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

#ifndef _CELTA_EVA_H_
#define _CELTA_EVA_H_

#include "EvaLine.h"

/**
   @class  Eva
   @author Alejandro Xalabarder
   @date   03.07.2003

   @memo
   An Eva is a structure that has a name (almost any text including white spaces)
   and a number of EvaLine's (see EvaLine) which are basically arrays of strings.
   Thus an Eva can define its contents in rows (EvaLines) and columns (index within the EvaLine).

   @todo General review, document it in english and make it conform with Eva specification 1.1

   @example
   <pre>

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
         cout << "eva named =" << nova.getName() << "= contains : \n";
         for (int rr = 0; rr < nova.rows(); rr ++)
         {
            cout << rr << ") ";
            for (int cc = 0; cc < nova.cols(rr); cc ++)
               cout << "[" << nova[rr][cc].c_str() << "] ";
            cout << "\n";
         }

         // deserializing (to save into a file or for debug)
         //
         cout << nova.toString ().c_str ();
   </pre>
*/
class Eva
{
public:

   string Nombre;
   vector<EvaLine> lis_EvaLine;

   // only create element if value != ""
   // so Eva("joana", "") does not create any line!
   //
   Eva (const string & nom = "", const string & value = ""): Nombre(nom)
   {
      if (value.length () > 0)
         addLine (EvaLine (value));
   }

   static bool isNILEvaLine (EvaLine & elin) { return &elin == &NIL_EvaLine (); }

   const string & getName () const { return Nombre; }
   void setName (const string & name) { Nombre = name; }

   void clear ()
   {
      clearLines ();
      Nombre = "";
   }

   void clearLines ()
   {
      lis_EvaLine.clear ();
      NIL_EvaLine ().clear ();
   }

   void addLine (const EvaLine & nova = EvaLine ())
   {
      lis_EvaLine.push_back (nova);
   }

   bool ensureRow (int indxRow)
   {
      // insert rows ?
      while (rows() < indxRow + 1)
         addLine ();
      return (indxRow >= 0 && indxRow < rows()); // so enrureRow(-1) returns false !
   }

   bool ensureDim (int rows, int cols)
   {
      if (!ensureRow (rows - 1)) return false;
      for (int rr = 0; rr < rows; rr ++)
         get (rr).ensureCol (cols - 1);

      return true;
   }

   bool dim (int rows, int cols)
   {
      clear ();
      return ensureDim (rows, cols);
   }

   EvaLine & get (int nLin)
   {
      return (nLin < rows () && nLin >= 0) ? lis_EvaLine [nLin]: NIL_EvaLine ();
   }

   int maxCols ()
   {
      int maxx = 0;
      if (rows () > 0)
      {
         for (int rr = 0; rr < lis_EvaLine.size (); rr ++)
            if (lis_EvaLine[rr].cols () > maxx)
               maxx = lis_EvaLine[rr].cols ();
      }
      return maxx;
   }

   string toString ()
   {
      string str;

      str = ("   <" + Nombre + ">\n");
      for (int ii = 0; ii < rows (); ii ++)
      {
         str.append ("      " + lis_EvaLine[ii].toString ());
         str.append ("\n");
      }
      return string (str);
   }

   void setValue (const string & sVal, int row = 0, int col = 0)
   {
      if (ensureRow (row))
         lis_EvaLine [row].setValue (sVal, col);
   }

   string getValue (int row = 0, int col = 0)
   {
      return copyValue (row, col);
   }

   string copyValue (int row = 0, int col = 0)
   {
      if (row < rows () && col < lis_EvaLine [row].size ())
      {
         return lis_EvaLine [row][col];
      }

      return string ();
   }

   int rows ()
   {
      return (int) lis_EvaLine.size ();
   }

   int cols (int nrow = 0)
   {
      return get (nrow).cols ();
   }

   EvaLine & operator [] (int nrow)
   {
      return get(nrow);
   }

protected:

   static EvaLine & NIL_EvaLine ()
   {
      static EvaLine inner;
      inner.clear (); // don't return corrupted values
      return inner;
   }
};

#endif // _CELTA_EVA_H_

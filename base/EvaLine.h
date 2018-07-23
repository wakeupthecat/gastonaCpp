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

/**

   EvaLine.h
   @author Alejandro Xalabarder 03.07.2003 23:35

   21.04.2017        Optimized version
   03.07.2003 23:36  Pasar a C++
*/

#ifndef _CELTA_EvaLine_H_
#define _CELTA_EvaLine_H_

#include <string.h>
#include <string>
#include <vector>

using namespace std;

/**
   @class  EvaLine
   @author Alejandro Xalabarder
   @date   03.07.2003-2017

   @memo
   A EvaLine is a structure that contains an array of strings, the indices of the array
   are called columns because EvaLine represents a single line within a Eva structure.

   @todo General review, document it in english and make it conform with Eva specification 1.1

   @example
   <pre>

         // creating a new eva line
         //
         EvaLine lineOne("id, name, description");

         // accesing all its Eva's
         //
         cout << "the evaline contains the strings : \n";
         for (int ii = 0; ii < lineOne.cols(); ii ++)
            cout << "\"" << lineOne[ii] << "\" ";

         // accesing columns per index
         //
         cout << "\nat index 2 " << lineOne[2];
         lineOne[5] = "five!ERROR";
         lineOne.ensureCol (6);
         lineOne[6] = "now 6 ok";
         lineOne.setValue ("string, with \"special\" characters", 7);

         // deserializing (to save into a file or for debug)
         cout << "\nwhole line [" << lineOne.toString ().c_str () << "]";
   </pre>
*/
class EvaLine
{

public:

   vector<string> arr_Column;

   EvaLine (const string & sLin = "") { set (sLin); }

   void clear ()
   {
      arr_Column.clear ();
   }

   bool existColumn (const int ncol = 0)
   {
      return ncol < cols () && ncol >= 0;
   }

   void setValue (const string & sVal, int ncol)
   {
      if (ensureCol (ncol))
         arr_Column[ncol] = sVal;
   }

   bool ensureCol (const int ncol)
   {
      while (cols() <= ncol)
         addValue ();
      return (ncol >= 0 && ncol < cols()); // so ensureCol(-1) returns false !
   }

   void addValue (const string & sVal = "")
   {
      arr_Column.push_back (sVal);
   }

   string & get (const int ncol)
   {
      return existColumn (ncol) ? arr_Column [ncol]: NIL_string ();
   }

   string toString ()
   {
      string sb;
      for (int ii = 0; ii < arr_Column.size (); ii ++)
         sb += (ii > 0 ? ",": "") + empaqueta (ii);

      return sb;
   }

protected:

   bool checkIsRestOfLineable (const string & str)
   {
      long L = str.length ();
      // we can use end of line only if not ended with blank!
      // or else it would be trimmed erroniously when parsed again
      return L == 0 || (str[L-1] != ' ' && str[L-1] != '\t');
   }

   string empaqueta (int indx)
   {
      string & str = arr_Column[indx];

      long LEN = str.length ();
      if (LEN == 0)
      {
         // if empty return "//" only if it is one column line otherwise nothing!
         //
         return arr_Column.size () == 1 ? "//": ""; // value is "" only if is empty an one column
      }

      // if last element always use //
      //
      if (indx == arr_Column.size () - 1 && checkIsRestOfLineable (str))
         return "//" + str;

      // check begin for literals, # and < and begin & end for blanks
      //
      string str2;
      bool quote = str[0] == ' ' || str[0] == '\t' || str[0] == '\'' ||
                   str[0] == '<' ||str[0] == '#' ||
                   (LEN > 1 && str[0] == '/' && str[1] == '/') ||
                   str[LEN-1] == ' ' || str[LEN-1] == '\t';

      // check at any position " and , (quote and comma)
      //
      int casos = 0;
      int pi = 0;
      while (pi < LEN)
      {
         quote |= (str[pi] == '\"' || str[pi] == ',');
         // insert double quotes when needed
         if (str[pi] == '\"')
         {
            if (casos == 0)
               str2 = str;
            str2 = str2.insert (pi + 1 + casos++, "\"");
         }
         pi ++;
      }
      return !quote ? str: string("\"") + (str2.length () > 0 ? str2: str) + string("\"");
   }

public:

   vector<string> & getColumns ()
   {
      return arr_Column;
   }

   void set (const string & sLin)
   {
      arr_Column = toVector (sLin);
   }

   void setRef (const vector<string> & aLin)
   {
      arr_Column = aLin;
   }

   void copy (const vector<string> & aLin)
   {
      for (int ii = 0; ii < aLin.size (); ii ++)
         arr_Column[ii] = aLin[ii];
   }

   int cols ()
   {
      return (int) arr_Column.size ();
   }

   int size ()
   {
      return cols ();
   }

   int indexOf (const string & compa)
   {
      for (int ii = 0; ii < arr_Column.size (); ii ++)
         if (compa == arr_Column[ii]) return ii;
      return -1;
   }

   string & operator [] (const int ncol)
   {
      return get(ncol);
   }

protected:

   // NOTES about optimization and comparation with java and javascript
   // This file is * C++ (fastCell)
   //
   //
   //    repetitions = 10 x 1000000 times (ten million)
   //    String      = "pablit\"has\"apit   , \"   en, todps \"\"los\"\", sentidos, // na, \"mosol\", jaj";
   //    it has two double quotes that force to use concatenation
   //
   //   java Cadena opt   ~   5012 millis  ..java
   //   js Firefox        ~   8000 millis  ..JS
   //   C (strcat)        ~  12000 millis  Only using strcpy, strcat (no std::string), therefore limited size (no malloc)
   // * C++ (fastCell)    ~  14700 millis  hybrid : strcpy, strcat until 600 chars then string::substr
   //   js Chrome         ~  15500 millis  ..JS
   //   C++ (substr)      ~  45700 millis  No strcpy, strcat but only std:string
   //   C++ (old Cadena)  ~ 169700 millis  old algorithm
   //

   enum {
      FASTCELL_SIZE = 1025,
   };

   bool getCell (const string & str, int & pi, string & cell, char * fastCell, bool & useFast)
   {
      long LEN = str.length ();
      useFast = LEN - pi <= FASTCELL_SIZE;

      if (useFast)
           fastCell[0] = 0;
      else cell = "";

     // trim
     while (pi < LEN && (str[pi] == ' ' || str[pi] == '\t')) pi++;
     if (pi >= LEN) return false;

     // NOTE: ignore warnings about strcpy and strncat being not safe
     //       they are used safely and are fast
     //       replacing them (e.g. useFast == false) would affect
     //       negatively the performance
     if (str[pi] == '\'')
     {
        if (useFast)
             strcpy (fastCell, str.c_str() + pi + 1);
        else cell = str.substr (pi+1);
        pi = LEN;
        return true;
     }

     // si // return str.substring (pos+2);
     //
     if (pi+1 < LEN && str[pi] == '/' && str[pi+1] == '/')
     {
        if (useFast)
             strcpy (fastCell, str.c_str() + pi + 2);
        else cell = str.substr (pi + 2);
        pi = LEN;
        return true;
     }

     int envolta = str[pi] == '\"';
     int ini = envolta ? ++pi: pi;
     do
     {
        if (envolta)
        {
           if (str[pi] != '\"') pi ++;
           else
              if (pi+1 < LEN && str[pi+1] == '\"')
              {
                 // double ""
                 // add a part including one " and continue
                 if (pi+1 > ini)
                 {
                    if (useFast)
                         strncat (fastCell, str.c_str() + ini, pi + 1 - ini);
                    else cell += str.substr (ini, pi + 1 - ini);
                 }
                 pi += 2;
                 ini = pi;
              }
              else break; // close "
        }
        else
        {
           if (str[pi] == ',') break;
           pi ++;
        }
     } while (pi < LEN);

      int fi2 = pi;

      // right trim if not quoted
      //
      if (! envolta)
        while (fi2 > ini && fi2 > 0 && (str[fi2-1] == ' ' || str[fi2-1] == '\t')) fi2 --;

      if (fi2 > ini)
        if (useFast)
            strncat (fastCell, str.c_str() + ini, fi2-ini);
        else cell += str.substr (ini, fi2-ini);

      pi ++;

      if (envolta)
      {
         // find the next comma if any
         while (pi < LEN && str[pi] != ',') pi++;
         pi ++; // skip the comma
      }

      return true;
   }

public:

   vector<string> toVector (const string & linea)
   {
       vector<string> eline;
       string cell;
       char fastCell [FASTCELL_SIZE + 1];
       int pi = 0;
       bool useFast = false;

       do {
            if (getCell (linea, pi, cell, fastCell, useFast))
               eline.push_back (useFast ? fastCell: cell);
       } while (pi < linea.length ());

       return eline;
   }

   static bool isNilString (string & str) { return &str == &NIL_string (); }

protected:

   static string & NIL_string ()
   {
      static string inner;
      return inner;
   }
};

#endif

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
   EvaFile.h
   @author Alejandro Xalabarder 03.07.2003 23:35

   xx.04.2017 23:36  Optimize and change EvaFile structure
   03.07.2003 23:36  Pasar a C++
*/

#ifndef _CELTA_EVAFILE_H_
#define _CELTA_EVAFILE_H_

#include "EvaUnit.h"

/**
   @class  EvaFile
   @author Alejandro Xalabarder
   @date   03.07.2003

   @memo
   Class to read and write EvaUnit structures in a file (persistency). An object of this
   class can do just three things : get the names of all EvaUnits in a file, load a specific
   EvaUnit from a file and save a EvaUnit in a file.


   @example
   <pre>
   </pre>
*/
class EvaFile
{
private:

   string pars_name;
   Eva pars_currentEva;
   EvaUnit pars_currentUnit;
   vector<string> unitCatalog;

public:

   string fileName;
   map<string, EvaUnit> mapAllUnits;
   typedef map<string, EvaUnit>::iterator autoMap;

   EvaFile (const string & filename2load = ""):
      pars_name        (),
      pars_currentEva  (),
      pars_currentUnit (),
      unitCatalog      (),
      fileName         (filename2load),
      mapAllUnits      ()
   {
      loadFile (fileName);
   }

   EvaFile (const string & filename2load, vector<string> & units2load):
      pars_name        (),
      pars_currentEva  (),
      pars_currentUnit (),
      unitCatalog      (),
      fileName         (filename2load),
      mapAllUnits      ()
   {
      loadFile (filename2load, units2load);
   }

   // return a vector with all eva unit names loaded
   vector<string> & getEvaUnitsCatalog ()
   {
      return unitCatalog;
   }

   EvaUnit & getUnit (const string & unitname)
   {
      return operator[] (unitname);
   }

   EvaUnit & getSomehowUnit (const string & unitname)
   {
      if (isNILEvaUnit (getUnit (unitname)))
      {
         mapAllUnits [unitname] = EvaUnit (unitname);
         unitCatalog.push_back (unitname);
      }
      return getUnit (unitname);
   }

protected:

   void setCurrentEva (const string & name)
   {
      if (pars_currentUnit.getName ().length () > 0 && pars_currentEva.getName ().length () > 0)
         pars_currentUnit.add (pars_currentEva);

      pars_currentEva = Eva (name);
   }

   void setCurrentUnit (const string & name)
   {
      if (pars_currentUnit.getName ().length () > 0)
      {
         setCurrentEva ("");
         mapAllUnits[pars_currentUnit.getName ()] = pars_currentUnit;
         unitCatalog.push_back (pars_currentUnit.getName ());
      }
      pars_currentUnit = EvaUnit (name);
   }

   /*
      Example of use:
      var sal = isname ("< isThis aName?> yes it is   ", "<", ">")
      out ("name = /" + sal.name + "/");
      out ("rest = /" + sal.rest + "/");
      // name = / isThis aName?/
      // rest = /yes it is/
   */
   bool isname (char * str, int & pos, char startch, char endch)
   {
      if (str[pos] != startch) return false;
      int ini = ++ pos;
      while (str[pos] != 0 && str[pos] != endch) pos ++;
      if (str[pos] == 0) return false;

      str[pos ++] = 0;

      pars_name = string (str+ini);
      return true;
   }

   bool nameRequired (const string & name, vector<string> & requestedOnes)
   {
      if (requestedOnes.size () == 0) return true; // all

      for (int rr = 0; rr < requestedOnes.size (); rr ++)
         if (requestedOnes[rr] == name) return true;
      return false;
   }

public:

   //Example
   //    evaFile ef = new evaFile ();
   //
   bool loadFile (const string & filename = "")
   {
      vector<string> vacio;
      return loadFile (filename, vacio);
   }

   bool loadFile (const string & filename, vector<string> & units2Load)
   {
      mapAllUnits.clear ();
      unitCatalog.clear ();

      fileName = filename;

      const int MAXLIN = 2048;
      char str [MAXLIN+1];

      FILE * fitx = fopen (fileName.c_str (), "rt");
      if (fitx == 0)
         return false;

      while (!feof (fitx))
      {
         if (fgets (str, MAXLIN, fitx) == 0) break;

         long LEN = strlen(str);
         // trim new line and right blanks
         while (LEN > 0 && (str[LEN-1] == 10 || str[LEN-1] == 13 || str[LEN-1] == ' ' || str[LEN-1] == '\t')) LEN--;
         str[LEN] = 0;

         int pi = 0;
         // trim left
         while (pi < LEN && (str[pi] == ' ' || str[pi] == '\t')) pi++;
         if (pi >= LEN) continue; // ignore empty lines

         // check for comment    <! etc ...
         if (pi+1 < LEN && str[pi] == '<' && str[pi+1] == '!') continue;

         // check if start of unit
         //
         if (isname (str, pi, '#', '#'))
         {
            if (pars_name.length () > 1 && pars_name[0] == '*' && pars_name[1] == '*') break; // logic end of file

            // load only if "all" or found in units2Load, otherwise we close the current unit
            // and wait for the next unit
            if (nameRequired (pars_name, units2Load))
                 setCurrentUnit (pars_name);
            else setCurrentUnit ("");
            continue;
         }
         if (pars_currentUnit.getName ().length () == 0) continue;

         // check if start of eva
         //
         if (isname (str, pi, '<', '>'))
         {
            setCurrentEva (pars_name);
            if (pi >= LEN) continue;
         }

         // line is not empty
         // and we have a current Eva active
         //
         if (pi < LEN && pars_currentEva.getName ().length () != 0)
            pars_currentEva.addLine (EvaLine (str + pi));
      }
      setCurrentUnit ("");
      return true;
   }

   bool saveFile (const string & fname = "")
   {
      vector<string> vacio;
      return saveFile (fname, vacio);
   }

   bool saveFile (const string & fname, vector<string> & units2Save)
   {
      string fn (fname);
      if (fn.length () == 0)
         fn = fileName;

      FILE * fitx = fopen (fn.c_str (), "w+");
      if (fitx == 0)
      {
         printf ("ERROR: EvaFile::Save, the file [%s] couln't be opened for write!", fn.c_str ());
         return false;
      }

      for (int ii = 0; ii < unitCatalog.size (); ii ++)
         if (nameRequired (unitCatalog[ii], units2Save))
            writeEvaUnitOnFile (fitx, getUnit (unitCatalog[ii]));

      fprintf (fitx, "\n#**FIN#");
      fclose (fitx);
      return true;
   }


   /**
      Parses the Eva-like lines of text given in 'textLines', then builds an unamed Eva
      with it which is returned
   */
   static Eva & text2Eva (vector<string> & textLines, Eva & theEva)
   {
      if (! EvaUnit::isNILEva (theEva))
      {
         string linStr = "";
         for (int ii = 0; ii < textLines.size (); ii ++)
         {
            int fi = 0;
            // trim
            while (fi < textLines[ii].length () && (textLines[ii][fi] == ' ' || textLines[ii][fi] == '\t')) fi ++;
            if (fi >= textLines[ii].length ()) continue; // ignore empty lines
            theEva.addLine (EvaLine (string (textLines[ii].c_str () + fi)));
         }
      }
      return theEva;
   }

   /**
      this function is needed because simply calling
         theFile.writeLine (ueva.toString ());
      could cause a java.lang.OutOfMemoryError if the evaunit is very large
   */
   void writeEvaUnitOnFile (FILE * fitx, EvaUnit & eunit)
   {
      vector<string> cata = eunit.getEvaCatalog ();

      fprintf (fitx, "#%s#\n", eunit.getName ().c_str ());
      for (int ee = 0; ee < cata.size (); ee ++)
      {
         Eva eva = eunit.getEva(cata[ee]);
         fprintf (fitx, "\n   <%s>%s", eva.getName ().c_str (), (eva.rows() > 1 ? "\n": ""));

         for (int rr = 0; rr < eva.rows(); rr ++)
         {
            EvaLine elin = eva.get(rr);
            fprintf (fitx, "      %s\n", elin.toString ().c_str ());
         }
      }
      fprintf (fitx, "\n");
   }

   EvaUnit & operator[] (const int indxEvaUnit)
   {
      autoMap it;
      int indx = 0;
      for (it = mapAllUnits.begin (), indx = 0;
           it != mapAllUnits.end () && indx < indxEvaUnit;
           it ++, indx ++);

      return (it != mapAllUnits.end ()) ? it->second: NIL_EvaUnit ();
   }

   EvaUnit & operator[] (const string & unitName)
   {
      autoMap it = mapAllUnits.find (unitName);
      return (it != mapAllUnits.end ()) ? it->second: NIL_EvaUnit ();
   }

   static bool isNILEvaUnit (EvaUnit & eu) { return &eu == &NIL_EvaUnit (); }

protected:

   static EvaUnit & NIL_EvaUnit ()
   {
      static EvaUnit inner;
      return inner;
   }

};

#endif


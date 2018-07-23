/*
*/

#ifndef GASTONACBASE_HEADER
#define GASTONACBASE_HEADER

#include <iostream>
#include <map>

#include "zWidget.h"
#include "EvaLayoutManager.h"
#include "EvaFile.h"

#include "listix.h"
#include "sqliteCommand.h"

#include "gastonaCGlobals.h"

using namespace std;

/**
   @name: gastonaCBase.h.h

   @desc: hace las funciones de javaj como fábrica de widgets


*/
class gastonaCBase
{
public:

   gastonaCBase (): bobo("")
   {
   }

   ~gastonaCBase ()
   {
      // gastonaCBase owns all its widgets!!
      //
      for (autoIter itero = mapzWidget.begin();
                 itero != mapzWidget.end();
                 itero ++)
      {
         (itero->second)->destroy ();
         free (itero->second);
      }
   }

protected:
   zWidget bobo; // dummy zWidget

   EvaLayoutManager layManager;

   map<string, zWidget *>  mapzWidget;
   typedef map<string, zWidget *>::iterator  autoIter; // needed to compile without std+11

   EvaFile wastEvafile;

public:

   // listix loListix;
   listix theListix;
   sqliteCommand theSliteCommand;

   bool startsWith (const string & source, const string & comp)
   {
      return source.find (comp) == 0;
   }

   vector<string> getAllWidgetNames (EvaUnit & ujavaj)
   {
      // get a list of all widget names present in all layouts
      //
      vector<string> widlist;

      for (int ee = 0; ee < ujavaj.size (); ee ++)
      {
         Eva elay = ujavaj[ee];

         if (startsWith (elay.getName (), "layout of "))
         {
            if (startsWith (elay[0][0], "EVA") || startsWith (elay[0][0], "Eva"))
            {
               // collect all widget names from EVALAYOUT type
               //
               for (int rr = 2; rr < elay.rows (); rr ++)
               {
                  for (int cc = 1; cc < elay[rr].cols (); cc ++)
                  {
                     string & winam = elay[rr][cc];

                     if (winam.length () > 1 && !startsWith (winam, "-") && !startsWith (winam, "+"))
                        widlist.push_back (winam);
                  }
               }
            }
            else
            {
               // print ("error: Layout type %s not supported!", elay[0][0]);
            }
         }
      }
      return widlist;
   }


   bool loadGast (const string & gastFileName)
   {
      // loading javaj layouts
      //
      wastEvafile = EvaFile (gastFileName);
      EvaUnit & euJavaj = wastEvafile["javaj"];
      if (EvaFile::isNilEvaUnit (euJavaj)) return false;
      printf ("jarjorlar :\n%s\n---\n", euJavaj.toString ().c_str ());
      
      // loading sqliteCommand engine
      //
      vector<string> in;
      vector<string> out;
      vector<string> err;
      in.push_back ("-version");
      theSliteCommand.executeCommand (in, out, err);
      printf ("sqliteEngines :\n%s\n---\n", out.size () > 0 ? out[0].c_str (): "?");

      // loading listix logic (lua)
      //
      cout << "rascos listix!" << endl;
      theListix.load (wastEvafile["luaix"], wastEvafile["data"]);
      printf ("listix loaded\n--\n");
      
      return true;
   }

   //
   // adds a new zWidget to GastonaC, NOTE: from this moment GastonaC owns the zWidget
   //
   void addAndControlzWidget (zWidget * zw)
   {
      if (zw == 0) return;
      // insert it into the two maps
      mapzWidget [zw->getName ()] = zw;
   }

   void showLayout (int dx, int dy)
   {
      // ---- EvaLayout: do the real job
      //
      layManager.doShowLayout("main", dx, dy);
   }

   bool haszWidgetName (const string & name)
   {
      return (mapzWidget.find (name) != mapzWidget.end());
   }

   zWidget * getzWidgetByName (const string & name)
   {
      autoIter iteroID = mapzWidget.find (name);
      if (iteroID == mapzWidget.end())
      {
         return & bobo;
      }
      return iteroID -> second;
   }
};

#endif  // _HEADER_H

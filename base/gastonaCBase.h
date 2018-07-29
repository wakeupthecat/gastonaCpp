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

   struct frameWindow
   {
      string name = "dummy";
      string title;
      int posx = 0;
      int posy = 0;
      int dx = 200;
      int dy = 100;
   };

   vector<frameWindow> frames;
   listix theListix;

   frameWindow getMainFrame ()
   {
      return frames.size () > 0 ? frames[0]: frameWindow ();
   }

   frameWindow getFrame (const string & name = "")
   {
      if (frames.size () == 0)
         return frameWindow ();
      if (name.length () == 0)
         return frames[0];
      for (int ii = 0; ii < frames.size (); ii ++)
         if (frames[ii].name == name)
            return frames[ii];
      return frameWindow ();
   }

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

   void loadFrames ()
   {
      if (EvaFile::isNILEvaUnit (wastEvafile["javaj"]))
         // keep frames.size () == 0
         return;

      Eva & framarr = wastEvafile["javaj"]["frames"];
      EvaUnit & udata = wastEvafile["data"];
      for (int ff = 0; ff < framarr.rows (); ff ++)
      {
         frameWindow fr;
         fr.name = framarr[ff][0];
         fr.title = framarr[ff][1];

         // only set initial sizes it if specified !
         if (framarr[ff].size () > 2)
            fr.dx = atoi (framarr[ff][2].c_str());
         if (framarr[ff].size () > 3)
            fr.dy = atoi (framarr[ff][3].c_str());

         // custom pos and size if made persistent
         //
         if (! EvaUnit::isNILEva (udata[fr.name + " posX"])) fr.posx = atoi (udata[fr.name + " posX" ][0][0].c_str ());
         if (! EvaUnit::isNILEva (udata[fr.name + " posY"])) fr.posy = atoi (udata[fr.name + " posY" ][0][0].c_str ());
         if (! EvaUnit::isNILEva (udata[fr.name + " sizeX"])) fr.dx  = atoi (udata[fr.name + " sizeX"][0][0].c_str ());
         if (! EvaUnit::isNILEva (udata[fr.name + " sizeY"])) fr.dy  = atoi (udata[fr.name + " sizeY"][0][0].c_str ());

         frames.push_back (fr);
      }

      // if #javaj# exists and no frames given then ensure at least 1 frame = "main"
      //
      if (frames.size () == 0)
      {
         frameWindow fr;
         fr.name = "main";
         fr.title = udata ["main title"][0][0];
         frames.push_back (fr);
      }
   }


   bool loadGast (const string & gastFileName)
   {
      // loading the entire gast file in memory
      //
      wastEvafile = EvaFile (gastFileName);

      loadFrames ();
      // javaj part will be loaded by each concrete loadGUI method
      //
      //    EvaUnit & euJavaj = wastEvafile["javaj"];
      //    if (EvaFile::isNILEvaUnit (euJavaj)) return false;
      //    printf ("jarjorlar :\n%s\n---\n", euJavaj.toString ().c_str ());

      // loading listix logic (lua)
      //
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
      layManager.doShowLayout(getMainFrame ().name, dx, dy);
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

/*
*/

#ifndef SQLITE_COMMAND_HEADER_
#define SQLITE_COMMAND_HEADER_

#include <string>
#include <vector>

#include "Mensaka.h"

using namespace std;

extern "C" int shellmain(int argc, char **argv);

class sqliteCommand: public MensakaTarget
{
   public:

   void executeCommand (vector<string> & args, vector<string> & out, vector<string> & err)
   {
      int nArgs = args.size () + 1; // +1 since first arg is command (e.g. sqlite3)
      char ** cArgs = (char **) malloc (sizeof (char *) * nArgs);
      if (cArgs == 0) return;
      cArgs[0] = (char *) "sqlite34gastonaC";
      for (int ii = 0; ii < args.size (); ii ++)
      {
         cArgs[ii + 1] = (char *) args[ii].c_str ();
      }
      shellmain(nArgs, cArgs);

      // how to capture stdout and stderr ?
      //
      out.push_back ("done!");
      err.push_back ("actually don't know...");
   }
   
   virtual bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
   }   
};

#endif 

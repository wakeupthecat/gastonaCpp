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
      // just prepared for possible use, of course a static notification to the messages
      // is needed as well. Some possible messages
      //    -->, sqlCommand.setDefaultDB, myTemp.db
      //    -->, sqlCommand.switchEngine, postgresql
   }
};

#endif

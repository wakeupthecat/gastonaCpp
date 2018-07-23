/*
*/

#ifndef LISTIX_HEADER_H
#define LISTIX_HEADER_H

#include <iostream>
#include <exception>

#include "EvaLine.h"
#include "Eva.h"
#include "EvaUnit.h"

#include "Mensaka.h"
#include "gastonaCGlobals.h"

extern "C" {
///     // #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
};

class listix : public MensakaTarget
{

public:
   listix () {};

   // we need pointers since not always is possible to construct
   // the object with the units and also because we want to be able to
   // change them
   //
   EvaUnit * uLsxFormats = 0;
   EvaUnit * uData = 0;
   
   EvaUnit nilFormats;
   EvaUnit nilData;
   
   EvaUnit & safeFormats ()
   {
      return (uLsxFormats == 0) ? nilFormats: *uLsxFormats;
   }

   EvaUnit & safeData ()
   {
      return (uLsxFormats == 0) ? nilFormats: *uLsxFormats;
   }

   void load (EvaUnit & logic, EvaUnit & data)
   {
      uLsxFormats = & logic;
      uData = & data;
      subscribe ();
      run ("main0");
   }

   enum {
      MAIN_FORMAT = -1, // negative in order to not collide with some other index
   };

   void subscribe ()
   {
      EvaUnit & forms = safeFormats ();
      // we notify the messages with the index of the format
      // supposing there is NO modification of lsxFormats !!
      //
      for (int ii = 0; ii < forms.size (); ii ++)
      {
         if (strncmp (forms[ii].getName ().c_str (), "-- ", 3) == 0)
         {
            cout << "listix mensaka subs [" << forms[ii].getName ().substr (3) << "]" << endl;
            Mensaka::subscribeToMessage (forms[ii].getName ().substr (3), ii, this);
         }
      }
      Mensaka::subscribeToMessage (gastonaCGlobals::msgFRAMES_VISIBLE (), MAIN_FORMAT, this);
   }

   void run (const char * format)
   {
      cout << "listix run format [" << format << "]" << endl;

      EvaUnit & forms = safeFormats ();
      Eva & eva = forms[format];
      cout << "listix eva rows " << eva.rows () << endl;
      if (false == EvaUnit::isNILEva (eva))
         callLua (eva);
   }

   void callLua (Eva & luascript, const vector<string> & params = vector<string> ())
   {
      (void) params; // not used by now until we learn a little bit more about Lua

      if (EvaUnit::isNILEva (luascript)) return;

      cout << "callLua with script of " << luascript.rows () << " lines" << endl;

      string txtscript;
      for (int ii = 0; ii < luascript.rows (); ii ++)
      {
         txtscript += luascript[ii][0];
         txtscript += "\n";
      }
      
      try 
      {
         lua_State *L = luaL_newstate();
         luaL_openlibs(L);
         luaL_loadstring(L, txtscript.c_str ());
         lua_pcall(L, 0, 0, 0); /* call the function */
         lua_close(L);
      }
      catch (exception e)
      {
         cout << "Exception executing Lua script: " << e.what () << endl;
      }
   }

   virtual bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      // again, the mappedMsg is the index of the format => assuming that lsxFormat does not change!
      if (mappedMsg == MAIN_FORMAT)
           run ("main");
      else callLua (safeFormats ()[mappedMsg], params);
      return true;
   }
};

#endif  // _HEADER_H

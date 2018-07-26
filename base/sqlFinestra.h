/*
*/

#ifndef SQLFINESTRA_HEADER_
#define SQLFINESTRA_HEADER_

#include <sstream>
#include <string>
#include <vector>

#include "Eva.h"
#include "sqlite3.h"
#include "traceMacro.h"

using namespace std;

/*

//========== test_sqlFinestra.cpp

#include "sqlFinestra.h"


int main (int nn, char ** aa)
{
   sqlFinestra sq ("mytest.db");

   sq.executeSQL ("CREATE TABLE IF NOT EXISTS prova (id, nom); DELETE FROM prova;");
   sq.executeSQL ("INSERT INTO prova VALUES (1, 'marla');"
                  "INSERT INTO prova VALUES (2, 'alea');"
                  "INSERT INTO prova VALUES (6, 'marvin');"
                  "INSERT INTO prova VALUES (7, 'abel');"
                 );

   sq.setSelectQuery ("SELECT * FROM prova");
   vector<string> cols = sq.getColumns ();

   for (long cc = 0; cc < cols.size (); cc++)
      printf ("%s, ", cols[cc].c_str ());
   printf ("\n");

   for (long rr = 1; rr <= sq.getCountRecords (); rr ++)
   {
      for (long cc = 0; cc < cols.size (); cc++)
         printf ("%s, ", sq.getValue (rr, cc).c_str ());
      printf ("\n");
   }
}

*/

// extern "C" int shellmain(int argc, char **argv);

class sqlFinestra
{
   public:

   enum {
      MAX_CACHE_ROWS = 4, // for testing
   };

   sqlFinestra (const string & dbname = ""): openByMe (true)
   {
      dbName = dbname.length () == 0 ? defaultDBName: dbname;
      if (sqlite3_open(dbName.c_str (), & db) == SQLITE_OK)
      {
         TRACE (("database %s opened", dbName.c_str () ));
      }
      else
      {
         TRACE_ERR (("Can't open database: %s", sqlite3_errmsg(db)));
      }
   }

   sqlFinestra (sqlite3 * dbopened): openByMe (false)
   {
      db = dbopened;
   }

   ~sqlFinestra ()
   {
      if (db && openByMe)
         sqlite3_close (db);
      db = 0;
   }

   bool executeSQL (const string & query, bool transaction = false)
   {
      if (db == 0) return false;

      std::stringstream sql;
      sql << (transaction ? "BEGIN; ":"") << query << (transaction ? "; END; ": ";");

      char * zErrMsg = 0;
      if (sqlite3_exec (
                db,
                sql.str ().c_str (),
                sqlFinestra::static_execute_callback, // static callback called on each record
                this,                                 // used by the callback to find this object
                & zErrMsg) != SQLITE_OK)
      {
         TRACE_ERR (("SQL error: %s in query [%s]", zErrMsg, sql.str ().c_str ()));
         sqlite3_free(zErrMsg);
         return false;
      }
      return true;
   }

   bool setSelectQuery (const string & selectQuery)
   {
      if (db == 0) return false;

      sqlSelQuery = selectQuery;
      dataBuffer.clear ();
      recordOffset = 0;
      recordTotalCount = -1;

      setCursor (1);
      return true;
   }

   // get the value of the column "colname" in the current record
   //
   const string & getCurrentValue (const string & colname)
   {
      return dataBuffer[currentCursor][getIndexColumn (colname)];
   }

   const string & getValue (long rowindx, int colindx)
   {
      setCurrentRecordIndex (rowindx);
      return dataBuffer[currentCursor][colindx];
   }

   const string & getValue (long rowindx, const string & colname)
   {
      return getValue (rowindx, getIndexColumn (colname));
   }

   // get a vector of strings with the column names of the result of the select query
   //
   vector<string> & getColumns ()
   {
      return dataBuffer[0].getColumns (); // by chance EvaLine has also a method called getColumns (another meaning...)
   }

   // return the index (0..n columns) of the column "colname" or -1 if not found
   //
   int getIndexColumn (const string & colname)
   {
      return dataBuffer[0].indexOf (colname);
   }

   // returns the current record index (1.. record count)
   //
   long getCurrentRecordIndex () { return currentCursor; }

   bool setCurrentRecordIndex (long indx)
   {
      return setCursor (indx);
   }

   // returns the total record count
   //
   long getCountRecords () { return recordTotalCount; }

   bool setCursor (long recordNr)
   {
      if (recordNr < 0) return false;
      if (recordTotalCount != -1 && recordNr > recordTotalCount) return false;

      currentCursor = recordNr;
      if (recordNr >= minRecordNr () && recordNr <= maxRecordNr ())
      {
         // the record is present
         return true;
      }

      // if the record is not present fetch a complete buffer
      // using a select ... LIMIT offset, size
      //

      // depending on the direction we want our recordNr to be the first or the last one
      // of the buffer.
      long offsetFetch = recordNr > maxRecordNr () ? recordNr: recordNr - min(0, 1 + recordNr - MAX_CACHE_ROWS);

      std::stringstream sqlFetch;
      sqlFetch << "select * from (" << sqlSelQuery << ") LIMIT " << offsetFetch << ", " << MAX_CACHE_ROWS;

      recordOffset = offsetFetch;
      indxBuffer = 0;
      executeSQL (sqlFetch.str (), false);

      if (recordTotalCount == -1) // unkown
      {
         //assert (recordOffset == 1)
         if (indxBuffer < MAX_CACHE_ROWS)
         {
            recordTotalCount = indxBuffer;
            TRACE (("totalRecords is %ld", recordTotalCount));
         }
         else
         {
            // we need an extra call to select count (*) to get the total nr of records
            // we pass the opened db so the query can be faster
            sqlFinestra extraq = sqlFinestra (db);

            sqlFetch << "select count(*) as ntot from (" << sqlSelQuery << ")";
            extraq.setSelectQuery (sqlFetch.str ());
            recordTotalCount = atol (extraq.getCurrentValue ("ntot").c_str ());
            TRACE (("obtain totalRecords %ld", recordTotalCount));
         }
      }
      else
      {
         // check is it still consistent
         //assert (recordTotalCount >=  recordOffset + recordOffset - 1)
         if (recordTotalCount < recordOffset + recordOffset - 1)
         {
            TRACE (("adjusted wrong totalRecords %ld to %ld", recordTotalCount, recordOffset + recordOffset - 1));
            recordTotalCount = recordOffset + recordOffset - 1;
         }
      }
      if (currentCursor < minRecordNr ())
      {
         TRACE_ERR (("adjusted wrong currentCursor %ld to %ld", currentCursor, minRecordNr ()));
         currentCursor = minRecordNr ();
         return false;
      }
      if (currentCursor > maxRecordNr ())
      {
         TRACE_ERR (("adjusted wrong currentCursor %ld to %ld", currentCursor, maxRecordNr ()));
         currentCursor = maxRecordNr ();
         return false;
      }
      return true;
   }

protected:

   long min (long a, long b) { return a < b ? a: b; }

   long minRecordNr () { return recordOffset ; }
   long maxRecordNr () { return min (recordOffset + MAX_CACHE_ROWS - 1, recordTotalCount) ; }

   string defaultDBName = "defetuosa.db";
   bool openByMe = false;
   sqlite3 * db = 0;

   string dbName;
   string sqlSelQuery;
   Eva dataBuffer;
   long indxBuffer = 0;  // last assigned index in dataBuffer

   long currentCursor = 0;
   long recordOffset = 0;
   long recordTotalCount = -1;

   // called by static_execute_callback on each record
   //
   void addRecord (int ncols, char ** colValues, char ** colNames)
   {
      if (indxBuffer == 0)
      {
         for (int cc = 0; cc < ncols; cc ++)
            dataBuffer[0][cc] = colNames[cc];
      }
      indxBuffer ++;

      if (indxBuffer > MAX_CACHE_ROWS + 1)
      {
         // it should never happen!
         TRACE_ERR (("sqlFinestra ERROR: indxBuffer %ld to big!", indxBuffer));
         return;
      }

      for (int cc = 0; cc < ncols; cc ++)
         dataBuffer[indxBuffer][cc] = colValues[cc];
   }

   static int static_execute_callback (void * obj, int ncols, char ** colValues, char ** colNames)
   {
      reinterpret_cast<sqlFinestra *> (obj) -> addRecord (ncols, colValues, colNames);
      return 0;
   }

};

#endif

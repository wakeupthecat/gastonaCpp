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

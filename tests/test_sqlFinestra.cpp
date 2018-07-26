//========== test_sqlFinestra.cpp

#include <stdio.h>
#include "sqlFinestra.h"

void prova (long cachesize)
{
   printf ("\ntest with max cache size %ld\n\n", cachesize);
   sqlFinestra sq ("mytest.db", cachesize);

   sq.executeSQL ("CREATE TABLE IF NOT EXISTS prova (id, nom); DELETE FROM prova;");
   sq.executeSQL ("INSERT INTO prova VALUES (1, 'marla');"
                  "INSERT INTO prova VALUES (2, 'alea');"
                  "INSERT INTO prova VALUES (3, 'marvin');"
                  "INSERT INTO prova VALUES (4, 'abel');"
                 );

   sq.setSelectQuery ("SELECT * FROM prova ORDER BY nom");
   vector<string> cols = sq.getColumns ();

   printf ("result %d columns x %ld records\n", cols.size (), sq.getCountRecords ());
   for (long cc = 0; cc < cols.size (); cc++)
      printf ("%s, ", cols[cc].c_str ());
   printf ("\n");

   for (long rr = 1; rr <= sq.getCountRecords (); rr ++)
   {
      for (long cc = 0; cc < cols.size (); cc++)
         printf ("%s, ", sq.getValue (rr, cc).c_str ());
      printf ("\n");
   }

   printf ("Inverse order ...\n");
   for (long rr = sq.getCountRecords (); rr > 0; rr --)
   {
      for (long cc = 0; cc < cols.size (); cc++)
         printf ("%s, ", sq.getValue (rr, cc).c_str ());
      printf ("\n");
   }
   printf ("fi\n");
}

int main (int nn, char ** aa)
{
   prova (10);
   prova (4);
   prova (3);
   getc (stdin);
   prova (1);
}

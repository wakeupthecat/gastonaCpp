/*
project gastonaCpp
Copyright (C) 2018 Alejandro Xalabarder Aulet

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef utilTree_H
#define utilTree_H

#include <sstream>
#include <string>

#include "EvaUnit.h"
#include "EBSTree.h"
#include "eva2TreeStruct.h"

using namespace std;

/*
   Esta versio'n carga un arbol de una eva
   p.e.

      <arbol separator>  "/"

      <arbol dbName>     aristothala.db
      <arbol sqlSelect>  //SELECT * FROM myTree ;

      <arbol baseImagen> images/miDesto/Node

      <arbol shortPath>  1
      <arbol rootTitle>   Broot

      <arbol expanded>
         indx, path

      <arbol selected>
         indx, path

      <arbol>
         patho, more, icon, desc

         arbre/branca/fulla     , anymore, 101, esto es un bonito node
         arbre/branca/segona    , anymore, 111, esto es un bonito node
         arbre/tronc/fi         , anymore, 110, esto es un bonito node
*/
class utilTree
{
   public:

   // simple split by a single separator
   // no escaping or quoting of the separator possible
   static vector<string> splitStr (const string & phrase, char separator)
   {
      stringstream line (phrase);
      vector<string> sal;
      string part;
      while(std::getline(line, part, separator))
         sal.push_back(part);
      return sal;
   }


   // private static logger log = new logger (new utilTree(), "javaj.widgets.tree.utilTree", null);

   static eva2TreeStruct sierra (EBSTree & model)
   {
      string sqlQuery  = model.getSqlSelect ();
      return sqlQuery.length () == 0 ? sierraFromEva (model): sierraFromDB (model);
   }

   static eva2TreeStruct sierraFromEva (EBSTree & model)
   {
      Eva    evaTable  = model.getAttribute ("", true);
      string separ     = model.getSeparator ();
      string dataColumn = model.getDataColumn ();

      Eva Eorigen = evaTable;
      eva2TreeStruct Etarget = eva2TreeStruct ("TIS(tree intern structure)", separ.length () > 0 ? separ[0]: '/');

      int colIndx = Eorigen[0].indexOf (dataColumn);
      if (colIndx == -1)
          colIndx = 0;

      int iconIndx = Eorigen[0].indexOf ("icon");

      vector<string> old;
      for (int ii = 1; ii < Eorigen.rows (); ii ++)
      {
         old = cutAndAddPath (Eorigen.getValue (ii, colIndx), Eorigen.getValue (ii, iconIndx), Etarget, old);
      }
//      System.out.println ("HE ASSERRAO");
//      System.out.println (Etarget.eva);
//      System.out.println ("---------------HE ASSERRAO");
      return Etarget;
   }

   static eva2TreeStruct sierraFromDB (EBSTree & model)
   {
      //  "PATH TO TREE", "evaTarget", "separ", "SQL", "database", "fieldName", "sqlSelectQuery"

      //!!  //String databaseName = model.getDbName ();
      //!!  String dataColumn    = model.getDataColumn ();
      //!!  //String sqlSelect    = model.getSqlSelect ();
      //!!  String separ        = model.getSeparator ();
      //!!  eva2TreeStruct Etarget = new eva2TreeStruct ("TIS(tree intern structure)");
      //!!
      //!!  de.elxala.db.sqlite.tableROSelect roSel = new de.elxala.db.sqlite.tableROSelect (model);
      //!!  // alternatively ...
      //!!  // tableROSelect roSel = listix.table.roSqlPool.get (model);
      //!!
      //!!  int colIndx = roSel.getColumnIndex (dataColumn);
      //!!  if (colIndx == -1)
      //!!      colIndx = 0;
      //!!
      //!!  int iconIndx = roSel.getColumnIndex ("icon");
      //!!
      //!!  String [] old = null;
      //!!  String iconStr = "";
      //!!  for (int ii = 0; ii < roSel.getRecordCount (); ii ++)
      //!!  {
      //!!     iconStr = (iconIndx == -1) ? "": roSel.getValue (ii, iconIndx);
      //!!     old = cutAndAddPath (roSel.getValue (ii, colIndx), iconStr, Etarget, old);
      //!!  }
      //!!  return Etarget;
      return eva2TreeStruct("na", '?');
   }

   /**
      cut the 'fullPath' acording to the separator 'separ' and places
      the nodes in the zTree way into the target eva 'target'
      returns the current node array to be used as previous record for the next
      cut

      from:
         base/folder/file1
         base/folder/file2
         base/folder/sub1/file3
         base/end

      to:
         base  , folder  , file1
               ,         , file2
               ,         , sub1   , file3
               , end

       detecting problems like

         base/subdir
         base/subdir/file1

       Error: previous node to "base/folder/file1" was a folder!

       or

         base/fo1/file1
         base/fo2/sub1/file2
         base/fo3/sub1/file3

         base  , fo1 , file1
               , fo2 , sub1   , file2
               , fo3 , sub1   , file3

   */
   static vector<string> cutAndAddPath (const string & fullPath, const string & iconStr, eva2TreeStruct & target, vector<string> & prevRecord)
   {
      vector <string> arr = splitStr (fullPath, target.separator);
      int rowAdd = target.countRows ();

      // add icon info
      target.setIcon (iconStr, rowAdd);

      bool stillIdentic = prevRecord.size () > 0;
      for (int cc = 0; cc < arr.size (); cc ++)
      {
         // check if item is identical to previous record
         if (stillIdentic && (cc >= prevRecord.size () || prevRecord[cc] != arr[cc]))
            stillIdentic = false;

         // Como en la estructura eva para los nodos NO tenemos contemplado el nodo vacío ("")
         // tenemos que ponerle un nombre, y le ponemos el nombre separador
         //
         string contents = (cc == 0 && fullPath.length () > 0 && fullPath[0] == target.separator) ?
                           string ("" + target.separator):
                           arr[cc];

         target.setNode (stillIdentic ? "": contents, rowAdd, cc);
      }

      return arr;
   }
};

#endif

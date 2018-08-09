
#ifndef _eva2TreeStruct_H_
#define _eva2TreeStruct_H_


#include <string>
#include "Eva.h"

using namespace std;

//
//  structure to handle tree nodes separated into an eva variable
//  plus icon and a separator for all nodes
//
//  for example
//       <myname>
//          icon1, tronc, branca, fulla
//          icon2,      ,       , segona
//          icon2,      , soBranca, etc
//
class eva2TreeStruct // extends Eva
{
   public:

   Eva eva;
   char separator;

   enum {
       IMG_OFFSET = 1,   // offset due to image information at column 0
   };

   eva2TreeStruct(const string & name = "", const char separ = '/'): eva(name), separator(separ) {}

   int countSubNodes (int row)
   {
      int cc = eva.cols (row);
      return (cc == 0) ? 0: cc - IMG_OFFSET;
   }

   bool isLeaf (int row, int col)
   {
      bool isLastColumn = (col == countSubNodes (row) - 1);
      bool isLastRow = (row == eva.rows () - 1);

      if (isLastRow)
         return isLastColumn;

      // it is node if it is in the last column AND the next node has (EITHER less columns or the column equivalent is not "")

      // row=R col=3 ("branca") is a leaf because has the next element is at a level with less columns
      //                     0         1      2      3      4
      //      row=R)      img/node,        ,     , branca
      //      row=R+1)    img/node,        , hoja
      if (eva[row+1].cols () < eva[row].cols ())
         return true;

      // row=R col=3 ("branca") is not a leaf because has a child ("hoja")
      //                     0         1      2      3      4
      //      row=R)      img/node,        ,     , branca
      //      row=R+1)    img/node,        ,     ,       , hoja
      //
      // row=R col=3 ("branca") is a leaf because has the next element is a 'brother'
      //                     0         1      2      3      4
      //      row=R)      img/node,        ,     , branca
      //      row=R+1)    img/node,        ,     , hoja
      return eva[row+1][col+IMG_OFFSET].length () > 0;
   }

   void setIcon (const string & iconName, int row)
   {
      eva.setValue (iconName, row, IMG_OFFSET-1);
   }

   void setNode (const string & content, int row, int col)
   {
      eva.setValue (content, row, col + IMG_OFFSET);
   }

   int countRows ()
   {
      return eva.rows ();
   }

   string & getSubNode (int row, int col)
   {
      // permitir llamada con -1!
      if (col < 0)
      {
         static string buit;
         return buit;
      }
      return eva[row][col + IMG_OFFSET];
   }

   string & getIconName (int row)
   {
      return eva[row][IMG_OFFSET-1];
   }

   // get the node key for a node
   //
   //    example:
   //       <myname>
   //          icon1, tronc, branca, fulla
   //          icon2,      ,       , segona
   //          icon2,      , soBranca, etc
   //
   //    getNodeKey(1) will return "tronc/branca/segona" if separator was '/'
   //
   string getNodeKey (int row)
   {
      string sal = "";
      for (int cc = 1; cc < eva[row].cols (); cc ++)
      {
         int sin = row;
         while (sin > 0 && eva[sin][cc].length () == 0) sin --;
         if (cc > 1)
            sal += separator;
         sal += eva[sin][cc];
      }
      return sal;
   }
};

#endif

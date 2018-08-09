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

#ifndef _EBSTree_H_
#define _EBSTree_H_


class EBS
{
public:
   string widname;
   EvaUnit & dataAndCtrl;

   EBS (const string & objname, EvaUnit & dataAndControl):
      dataAndCtrl (dataAndControl),
      widname (objname)
   {
   }

   string varname (const string & attname)
   {
      if (attname.length () == 0)
         return widname;
      return widname + " " + attname;
   }

   string getSimpleAttribute (const string & attname, const string & defaultVal)
   {
      string varn = varname (attname);
      if (dataAndCtrl.containsEva (varn))
         return dataAndCtrl[varn][0][0];
      return defaultVal;
   }

   void setSimpleAttribute (const string & attname, const string & value)
   {
      string varn = varname (attname);
      if (!dataAndCtrl.containsEva (varn))
         dataAndCtrl.addEva (Eva (varn));

      dataAndCtrl[varn].setValue (value, 0, 0);
   }

   bool getBool (const string & attname, bool defaultval)
   {
      return getSimpleAttribute (attname, defaultval ? "1": "0") == "1";
   }

   void setBool (const string & attname, bool value)
   {
      setSimpleAttribute (attname, value ? "1": "0");
   }

   Eva & getAttribute (const string & attname, bool forcecreation = false)
   {
      string varn = varname (attname);
      if (!dataAndCtrl.containsEva (varn) && forcecreation)
         dataAndCtrl.addEva (Eva (varn));

      return dataAndCtrl[varn];
   }

};

/**
  EvaBasedStructure (EBS) for zWidgets of type tree

  Attributes:

      "visible"    Value 0 to make the widget not visible
      "enabled"    Value 0 to disable the widget
      "separator"    Character to be used as node separator (or path separator)
      "dataColumn"   Column name of the data that contains the tree path info (if not given column 0 will be take)
      "shortPath"    if 1 (true) the nodes with just one child are grouped using the nodeSeparator thus reducing nodes
      "rootTitle"    string to be used as label of the root node in the tree but not to build the fullpath
      "expandedNodes"    list of nodes that are expanded
      "selectedNodes"    table of one column "node" where the selected nodes are set
      "folderIcons"    list of icon names for the folders (general, root0, root1, root2). Note that here applies the same rules as for leaf nodes using baseIcons and endIcons
      "dbName"    If specified then it is the database name from which the data will be retrieved (see defaultDbName)
      "sqlSelect"    If specified then the data will be retrieved from a database (see dbName)
      "sqlExtraFilter"    Filter that is added to the query
      "droppedFiles"    Table with all dropped files
      "baseIcons"    Base part of the path where the Icon for this record is to be found, the rest is found in the field "icon" and the attribute "extensionIcons". Note that this attribute will be applied to all records
      "endIcons"    End of of the path where the Icon for this node is to be found. Note that this attribute will be applied to all nodes
      "disableIcons"    Disable custom Icons for this tree (it might be more performant for big trees)

  Messages:

*/
class EBSTree: public EBS
{
   typedef EBS super;

   public:

   // Attributes
   //
   static const char * ATTR_VISIBLE        () { return "visible";         }
   static const char * ATTR_ENABLED        () { return "enabled";         }
   static const char * ATTR_SEPARATOR      () { return "separator";       }
   static const char * ATTR_DATACOLUMN     () { return "dataColumn";      }
   static const char * ATTR_SHORTPATH      () { return "shortPath";       }
   static const char * ATTR_ROOTTITLE      () { return "rootTitle";       }
   static const char * ATTR_EXPANDEDNODES  () { return "expandedNodes";   }
   static const char * ATTR_SELECTEDNODES  () { return "selectedNodes";   }
   static const char * ATTR_FOLDERICONS    () { return "folderIcons";     }
   static const char * ATTR_DBNAME         () { return "dbName";          }
   static const char * ATTR_SQLSELECT      () { return "sqlSelect";       }
   static const char * ATTR_SQLEXTRAFILTER () { return "sqlExtraFilter";  }
   static const char * ATTR_DROPPEDFILES   () { return "droppedFiles";    }
   static const char * ATTR_BASEICONS      () { return "baseIcons";       }
   static const char * ATTR_ENDICONS       () { return "endIcons";        }
   static const char * ATTR_DISABLEICONS   () { return "disableIcons";    }

   // Messages
   //


   // Other variables
   //
   EBSTree (const string & nameWidget, EvaUnit & pDataAndControl):
      super (nameWidget, pDataAndControl)
   {
   }

   bool getIsVisible ()      {   return getBool (ATTR_VISIBLE (), true);  }
   bool getIsEnabled ()      {   return getBool (ATTR_ENABLED (), true);  }
   bool getIsShortPath ()    {   return getBool (ATTR_SHORTPATH (), true);  }
   bool getIsDisableIcons () {   return getBool (ATTR_DISABLEICONS (), true);  }

   void setIsVisible   (bool value)     {  setBool (ATTR_VISIBLE (), value);   }
   void setIsEnabled   (bool value)     {  setBool (ATTR_ENABLED (), value);   }
   void setIsShortPath (bool value)     {  setBool (ATTR_SHORTPATH (), value);   }
   void setIsDisableIcons (bool value)  {  setBool (ATTR_DISABLEICONS (), value);   }

   /// shortPath: if 1 (true) the nodes with just one child are grouped using the nodeSeparator thus reducing nodes

   string getSeparator      ()  { return getSimpleAttribute (ATTR_SEPARATOR (), "/");   }
   string getDataColumn     ()  { return getSimpleAttribute (ATTR_DATACOLUMN (), "");   }
   string getRootTitle      ()  { return getSimpleAttribute (ATTR_ROOTTITLE (), "");   }
   string getDbName         ()  { return getSimpleAttribute (ATTR_DBNAME (), "");   }
   string getSqlSelect      ()  { return getSimpleAttribute (ATTR_SQLSELECT (), "");   }
   string getSqlExtraFilter ()  { return getSimpleAttribute (ATTR_SQLEXTRAFILTER (), "");   }
   string getBaseIcons      ()  { return getSimpleAttribute (ATTR_BASEICONS (), "");   }
   string getEndIcons       ()  { return getSimpleAttribute (ATTR_ENDICONS (), ".png");   }

   void setSeparator      (const string & value)  {  setSimpleAttribute (ATTR_SEPARATOR (), value);  }
   void setDataColumn     (const string & value)  {  setSimpleAttribute (ATTR_DATACOLUMN (), value); }
   void setRootTitle      (const string & value)  {  setSimpleAttribute (ATTR_ROOTTITLE (), value); }
   void setDbName         (const string & value)  {  setSimpleAttribute (ATTR_DBNAME (), value); }
   void setSqlSelect      (const string & value)  {  setSimpleAttribute (ATTR_SQLSELECT (), value); }
   void setSqlExtraFilter (const string & value)  {  setSimpleAttribute (ATTR_SQLEXTRAFILTER (), value); }
   void setBaseIcons      (const string & value)  {  setSimpleAttribute (ATTR_BASEICONS (), value); }
   void setEndIcons       (const string & value)  {  setSimpleAttribute (ATTR_ENDICONS (), value); }

   /** getter for attribute expandedNodes: list of nodes that are expanded
       returns the Eva associated with this attribute or null if it does not exists.
       if 'forceCreation' is true no null will be returned but a new Eva will be created and returned.
       Note that this attribute has no setter. This is to avoid in many cases an ineffective
       copy from an eva to another. To perform an equivalent setExpandedNodes just get the eva
       using forceCreation = true (getExpandedNodes (true)) and then set the value directly on it.
   */
   Eva & getExpandedNodes (bool forceCreation = false)
   {
      return getAttribute (ATTR_EXPANDEDNODES (), forceCreation);
   }

   /** getter for attribute selectedNodes: table of one column "node" where the selected nodes are set
       returns the Eva associated with this attribute or null if it does not exists.
       if 'forceCreation' is true no null will be returned but a new Eva will be created and returned.
       Note that this attribute has no setter. This is to avoid in many cases an ineffective
       copy from an eva to another. To perform an equivalent setSelectedNodes just get the eva
       using forceCreation = true (getSelectedNodes (true)) and then set the value directly on it.
   */
   Eva & getSelectedNodes (bool forceCreation = false)
   {
      return getAttribute (ATTR_SELECTEDNODES (), forceCreation);
   }

   /** getter for attribute folderIcons: list of icon names for the folders (general, root0, root1, root2). Note that here applies the same rules as for leaf nodes using baseIcons and endIcons
       returns the Eva associated with this attribute or null if it does not exists.
       if 'forceCreation' is true no null will be returned but a new Eva will be created and returned.
       Note that this attribute has no setter. This is to avoid in many cases an ineffective
       copy from an eva to another. To perform an equivalent setFolderIcons just get the eva
       using forceCreation = true (getFolderIcons (true)) and then set the value directly on it.
   */
   Eva & getFolderIcons (bool forceCreation = false)
   {
      return getAttribute (ATTR_FOLDERICONS (), forceCreation);
   }

   /** getter for attribute droppedFiles: Table with all dropped files
       returns the Eva associated with this attribute or null if it does not exists.
       if 'forceCreation' is true no null will be returned but a new Eva will be created and returned.
       Note that this attribute has no setter. This is to avoid in many cases an ineffective
       copy from an eva to another. To perform an equivalent setDroppedFiles just get the eva
       using forceCreation = true (getDroppedFiles (true)) and then set the value directly on it.
   */
   Eva & getDroppedFiles (bool forceCreation = false)
   {
      return getAttribute (ATTR_DROPPEDFILES (), forceCreation);
   }
};

#endif

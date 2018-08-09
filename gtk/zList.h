/*
   doc at 
   https://en.wikibooks.org/wiki/GTK%2B_By_Example/Tree_View
   
   https://en.wikibooks.org/wiki/GTK%2B_By_Example/Tree_View/Custom_Models

*/

#ifndef zLIST_HEADER_H
#define zLIST_HEADER_H

#include <gtk/gtk.h>

#include "zWidget.h"
#include "Mensaka.h"
#include "uniRect.h"

using namespace std;

class zList : public zWidget, public MensakaTarget
{
private:

public:
   enum
   {
      MSK_UPDATE_DATA
   };

   enum {
     LIST_ITEM = 0,
     N_COLUMNS
   };
   
   zList (GtkWidget * hwinParent, const string & pname): zWidget(pname)
   {
      hwnd = gtk_tree_view_new ();
      
      gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(hwnd), FALSE);
      
      GtkCellRenderer * renderer = gtk_cell_renderer_text_new ();

      // ADD column (headers)
      //      
      GtkTreeViewColumn * column = 
                        gtk_tree_view_column_new_with_attributes (
                           pname.c_str (),
                           renderer,
                           "text",
                           LIST_ITEM,
                           NULL
                           );
      gtk_tree_view_append_column (GTK_TREE_VIEW(hwnd), column);

      // ADD store (model)
      //      
      GtkListStore * store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
                  
      gtk_tree_view_set_model(GTK_TREE_VIEW(hwnd), GTK_TREE_MODEL(store));


      // segun ejemplo.. (porque no se hace lo mismo con column o renderer ??)
      g_object_unref(store);
                  
      GtkTreeSelection * selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(hwnd));
      gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);

      gtk_signal_connect (GTK_OBJECT (hwnd), 
                          "clicked", 
                          GTK_SIGNAL_FUNC (zList::static_clicked_callback), 
                          this);
                          

      // ?? g_signal_connect(selection, "changed", G_CALLBACK(zList::static_clicked_callback), label);
                          
      Mensaka::subscribeToMessage (getName () + " data!", MSK_UPDATE_DATA, this);
   }

   ~zList ()
   {
   }

   struct indox { indox (int indx): index(indx) {}; int index; };

   uniRect getDefaultSize ()
   {
      return uniRect (0, 0, widthForText(getName ()), heightForText());
   }


   bool takePacket (int mappedMsg, EvaUnit & pk, const vector<string> & params)
   {
      if (mappedMsg == MSK_UPDATE_DATA)
      {
         printf ("mira var[%s] val[%s]\n", params.size () > 1 ? params[0].c_str ():"?", params.size () > 1 ? params[1].c_str ():"?");
         setVariablesFromArray (params);
         updateData ();
         return true;
      }
      return false;
   }
   
   void fillWidget (Eva & edata)
   {
      GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model (GTK_TREE_VIEW(hwnd)));
      GtkTreeIter iter;

      gtk_list_store_clear (store);

      Eva & data = getAttribute ("");
      for (int ii = 1; ii < data.rows (); ii ++)
      {
         gtk_list_store_append (store, &iter);
         iter.user_data3 = (gpointer) new indox(ii); // mi token
         gtk_list_store_set (store, &iter, LIST_ITEM, data[ii][0].c_str (), -1);
      }
   }
   

   void updateData ()
   {
      fillWidget (getAttribute (""));

      Eva & atip = getAttribute ("tooltip");
      if (not EvaUnit::isNILEva (atip))
           gtk_widget_set_tooltip_text(hwnd, atip.getValue ().c_str ());
   }
   

   void on_list_clicked ()
   {
      GtkTreeSelection * selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(hwnd));

      gtk_tree_selection_selected_foreach (selection, zList::elem_is_selected, (gpointer) this);

      Eva & data = getAttribute ("");
      Eva & atip = getAttribute ("subTableSelection", true);
      atip.clear ();
      atip.addLine (data[0]);

      // send mensaka with selection message (same as name)
      //
      Mensaka::sendPacket (getName (), getDataAndControl ());
   }
   
   void setSelected (int indx)
   {
      Eva & data = getAttribute ("");
      Eva & atip = getAttribute ("subTableSelection", true);
      if (data.rows () > indx) {
         atip.addLine (data[indx]);
         printf ("selectings [%s]\n", data[indx].toString ().c_str ());
      }
   }

   // callback to handle multi selection!
   static void elem_is_selected (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer obj)
   {
      reinterpret_cast<zList *> (obj) -> setSelected ((static_cast<indox *>(iter -> user_data3))->index);
   }

   // gtk callback, it has to be a static method
   // data will be the pointer to the object that 
   // has been actually clicked
   static void static_clicked_callback (GtkWidget * button, gpointer* data)
   {
      reinterpret_cast<zList *>(data) -> on_list_clicked ();
   }
};

#endif //  HEADER_H


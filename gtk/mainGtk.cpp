/*
 * mainGtk.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <gtk/gtk.h>

#include "GastonaGtk.h"

GastonaGtk gaston;
GtkWidget * mainFrame = 0;

gint on_delete_event (GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
    return (FALSE);
}

gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    return gaston.broadcastKey (widget, event, data);
}

gboolean on_configure_event (GtkWidget * widget, GdkEvent  *event, gpointer user_data)
{
   // class GdkEventConfigure
   // {
   //     height;
   //     send_event;
   //     type;
   //     width;
   //     window;
   //     x;
   //     y;
   // }
   if (widget == mainFrame)
   {
      GdkEventConfigure * econf = (GdkEventConfigure *) event;
      gaston.showLayout (econf->width, econf->height);
      return true;
   }
   return false;
}

// C main 
//
int main (int nn, char ** aa)
{
   /* --- GTK initialization --- */
   gtk_init (&nn, &aa);

   // Main frame
   //
   mainFrame = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_window_set_title (GTK_WINDOW (mainFrame), "Gastona_Gtk");
   gtk_widget_set_usize (mainFrame, 200, 200);
   gtk_signal_connect (GTK_OBJECT (mainFrame), "delete_event", GTK_SIGNAL_FUNC (on_delete_event), NULL);

   gtk_signal_connect (GTK_OBJECT (mainFrame), "key_press_event", GTK_SIGNAL_FUNC (on_key_press), NULL);

   GtkWidget * fixer = gtk_fixed_new ();
   gtk_container_add(GTK_CONTAINER(mainFrame), fixer);
    
   // in order to handle resize!
   gtk_widget_add_events(GTK_WIDGET(mainFrame), GDK_CONFIGURE);
   g_signal_connect (G_OBJECT(mainFrame), "configure-event", G_CALLBACK(on_configure_event), NULL);

   gaston.loadGUI (mainFrame, fixer, (nn > 1) ? aa[1]: "");
   gaston.showLayout (mainFrame);
   gtk_widget_show (mainFrame);

   gtk_main ();
   return (0);
}

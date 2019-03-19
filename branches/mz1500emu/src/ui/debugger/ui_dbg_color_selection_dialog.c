/* 
 * File:   ui_dbg_color_selection_dialog.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 19. března 2019, 9:56
 * 
 * 
 * ----------------------------- License -------------------------------------
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * ---------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdint.h>
#include <glib.h>
#include <gtk/gtk.h>


typedef struct st_CSBDATA {
    GtkWidget *dialog;
    int *reply;
    int value;
} st_CSBDATA;


static void on_ui_dbg_color_selection_button_clicked ( GtkButton *button, gpointer user_data ) {
    st_CSBDATA *csdata = user_data;
    *csdata->reply = csdata->value;
    gtk_widget_hide ( csdata->dialog );
}


static GtkWidget* ui_dbg_color_selection_create_button ( st_CSBDATA *csbdata, GdkRGBA rgba ) {
    GtkWidget *button = gtk_color_button_new_with_rgba ( &rgba );
    g_signal_connect ( G_OBJECT ( button ), "clicked",
                       G_CALLBACK ( on_ui_dbg_color_selection_button_clicked ), csbdata );
    return button;
}


int ui_dbg_color_selection_dialog ( const char *title, GdkRGBA *rgba, int count, GtkWindow *parent ) {

    int reply = -1;

    st_CSBDATA *csbdata = g_new ( st_CSBDATA, count );

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_dialog_new_with_buttons ( title,
                                                      parent,
                                                      flags,
                                                      "_Cancel",
                                                      GTK_RESPONSE_REJECT,
                                                      NULL );
    
    gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_MOUSE );

    GtkWidget *content_area = gtk_dialog_get_content_area ( GTK_DIALOG ( dialog ) );

    GtkWidget *grid = gtk_grid_new ( );

    int i;
    int t = 0;
    for ( i = 0; i < count; i++ ) {
        csbdata[i].reply = &reply;
        csbdata[i].value = i;
        csbdata[i].dialog = dialog;

        GtkWidget *button = ui_dbg_color_selection_create_button ( &csbdata[i], rgba[i] );
        int l = i % 8;
        t = i / 8;
        gtk_grid_attach ( GTK_GRID ( grid ), button, l, t, 1, 1 );
    };

    GtkWidget *sep = gtk_separator_new ( GTK_ORIENTATION_HORIZONTAL );
    gtk_widget_set_margin_top ( sep, 10 );
    gtk_widget_set_margin_bottom ( sep, 10 );

    gtk_grid_attach ( GTK_GRID ( grid ), sep, 0, ( t + 1 ), 8, 1 );

    gtk_container_add ( GTK_CONTAINER ( content_area ), grid );
    gtk_widget_show_all ( dialog );


    gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    gtk_widget_destroy ( dialog );
    g_free ( csbdata );

    return reply;
}

#if 0
#include "ui/ui_main.h"
#include "gdg/framebuffer_mz1500.h"
#include "display.h"


void ui_dbg_color_selection_dialog_test ( void ) {

    //GdkRGBA rgba[G_N_ELEMENTS ( c_MZ1500_COLORMAP )];
    GdkRGBA rgba[G_N_ELEMENTS ( g_display_predef_colors )];

    int i;
    for ( i = 0; i < G_N_ELEMENTS ( rgba ); i++ ) {
        //uint32_t color = g_display_predef_colors[c_MZ1500_COLORMAP[i]];
        uint32_t color = g_display_predef_colors[i];
        GString *s = g_string_new ( "" );
        g_string_append_printf ( s, "#%02x%02x%02x", ( color >> 16 ), ( ( color >> 8 ) & 0xff ), ( color & 0xff ) );
        gdk_rgba_parse ( &rgba[i], s->str );
        g_string_free ( s, TRUE );
    };

    int ret = ui_dbg_color_selection_dialog ( "Select PAL0 color", rgba, G_N_ELEMENTS ( rgba ), ui_get_window ( "main_window" ) );

    printf ( "Color: %d\n", ret );
}
#endif

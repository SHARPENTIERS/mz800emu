/* 
 * File:   ui_cmt.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. srpna 2015, 19:40
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

#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>

#include "ui_main.h"
#include "ui_cmt.h"

#include "cmt/cmt.h"
#include "cmt/cmt_extension.h"
#include "cmt/cmt_hack.h"

#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"
#include "libs/cmt_stream/cmt_stream.h"


typedef enum en_UICMT_SHOW {
    UICMT_SHOW_PLAY_TIME = 0,
    UICMT_SHOW_REMAINING_TIME,
} en_UICMT_SHOW;


typedef struct st_UICMT {
    st_UIWINPOS pos;
    en_UICMT_SHOW show_time;
} st_UICMT;

static st_UICMT g_uicmt;


void ui_cmt_init ( void ) {
    ui_main_setpos ( &g_uicmt.pos, -1, -1 );
    g_uicmt.show_time = UICMT_SHOW_REMAINING_TIME;
}


G_MODULE_EXPORT gboolean on_cmt_window_key_press_event ( GtkWidget *widget, GdkEventKey *event, gpointer user_data ) {
    if ( event->keyval == GDK_KEY_Escape ) {
        ui_cmt_window_show_hide ( );
        return TRUE;
    };
    return FALSE;
}


G_MODULE_EXPORT void on_menuitem_cmt_hack ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    if ( gtk_check_menu_item_get_active ( ui_get_check_menu_item ( "menuitem_cmt_hack" ) ) ) {
        cmthack_load_rom_patch ( 1 );
    } else {
        cmthack_load_rom_patch ( 0 );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_1200 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( CMT_SPEED_1200 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_2400 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( CMT_SPEED_2400 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_3600 ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( CMT_SPEED_3600 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_open_and_play ( GtkMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( EXIT_SUCCESS == cmt_open ( ) ) {
        cmt_play ( );
    };
}


void ui_cmt_window_show_hide ( void ) {
    GtkWidget *window = ui_get_widget ( "cmt_window" );
    if ( gtk_widget_get_visible ( window ) ) {
        ui_main_win_get_pos ( GTK_WINDOW ( window ), &g_uicmt.pos );
        gtk_widget_hide ( window );
    } else {
        gtk_widget_show ( window );
        ui_main_win_move_to_pos ( GTK_WINDOW ( window ), &g_uicmt.pos );
        ui_cmt_window_update ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_show_window ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;
#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif
    ui_cmt_window_show_hide ( );
}


void ui_cmt_hack_menu_update ( void ) {
    LOCK_UICALLBACKS ( );
    if ( g_cmthack.load_patch_installed ) {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_hack" ), TRUE );
    } else {
        gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_hack" ), FALSE );
    };
    UNLOCK_UICALLBACKS ( );
}


void ui_cmt_speed_menu_update ( void ) {

    LOCK_UICALLBACKS ( );

    if ( !TEST_CMT_STOP ) {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_cmt_speed" ), FALSE );
    } else {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_cmt_speed" ), TRUE );
    };

    switch ( g_cmt.speed ) {
        case CMT_SPEED_1200:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_1200" ), TRUE );
            break;
        case CMT_SPEED_2400:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_2400" ), TRUE );
            break;
        case CMT_SPEED_3600:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_3600" ), TRUE );
            break;
    };

    UNLOCK_UICALLBACKS ( );
}


G_MODULE_EXPORT gboolean on_cmt_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer data ) {
    (void) widget;
    (void) event;
    (void) data;

    ui_cmt_window_show_hide ( );
    return TRUE;
}


G_MODULE_EXPORT void on_cmt_hide_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    ui_cmt_window_show_hide ( );
}


G_MODULE_EXPORT void on_cmt_stop_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    cmt_stop ( );
}


G_MODULE_EXPORT void on_cmt_record_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_play_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    cmt_play ( );
}


G_MODULE_EXPORT void on_cmt_pause_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_open_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    cmt_open ( );
}


G_MODULE_EXPORT void on_cmt_eject_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    cmt_eject ( );
}


G_MODULE_EXPORT void on_cmt_speed_comboboxtext_changed ( GtkComboBox *combobox, gpointer data ) {
    //(void) combobox;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

    LOCK_UICALLBACKS ( );
    cmt_change_speed ( gtk_combo_box_get_active ( combobox ) );
    UNLOCK_UICALLBACKS ( );
}


static void ui_cmt_show_items_by_extension ( en_CMT_EXTENSION_NAME extname ) {
    switch ( extname ) {
        case CMT_EXTENSION_NAME_MZF:
            gtk_widget_show ( ui_get_widget ( "cmt_mzf_info_table" ) );
            gtk_widget_hide ( ui_get_widget ( "cmt_stream_info_table" ) );
            gtk_widget_show ( ui_get_widget ( "cmt_speed_label" ) );
            gtk_widget_show ( ui_get_widget ( "cmt_speed_comboboxtext" ) );
            break;
        case CMT_EXTENSION_NAME_WAV:
            gtk_widget_hide ( ui_get_widget ( "cmt_mzf_info_table" ) );
            gtk_widget_show ( ui_get_widget ( "cmt_stream_info_table" ) );
            gtk_widget_hide ( ui_get_widget ( "cmt_speed_label" ) );
            gtk_widget_hide ( ui_get_widget ( "cmt_speed_comboboxtext" ) );
            break;
    };
}


void ui_cmt_window_update ( void ) {

    static en_CMT_EXTENSION_NAME extname = CMT_EXTENSION_NAME_MZF;

    if ( !TEST_CMT_FILLED ) {

        ui_cmt_show_items_by_extension ( extname );

        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), TRUE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), FALSE );

        gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), "*** Empty ***" );

        gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfsize_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfexec_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_mzfstart_label" ), "--" );

        gtk_label_set_text ( ui_get_label ( "cmt_stream_source_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_stream_rate_label" ), "--" );

        gtk_label_set_markup ( ui_get_label ( "cmt_play_button_label" ), "Play" );

    } else {

        en_CMT_EXTENSION_NAME extname = g_cmt.ext->get_name ( );
        ui_cmt_show_items_by_extension ( extname );

        switch ( extname ) {
            case CMT_EXTENSION_NAME_MZF:
            {
                g_assert ( g_cmt.ext->get_mzfheader != NULL );

                if ( g_cmt.ext->get_mzfheader != NULL ) {
                    st_MZF_HEADER *hdr = g_cmt.ext->get_mzfheader ( );

                    char buff [ 100 ];

                    sprintf ( buff, "%02x", hdr->ftype );
                    gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), buff );

                    mzf_tools_get_fname ( hdr, (char*) &buff );
                    gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), buff );

                    sprintf ( buff, "0x%04x", hdr->fsize );
                    gtk_label_set_text ( ui_get_label ( "cmt_mzfsize_label" ), buff );

                    sprintf ( buff, "0x%04x", hdr->fexec );
                    gtk_label_set_text ( ui_get_label ( "cmt_mzfexec_label" ), buff );

                    sprintf ( buff, "0x%04x", hdr->fstrt );
                    gtk_label_set_text ( ui_get_label ( "cmt_mzfstart_label" ), buff );
                };
                break;
            }

            case CMT_EXTENSION_NAME_WAV:
            {
                char buff [ 100 ];

                st_CMT_STREAM *cmt_stream = g_cmt.ext->get_stream ( );

                sprintf ( buff, "%d Hz", cmt_stream->rate );
                gtk_label_set_text ( ui_get_label ( "cmt_stream_rate_label" ), buff );
                gtk_label_set_text ( ui_get_label ( "cmt_stream_source_label" ), "WAV" );
                break;
            }
        };

        gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), g_cmt.ext->get_playfile_name ( ) );

        if ( TEST_CMT_STOP ) {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), TRUE );
            gtk_label_set_markup ( ui_get_label ( "cmt_play_button_label" ), "Play" );
        } else {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), FALSE );
            gtk_label_set_markup ( ui_get_label ( "cmt_play_button_label" ), "<b>Play</b>" );
        };
    };

    ui_cmt_update_player ( );

    LOCK_UICALLBACKS ( );
    gtk_combo_box_set_active ( ui_get_combo_box ( "cmt_speed_comboboxtext" ), g_cmt.speed );
    UNLOCK_UICALLBACKS ( );

    ui_cmt_speed_menu_update ( );
}


void ui_cmt_update_player ( void ) {

    gdouble total_time = 0;
    gdouble play_time = 0;
    gdouble fraction = 0;
    guint32 print_time;

    if ( TEST_CMT_FILLED ) {
        st_CMT_STREAM *cmt_stream = g_cmt.ext->get_stream ( );
        total_time = ( cmt_stream->scan_time * cmt_stream->scans );
        if ( !TEST_CMT_STOP ) {
            play_time = cmt_get_playtime ( );
            fraction = ( play_time / total_time );
        };
    };

    char *minus = "";

    if ( g_uicmt.show_time == UICMT_SHOW_REMAINING_TIME ) {
        print_time = ( total_time - play_time );
        if ( ( total_time - play_time ) > print_time ) {
            print_time++;
        };
        minus = "-";
    } else {
        print_time = play_time;
    };

    char buff [ 100 ];

    sprintf ( buff, "<b><span font='50'>%s%02d:%02d</span></b>", minus, ( print_time / 60 ), ( print_time % 60 ) );
    gtk_label_set_markup ( ui_get_label ( "cmt_time_label" ), buff );

    gtk_progress_bar_set_fraction ( ui_get_progress_bar ( "cmt_progressbar" ), fraction );

}


void ui_cmt_set_filename ( char *filename ) {
    gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), filename );
}


void ui_cmt_show_time_changed ( void ) {
    if ( g_uicmt.show_time == UICMT_SHOW_REMAINING_TIME ) {
        g_uicmt.show_time = UICMT_SHOW_PLAY_TIME;
        gtk_label_set_text ( ui_get_label ( "cmt_time_info_label" ), "Play time:" );
    } else {
        g_uicmt.show_time = UICMT_SHOW_REMAINING_TIME;
        gtk_label_set_text ( ui_get_label ( "cmt_time_info_label" ), "Remaining time:" );
    };

    ui_cmt_update_player ( );
}


G_MODULE_EXPORT gboolean on_cmt_time_info_eventbox_button_press_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_cmt_show_time_changed ( );
    return FALSE;
}


G_MODULE_EXPORT gboolean on_cmt_time_eventbox_button_press_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_cmt_show_time_changed ( );
    return FALSE;
}

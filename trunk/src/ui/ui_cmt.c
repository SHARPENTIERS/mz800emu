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
#include "cmt/cmthack.h"
#include "cmt/cmtext.h"
#include "cmt/cmtext_block.h"
#include "cmt/cmtext_container.h"
#include "cmt/cmt_mzf.h"

#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"

#include "libs/mztape/cmtspeed.h"
#include "cmt/cmt_tap.h"


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


G_MODULE_EXPORT void on_menuitem_cmt_speed_1_1_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( CMTSPEED_1_1 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_2_1_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( CMTSPEED_2_1 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_7_3_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( CMTSPEED_7_3 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_8_3_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( CMTSPEED_8_3 );
        UNLOCK_UICALLBACKS ( );
    };
}


G_MODULE_EXPORT void on_menuitem_cmt_speed_3_1_toggled ( GtkCheckMenuItem *menuitem, gpointer data ) {
    (void) menuitem;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

#ifdef UI_TOPMENU_IS_WINDOW
    ui_hide_main_menu ( );
#endif

    if ( gtk_check_menu_item_get_active ( menuitem ) ) {
        LOCK_UICALLBACKS ( );
        cmt_change_speed ( CMTSPEED_3_1 );
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
        ui_cmt_tape_index_hide ( );
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

    switch ( g_cmt.mz_cmtspeed ) {
        case CMTSPEED_1_1:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_1_1" ), TRUE );
            break;
        case CMTSPEED_2_1:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_2_1" ), TRUE );
            break;
        case CMTSPEED_7_3:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_7_3" ), TRUE );
            break;
        case CMTSPEED_8_3:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_8_3" ), TRUE );
            break;
        case CMTSPEED_3_1:
            gtk_check_menu_item_set_active ( ui_get_check_menu_item ( "menuitem_cmt_speed_3_1" ), TRUE );
            break;
        case CMTSPEED_NONE:
        case CMTSPEED_3_2:
        case CMTSPEED_9_7:
        case CMTSPEED_25_14:
            fprintf ( stderr, "%s():%d - Unsoported mz_cmtspeed (%d)\n", __func__, __LINE__, g_cmt.mz_cmtspeed );
    };

    UNLOCK_UICALLBACKS ( );
}


void ui_cmt_polarity_menu_update ( void ) {
    if ( !TEST_CMT_STOP ) {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_dip_switch_cmt_inverted_polarity" ), FALSE );
    } else {
        gtk_widget_set_sensitive ( ui_get_widget ( "menuitem_dip_switch_cmt_inverted_polarity" ), TRUE );
    };
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


G_MODULE_EXPORT void on_cmt_record_togglebutton_toggled ( GtkToggleButton *togglebutton, gpointer data ) {
    (void) togglebutton;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_play_togglebutton_toggled ( GtkToggleButton *togglebutton, gpointer data ) {
    (void) togglebutton;
    (void) data;

    if ( ( gtk_toggle_button_get_active ( togglebutton ) ) && ( TEST_CMT_STOP ) ) {
        cmt_play ( );
    };
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


G_MODULE_EXPORT void on_cmt_previous_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    assert ( g_cmt.ext->container->cb_previous_block );
    if ( !g_cmt.ext->container->cb_previous_block ) return;

    gboolean fl_play = TEST_CMT_PLAY;
    if ( fl_play ) cmt_stop ( );

    if ( EXIT_FAILURE == g_cmt.ext->container->cb_previous_block ( ) ) return;

    if ( fl_play ) {
        cmt_play ( );
    } else {
        ui_cmt_window_update ( );
    };
}


G_MODULE_EXPORT void on_cmt_next_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    assert ( g_cmt.ext->container->cb_next_block );
    if ( !g_cmt.ext->container->cb_next_block ) return;

    gboolean fl_play = TEST_CMT_PLAY;
    if ( fl_play ) cmt_stop ( );

    if ( EXIT_FAILURE == g_cmt.ext->container->cb_next_block ( ) ) return;

    if ( fl_play ) {
        cmt_play ( );
    } else {
        ui_cmt_window_update ( );
    };
}


G_MODULE_EXPORT void on_cmt_rewind_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_forward_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    printf ( "%s()\n", __func__ );
}


G_MODULE_EXPORT void on_cmt_speed_comboboxtext_changed ( GtkComboBox *combobox, gpointer data ) {
    //(void) combobox;
    (void) data;

    if ( TEST_UICALLBACKS_LOCKED ) return;

    int mztape_speed_id = gtk_combo_box_get_active ( combobox );
    LOCK_UICALLBACKS ( );
    cmt_change_speed ( g_mztape_speed[mztape_speed_id] );
    UNLOCK_UICALLBACKS ( );
}


void ui_cmt_window_update ( void ) {

    GtkWidget *window = ui_get_widget ( "cmt_window" );
    if ( !gtk_widget_get_visible ( window ) ) {
        ui_cmt_speed_menu_update ( );
        ui_cmt_polarity_menu_update ( );
        return;
    };

    char buff [ 100 ];

    int total_blocks = 0;
    int play_block = 0;
    st_CMTEXT_CONTAINER *container;

    if ( TEST_CMT_FILLED ) {
        container = cmtext_get_container ( g_cmt.ext );
        total_blocks = cmtext_container_get_count_blocks ( container );
        play_block = cmtext_block_get_block_id ( g_cmt.ext->block ) + 1;
    };

    // Tape info
    if ( total_blocks == 0 ) {
        gtk_label_set_text ( ui_get_label ( "cmt_play_block_label" ), "--" );
        gtk_label_set_text ( ui_get_label ( "cmt_total_blocks_label" ), "--" );
    } else {
        snprintf ( buff, sizeof (buff ), "%02d", play_block );
        gtk_label_set_text ( ui_get_label ( "cmt_play_block_label" ), buff );
        snprintf ( buff, sizeof (buff ), "%02d", total_blocks );
        gtk_label_set_text ( ui_get_label ( "cmt_total_blocks_label" ), buff );
    };


    gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), "--" );
    gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), "--" );
    gtk_label_set_text ( ui_get_label ( "cmt_fsize_label" ), "--" );
    gtk_label_set_text ( ui_get_label ( "cmt_fexec_label" ), "--" );
    gtk_label_set_text ( ui_get_label ( "cmt_fstart_label" ), "--" );
    gtk_label_set_text ( ui_get_label ( "cmt_file_speed_label" ), "--" );
    gtk_label_set_text ( ui_get_label ( "cmt_stream_source_label" ), "--" );
    gtk_label_set_text ( ui_get_label ( "cmt_stream_rate_label" ), "--" );


    if ( ( TEST_CMT_FILLED ) && ( g_cmt.playsts != CMTEXT_BLOCK_PLAYSTS_PAUSE ) ) {


        uint16_t file_bdspeed = 0;
        st_MZF_HEADER *mzfhdr = NULL;
        st_CMTEXT_TAPE_ITEM_TAPHDR *taphdr = NULL;
        st_CMTEXT_TAPE_ITEM_TAPDATA *tapdata = NULL;

        switch ( cmtext_block_get_type ( g_cmt.ext->block ) ) {
            case CMTEXT_BLOCK_TYPE_MZF:
                file_bdspeed = ( !g_cmt.ext->block->cb_get_bdspeed ) ? 0 : g_cmt.ext->block->cb_get_bdspeed ( g_cmt.ext );
                mzfhdr = cmtmzf_block_get_spec_mzfheader ( g_cmt.ext->block );
                g_assert ( mzfhdr != NULL );

                snprintf ( buff, sizeof (buff ), "0x%02x", mzfhdr->ftype );
                gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), buff );

                mzf_tools_get_fname ( mzfhdr, (char*) &buff );
                gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), buff );

                snprintf ( buff, sizeof (buff ), "0x%04x", mzfhdr->fsize );
                gtk_label_set_text ( ui_get_label ( "cmt_fsize_label" ), buff );

                snprintf ( buff, sizeof (buff ), "0x%04x", mzfhdr->fexec );
                gtk_label_set_text ( ui_get_label ( "cmt_fexec_label" ), buff );

                snprintf ( buff, sizeof (buff ), "0x%04x", mzfhdr->fstrt );
                gtk_label_set_text ( ui_get_label ( "cmt_fstart_label" ), buff );

                snprintf ( buff, sizeof (buff ), "%d Bd", file_bdspeed );
                gtk_label_set_text ( ui_get_label ( "cmt_file_speed_label" ), buff );
                break;

            case CMTEXT_BLOCK_TYPE_WAV:
                gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), "WAV" );
                break;

            case CMTEXT_BLOCK_TYPE_TAPHEADER:
                file_bdspeed = ( !g_cmt.ext->block->cb_get_bdspeed ) ? 0 : g_cmt.ext->block->cb_get_bdspeed ( g_cmt.ext );
                taphdr = cmttap_block_get_spec_tapheader ( g_cmt.ext->block );
                g_assert ( taphdr != NULL );

                snprintf ( buff, sizeof (buff ), "0x%02x", taphdr->code );
                gtk_label_set_text ( ui_get_label ( "cmt_filetype_label" ), buff );

                gtk_label_set_text ( ui_get_label ( "cmt_filename_label" ), taphdr->fname );

                snprintf ( buff, sizeof (buff ), "%d Bd", file_bdspeed );
                gtk_label_set_text ( ui_get_label ( "cmt_file_speed_label" ), buff );
                break;

            case CMTEXT_BLOCK_TYPE_TAPDATA:
                file_bdspeed = ( !g_cmt.ext->block->cb_get_bdspeed ) ? 0 : g_cmt.ext->block->cb_get_bdspeed ( g_cmt.ext );
                tapdata = cmttap_block_get_spec_tapdata ( g_cmt.ext->block );
                g_assert ( tapdata != NULL );

                snprintf ( buff, sizeof (buff ), "0x%04x", tapdata->size - 2 );
                gtk_label_set_text ( ui_get_label ( "cmt_fsize_label" ), buff );

                snprintf ( buff, sizeof (buff ), "%d Bd", file_bdspeed );
                gtk_label_set_text ( ui_get_label ( "cmt_file_speed_label" ), buff );
                break;

            default:
                fprintf ( stderr, "%s():%d - Unknown cmtext block type '%d'\n", __func__, __LINE__, cmtext_block_get_type ( g_cmt.ext->block ) );
        };


        // stream info
        en_CMT_STREAM_TYPE stream_type = cmtext_block_get_stream_type ( g_cmt.ext->block );
        if ( stream_type == CMT_STREAM_TYPE_BITSTREAM ) {
            gtk_label_set_text ( ui_get_label ( "cmt_stream_source_label" ), "bitstream" );
        } else if ( stream_type == CMT_STREAM_TYPE_VSTREAM ) {
            gtk_label_set_text ( ui_get_label ( "cmt_stream_source_label" ), "vstream" );
        } else {
            fprintf ( stderr, "%s():%d - Unknown cmt stream type '%d'\n", __func__, __LINE__, stream_type );
        };

        uint32_t rate = cmtext_block_get_rate ( g_cmt.ext->block );
        if ( rate < 1000000 ) {
            snprintf ( buff, sizeof (buff ), "%0.2f kHz", ( (float) rate / 1000 ) );
        } else {
            snprintf ( buff, sizeof (buff ), "%0.2f MHz", ( (float) rate / 1000000 ) );
        };
        gtk_label_set_text ( ui_get_label ( "cmt_stream_rate_label" ), buff );

    };


    // progress bar
    if ( !TEST_CMT_FILLED ) {
        gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), "*** Empty ***" );
    } else {
        if ( g_cmt.playsts == CMTEXT_BLOCK_PLAYSTS_PAUSE ) {
            snprintf ( buff, sizeof (buff ), "*** Playing a gap space of %d ms ***", cmtext_block_get_pause_after ( g_cmt.ext->block ) );
            gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), buff );
        } else {
            gtk_progress_bar_set_text ( ui_get_progress_bar ( "cmt_progressbar" ), g_cmt.ext->block->cb_get_playname ( g_cmt.ext ) );
        };
    };


    // Ovladaci prvky    
    if ( !TEST_CMT_FILLED ) {
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_filelist_button" ), FALSE );

        gtk_widget_show ( ui_get_widget ( "cmt_speed_label" ) );
        gtk_widget_show ( ui_get_widget ( "cmt_speed_comboboxtext" ) );

        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), TRUE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), FALSE );

        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_togglebutton" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_togglebutton" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), TRUE );

        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_previous_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_next_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_rewind_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_forward_button" ), FALSE );

    } else {

        if ( CMTEXT_CONTAINER_TYPE_SIMPLE_TAPE == cmtext_container_get_type ( container ) ) {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_filelist_button" ), TRUE );
        } else {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_filelist_button" ), FALSE );
        };

        if ( cmtext_block_get_block_speed ( g_cmt.ext->block ) == CMTEXT_BLOCK_SPEED_DEFAULT ) {
            gtk_widget_show ( ui_get_widget ( "cmt_speed_label" ) );
            gtk_widget_show ( ui_get_widget ( "cmt_speed_comboboxtext" ) );
        } else {
            gtk_widget_hide ( ui_get_widget ( "cmt_speed_label" ) );
            gtk_widget_hide ( ui_get_widget ( "cmt_speed_comboboxtext" ) );
        };


        if ( play_block == 1 ) {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_previous_button" ), FALSE );
        } else {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_previous_button" ), TRUE );
        };

        if ( play_block < total_blocks ) {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_next_button" ), TRUE );
        } else {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_next_button" ), FALSE );
        };


        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_rewind_button" ), FALSE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_forward_button" ), FALSE );

        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_pause_button" ), FALSE );

        if ( TEST_CMT_STOP ) {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), TRUE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_togglebutton" ), TRUE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_togglebutton" ), FALSE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), FALSE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), TRUE );

            gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( ui_get_widget ( "cmt_play_togglebutton" ) ), FALSE );
        } else {
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_open_button" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_eject_button" ), TRUE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_play_togglebutton" ), FALSE );
            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_record_togglebutton" ), FALSE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_stop_button" ), TRUE );

            gtk_widget_set_sensitive ( ui_get_widget ( "cmt_speed_comboboxtext" ), FALSE );

            if ( TEST_CMT_PLAY ) {
                LOCK_UICALLBACKS ( );
                gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( ui_get_widget ( "cmt_play_togglebutton" ) ), TRUE );
                UNLOCK_UICALLBACKS ( );
            };
        };
    };


    ui_cmt_update_player ( );

    int mztape_speed_id = 0;
    while ( g_mztape_speed[mztape_speed_id] != CMTSPEED_NONE ) {
        if ( g_mztape_speed[mztape_speed_id] == g_cmt.mz_cmtspeed ) break;
        mztape_speed_id++;
    }

    LOCK_UICALLBACKS ( );
    gtk_combo_box_set_active ( ui_get_combo_box ( "cmt_speed_comboboxtext" ), mztape_speed_id );
    UNLOCK_UICALLBACKS ( );

    ui_cmt_speed_menu_update ( );
    ui_cmt_polarity_menu_update ( );
    ui_cmt_tape_index_update_filelist ( );
}


void ui_cmt_update_player ( void ) {
    GtkWidget *window = ui_get_widget ( "cmt_window" );
    if ( !gtk_widget_get_visible ( window ) ) return;

    gdouble total_time = 0;
    gdouble play_time = 0;
    gdouble fraction = 0;
    guint32 print_time;

    if ( TEST_CMT_FILLED ) {
        double scan_time = cmtext_block_get_scantime ( g_cmt.ext->block );
        uint64_t scans = cmtext_block_get_count_scans ( g_cmt.ext->block );
        gdouble body_total_time = ( scan_time * scans );
        if ( !TEST_CMT_STOP ) {
            if ( g_cmt.playsts == CMTEXT_BLOCK_PLAYSTS_PAUSE ) {
                total_time = 0.001 * cmtext_block_get_pause_after ( g_cmt.ext->block );
                play_time = cmt_get_playtime ( ) - body_total_time;
            } else {
                total_time = body_total_time;
                play_time = cmt_get_playtime ( );
            };
            fraction = ( play_time / total_time );
        } else {
            total_time = body_total_time;
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


/*
 * 
 * Tape index
 * 
 */

typedef enum en_CMT_FILELIST {
    CMT_FILELIST_FILE_POSITION, // uint
    CMT_FILELIST_FNAME,
    CMT_FILELIST_FTYPE_TXT,
    CMT_FILELIST_FSIZE_TXT,
    CMT_FILELIST_FSTRT_TXT,
    CMT_FILELIST_FEXEC_TXT,
    CMT_FILELIST_BLOCK_TYPE, // uint
    CMT_FILELIST_BLOCK_SPEED, // uint
    CMT_FILELIST_PLAY_PIXBUF,
    CMT_FILELIST_PLAYED, // bool
    CMT_FILELIST_CMTSPEED, // uint
    CMT_FILELIST_CMTSPEED_TXT,
    CMT_FILELIST_CMTSPEED_SENSITIVE, // bool
    CMT_FILELIST_CMTSPEED_MZ_VISIBLE, // bool
    CMT_FILELIST_CMTSPEED_ZX_VISIBLE, // bool
    CMT_FILELIST_CMTSPEED_PENCIL_VISIBLE, // bool
    CMT_FILELIST_COUNT_ITEMS
} en_CMT_FILELIST;


typedef struct st_UI_CMT_FILELIST {
    st_UIWINPOS main_pos;
    GdkPixbuf *play_pixbuf;
} st_UI_CMT_FILELIST;

static st_UI_CMT_FILELIST g_cmt_filelist;


static void ui_cmt_tape_main_window_sensitive ( gboolean sensitive ) {
    gtk_widget_set_sensitive ( ui_get_widget ( "cmt_display_vbox" ), sensitive );
    gtk_widget_set_sensitive ( ui_get_widget ( "cmt_buttons_vbox" ), sensitive );
}


void ui_cmt_tape_index_hide ( void ) {
    ui_cmt_tape_main_window_sensitive ( TRUE );
    GtkWidget *window = ui_get_widget ( "cmt_tape_index_window" );
    if ( !gtk_widget_is_visible ( window ) ) return;
    ui_main_win_get_pos ( GTK_WINDOW ( window ), &g_cmt_filelist.main_pos );
    gtk_widget_hide ( window );
}


static void ui_cmt_tape_index_get_speed_txt ( char *buff, uint32_t buff_size, en_CMTEXT_BLOCK_TYPE bltype, en_CMTEXT_BLOCK_SPEED blspeed, en_CMTSPEED cmtspeed, gboolean rateio ) {

    uint16_t base_bdspeed;

    if ( blspeed == CMTEXT_BLOCK_SPEED_NONE ) {
        buff[0] = 0x00;
        return;
    };

    if ( blspeed == CMTEXT_BLOCK_SPEED_DEFAULT ) {
        snprintf ( buff, buff_size, "Default" );
        return;
    };

    if ( bltype == CMTEXT_BLOCK_TYPE_MZF ) {
        base_bdspeed = MZTAPE_DEFAULT_BDSPEED;
    } else if ( ( bltype == CMTEXT_BLOCK_TYPE_TAPHEADER ) || ( bltype == CMTEXT_BLOCK_TYPE_TAPDATA ) ) {
        base_bdspeed = ZXTAPE_DEFAULT_BDSPEED;
    } else {
        snprintf ( buff, buff_size, "UNKNOWN" );
        return;
    };

    if ( rateio ) {
        cmtspeed_get_ratiospeedtxt ( buff, buff_size, cmtspeed, base_bdspeed );
    } else {
        cmtspeed_get_speedtxt ( buff, buff_size, cmtspeed, base_bdspeed );
    };
}


static void ui_cmt_tape_index_prepare_cmtspeed_mz_model ( void ) {

    GtkListStore *store = GTK_LIST_STORE ( ui_get_object ( "cmtspeed_mz_combo_liststore" ) );
    gtk_list_store_clear ( store );
    GtkTreeIter iter;

    gtk_list_store_append ( store, &iter );
    char buff[20];
    ui_cmt_tape_index_get_speed_txt ( buff, sizeof ( buff ), CMTEXT_BLOCK_TYPE_MZF, CMTEXT_BLOCK_SPEED_DEFAULT, CMTSPEED_NONE, TRUE );
    gtk_list_store_set ( store, &iter, 0, 0, 1, buff, -1 );

    int i = 0;
    while ( g_mztape_speed[i] != CMTSPEED_NONE ) {
        gtk_list_store_append ( store, &iter );
        ui_cmt_tape_index_get_speed_txt ( buff, sizeof ( buff ), CMTEXT_BLOCK_TYPE_MZF, CMTEXT_BLOCK_SPEED_SET, g_mztape_speed[i], TRUE );
        gtk_list_store_set ( store, &iter, 0, g_mztape_speed[i], 1, buff, -1 );
        i++;
    };

}


static void ui_cmt_tape_index_prepare_cmtspeed_zx_model ( void ) {

    GtkListStore *store = GTK_LIST_STORE ( ui_get_object ( "cmtspeed_zx_combo_liststore" ) );
    gtk_list_store_clear ( store );
    GtkTreeIter iter;

    int i = 0;
    while ( g_zxtape_speed[i] != CMTSPEED_NONE ) {
        gtk_list_store_append ( store, &iter );
        char buff[20];
        ui_cmt_tape_index_get_speed_txt ( buff, sizeof ( buff ), CMTEXT_BLOCK_TYPE_TAPHEADER, CMTEXT_BLOCK_SPEED_SET, g_zxtape_speed[i], TRUE );
        gtk_list_store_set ( store, &iter, 0, g_zxtape_speed[i], 1, buff, -1 );
        i++;
    };

}


void ui_cmt_tape_index_update_filelist ( void ) {
    GtkWidget *window = ui_get_widget ( "cmt_tape_index_window" );
    if ( !gtk_widget_is_visible ( window ) ) return;

    if ( !TEST_CMT_FILLED ) {
        ui_cmt_tape_index_hide ( );
        return;
    };

    st_CMTEXT_CONTAINER *container = cmtext_get_container ( g_cmt.ext );

    if ( cmtext_container_get_type ( container ) != CMTEXT_CONTAINER_TYPE_SIMPLE_TAPE ) {
        ui_cmt_tape_index_hide ( );
        return;
    };

    GtkWidget *treeview = ui_get_widget ( "cmt_tape_index_treeview" );
    GtkListStore *store = GTK_LIST_STORE ( gtk_tree_view_get_model ( GTK_TREE_VIEW ( treeview ) ) );
    gtk_list_store_clear ( store );

    int count_blocks = cmtext_container_get_count_blocks ( container );

    int play_block = cmtext_block_get_block_id ( g_cmt.ext->block ) + 1;

    int i;
    for ( i = 0; i < count_blocks; i++ ) {

        gboolean played = ( ( i == ( play_block - 1 ) ) && ( TEST_CMT_PLAY ) ) ? TRUE : FALSE;

        en_CMTEXT_BLOCK_TYPE bltype = cmtext_container_get_block_type ( container, i );

        const char *ftype_txt;
        int ftype = cmtext_container_get_block_ftype ( container, i );
        if ( ( bltype == CMTEXT_BLOCK_TYPE_TAPHEADER ) || ( bltype == CMTEXT_BLOCK_TYPE_TAPDATA ) ) {
            ftype_txt = cmttap_get_block_code_txt ( ftype );
        } else {
            char ftype_num_txt[5];
            ftype_num_txt[0] = 0x00;
            if ( ftype != -1 ) {
                snprintf ( ftype_num_txt, sizeof ( ftype_num_txt ), "0x%02X", ftype );
            };
            ftype_txt = ftype_num_txt;
        };

        int fsize = cmtext_container_get_block_fsize ( container, i );
        char fsize_txt[7];
        fsize_txt[0] = 0x00;
        if ( fsize != -1 ) {
            snprintf ( fsize_txt, sizeof ( fsize_txt ), "0x%04X", fsize );
        };

        int fstrt = cmtext_container_get_block_fstrt ( container, i );
        char fstrt_txt[7];
        fstrt_txt[0] = 0x00;
        if ( fstrt != -1 ) {
            snprintf ( fstrt_txt, sizeof ( fstrt_txt ), "0x%04X", fstrt );
        };

        int fexec = cmtext_container_get_block_fexec ( container, i );
        char fexec_txt[7];
        fexec_txt[0] = 0x00;
        if ( fexec != -1 ) {
            snprintf ( fexec_txt, sizeof ( fexec_txt ), "0x%04X", fexec );
        };

        en_CMTEXT_BLOCK_SPEED blspeed = cmtext_container_get_block_speed ( container, i );

        gboolean cmtspeed_mz_visible = ( ( bltype == CMTEXT_BLOCK_TYPE_MZF ) && ( blspeed != CMTEXT_BLOCK_SPEED_NONE ) ) ? TRUE : FALSE;
        gboolean cmtspeed_zx_visible = ( ( ( bltype == CMTEXT_BLOCK_TYPE_TAPHEADER ) || ( bltype == CMTEXT_BLOCK_TYPE_TAPDATA ) ) && ( blspeed != CMTEXT_BLOCK_SPEED_NONE ) ) ? TRUE : FALSE;
        gboolean cmtspeed_sensitive = ( played ) ? FALSE : TRUE;

        en_CMTSPEED cmtspeed = cmtext_container_get_block_cmt_speed ( container, i );

        char cmtspeed_txt[20];
        ui_cmt_tape_index_get_speed_txt ( cmtspeed_txt, sizeof ( cmtspeed_txt ), bltype, blspeed, cmtspeed, FALSE );


        GtkTreeIter iter;
        gtk_list_store_append ( store, &iter );
        gtk_list_store_set ( store, &iter,
                             CMT_FILELIST_FILE_POSITION, ( i + 1 ),
                             CMT_FILELIST_BLOCK_TYPE, bltype,
                             CMT_FILELIST_BLOCK_SPEED, blspeed,
                             CMT_FILELIST_FNAME, cmtext_container_get_block_fname ( container, i ),
                             CMT_FILELIST_FTYPE_TXT, ftype_txt,
                             CMT_FILELIST_FSIZE_TXT, fsize_txt,
                             CMT_FILELIST_FSTRT_TXT, fstrt_txt,
                             CMT_FILELIST_FEXEC_TXT, fexec_txt,
                             CMT_FILELIST_PLAY_PIXBUF, g_cmt_filelist.play_pixbuf,
                             CMT_FILELIST_PLAYED, played,
                             CMT_FILELIST_CMTSPEED, cmtspeed,
                             CMT_FILELIST_CMTSPEED_TXT, cmtspeed_txt,
                             CMT_FILELIST_CMTSPEED_SENSITIVE, cmtspeed_sensitive,
                             CMT_FILELIST_CMTSPEED_MZ_VISIBLE, cmtspeed_mz_visible,
                             CMT_FILELIST_CMTSPEED_ZX_VISIBLE, cmtspeed_zx_visible,
                             CMT_FILELIST_CMTSPEED_PENCIL_VISIBLE, ( cmtspeed_mz_visible | cmtspeed_zx_visible ),
                             -1 );
    };
}


static void ui_cmt_tape_index_show ( void ) {
    ui_cmt_tape_main_window_sensitive ( FALSE );
    static gboolean initialised = FALSE;
    if ( !initialised ) {
        ui_main_setpos ( &g_cmt_filelist.main_pos, -1, -1 );

        GError *error = NULL;
        g_cmt_filelist.play_pixbuf = gdk_pixbuf_new_from_file ( "ui_resources/icons/cmt/play_icon.png", &error );
        if ( error ) {
            fprintf ( stderr, "%s():%d - Unable to read icon: %s\n", __func__, __LINE__, error->message );
        };

        ui_cmt_tape_index_prepare_cmtspeed_mz_model ( );
        ui_cmt_tape_index_prepare_cmtspeed_zx_model ( );

        initialised = TRUE;
    };
    GtkWidget *window = ui_get_widget ( "cmt_tape_index_window" );
    ui_main_win_move_to_pos ( GTK_WINDOW ( window ), &g_cmt_filelist.main_pos );


    gtk_widget_show ( window );
    ui_cmt_tape_index_update_filelist ( );
    gtk_widget_grab_focus ( window );
}


G_MODULE_EXPORT gboolean on_cmt_tape_index_window_key_press_event ( GtkWidget *widget, GdkEventKey *event, gpointer user_data ) {
    if ( event->keyval == GDK_KEY_Escape ) {
        ui_cmt_tape_index_hide ( );
        return TRUE;
    };
    return FALSE;
}


G_MODULE_EXPORT gboolean on_cmt_tape_index_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer data ) {
    (void) widget;
    (void) event;
    (void) data;
    ui_cmt_tape_index_hide ( );
    return TRUE;
}


G_MODULE_EXPORT void on_cmt_tape_index_treeview_row_activated ( GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data ) {
    gint *indices = gtk_tree_path_get_indices ( path );
    int new_block = indices[0];
    int old_block = cmtext_block_get_block_id ( g_cmt.ext->block );

    gboolean fl_play = TEST_CMT_PLAY;
    if ( fl_play ) cmt_stop ( );

    if ( old_block != new_block ) {
        g_cmt.ext->container->cb_open_block ( new_block );
        cmt_play ( );
    } else if ( !fl_play ) {
        cmt_play ( );
    };

    ui_cmt_window_update ( );
}


G_MODULE_EXPORT void on_cmtspeed_mz_cellrenderercombo_changed ( GtkCellRendererCombo *combo, gchar *path_string, GtkTreeIter *new_iter, gpointer user_data ) {

    GValue gv = G_VALUE_INIT;
    gtk_tree_model_get_value ( GTK_TREE_MODEL ( ui_get_object ( "cmtspeed_mz_combo_liststore" ) ), new_iter, 0, &gv );
    gint value = g_value_get_uint ( &gv );

    GtkTreeModel *model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( ui_get_widget ( "cmt_tape_index_treeview" ) ) );
    GtkListStore *store = GTK_LIST_STORE ( model );
    GtkTreeIter iter;
    gtk_tree_model_get_iter_from_string ( model, &iter, path_string );

    en_CMTEXT_BLOCK_SPEED blspeed;
    en_CMTSPEED cmtspeed;

    if ( value == 0 ) {
        blspeed = CMTEXT_BLOCK_SPEED_DEFAULT;
        cmtspeed = CMTSPEED_NONE;
    } else {
        blspeed = CMTEXT_BLOCK_SPEED_SET;
        cmtspeed = value;
    };

    char cmtspeed_txt[20];
    ui_cmt_tape_index_get_speed_txt ( cmtspeed_txt, sizeof ( cmtspeed_txt ), CMTEXT_BLOCK_TYPE_MZF, blspeed, cmtspeed, FALSE );

    GtkTreePath *path = gtk_tree_path_new_from_string ( path_string );
    gint *indices = gtk_tree_path_get_indices ( path );
    int block_id = indices[0];

    if ( ( block_id == cmtext_block_get_block_id ( g_cmt.ext->block ) ) && ( TEST_CMT_PLAY ) ) {
        ui_cmt_tape_index_update_filelist ( );
    } else {
        st_CMTEXT_CONTAINER *container = cmtext_get_container ( g_cmt.ext );
        cmtext_container_set_block_speed ( container, block_id, blspeed );
        cmtext_container_set_block_cmt_speed ( container, block_id, cmtspeed );

        gtk_list_store_set ( store, &iter,
                             CMT_FILELIST_BLOCK_SPEED, blspeed,
                             CMT_FILELIST_CMTSPEED, cmtspeed,
                             CMT_FILELIST_CMTSPEED_TXT, cmtspeed_txt,
                             -1 );

        if ( block_id == cmtext_block_get_block_id ( g_cmt.ext->block ) ) {
            g_cmt.ext->container->cb_open_block ( block_id );
            ui_cmt_window_update ( );
        } else {
            ui_cmt_tape_index_update_filelist ( );
        };
    };
    gtk_tree_path_free ( path );

}


G_MODULE_EXPORT void on_cmtspeed_zx_cellrenderercombo_changed ( GtkCellRendererCombo *combo, gchar *path_string, GtkTreeIter *new_iter, gpointer user_data ) {

    GValue gv = G_VALUE_INIT;
    gtk_tree_model_get_value ( GTK_TREE_MODEL ( ui_get_object ( "cmtspeed_zx_combo_liststore" ) ), new_iter, 0, &gv );
    gint value = g_value_get_uint ( &gv );

    GtkTreeModel *model = gtk_tree_view_get_model ( GTK_TREE_VIEW ( ui_get_widget ( "cmt_tape_index_treeview" ) ) );
    GtkListStore *store = GTK_LIST_STORE ( model );
    GtkTreeIter iter;
    gtk_tree_model_get_iter_from_string ( model, &iter, path_string );

    en_CMTSPEED cmtspeed = value;

    char cmtspeed_txt[20];
    ui_cmt_tape_index_get_speed_txt ( cmtspeed_txt, sizeof ( cmtspeed_txt ), CMTEXT_BLOCK_TYPE_TAPHEADER, CMTEXT_BLOCK_SPEED_SET, cmtspeed, FALSE );

    GtkTreePath *path = gtk_tree_path_new_from_string ( path_string );
    gint *indices = gtk_tree_path_get_indices ( path );
    int block_id = indices[0];

    if ( ( block_id == cmtext_block_get_block_id ( g_cmt.ext->block ) ) && ( TEST_CMT_PLAY ) ) {
        ui_cmt_tape_index_update_filelist ( );
    } else {
        st_CMTEXT_CONTAINER *container = cmtext_get_container ( g_cmt.ext );
        cmtext_container_set_block_cmt_speed ( container, block_id, cmtspeed );

        gtk_list_store_set ( store, &iter,
                             CMT_FILELIST_CMTSPEED, cmtspeed,
                             CMT_FILELIST_CMTSPEED_TXT, cmtspeed_txt,
                             -1 );

        if ( block_id == cmtext_block_get_block_id ( g_cmt.ext->block ) ) {
            g_cmt.ext->container->cb_open_block ( block_id );
            ui_cmt_window_update ( );
        } else {
            ui_cmt_tape_index_update_filelist ( );
        };
    };
    gtk_tree_path_free ( path );
}


G_MODULE_EXPORT void on_cmt_filelist_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;
    ui_cmt_tape_index_show ( );
}

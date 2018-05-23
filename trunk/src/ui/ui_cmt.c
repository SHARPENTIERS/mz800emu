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


G_MODULE_EXPORT void on_cmt_filelist_button_clicked ( GtkButton *button, gpointer data ) {
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

    if ( TEST_CMT_FILLED ) {
        total_blocks = cmtext_container_get_count_blocks ( g_cmt.ext->container );
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

        //gtk_widget_set_sensitive ( ui_get_widget ( "cmt_filelist_button" ), TRUE );
        gtk_widget_set_sensitive ( ui_get_widget ( "cmt_filelist_button" ), FALSE );

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
        uint64_t scans = cmtext_block_get_scans ( g_cmt.ext->block );
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

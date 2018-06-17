/* 
 * File:   ui_memsave.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 12. června 2018, 12:36
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
#include "mz800emu_cfg.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED

#include "z80ex/include/z80ex.h"
#include "ui_membrowser.h"
#include "../ui_main.h"
#include "ui/ui_hexeditable.h"
#include "debugger/debugger.h"
#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"
#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"
#include "sharpmz_ascii.h"
#include "ui_memsave.h"

static st_DRIVER *g_driver_realloc = &g_ui_memory_driver_realloc;

static gboolean g_ui_memsave_memaloc_lock = FALSE;
static gboolean g_ui_memsave_initialized = FALSE;
static char g_ui_memsave_last_mzf_cmnt[MZF_CMNT_LENGTH + 1];
static volatile gboolean g_ui_memsave_textbuffer_lock = FALSE;


static void ui_memsave_save_button_sensitivity ( void ) {
    GtkWidget *button = ui_get_widget ( "dbg_memsave_save_button" );
    Z80EX_WORD size = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ) ) );
    gboolean save_enabled = ( size ) ? TRUE : FALSE;
    gtk_widget_set_sensitive ( button, save_enabled );
}


static void ui_memsave_window_hide ( void ) {
    GtkWidget *window = ui_get_widget ( "dbg_memsave_window" );
    gtk_widget_hide ( window );
}


G_MODULE_EXPORT gboolean on_dbg_memsave_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_memsave_window_hide ( );
    return TRUE;
}


G_MODULE_EXPORT void on_dbg_memsave_close_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;
    ui_memsave_window_hide ( );
}


static int ui_memsave_save_file ( char *filename, gboolean add_mzfheader ) {

    st_HANDLER *h = generic_driver_open_memory ( NULL, g_driver_realloc, 1 );
    if ( !h ) {
        fprintf ( stderr, "%s():%d - Can't open handler\n", __func__, __LINE__ );
        return EXIT_FAILURE;
    };

    generic_driver_set_handler_readonly_status ( h, 0 );
    h->spec.memspec.swelling_enabled = 1;

    uint32_t offset = 0;

    if ( add_mzfheader ) {

        st_MZF_HEADER mzfhdr;
        memset ( &mzfhdr, 0x00, sizeof ( mzfhdr ) );

        GtkEntry *entry;

        entry = ui_get_entry ( "dbg_memsave_mzfhdr_ftype_entry" );
        if ( !gtk_entry_get_text_length ( entry ) ) {
            gtk_entry_set_text ( entry, "01" );
        };
        mzfhdr.ftype = (Z80EX_BYTE) debuger_hextext_to_uint32 ( gtk_entry_get_text ( entry ) );

        entry = ui_get_entry ( "dbg_memsave_mzfhdr_fstrt_entry" );
        if ( !gtk_entry_get_text_length ( entry ) ) {
            gtk_entry_set_text ( entry, "0000" );
        };
        mzfhdr.fstrt = debuger_hextext_to_uint32 ( gtk_entry_get_text ( entry ) );

        entry = ui_get_entry ( "dbg_memsave_mzfhdr_fexec_entry" );
        if ( !gtk_entry_get_text_length ( entry ) ) {
            gtk_entry_set_text ( entry, "0000" );
        };
        mzfhdr.fexec = debuger_hextext_to_uint32 ( gtk_entry_get_text ( entry ) );

        mzfhdr.fsize = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_mzfhdr_fsize_entry" ) ) );

        entry = ui_get_entry ( "dbg_memsave_mzfhdr_fname_entry" );
        if ( !gtk_entry_get_text_length ( entry ) ) {
            gtk_entry_set_text ( entry, "FILENAME" );
        };
        mzf_tools_set_fname ( &mzfhdr, (char*) gtk_entry_get_text ( entry ) );

        GtkWidget *view = ui_get_widget ( "dbg_memsave_mzfhdr_cmnt_textview" );
        GtkTextBuffer *buffer = gtk_text_view_get_buffer ( GTK_TEXT_VIEW ( view ) );
        GtkTextIter start;
        GtkTextIter end;
        gtk_text_buffer_get_start_iter ( buffer, &start );
        gtk_text_buffer_get_end_iter ( buffer, &end );
        char *cmnt = gtk_text_buffer_get_text ( buffer, &start, &end, FALSE );

        int cmnt_length = strlen ( cmnt );
        if ( cmnt_length ) {

            if ( cmnt_length <= sizeof ( mzfhdr.cmnt ) ) {

                gboolean convert_mzfcomment = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( ui_get_widget ( "dbg_memsave_convert_mzfcomment_checkbutton" ) ) );
                if ( convert_mzfcomment ) {
                    int i;
                    uint8_t *src = (uint8_t*) cmnt;
                    uint8_t *dst = mzfhdr.cmnt;
                    for ( i = 0; i < cmnt_length; i++ ) {
                        *dst++ = sharpmz_cnv_to ( *src++ );
                    };
                } else {
                    memcpy ( &mzfhdr.cmnt, cmnt, cmnt_length );
                };

            } else {
                assert ( cmnt_length <= sizeof ( mzfhdr.cmnt ) );
                fprintf ( stderr, "%s():%d - Error: comment length is too big! (%d)\n", __func__, __LINE__, cmnt_length );
            };
        };

        g_free ( cmnt );

        // save header
        if ( EXIT_FAILURE == mzf_write_header ( h, &mzfhdr ) ) {
            fprintf ( stderr, "%s():%d - Can't write mzfheader\n", __func__, __LINE__ );
            generic_driver_close ( h );
            return EXIT_FAILURE;
        };

        offset += sizeof ( st_MZF_HEADER );
    };

    // save body

    Z80EX_WORD addr = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_from_entry" ) ) );
    Z80EX_WORD size = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ) ) );

    Z80EX_WORD mem_max = g_membrowser.mem_size - 1;

    while ( size ) {

        uint32_t i = addr + size;
        uint32_t save_size = size;

        if ( i > mem_max ) {
            save_size = mem_max - addr;
        };

        generic_driver_write ( h, offset, &g_membrowser.data_current[addr], save_size );

        offset += save_size;
        size -= save_size;
        addr += save_size + 1;
        addr &= mem_max;
    };

    printf ( "Save: %s\n", filename );
    int ret = generic_driver_save_memory ( h, filename );

    if ( EXIT_FAILURE == ret ) {
        fprintf ( stderr, "%s():%d - Can't write file '%s'\n", __func__, __LINE__, filename );
    };

    generic_driver_close ( h );

    return ret;
}


G_MODULE_EXPORT void on_dbg_memsave_save_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;

    ui_memsave_window_hide ( );

    gboolean add_mzfheader = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( ui_get_widget ( "dbg_memsave_add_mzfheader_checkbutton" ) ) );

    char *dialog_title = NULL;
    char *predefined_filename = NULL;
    if ( add_mzfheader ) {
        dialog_title = "Save MZF from current memory dump";
        predefined_filename = "newfile.mzf";
    } else {
        dialog_title = "Save binary file from current memory dump";
        predefined_filename = "newfile.bin";
    };

    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new ( dialog_title,
                                           ui_get_window ( "dbg_membrowser_window" ),
                                           action,
                                           "_Cancel",
                                           GTK_RESPONSE_CANCEL,
                                           "_Save",
                                           GTK_RESPONSE_ACCEPT,
                                           NULL );
    chooser = GTK_FILE_CHOOSER ( dialog );

    gtk_file_chooser_set_do_overwrite_confirmation ( chooser, TRUE );

    gtk_file_chooser_set_current_name ( chooser, predefined_filename );

    int ret = EXIT_FAILURE;

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );
    if ( res == GTK_RESPONSE_ACCEPT ) {
        char *filename;
        filename = gtk_file_chooser_get_filename ( chooser );
        ret = ui_memsave_save_file ( filename, add_mzfheader );
        g_free ( filename );
    };

    gtk_widget_destroy ( dialog );

    if ( ret == EXIT_FAILURE ) {
        ui_memsave_window_show ( );
    };
}


static void ui_memsave_add_mzfheader_changed ( void ) {
    GtkWidget *mzfheader_grid = ui_get_widget ( "dbg_memsave_mzfheader_grid" );
    GtkToggleButton *togglebutton = GTK_TOGGLE_BUTTON ( ui_get_widget ( "dbg_memsave_add_mzfheader_checkbutton" ) );
    gtk_widget_set_sensitive ( mzfheader_grid, gtk_toggle_button_get_active ( togglebutton ) );
}


G_MODULE_EXPORT void on_dbg_memsave_add_mzfheader_checkbutton_toggled ( GtkToggleButton *togglebutton, gpointer user_data ) {
    (void) togglebutton;
    (void) user_data;
    ui_memsave_add_mzfheader_changed ( );
}


static void ui_memsave_set_hex_size ( Z80EX_WORD size ) {
    g_ui_memsave_memaloc_lock = TRUE;
    char buff[5];
    snprintf ( buff, sizeof ( buff ), "%04X", size );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ), buff );
    g_ui_memsave_memaloc_lock = FALSE;
    ui_memsave_save_button_sensitivity ( );
}


static void ui_memsave_set_dec_size ( Z80EX_WORD size ) {
    g_ui_memsave_memaloc_lock = TRUE;
    char buff[6];
    snprintf ( buff, sizeof ( buff ), "%d", size );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_dec_entry" ), buff );
    g_ui_memsave_memaloc_lock = FALSE;
    ui_memsave_save_button_sensitivity ( );
}


static void ui_memsave_set_size ( Z80EX_WORD size ) {
    ui_memsave_set_hex_size ( size );
    ui_memsave_set_dec_size ( size );
}


static void ui_memsave_set_to ( Z80EX_WORD to ) {
    g_ui_memsave_memaloc_lock = TRUE;
    char buff[5];
    snprintf ( buff, sizeof ( buff ), "%04X", to );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_to_entry" ), buff );
    g_ui_memsave_memaloc_lock = FALSE;
}


G_MODULE_EXPORT void on_dbg_memsave_from_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memsave_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    if ( !gtk_entry_get_text_length ( ui_get_entry ( "dbg_memsave_to_entry" ) ) ) {
        ui_memsave_save_button_sensitivity ( );
        return;
    };

    Z80EX_WORD addr_from = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_from_entry" ) ) );
    Z80EX_WORD addr_to = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_to_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {
        mask = 0x1fff;
        if ( addr_from > 0x1fff ) {
            addr_from = 0x1fff;
            g_ui_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_from_entry" ), "1FFF" );
            g_ui_memsave_memaloc_lock = FALSE;
        };
    };

    ui_memsave_set_size ( ( addr_to - addr_from ) & mask );
}


static Z80EX_WORD ui_memsave_get_from ( void ) {
    if ( !gtk_entry_get_text_length ( ui_get_entry ( "dbg_memsave_from_entry" ) ) ) {
        g_ui_memsave_memaloc_lock = TRUE;
        gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_from_entry" ), "0000" );
        g_ui_memsave_memaloc_lock = FALSE;
    };

    return debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_from_entry" ) ) );
}


G_MODULE_EXPORT void on_dbg_memsave_to_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memsave_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    Z80EX_WORD addr_from = ui_memsave_get_from ( );
    Z80EX_WORD addr_to = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_to_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {
        mask = 0x1fff;
        if ( addr_to > 0x1fff ) {
            addr_to = 0x1fff;
            g_ui_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_to_entry" ), "1FFF" );
            g_ui_memsave_memaloc_lock = FALSE;
        };
    };

    ui_memsave_set_size ( ( addr_to - addr_from ) & mask );
}


G_MODULE_EXPORT void on_dbg_memsave_size_hex_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memsave_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    Z80EX_WORD addr_from = ui_memsave_get_from ( );
    Z80EX_WORD size = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {
        mask = 0x1fff;
        if ( size > 0x1fff ) {
            size = 0x1fff;
            g_ui_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ), "1FFF" );
            g_ui_memsave_memaloc_lock = FALSE;
        };
    };

    ui_memsave_set_to ( ( addr_from + size ) & mask );
    ui_memsave_set_dec_size ( size );
}


G_MODULE_EXPORT void on_dbg_memsave_size_dec_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memsave_memaloc_lock ) return;

    Z80EX_WORD addr_from = ui_memsave_get_from ( );
    int atoi_size = atoi ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_size_dec_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {
        mask = 0x1fff;
        if ( atoi_size > 0x1fff ) {
            atoi_size = 0x1fff;
            g_ui_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_dec_entry" ), "8191" );
            g_ui_memsave_memaloc_lock = FALSE;
        };
    } else {
        if ( atoi_size > 0xffff ) {
            atoi_size = 0xffff;
            g_ui_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_dec_entry" ), "65535" );
            g_ui_memsave_memaloc_lock = FALSE;
        };
    }

    ui_memsave_set_to ( ( addr_from + (Z80EX_WORD) atoi_size ) & mask );
    ui_memsave_set_hex_size ( (Z80EX_WORD) atoi_size );
}


static gboolean ui_memsave_display_status_textbuffer ( gpointer user_data ) {
    g_ui_memsave_textbuffer_lock = TRUE;
    GtkWidget *view = ui_get_widget ( "dbg_memsave_mzfhdr_cmnt_textview" );
    GtkTextBuffer *buffer = gtk_text_view_get_buffer ( GTK_TEXT_VIEW ( view ) );
    gtk_text_buffer_set_text ( buffer, g_ui_memsave_last_mzf_cmnt, -1 );
    g_ui_memsave_textbuffer_lock = FALSE;
    return G_SOURCE_REMOVE;
}


static void on_ui_memsave_mzfhdr_cmnt_buffer_changed ( GtkTextBuffer *textbuffer, gpointer user_data ) {
    if ( g_ui_memsave_textbuffer_lock ) return;

    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_start_iter ( textbuffer, &start );
    gtk_text_buffer_get_end_iter ( textbuffer, &end );
    char *new_text = gtk_text_buffer_get_text ( textbuffer, &start, &end, FALSE );
    int new_length = strlen ( new_text );

    if ( new_length > MZF_CMNT_LENGTH ) {
        new_length = strlen ( g_ui_memsave_last_mzf_cmnt );
        gdk_threads_add_idle ( ui_memsave_display_status_textbuffer, NULL );
    } else {
        strncpy ( g_ui_memsave_last_mzf_cmnt, new_text, sizeof ( g_ui_memsave_last_mzf_cmnt ) );
    };
    g_free ( new_text );

    char buff[4];
    snprintf ( buff, sizeof ( buff ), "%d", MZF_CMNT_LENGTH - new_length );
    gtk_label_set_text ( ui_get_label ( "dbg_memsave_cmnt_bytes_remaining_label" ), buff );
}


void ui_memsave_window_show ( void ) {
    g_ui_memsave_memaloc_lock = FALSE;
    if ( !g_ui_memsave_initialized ) {
        memset ( g_ui_memsave_last_mzf_cmnt, 0x00, sizeof ( g_ui_memsave_last_mzf_cmnt ) );
        GtkWidget *view = ui_get_widget ( "dbg_memsave_mzfhdr_cmnt_textview" );
        GtkTextBuffer *buffer = gtk_text_view_get_buffer ( GTK_TEXT_VIEW ( view ) );
        g_signal_connect ( G_OBJECT ( buffer ), "changed",
                           G_CALLBACK ( on_ui_memsave_mzfhdr_cmnt_buffer_changed ), NULL );
        GtkToggleButton *add_mzfheader = GTK_TOGGLE_BUTTON ( ui_get_widget ( "dbg_memsave_add_mzfheader_checkbutton" ) );
        gtk_toggle_button_set_active ( add_mzfheader, TRUE );
        GtkToggleButton *convert_mzfcomment = GTK_TOGGLE_BUTTON ( ui_get_widget ( "dbg_memsave_convert_mzfcomment_checkbutton" ) );
        gtk_toggle_button_set_active ( convert_mzfcomment, TRUE );
        g_ui_memsave_initialized = TRUE;
    };

    if ( g_membrowser.memsrc == MEMBROWSER_SOURCE_VRAM ) {

        Z80EX_WORD addr_from = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_from_entry" ) ) );
        if ( addr_from > 0x1fff ) {
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_from_entry" ), "1FFF" );
        };

        Z80EX_WORD addr_to = debuger_hextext_to_uint32 ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_to_entry" ) ) );
        if ( addr_to > 0x1fff ) {
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_to_entry" ), "1FFF" );
        };
    }

    ui_memsave_save_button_sensitivity ( );

    GtkWidget *window = ui_get_widget ( "dbg_memsave_window" );
    gtk_widget_grab_focus ( ui_get_widget ( "dbg_memsave_from_entry" ) );
    ui_memsave_add_mzfheader_changed ( );
    gtk_widget_show ( window );
}
#endif

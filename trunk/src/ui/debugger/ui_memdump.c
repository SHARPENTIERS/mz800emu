/* 
 * File:   ui_memdump.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 26. října 2015, 9:45
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
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED

#include "z80ex/include/z80ex.h"
#include "ui_memdump.h"
#include "../ui_main.h"
#include "debugger/debugger.h"
#include "memory/memory.h"
#include "ramdisk/ramdisk.h"
#include "sharpmz_ascii.h"
#include "ui/ui_hexeditable.h"
#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"

#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"

static st_DRIVER *g_driver_realloc = &g_ui_memory_driver_realloc;

#define UI_MEMDUMP_MAXDUMP_LENGTH   0x10000

#define UI_MEMDUMP_HEXVAL_CHANGED "<span foreground=\"#FF1010\"><b>%02X</b></span>"
//#define UI_MEMDUMP_ASCIIVAL_CHANGED "<span background=\"#000080\" foreground=\"#00ff00\">%c</span>"


typedef enum en_MEMSRC {
    MEMSRC_MAPED = 0,
    MEMSRC_RAM,
    MEMSRC_VRAM,
    MEMSRC_PEZIK_E8,
    MEMSRC_PEZIK_68,
    MEMSRC_MZ1R18,
} en_MEMSRC;


typedef enum en_MEMCHARSET {
    MEMCHAR_ASCII = 0,
    MEMCHAR_SHASCII,
} en_MEMCHARSET;


typedef struct st_UIMEMDUMP {
    st_UIWINPOS main_pos;

    en_MEMCHARSET charset;
    en_MEMSRC memsrc;
    unsigned vram_plane;
    unsigned pezik_e8_bank;
    unsigned pezik_68_bank;
    unsigned pezik_endianity;
    unsigned mr1z18_bank;
    gboolean comparative_mode;
    uint8_t dump_data[UI_MEMDUMP_MAXDUMP_LENGTH];
} st_UIMEMDUMP;

st_UIMEMDUMP g_uimemdump;


typedef enum en_DBG_MEMDUMP {
    DBG_MEMDUMP_ADDR = 0,
    DBG_MEMDUMP_ADDR_TXT,
    DBG_MEMDUMP_VALUES,
    DBG_MEMDUMP_ASCII,
    DBG_MEMDUMP_COUNT_COLUMNS
} en_DBG_MEMDUMP;


void ui_memdump_update ( void ) {

    LOCK_UICALLBACKS ( );

    gtk_combo_box_set_active ( ui_get_combo_box ( "dbg_memdump_src_comboboxtext" ), g_uimemdump.memsrc );

    GtkWidget *label = ui_get_widget ( "dbg_memdump_bank_label" );
    GtkWidget *spinbutton = ui_get_widget ( "dbg_memdump_bank_spinbutton" );
    GtkWidget *endianness_combo = ui_get_widget ( "dbg_memdump_pezik_endianness_comboboxtext" );

    gtk_combo_box_set_active ( (GtkComboBox*) endianness_combo, g_uimemdump.pezik_endianity );
    gtk_widget_hide ( endianness_combo );

    if ( g_uimemdump.memsrc >= MEMSRC_VRAM ) {

        gtk_widget_show ( label );
        gtk_widget_show ( spinbutton );

        if ( g_uimemdump.memsrc == MEMSRC_VRAM ) {
            gtk_label_set_text ( GTK_LABEL ( label ), "Plane:" );
            if ( g_uimemdump.vram_plane > 3 ) g_uimemdump.vram_plane = 0;
            gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( spinbutton ), 0, 3 );
            gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( spinbutton ), g_uimemdump.vram_plane );
        } else {
            gtk_label_set_text ( GTK_LABEL ( label ), "Bank:" );
            if ( g_uimemdump.memsrc == MEMSRC_PEZIK_68 ) {
                gtk_widget_show ( endianness_combo );
                if ( !g_ramdisk.pezik [ RAMDISK_PEZIK_68 ].connected ) {
                    g_uimemdump.memsrc = MEMSRC_MAPED;
                    ui_memdump_update ( );
                    return;
                };
                if ( g_uimemdump.pezik_68_bank > 7 ) g_uimemdump.pezik_68_bank = 0;
                gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( spinbutton ), 0, 7 );
                gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( spinbutton ), g_uimemdump.pezik_68_bank );
            } else if ( g_uimemdump.memsrc == MEMSRC_PEZIK_E8 ) {
                gtk_widget_show ( endianness_combo );
                if ( !g_ramdisk.pezik [ RAMDISK_PEZIK_E8 ].connected ) {
                    g_uimemdump.memsrc = MEMSRC_MAPED;
                    ui_memdump_update ( );
                    return;
                };
                if ( g_uimemdump.pezik_e8_bank > 7 ) g_uimemdump.pezik_e8_bank = 0;
                gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( spinbutton ), 0, 7 );
                gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( spinbutton ), g_uimemdump.pezik_e8_bank );
            } else if ( g_uimemdump.memsrc == MEMSRC_MZ1R18 ) {
                if ( !g_ramdisk.std.connected ) {
                    g_uimemdump.memsrc = MEMSRC_MAPED;
                    ui_memdump_update ( );
                    return;
                };
                g_uimemdump.mr1z18_bank &= g_ramdisk.std.size;
                if ( g_ramdisk.std.size == RAMDISK_SIZE_64 ) {
                    gtk_widget_hide ( label );
                    gtk_widget_hide ( spinbutton );
                } else {
                    gtk_spin_button_set_range ( GTK_SPIN_BUTTON ( spinbutton ), 0, g_ramdisk.std.size );
                    gtk_spin_button_set_value ( GTK_SPIN_BUTTON ( spinbutton ), g_uimemdump.mr1z18_bank );
                };
            };
        };
    } else {
        gtk_widget_hide ( label );
        gtk_widget_hide ( spinbutton );
    };

    gtk_toggle_button_set_active ( ui_get_toggle ( "dbg_memdump_comparativemode_checkbutton" ), g_uimemdump.comparative_mode );

    gtk_combo_box_set_active ( ui_get_combo_box ( "dbg_memdump_charset_comboboxtext" ), g_uimemdump.charset );

    UNLOCK_UICALLBACKS ( );
}


void ui_memdump_load ( void ) {

    ui_memdump_update ( );

    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_memdump_liststore" ) );

    unsigned addr = 0x0000;
    unsigned addr_end = UI_MEMDUMP_MAXDUMP_LENGTH;

    uint8_t *MEM = NULL;

    switch ( g_uimemdump.memsrc ) {

        case MEMSRC_MAPED:
            break;

        case MEMSRC_RAM:
            MEM = g_memory.RAM;
            break;

        case MEMSRC_VRAM:
            addr_end = 0x2000;
            switch ( g_uimemdump.vram_plane ) {
                case 0:
                    MEM = g_memoryVRAM_I;
                    break;
                case 1:
                    MEM = g_memoryVRAM_II;
                    break;
                case 2:
                    MEM = g_memoryVRAM_III;
                    break;
                case 3:
                    MEM = g_memoryVRAM_IV;
                    break;
            };
            break;

        case MEMSRC_PEZIK_E8:

            MEM = &g_ramdisk.pezik [ RAMDISK_PEZIK_E8 ].memory [ g_uimemdump.pezik_e8_bank << 16 ];
            break;

        case MEMSRC_PEZIK_68:
            MEM = &g_ramdisk.pezik [ RAMDISK_PEZIK_68 ].memory [ g_uimemdump.pezik_68_bank << 16 ];
            break;

        case MEMSRC_MZ1R18:
            MEM = &g_ramdisk.std.memory [ g_uimemdump.mr1z18_bank << 16 ];
            break;
    };

    static int last_dump_src = -1;

    GtkTreeIter iter;

    gboolean comparative_mode = FALSE;

    if ( last_dump_src != g_uimemdump.memsrc ) {
        if ( ( last_dump_src == -1 ) || ( g_uimemdump.memsrc == MEMSRC_VRAM ) || ( last_dump_src == MEMSRC_VRAM ) ) {
            gtk_list_store_clear ( GTK_LIST_STORE ( model ) );
            last_dump_src = -1;
        };
    } else {
        comparative_mode = g_uimemdump.comparative_mode;
    };

    do {
        char addr_txt [ 6 ];
        sprintf ( addr_txt, "%04X:", addr );
        unsigned i;
        char values_row [ ( 16 * ( sizeof ( UI_MEMDUMP_HEXVAL_CHANGED ) + 1 ) ) + 1 + 1 ];
        char ascii_row [ ( 16 * 1 ) + 1 ];
        *values_row = 0x00;
        ascii_row [ 16 ] = 0x00;

        for ( i = 0; i < 16; i++ ) {

            Z80EX_BYTE value;

            if ( g_uimemdump.memsrc == MEMSRC_MAPED ) {
                value = debugger_memory_read_byte ( addr + i );
            } else {
                if ( ( ( g_uimemdump.memsrc == MEMSRC_PEZIK_E8 ) || ( g_uimemdump.memsrc == MEMSRC_PEZIK_68 ) ) && ( g_uimemdump.pezik_endianity == 1 ) ) {
                    /* pezik low endian */
                    unsigned le_addr = addr + i;
                    value = MEM [ ( ( le_addr & 0xff ) << 8 ) | ( le_addr >> 8 ) ];
                } else {
                    /* standardni endianita adresovani + pezik big endian */
                    value = MEM [ addr + i ];
                };
            };

            gboolean value_changed = ( ( comparative_mode ) && ( g_uimemdump.dump_data[ addr + i ] != value ) ) ? TRUE : FALSE;

            if ( i != 0 ) {
                strcat ( values_row, "  " );
            };

            char value_txt [ sizeof (UI_MEMDUMP_HEXVAL_CHANGED ) ];

            if ( !value_changed ) {
                sprintf ( value_txt, "%02X", value );
            } else {
                sprintf ( value_txt, UI_MEMDUMP_HEXVAL_CHANGED, value );
            };

            strcat ( values_row, value_txt );

            if ( i == 7 ) {
                strcat ( values_row, "  " );
            };
            if ( ( value >= 0x20 ) && ( value < 0x7f ) ) {
                if ( g_uimemdump.charset == MEMCHAR_ASCII ) {
                    ascii_row [ i ] = value;
                } else {
                    ascii_row [ i ] = sharpmz_cnv_from ( value );
                };
            } else {
                ascii_row [ i ] = '.';
            };

            g_uimemdump.dump_data[ addr + i ] = value;
        };

        //printf ( "%s: %s\n", addr_txt, values_row );

        if ( last_dump_src == -1 ) {
            gtk_list_store_append ( GTK_LIST_STORE ( model ), &iter );
        } else {
            if ( addr == 0 ) {
                gtk_tree_model_get_iter_first ( model, &iter );
            } else {
                gtk_tree_model_iter_next ( model, &iter );
            };
        };

        gtk_list_store_set ( GTK_LIST_STORE ( model ), &iter,
                             DBG_MEMDUMP_ADDR, addr,
                             DBG_MEMDUMP_ADDR_TXT, addr_txt,
                             DBG_MEMDUMP_VALUES, values_row,
                             DBG_MEMDUMP_ASCII, ascii_row,
                             -1 );
        addr += i;
    } while ( addr != addr_end );

    last_dump_src = g_uimemdump.memsrc;
}


void ui_memdump_show_window ( void ) {

    GtkWidget *window = ui_get_widget ( "dbg_memdump_window" );
    if ( gtk_widget_get_visible ( window ) ) return;

    static int initialised = 0;
    if ( initialised == 0 ) {
        initialised = 1;
        ui_main_setpos ( &g_uimemdump.main_pos, -1, -1 );
        g_uimemdump.charset = MEMCHAR_ASCII;
        g_uimemdump.memsrc = MEMSRC_MAPED;
        g_uimemdump.mr1z18_bank = 0;
        g_uimemdump.pezik_68_bank = 0;
        g_uimemdump.pezik_e8_bank = 0;
        g_uimemdump.pezik_endianity = 0;
        g_uimemdump.mr1z18_bank = 0;
        g_uimemdump.vram_plane = 0;
        g_uimemdump.comparative_mode = TRUE;
        memset ( &g_uimemdump.dump_data, 0x00, sizeof ( g_uimemdump.dump_data ) );
    };
    ui_main_win_move_to_pos ( GTK_WINDOW ( window ), &g_uimemdump.main_pos );
    gtk_widget_show ( window );
    ui_memdump_load ( );
}


void ui_memdump_hide_window ( void ) {
    GtkWidget *window = ui_get_widget ( "dbg_memdump_window" );
    ui_main_win_get_pos ( GTK_WINDOW ( window ), &g_uimemdump.main_pos );
    gtk_widget_hide ( window );
}


void ui_memdump_show_hide_window ( void ) {
    GtkWidget *window = ui_get_widget ( "dbg_memdump_window" );
    if ( gtk_widget_get_visible ( window ) ) {
        ui_memdump_hide_window ( );
    } else {
        ui_memdump_show_window ( );
    };
}


/*
 * 
 * Callbacks
 * 
 */

G_MODULE_EXPORT gboolean on_dbg_memdump_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_memdump_hide_window ( );
    return TRUE;
}


G_MODULE_EXPORT gboolean on_dbg_memdump_window_key_press_event ( GtkWidget *widget, GdkEventKey *event, gpointer user_data ) {
    if ( event->keyval == GDK_KEY_Escape ) {
        ui_memdump_hide_window ( );
        return TRUE;
    };
    return FALSE;
}


G_MODULE_EXPORT void on_dbg_memdump_refresh_button_clicked ( GtkButton *button, gpointer user_data ) {
    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_src_comboboxtext_changed ( GtkComboBox *combobox, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;

    en_MEMSRC memsrc = gtk_combo_box_get_active ( combobox );

    unsigned not_connected = 0;

    if ( memsrc == MEMSRC_PEZIK_68 ) {
        not_connected = ~g_ramdisk.pezik [ RAMDISK_PEZIK_68 ].connected & 1;
    } else if ( memsrc == MEMSRC_PEZIK_E8 ) {
        not_connected = ~g_ramdisk.pezik [ RAMDISK_PEZIK_E8 ].connected & 1;
    } else if ( memsrc == MEMSRC_MZ1R18 ) {
        not_connected = ~g_ramdisk.std.connected & 1;
    };

    if ( not_connected ) {
        ui_show_error ( "This memory disc device is not connected!" );
        ui_memdump_update ( );
        return;
    };

    g_uimemdump.memsrc = memsrc;
    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_charset_comboboxtext_changed ( GtkComboBox *combobox, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;

    g_uimemdump.charset = gtk_combo_box_get_active ( combobox );
    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_bank_spinbutton_value_changed ( GtkSpinButton *spinbutton, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;

    unsigned value = gtk_spin_button_get_value_as_int ( GTK_SPIN_BUTTON ( spinbutton ) );

    if ( g_uimemdump.memsrc == MEMSRC_VRAM ) {
        g_uimemdump.vram_plane = value;
    } else if ( g_uimemdump.memsrc == MEMSRC_PEZIK_68 ) {
        g_uimemdump.pezik_68_bank = value;
    } else if ( g_uimemdump.memsrc == MEMSRC_PEZIK_E8 ) {
        g_uimemdump.pezik_e8_bank = value;
    } else if ( g_uimemdump.memsrc == MEMSRC_MZ1R18 ) {
        g_uimemdump.mr1z18_bank = value;
    };

    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_pezik_endianness_comboboxtext_changed ( GtkComboBox *combobox, gpointer user_data ) {
    if ( TEST_UICALLBACKS_LOCKED ) return;

    g_uimemdump.pezik_endianity = gtk_combo_box_get_active ( combobox );
    ui_memdump_load ( );
}


G_MODULE_EXPORT void on_dbg_memdump_comparativemode_checkbutton_toggled ( GtkToggleButton *togglebutton, gpointer user_data ) {
    g_uimemdump.comparative_mode = gtk_toggle_button_get_active ( togglebutton );
}


G_MODULE_EXPORT void on_dbg_memdump_save_toolbutton_clicked ( GtkToolButton *toolbutton, gpointer user_data ) {
    (void) toolbutton;
    (void) user_data;
    ui_memdump_memsave_window_show ( );
}


G_MODULE_EXPORT void on_dbg_memdump_load_bin_toolbutton_clicked ( GtkToolButton *toolbutton, gpointer user_data ) {
    (void) toolbutton;
    (void) user_data;
    printf ( "%s() - not implemented\n", __func__ );
}


G_MODULE_EXPORT void on_dbg_memdump_load_mzf_toolbutton_clicked ( GtkToolButton *toolbutton, gpointer user_data ) {
    (void) toolbutton;
    (void) user_data;
    printf ( "%s() - not implemented\n", __func__ );
}


/*
 * 
 * Memsave
 * 
 */


static gboolean g_ui_memdump_memsave_memaloc_lock = FALSE;
static gboolean g_ui_memdump_memsave_initialized = FALSE;
static char g_ui_memdump_memsave_last_mzf_cmnt[MZF_CMNT_LENGTH + 1];
static volatile gboolean g_ui_memdump_memsave_textbuffer_lock = FALSE;


static void ui_memdump_memsave_save_button_sensitivity ( void ) {
    GtkWidget *button = ui_get_widget ( "dbg_memsave_save_button" );
    Z80EX_WORD size = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ) ) );
    gboolean save_enabled = ( size ) ? TRUE : FALSE;
    gtk_widget_set_sensitive ( button, save_enabled );
}


static void ui_memdump_memsave_window_hide ( void ) {
    GtkWidget *window = ui_get_widget ( "dbg_memsave_window" );
    gtk_widget_hide ( window );
}


G_MODULE_EXPORT gboolean on_dbg_memsave_window_delete_event ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    ui_memdump_memsave_window_hide ( );
    return TRUE;
}


G_MODULE_EXPORT void on_dbg_memsave_close_button_clicked ( GtkButton *button, gpointer data ) {
    (void) button;
    (void) data;
    ui_memdump_memsave_window_hide ( );
}


static int ui_memdump_memsave_save_file ( char *filename, gboolean add_mzfheader ) {

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
        mzfhdr.ftype = (Z80EX_BYTE) debuger_text_to_z80_word ( gtk_entry_get_text ( entry ) );

        entry = ui_get_entry ( "dbg_memsave_mzfhdr_fstrt_entry" );
        if ( !gtk_entry_get_text_length ( entry ) ) {
            gtk_entry_set_text ( entry, "0000" );
        };
        mzfhdr.fstrt = debuger_text_to_z80_word ( gtk_entry_get_text ( entry ) );

        entry = ui_get_entry ( "dbg_memsave_mzfhdr_fexec_entry" );
        if ( !gtk_entry_get_text_length ( entry ) ) {
            gtk_entry_set_text ( entry, "0000" );
        };
        mzfhdr.fexec = debuger_text_to_z80_word ( gtk_entry_get_text ( entry ) );

        mzfhdr.fsize = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_mzfhdr_fsize_entry" ) ) );

        entry = ui_get_entry ( "dbg_memsave_mzfhdr_fname_entry" );
        if ( !gtk_entry_get_text_length ( entry ) ) {
            gtk_entry_set_text ( entry, "FILENAME" );
        };
        mzf_tools_set_fname ( &mzfhdr, (char*) gtk_entry_get_text ( entry ) );

        GtkWidget *view = ui_get_widget ( "dbg_memsave_mzfhdr_cmnt_textview" );
        GtkTextBuffer *buffer = gtk_text_view_get_buffer ( GTK_TEXT_VIEW ( view ) );
        GtkTextIter start;
        GtkTextIter end;
        gtk_text_buffer_get_iter_at_line ( buffer, &start, 0 );
        gtk_text_buffer_get_iter_at_line ( buffer, &end, gtk_text_buffer_get_line_count ( buffer ) );
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

    Z80EX_WORD addr = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_from_entry" ) ) );
    Z80EX_WORD size = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ) ) );

    Z80EX_WORD mem_max = 0xffff;
    en_MEMSRC memsrc = gtk_combo_box_get_active ( ui_get_combo_box ( "dbg_memdump_src_comboboxtext" ) );
    if ( memsrc == MEMSRC_VRAM ) {
        mem_max = 0x1fff;
    };

    while ( size ) {

        uint32_t i = addr + size;
        uint32_t save_size = size;

        if ( i > mem_max ) {
            save_size = mem_max - addr;
        };

        generic_driver_write ( h, offset, &g_uimemdump.dump_data[addr], save_size );

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

    ui_memdump_memsave_window_hide ( );

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
                                           ui_get_window ( "dbg_memdump_window" ),
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
        ret = ui_memdump_memsave_save_file ( filename, add_mzfheader );
        g_free ( filename );
    };

    gtk_widget_destroy ( dialog );

    if ( ret == EXIT_FAILURE ) {
        ui_memdump_memsave_window_show ( );
    };
}


static void ui_memdump_memsave_add_mzfheader_changed ( void ) {
    GtkWidget *mzfheader_grid = ui_get_widget ( "dbg_memsave_mzfheader_grid" );
    GtkToggleButton *togglebutton = GTK_TOGGLE_BUTTON ( ui_get_widget ( "dbg_memsave_add_mzfheader_checkbutton" ) );
    gtk_widget_set_sensitive ( mzfheader_grid, gtk_toggle_button_get_active ( togglebutton ) );
}


G_MODULE_EXPORT void on_dbg_memsave_add_mzfheader_checkbutton_toggled ( GtkToggleButton *togglebutton, gpointer user_data ) {
    (void) togglebutton;
    (void) user_data;
    ui_memdump_memsave_add_mzfheader_changed ( );
}


static void ui_memdump_memsave_set_hex_size ( Z80EX_WORD size ) {
    g_ui_memdump_memsave_memaloc_lock = TRUE;
    char buff[5];
    snprintf ( buff, sizeof ( buff ), "%04X", size );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ), buff );
    g_ui_memdump_memsave_memaloc_lock = FALSE;
    ui_memdump_memsave_save_button_sensitivity ( );
}


static void ui_memdump_memsave_set_dec_size ( Z80EX_WORD size ) {
    g_ui_memdump_memsave_memaloc_lock = TRUE;
    char buff[6];
    snprintf ( buff, sizeof ( buff ), "%d", size );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_dec_entry" ), buff );
    g_ui_memdump_memsave_memaloc_lock = FALSE;
    ui_memdump_memsave_save_button_sensitivity ( );
}


static void ui_memdump_memsave_set_size ( Z80EX_WORD size ) {
    ui_memdump_memsave_set_hex_size ( size );
    ui_memdump_memsave_set_dec_size ( size );
}


static void ui_memdump_memsave_set_to ( Z80EX_WORD to ) {
    g_ui_memdump_memsave_memaloc_lock = TRUE;
    char buff[5];
    snprintf ( buff, sizeof ( buff ), "%04X", to );
    gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_to_entry" ), buff );
    g_ui_memdump_memsave_memaloc_lock = FALSE;
}


G_MODULE_EXPORT void on_dbg_memsave_from_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memdump_memsave_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    if ( !gtk_entry_get_text_length ( ui_get_entry ( "dbg_memsave_to_entry" ) ) ) {
        ui_memdump_memsave_save_button_sensitivity ( );
        return;
    };

    Z80EX_WORD addr_from = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_from_entry" ) ) );
    Z80EX_WORD addr_to = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_to_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    en_MEMSRC memsrc = gtk_combo_box_get_active ( ui_get_combo_box ( "dbg_memdump_src_comboboxtext" ) );
    if ( memsrc == MEMSRC_VRAM ) {
        mask = 0x1fff;
        if ( addr_from > 0x1fff ) {
            addr_from = 0x1fff;
            g_ui_memdump_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_from_entry" ), "1FFF" );
            g_ui_memdump_memsave_memaloc_lock = FALSE;
        };
    };

    ui_memdump_memsave_set_size ( ( addr_to - addr_from ) & mask );
}


static Z80EX_WORD ui_memdump_memsave_get_from ( void ) {
    if ( !gtk_entry_get_text_length ( ui_get_entry ( "dbg_memsave_from_entry" ) ) ) {
        g_ui_memdump_memsave_memaloc_lock = TRUE;
        gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_from_entry" ), "0000" );
        g_ui_memdump_memsave_memaloc_lock = FALSE;
    };

    return debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_from_entry" ) ) );
}


G_MODULE_EXPORT void on_dbg_memsave_to_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memdump_memsave_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    Z80EX_WORD addr_from = ui_memdump_memsave_get_from ( );
    Z80EX_WORD addr_to = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_to_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    en_MEMSRC memsrc = gtk_combo_box_get_active ( ui_get_combo_box ( "dbg_memdump_src_comboboxtext" ) );
    if ( memsrc == MEMSRC_VRAM ) {
        mask = 0x1fff;
        if ( addr_to > 0x1fff ) {
            addr_to = 0x1fff;
            g_ui_memdump_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_to_entry" ), "1FFF" );
            g_ui_memdump_memsave_memaloc_lock = FALSE;
        };
    };

    ui_memdump_memsave_set_size ( ( addr_to - addr_from ) & mask );
}


G_MODULE_EXPORT void on_dbg_memsave_size_hex_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memdump_memsave_memaloc_lock ) return;
    ui_hexeditable_changed ( ed, user_data );

    Z80EX_WORD addr_from = ui_memdump_memsave_get_from ( );
    Z80EX_WORD size = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    en_MEMSRC memsrc = gtk_combo_box_get_active ( ui_get_combo_box ( "dbg_memdump_src_comboboxtext" ) );
    if ( memsrc == MEMSRC_VRAM ) {
        mask = 0x1fff;
        if ( size > 0x1fff ) {
            size = 0x1fff;
            g_ui_memdump_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_hex_entry" ), "1FFF" );
            g_ui_memdump_memsave_memaloc_lock = FALSE;
        };
    };

    ui_memdump_memsave_set_to ( ( addr_from + size ) & mask );
    ui_memdump_memsave_set_dec_size ( size );
}


G_MODULE_EXPORT void on_dbg_memsave_size_dec_entry_changed ( GtkEditable *ed, gpointer user_data ) {
    if ( g_ui_memdump_memsave_memaloc_lock ) return;

    Z80EX_WORD addr_from = ui_memdump_memsave_get_from ( );
    int atoi_size = atoi ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_size_dec_entry" ) ) );

    Z80EX_WORD mask = 0xffff;
    en_MEMSRC memsrc = gtk_combo_box_get_active ( ui_get_combo_box ( "dbg_memdump_src_comboboxtext" ) );
    if ( memsrc == MEMSRC_VRAM ) {
        mask = 0x1fff;
        if ( atoi_size > 0x1fff ) {
            atoi_size = 0x1fff;
            g_ui_memdump_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_dec_entry" ), "8191" );
            g_ui_memdump_memsave_memaloc_lock = FALSE;
        };
    } else {
        if ( atoi_size > 0xffff ) {
            atoi_size = 0xffff;
            g_ui_memdump_memsave_memaloc_lock = TRUE;
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_size_dec_entry" ), "65535" );
            g_ui_memdump_memsave_memaloc_lock = FALSE;
        };
    }

    ui_memdump_memsave_set_to ( ( addr_from + (Z80EX_WORD) atoi_size ) & mask );
    ui_memdump_memsave_set_hex_size ( (Z80EX_WORD) atoi_size );
}


static gboolean display_status_textbuffer ( gpointer user_data ) {
    g_ui_memdump_memsave_textbuffer_lock = TRUE;
    GtkWidget *view = ui_get_widget ( "dbg_memsave_mzfhdr_cmnt_textview" );
    GtkTextBuffer *buffer = gtk_text_view_get_buffer ( GTK_TEXT_VIEW ( view ) );
    gtk_text_buffer_set_text ( buffer, g_ui_memdump_memsave_last_mzf_cmnt, -1 );
    g_ui_memdump_memsave_textbuffer_lock = FALSE;
    return G_SOURCE_REMOVE;
}


static void on_ui_memdump_memsave_mzfhdr_cmnt_buffer_changed ( GtkTextBuffer *textbuffer, gpointer user_data ) {
    if ( g_ui_memdump_memsave_textbuffer_lock ) return;

    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_iter_at_line ( textbuffer, &start, 0 );
    gtk_text_buffer_get_iter_at_line ( textbuffer, &end, gtk_text_buffer_get_line_count ( textbuffer ) );
    char *new_text = gtk_text_buffer_get_text ( textbuffer, &start, &end, FALSE );
    int new_length = strlen ( new_text );

    if ( new_length > MZF_CMNT_LENGTH ) {
        new_length = strlen ( g_ui_memdump_memsave_last_mzf_cmnt );
    } else {
        strncpy ( g_ui_memdump_memsave_last_mzf_cmnt, new_text, sizeof ( g_ui_memdump_memsave_last_mzf_cmnt ) );
    };
    g_free ( new_text );

    char buff[4];
    snprintf ( buff, sizeof ( buff ), "%d", MZF_CMNT_LENGTH - new_length );
    gtk_label_set_text ( ui_get_label ( "dbg_memsave_cmnt_bytes_remaining_label" ), buff );

    gdk_threads_add_idle ( display_status_textbuffer, NULL );
}


void ui_memdump_memsave_window_show ( void ) {
    g_ui_memdump_memsave_memaloc_lock = FALSE;
    if ( !g_ui_memdump_memsave_initialized ) {
        memset ( g_ui_memdump_memsave_last_mzf_cmnt, 0x00, sizeof ( g_ui_memdump_memsave_last_mzf_cmnt ) );
        GtkWidget *view = ui_get_widget ( "dbg_memsave_mzfhdr_cmnt_textview" );
        GtkTextBuffer *buffer = gtk_text_view_get_buffer ( GTK_TEXT_VIEW ( view ) );
        g_signal_connect ( G_OBJECT ( buffer ), "changed",
                           G_CALLBACK ( on_ui_memdump_memsave_mzfhdr_cmnt_buffer_changed ), NULL );
        GtkToggleButton *add_mzfheader = GTK_TOGGLE_BUTTON ( ui_get_widget ( "dbg_memsave_add_mzfheader_checkbutton" ) );
        gtk_toggle_button_set_active ( add_mzfheader, TRUE );
        GtkToggleButton *convert_mzfcomment = GTK_TOGGLE_BUTTON ( ui_get_widget ( "dbg_memsave_convert_mzfcomment_checkbutton" ) );
        gtk_toggle_button_set_active ( convert_mzfcomment, TRUE );
        g_ui_memdump_memsave_initialized = TRUE;
    };

    en_MEMSRC memsrc = gtk_combo_box_get_active ( ui_get_combo_box ( "dbg_memdump_src_comboboxtext" ) );
    if ( memsrc == MEMSRC_VRAM ) {

        Z80EX_WORD addr_from = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_from_entry" ) ) );
        if ( addr_from > 0x1fff ) {
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_from_entry" ), "1FFF" );
        };

        Z80EX_WORD addr_to = debuger_text_to_z80_word ( gtk_entry_get_text ( ui_get_entry ( "dbg_memsave_to_entry" ) ) );
        if ( addr_to > 0x1fff ) {
            gtk_entry_set_text ( ui_get_entry ( "dbg_memsave_to_entry" ), "1FFF" );
        };
    }

    ui_memdump_memsave_save_button_sensitivity ( );

    GtkWidget *window = ui_get_widget ( "dbg_memsave_window" );
    gtk_widget_grab_focus ( ui_get_widget ( "dbg_memsave_from_entry" ) );
    ui_memdump_memsave_add_mzfheader_changed ( );
    gtk_widget_show ( window );
}

#endif

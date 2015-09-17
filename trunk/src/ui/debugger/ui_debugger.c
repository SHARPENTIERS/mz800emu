/* 
 * File:   ui_debugger.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. srpna 2015, 16:20
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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#ifdef MZ800_DEBUGGER


#include "ui_debugger.h"
#include "ui/ui_main.h"
#include "debugger/debugger.h"

#include "mz800.h"
#include "z80ex/include/z80ex.h"
#include "z80ex/include/z80ex_dasm.h"
#include "gdg/gdg.h"
#include "memory/memory.h"


#define DEBUGGER_MMAP_COLOR_RAM        "green"
#define DEBUGGER_MMAP_COLOR_ROM        "red"
#define DEBUGGER_MMAP_COLOR_CGROM      "black"
#define DEBUGGER_MMAP_COLOR_CGRAM      "white"
#define DEBUGGER_MMAP_COLOR_VRAM       "pink"
#define DEBUGGER_MMAP_COLOR_PORTS      "blue"

#define DEBUGGER_STACK_ROWS             20
#define DEBUGGER_DISASSEMBLED_ROWS      30
#define DEBUGGER_MNEMONIC_MAXLEN        20


struct st_UIDEBUGGER g_uidebugger;

/* Glade momentalne neumi spravne vyrobit funkcni scale, takze jej produkujeme cely zde */
static GtkWidget *dbg_disassembled_addr_vscale = NULL;


void ui_debugger_update_flag_reg ( void ) {

    char checkbutton_name[] = "dbg_flagreg_bit0_checkbutton";
    uint8_t flag_reg = z80ex_get_reg ( g_mz800.cpu, regAF ) & 0xff;
    unsigned i;

    LOCK_UICALLBACKS ( );

    for ( i = 0; i < 8; i++ ) {
        checkbutton_name [ 15 ] = '0' + i;
        gboolean state = ( flag_reg & 1 ) ? TRUE : FALSE;
        gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( ui_get_check_button ( checkbutton_name ) ), state );
        flag_reg = flag_reg >> 1;
    };

    UNLOCK_UICALLBACKS ( );
}


gboolean ui_debugger_update_register ( GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data ) {
    gint row = gtk_tree_path_get_indices ( path )[0];
    GValue gv = G_VALUE_INIT;
    gtk_tree_model_get_value ( model, iter, DBG_REG_ID, &gv );
    char value_txt [5];
    if ( row != 6 ) {
        sprintf ( value_txt, "%04X", z80ex_get_reg ( g_mz800.cpu, g_value_get_uint ( &gv ) ) );
    } else {
        sprintf ( value_txt, "%02X", z80ex_get_reg ( g_mz800.cpu, g_value_get_uint ( &gv ) ) & 0xff );
    };
    gtk_list_store_set ( (GtkListStore*) model, iter, DBG_REG_VALUE, value_txt, -1 );
    return FALSE;
}


void ui_debugger_update_registers ( void ) {
    gtk_tree_model_foreach ( GTK_TREE_MODEL ( ui_get_object ( "dbg_reg0_liststore" ) ), ui_debugger_update_register, NULL );
    gtk_tree_model_foreach ( GTK_TREE_MODEL ( ui_get_object ( "dbg_reg1_liststore" ) ), ui_debugger_update_register, NULL );
}


void ui_debugger_update_internals ( void ) {

    LOCK_UICALLBACKS ( );

    /* internals: IM */
    gtk_combo_box_set_active ( ui_get_combo_box ( "dbg_im_comboboxtext" ), z80ex_get_reg ( g_mz800.cpu, regIM ) );

    gboolean state;

    /* internals: IFF1 */
    state = ( z80ex_get_reg ( g_mz800.cpu, regIFF1 ) & 1 ) ? TRUE : FALSE;
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( ui_get_check_button ( "dbg_regiff1_checkbutton" ) ), state );

    /* internals: IFF2 */
    state = ( z80ex_get_reg ( g_mz800.cpu, regIFF2 ) & 1 ) ? TRUE : FALSE;
    gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( ui_get_check_button ( "dbg_regiff2_checkbutton" ) ), state );

    /* regDMD */
    gtk_combo_box_set_active ( ui_get_combo_box ( "dbg_regdmd_comboboxtext" ), g_gdg.regDMD );

    UNLOCK_UICALLBACKS ( );
}


void ui_debugger_update_mmap ( void ) {

    static int last_map = -1;
    static int last_dmd = -1;
    GdkRGBA rgba_color;

    if ( ( g_gdg.regDMD == last_dmd ) && ( g_memory.map == last_map ) ) {
        return;
    };

    last_map = g_memory.map;
    last_dmd = g_gdg.regDMD;

    if ( MEMORY_MAP_TEST_ROM_0000 ) {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_ROM );
    } else {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
    };
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea0" ), GTK_STATE_NORMAL, &rgba_color );

    if ( MEMORY_MAP_TEST_ROM_1000 ) {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_CGROM );
    } else {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
    };
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea1" ), GTK_STATE_NORMAL, &rgba_color );

    if ( MEMORY_MAP_TEST_VRAM_8000 ) {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_VRAM );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea8" ), GTK_STATE_NORMAL, &rgba_color );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea9" ), GTK_STATE_NORMAL, &rgba_color );
    } else {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea8" ), GTK_STATE_NORMAL, &rgba_color );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea9" ), GTK_STATE_NORMAL, &rgba_color );
    };

    if ( MEMORY_MAP_TEST_VRAM_A000 ) {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_VRAM );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaA" ), GTK_STATE_NORMAL, &rgba_color );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaB" ), GTK_STATE_NORMAL, &rgba_color );
    } else {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaA" ), GTK_STATE_NORMAL, &rgba_color );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaB" ), GTK_STATE_NORMAL, &rgba_color );
    };

    if ( MEMORY_MAP_TEST_CGRAM ) {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_CGRAM );
    } else {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
    };
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaC" ), GTK_STATE_NORMAL, &rgba_color );

    if ( DMD_TEST_MZ700 ) {
        if ( MEMORY_MAP_TEST_ROM_E000 ) {
            gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_VRAM );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaD" ), GTK_STATE_NORMAL, &rgba_color );
            gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_PORTS );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaE_ports" ), GTK_STATE_NORMAL, &rgba_color );
            gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_ROM );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaE" ), GTK_STATE_NORMAL, &rgba_color );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaF" ), GTK_STATE_NORMAL, &rgba_color );
        } else {
            gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaD" ), GTK_STATE_NORMAL, &rgba_color );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaE_ports" ), GTK_STATE_NORMAL, &rgba_color );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaE" ), GTK_STATE_NORMAL, &rgba_color );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaF" ), GTK_STATE_NORMAL, &rgba_color );
        };
    } else {
        gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
        gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaD" ), GTK_STATE_NORMAL, &rgba_color );

        if ( MEMORY_MAP_TEST_ROM_E000 ) {
            gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_ROM );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaE_ports" ), GTK_STATE_NORMAL, &rgba_color );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaE" ), GTK_STATE_NORMAL, &rgba_color );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaF" ), GTK_STATE_NORMAL, &rgba_color );
        } else {
            gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaE_ports" ), GTK_STATE_NORMAL, &rgba_color );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaE" ), GTK_STATE_NORMAL, &rgba_color );
            gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingareaF" ), GTK_STATE_NORMAL, &rgba_color );
        };
    };
}


void ui_debugger_init_stack ( GtkTreeModel *model ) {
    unsigned i;
    for ( i = 0; i < DEBUGGER_STACK_ROWS; i++ ) {
        GtkTreeIter iter;
        gtk_list_store_append ( GTK_LIST_STORE ( model ), &iter );
        gtk_list_store_set ( GTK_LIST_STORE ( model ), &iter,
                DBG_STACK_ADDR, 0,
                DBG_STACK_ADDR_TXT, "",
                DBG_STACK_VALUE, "",
                -1 );
    };
}


void ui_debugger_update_stack ( void ) {

    Z80EX_WORD addr = z80ex_get_reg ( g_mz800.cpu, regSP ) - DEBUGGER_STACK_ROWS;

    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_stack_liststore" ) );

    if ( 0 == gtk_tree_model_iter_n_children ( model, NULL ) ) ui_debugger_init_stack ( model );

    GtkTreeIter iter;
    gtk_tree_model_get_iter_first ( model, &iter );

    unsigned i;
    for ( i = 0; i < DEBUGGER_STACK_ROWS; i++ ) {

        if ( i == ( DEBUGGER_STACK_ROWS / 2 ) ) {
            GtkTreeSelection *selection = gtk_tree_view_get_selection ( ui_get_tree_view ( "dbg_stack_treeview" ) );
            gtk_tree_selection_select_iter ( selection, &iter );
        } else if ( i == ( DEBUGGER_STACK_ROWS / 2 ) - 3 ) {
            GtkTreePath *path = gtk_tree_model_get_path ( model, &iter );
            gtk_tree_view_scroll_to_cell ( ui_get_tree_view ( "dbg_stack_treeview" ), path, NULL, FALSE, 0.0, 0.0 );
        };

        Z80EX_WORD value = debugger_memory_read_byte ( addr );
        value |= debugger_memory_read_byte ( addr + 1 ) << 8;

        char addr_txt [ 6 ];
        char value_txt [ 5 ];

        sprintf ( addr_txt, "%04X:", addr );
        sprintf ( value_txt, "%04X", value );

        gtk_list_store_set ( GTK_LIST_STORE ( model ), &iter,
                DBG_STACK_ADDR, addr,
                DBG_STACK_ADDR_TXT, addr_txt,
                DBG_STACK_VALUE, value_txt,
                -1 );
        addr += 2;
        gtk_tree_model_iter_next ( model, &iter );
    };

}


void ui_debugger_init_disassembled ( GtkTreeModel *model, unsigned start_row, unsigned count ) {
    unsigned i;
    unsigned end_row = start_row + count;

    /* TODO: casem nejaky assert */
    if ( !( start_row < end_row ) ) return;

    for ( i = start_row; i != end_row; i++ ) {
        GtkTreeIter iter;
        gtk_list_store_append ( GTK_LIST_STORE ( model ), &iter );
        gtk_list_store_set ( GTK_LIST_STORE ( model ), &iter,
                DBG_DIS_ADDR, 0,
                DBG_DIS_ADDR_TXT, "",
                DBG_DIS_BYTE0, "",
                DBG_DIS_BYTE1, "",
                DBG_DIS_BYTE2, "",
                DBG_DIS_BYTE3, "",
                DBG_DIS_MNEMONIC, "",
                -1 );
    };
}


/*
 * 
 * Vzdy je potreba nastavit addr.
 * row:
 *      -1 = update noveho seznamu
 *      0 - xx = update po editaci radku
 * 
 */
void ui_debugger_update_disassembled ( Z80EX_WORD addr, int row ) {

    GtkTreeModel *model = GTK_TREE_MODEL ( ui_get_object ( "dbg_disassembled_liststore" ) );

    if ( 0 == gtk_tree_model_iter_n_children ( model, NULL ) ) ui_debugger_init_disassembled ( model, 0, DEBUGGER_DISASSEMBLED_ROWS );

    GtkTreeIter iter;
    unsigned select_row = 0;

    if ( row == -1 ) {
        /* Provedeme kompletni update celeho seznamu a selectujeme prvni radek. */
        row = 0;
        gtk_tree_model_get_iter_first ( model, &iter );

    } else {
        /* Provedeme update jen od konkretniho radku a selectujeme nasledujici radek. */

        if ( row == ( DEBUGGER_DISASSEMBLED_ROWS - 1 ) ) {
            /* Jsme na konci seznamu. Prvni radek odstranime a za posledni jeden pridame */
            gtk_tree_model_get_iter_first ( model, &iter );
            gtk_list_store_remove ( GTK_LIST_STORE ( model ), &iter );
            ui_debugger_init_disassembled ( model, row, 1 );
            row--;
        };

        select_row = row + 1;

        char path_str [ 5 ];
        sprintf ( path_str, "%d", row );
        gtk_tree_model_get_iter_from_string ( model, &iter, path_str );
    };

    //gtk_range_set_value ( ui_get_range ( "dbg_disassembled_addr_vscale" ), addr );
    gtk_range_set_value ( GTK_RANGE ( dbg_disassembled_addr_vscale ), addr );

    unsigned i;
    for ( i = row; i < DEBUGGER_DISASSEMBLED_ROWS; i++ ) {

        char addr_txt [ 6 ];
        char byte0 [ 3 ];
        char byte1 [ 3 ];
        char byte2 [ 3 ];
        char byte3 [ 3 ];
        char mnemonic [ DEBUGGER_MNEMONIC_MAXLEN ];
        int t_states, t_states2;

        sprintf ( addr_txt, "%04X:", addr );
        Z80EX_WORD addr_row = addr;

        unsigned bytecode_length = z80ex_dasm ( mnemonic, DEBUGGER_MNEMONIC_MAXLEN - 1, 0, &t_states, &t_states2, debugger_dasm_read_cb, addr, NULL );

        sprintf ( byte0, "%02X", debugger_memory_read_byte ( addr++ ) );

        if ( 1 < bytecode_length ) {
            sprintf ( byte1, "%02X", debugger_memory_read_byte ( addr++ ) );
        } else {
            byte1 [ 0 ] = 0x00;
        };

        if ( 2 < bytecode_length ) {
            sprintf ( byte2, "%02X", debugger_memory_read_byte ( addr++ ) );
        } else {
            byte2 [ 0 ] = 0x00;
        };

        if ( 3 < bytecode_length ) {
            sprintf ( byte3, "%02X ", debugger_memory_read_byte ( addr++ ) );
        } else {
            byte3 [ 0 ] = 0x00;
        };

        /* mnemonics se mi ctou lepe, kdyz jsou napsany malym pismem */
        char *c = mnemonic;
        while ( *c != 0x00 ) {
            if ( ( *c >= 'A' ) && ( *c <= 'Z' ) ) {
                *c += 0x20;
            };
            c++;
        };

        gtk_list_store_set ( GTK_LIST_STORE ( model ), &iter,
                DBG_DIS_ADDR, addr_row,
                DBG_DIS_BYTES, bytecode_length,
                DBG_DIS_ADDR_TXT, addr_txt,
                DBG_DIS_BYTE0, byte0,
                DBG_DIS_BYTE1, byte1,
                DBG_DIS_BYTE2, byte2,
                DBG_DIS_BYTE3, byte3,
                DBG_DIS_MNEMONIC, mnemonic,
                -1 );

        if ( i == select_row ) {
            GtkTreeSelection *selection = gtk_tree_view_get_selection ( ui_get_tree_view ( "dbg_disassembled_treeview" ) );
            gtk_tree_selection_select_iter ( selection, &iter );
            
            
            /* 
             * TODO: tady je nejaka chybka
             *      1) provedu editaci 15. radku na nejake vysoke adrese a ulozim jej na adresu 0x0000
             *      2) udela se full update, na 1. radku je moje nova instrukce, 2. radek je oznacen - OK
             * 
             *      CHYBA:
             *         klepnu do enter a edituju aktualni radek 15 !!!
             *         pripadne klepnu do cursor down a jsem na radku 16
             */
            GtkTreePath *path = gtk_tree_model_get_path ( model, &iter );
            gtk_tree_view_scroll_to_cell ( ui_get_tree_view ( "dbg_disassembled_treeview" ), path, NULL, FALSE, 0.0, 0.0 );

        };

        gtk_tree_model_iter_next ( model, &iter );
    };
}


void ui_debugger_update_all ( void ) {
    ui_debugger_update_flag_reg ( );
    ui_debugger_update_registers ( );
    ui_debugger_update_internals ( );
    ui_debugger_update_mmap ( );
    ui_debugger_update_stack ( );
    ui_debugger_update_disassembled ( z80ex_get_reg ( g_mz800.cpu, regPC ), -1 );
}


void ui_debugger_update_animated ( void ) {
    static unsigned animate_call = 0;
    static unsigned animate_phase = 0;

    if ( animate_call++ < 2 ) return;

    switch ( animate_phase ) {

        case 0:
            ui_debugger_update_flag_reg ( );
            ui_debugger_update_registers ( );
            ui_debugger_update_internals ( );
            break;

        case 1:
            ui_debugger_update_mmap ( );
            break;

        case 2:
            ui_debugger_update_stack ( );
            break;

        case 3:
            ui_debugger_update_disassembled ( z80ex_get_reg ( g_mz800.cpu, regPC ), -1 );
            //break;

        case 4:
            animate_phase = 0;
            animate_call = 0;
            return;

    };
    animate_phase++;
}


void ui_debugger_mmap_init ( void ) {
    GdkRGBA rgba_color;
    gdk_rgba_parse ( &rgba_color, DEBUGGER_MMAP_COLOR_RAM );
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea2" ), GTK_STATE_NORMAL, &rgba_color );
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea3" ), GTK_STATE_NORMAL, &rgba_color );
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea4" ), GTK_STATE_NORMAL, &rgba_color );
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea5" ), GTK_STATE_NORMAL, &rgba_color );
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea6" ), GTK_STATE_NORMAL, &rgba_color );
    gtk_widget_override_background_color ( ui_get_widget ( "dbg_mmap_drawingarea7" ), GTK_STATE_NORMAL, &rgba_color );
}


/* Tyto callbacky jsou zde proto, ze glade momentalne neumi spravne vyrobit scale, takze jej produkujeme cely zde */
void on_dbg_disassembled_addr_vscale_adjust_bounds ( GtkRange *range, gdouble value, gpointer user_data ) {
    //printf ( "new addr: 0x%04X\n", (unsigned int) gtk_range_get_value ( GTK_RANGE ( dbg_disassembled_addr_vscale ) ) );
    ui_debugger_update_disassembled ( (Z80EX_WORD) gtk_range_get_value ( GTK_RANGE ( dbg_disassembled_addr_vscale ) ), -1 );
}


gboolean on_dbg_disassembled_addr_vscale_button_press_event ( GtkWidget *widget, GdkEventButton *event, gpointer user_data ) {
    if ( !TEST_EMULATION_PAUSED ) {
        ui_debugger_pause_emulation ( );
        return TRUE;
    };
    return FALSE;
}


void ui_debugger_show_main_window ( void ) {

    g_uidebugger.accelerators_locked = 0;

    GtkWidget *window = ui_get_widget ( "debugger_main_window" );
    gtk_widget_show ( window );
    gtk_widget_grab_focus ( window );

    /* inicializace prvku, ktere neumime udelat pres glade */
    static unsigned initialised = 0;
    if ( !initialised ) {
        initialised = 1;

        ui_debugger_mmap_init ( );

        /* Tyto vlastnosti se mi nepovedlo nastavit pomoci glade */
        g_object_set ( ui_get_object ( "dbg_reg0_value_cellrenderertext" ), "editable", TRUE, "xalign", 1.0, NULL );
        g_object_set ( ui_get_object ( "dbg_reg1_value_cellrenderertext" ), "editable", TRUE, "xalign", 1.0, NULL );
        g_object_set ( ui_get_object ( "dbg_stack_value_cellrenderertext" ), "editable", TRUE, "xalign", 1.0, NULL );

        /* scale vyrobeny pres glade nefunguje spravne */
        dbg_disassembled_addr_vscale = gtk_scale_new ( GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT ( gtk_adjustment_new ( 994.389, 0, 65535, 1, 256, 0 ) ) );
        gtk_widget_set_name ( dbg_disassembled_addr_vscale, "dbg_disassembled_addr_vscale" );
        gtk_widget_show ( dbg_disassembled_addr_vscale );
        gtk_box_pack_start ( GTK_BOX ( ui_get_object ( "dbg_disassembled_hbox" ) ), dbg_disassembled_addr_vscale, FALSE, TRUE, 2 );
        gtk_scale_set_draw_value ( GTK_SCALE ( dbg_disassembled_addr_vscale ), FALSE );

        g_signal_connect ( (gpointer) dbg_disassembled_addr_vscale, "adjust_bounds",
                G_CALLBACK ( on_dbg_disassembled_addr_vscale_adjust_bounds ),
                NULL );

        g_signal_connect ( (gpointer) dbg_disassembled_addr_vscale, "button_press_event",
                G_CALLBACK ( on_dbg_disassembled_addr_vscale_button_press_event ),
                NULL );

        g_uidebugger.last_focus_addr = 0x0000;

    };

    ui_debugger_update_all ( );


}


void ui_debugger_hide_main_window ( void ) {
    GtkWidget *window = ui_get_widget ( "debugger_main_window" );
    gtk_widget_hide ( window );
}


void ui_debugger_pause_emulation ( void ) {

    gchar msg[] = "Emulation was paused. Now you can make any changes...";
    mz800_pause_emulation ( 1 );
    GtkWidget *dialog = gtk_message_dialog_new ( NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, msg );
    gtk_window_set_position ( GTK_WINDOW ( dialog ), GTK_WIN_POS_MOUSE );
    gtk_dialog_run ( GTK_DIALOG ( dialog ) );
    gtk_widget_destroy ( dialog );
    //ui_iteration ( );
}


void ui_debugger_show_hide_mmap_menu ( void ) {
    gtk_menu_popup ( GTK_MENU ( ui_get_widget ( "dbg_mmap_menu" ) ), NULL, NULL, NULL, NULL, 0, 0 );
}


void ui_debugger_show_hide_disassembled_menu ( void ) {
    gtk_menu_popup ( GTK_MENU ( ui_get_widget ( "dbg_disassembled_menu" ) ), NULL, NULL, NULL, NULL, 0, 0 );
}

#endif

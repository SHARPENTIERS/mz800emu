/* 
 * File:   ui_debugger.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. srpna 2015, 16:19
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

#ifndef UI_DEBUGGER_H
#define UI_DEBUGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"
#include "ui/ui_main.h"


    typedef enum en_DBGREGLSTORE {
        DBG_REG_ID = 0,
        DBG_REG_NAME,
        DBG_REG_SEP,
        DBG_REG_VALUE,
        DBG_REG_COUNT
    } en_DBGREGLSTORE;


    typedef enum en_DBGSTACKLSTORE {
        DBG_STACK_ADDR = 0,
        DBG_STACK_ADDR_TXT,
        DBG_STACK_VALUE,
        DBG_STACK_COUNT
    } en_DBGSTACKLSTORE;


    typedef enum en_DBGDISLSTORE {
        DBG_DIS_ADDR = 0,
        DBG_DIS_BYTES,
        DBG_DIS_ADDR_TXT,
        DBG_DIS_BYTE0,
        DBG_DIS_BYTE1,
        DBG_DIS_BYTE2,
        DBG_DIS_BYTE3,
        DBG_DIS_MNEMONIC,
        DBG_DIS_COUNT
    } en_DBGDISLSTORE;


    typedef enum en_UI_MMBSTATE {
        MMBSTATE_RAM = 0,
        MMBSTATE_ROM,
        MMBSTATE_CGROM,
        MMBSTATE_CGRAM,
        MMBSTATE_VRAM,
        MMBSTATE_PORTS,
        MMBSTATE_COUNT
    } en_UI_MMBSTATE;


    typedef struct st_UIMMAPBANK {
        GtkWidget *drawing_area;
        GdkPixbuf *pixbuf;
        en_UI_MMBSTATE state;
    } st_UIMMAPBANK;


    typedef enum en_MMBANK {
        MMBANK_0 = 0,
        MMBANK_1,
        MMBANK_2,
        MMBANK_3,
        MMBANK_4,
        MMBANK_5,
        MMBANK_6,
        MMBANK_7,
        MMBANK_8,
        MMBANK_9,
        MMBANK_A,
        MMBANK_B,
        MMBANK_C,
        MMBANK_D,
        MMBANK_E_PORTS,
        MMBANK_E,
        MMBANK_F,
        MMBANK_COUNT
    } en_MMBANK;


#define DBG_FOCUS_ADDR_HIST_LENGTH  10


    typedef struct st_UIDEBUGGER {
        unsigned accelerators_locked; /* pri editaci zasobniku a registru blokujeme nektere akceleratory, protoze jinak by ESC zavrel cele okno */

        // inicializuje se v ui_debugger_focus_to_addr_history_propagatecfg_cb()
        Z80EX_WORD last_focus_addr;
        int focus_addr_hist_count;
        Z80EX_WORD focus_addr_history[DBG_FOCUS_ADDR_HIST_LENGTH];

        st_UIWINPOS pos;
        st_UIMMAPBANK mmapbank[MMBANK_COUNT];
    } st_UIDEBUGGER;

    extern struct st_UIDEBUGGER g_uidebugger;

    extern void ui_debugger_show_main_window ( void );
    extern void ui_debugger_hide_main_window ( void );
    extern void ui_debugger_pause_emulation ( void );

    extern void ui_debugger_show_spinner_window ( void );
    extern void ui_debugger_hide_spinner_window ( void );

    extern void ui_debugger_update_all ( void );
    extern void ui_debugger_update_animated ( void );
    extern void ui_debugger_update_mmap ( void );
    extern void ui_debugger_update_flag_reg ( void );
    extern void ui_debugger_update_registers ( void );
    extern void ui_debugger_update_stack ( void );
    extern void ui_debugger_update_disassembled ( Z80EX_WORD addr, int row );

    extern void ui_debugger_focus_to_addr_history_propagatecfg_cb ( void *e, void *data );
    extern void ui_debugger_focus_to_addr_history_save_cb ( void *e, void *data );

#ifdef __cplusplus
}
#endif

#endif /* UI_DEBUGGER_H */


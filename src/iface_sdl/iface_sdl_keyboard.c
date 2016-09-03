/* 
 * File:   iface_sdl_keyboard.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 21. června 2015, 22:45
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

#include "iface_sdl.h"

#include "ui/ui_main.h"
#include "ui/ui_cmt.h"

#include "pio8255/pio8255.h"

#include "mz800.h"

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED
#include "debugger/debugger.h"
#include "ui/debugger/ui_breakpoints.h"
#endif


//#define RESET_MZKEYBIT(bit) g_pio8255.keyboard_matrix [ g_pio8255.signal_PA_keybord_column ] &= ~ ( 1 << bit );
#define RESET_MZKEYBIT( column, bit ) g_pio8255.keyboard_matrix [ column ] &= ~ ( 1 << bit );

static int g_iface_alt_key = 0;


static inline void iface_sdl_keyboard_scan_col0 ( const Uint8 *state ) {
    /* BLANK GRAPH LIBRA ALPHA TAB ; : CR */

    if ( state [ SDL_SCANCODE_GRAVE ] ) {
        RESET_MZKEYBIT ( 0, 7 ); /* BLANK */
    };
    if ( state [ SDL_SCANCODE_CAPSLOCK ] ) {
        RESET_MZKEYBIT ( 0, 6 ); /* GRAPH */
    };
    if ( state [ SDL_SCANCODE_F9 ] ) {
        RESET_MZKEYBIT ( 0, 5 ); /* LIBRA */
    };
    if ( state [ SDL_SCANCODE_BACKSLASH ] ) {
        RESET_MZKEYBIT ( 0, 4 ); /* ALPHA */
    };
    if ( state [ SDL_SCANCODE_TAB ] ) {
        RESET_MZKEYBIT ( 0, 3 ); /* TAB */
    };
    if ( state [ SDL_SCANCODE_SEMICOLON ] ) {
        RESET_MZKEYBIT ( 0, 2 ); /* ; */
    };
    if ( state [ SDL_SCANCODE_APOSTROPHE ] ) {
        RESET_MZKEYBIT ( 0, 1 ); /* : */
    };
    /* samotny RETURN udajne zlobi na nejakem notebooku s winXP pri aktivnim NumLock */
    if ( ( state [ SDL_SCANCODE_RETURN ] ) ||
         ( state [ SDL_SCANCODE_RETURN2 ] ) ||
         ( state [ SDL_SCANCODE_KP_ENTER ] ) ) {
        RESET_MZKEYBIT ( 0, 0 ); /* CR */
    };
}


static inline void iface_sdl_keyboard_scan_col1 ( const Uint8 *state ) {
    /* Y Z @ [ ] */

    if ( state [ SDL_SCANCODE_Y ] ) {
        RESET_MZKEYBIT ( 1, 7 ); /* Y */
    };
    if ( state [ SDL_SCANCODE_Z ] ) {
        RESET_MZKEYBIT ( 1, 6 ); /* Z */
    };
    if ( state [ SDL_SCANCODE_F6 ] ) {
        RESET_MZKEYBIT ( 1, 5 ); /* @ */
    };
    if ( state [ SDL_SCANCODE_LEFTBRACKET ] ) {
        RESET_MZKEYBIT ( 1, 4 ); /* [ */
    };
    if ( state [ SDL_SCANCODE_RIGHTBRACKET ] ) {
        RESET_MZKEYBIT ( 1, 3 ); /* ] */
    };
}


static inline void iface_sdl_keyboard_scan_col2 ( const Uint8 *state ) {
    /* Q R S T U V W X */

    if ( state [ SDL_SCANCODE_Q ] ) {
        RESET_MZKEYBIT ( 2, 7 ); /* Q */
    };
    if ( state [ SDL_SCANCODE_R ] ) {
        RESET_MZKEYBIT ( 2, 6 ); /* R */
    };
    if ( state [ SDL_SCANCODE_S ] ) {
        RESET_MZKEYBIT ( 2, 5 ); /* S */
    };
    if ( state [ SDL_SCANCODE_T ] ) {
        RESET_MZKEYBIT ( 2, 4 ); /* T */
    };
    if ( state [ SDL_SCANCODE_U ] ) {
        RESET_MZKEYBIT ( 2, 3 ); /* U */
    };
    if ( state [ SDL_SCANCODE_V ] ) {
        RESET_MZKEYBIT ( 2, 2 ); /* V */
    };
    if ( state [ SDL_SCANCODE_W ] ) {
        RESET_MZKEYBIT ( 2, 1 ); /* W */
    };
    if ( state [ SDL_SCANCODE_X ] ) {
        RESET_MZKEYBIT ( 2, 0 ); /* X */
    };
}


static inline void iface_sdl_keyboard_scan_col3 ( const Uint8 *state ) {
    /* I J K L M N O P */

    if ( state [ SDL_SCANCODE_I ] ) {
        RESET_MZKEYBIT ( 3, 7 ); /* I */
    };
    if ( state [ SDL_SCANCODE_J ] ) {
        RESET_MZKEYBIT ( 3, 6 ); /* J */
    };
    if ( state [ SDL_SCANCODE_K ] ) {
        RESET_MZKEYBIT ( 3, 5 ); /* K */
    };
    if ( state [ SDL_SCANCODE_L ] ) {
        RESET_MZKEYBIT ( 3, 4 ); /* L */
    };
    if ( state [ SDL_SCANCODE_M ] ) {
        RESET_MZKEYBIT ( 3, 3 ); /* M */
    };
    if ( state [ SDL_SCANCODE_N ] ) {
        RESET_MZKEYBIT ( 3, 2 ); /* N */
    };
    if ( state [ SDL_SCANCODE_O ] ) {
        RESET_MZKEYBIT ( 3, 1 ); /* O */
    };
    if ( state [ SDL_SCANCODE_P ] ) {
        RESET_MZKEYBIT ( 3, 0 ); /* P */
    };
}


static inline void iface_sdl_keyboard_scan_col4 ( const Uint8 *state ) {
    /* A B C D E F G H  */

    if ( state [ SDL_SCANCODE_A ] ) {
        RESET_MZKEYBIT ( 4, 7 ); /* A */
    };
    if ( state [ SDL_SCANCODE_B ] ) {
        RESET_MZKEYBIT ( 4, 6 ); /* B */
    };
    if ( state [ SDL_SCANCODE_C ] ) {
        RESET_MZKEYBIT ( 4, 5 ); /* C */
    };
    if ( state [ SDL_SCANCODE_D ] ) {
        RESET_MZKEYBIT ( 4, 4 ); /* D */
    };
    if ( state [ SDL_SCANCODE_E ] ) {
        RESET_MZKEYBIT ( 4, 3 ); /* E */
    };
    if ( state [ SDL_SCANCODE_F ] ) {
        RESET_MZKEYBIT ( 4, 2 ); /* F */
    };
    if ( state [ SDL_SCANCODE_G ] ) {
        RESET_MZKEYBIT ( 4, 1 ); /* G */
    };
    if ( state [ SDL_SCANCODE_H ] ) {
        RESET_MZKEYBIT ( 4, 0 ); /* H */
    };
}


static inline void iface_sdl_keyboard_scan_col5 ( const Uint8 *state ) {
    /* 1 2 3 4 5 6 7 8 */

    if ( state [ SDL_SCANCODE_1 ] ) {
        RESET_MZKEYBIT ( 5, 7 ); /* 1 */
    };
    if ( state [ SDL_SCANCODE_2 ] ) {
        RESET_MZKEYBIT ( 5, 6 ); /* 2 */
    };
    if ( state [ SDL_SCANCODE_3 ] ) {
        RESET_MZKEYBIT ( 5, 5 ); /* 3 */
    };
    if ( state [ SDL_SCANCODE_4 ] ) {
        RESET_MZKEYBIT ( 5, 4 ); /* 4 */
    };
    if ( state [ SDL_SCANCODE_5 ] ) {
        RESET_MZKEYBIT ( 5, 3 ); /* 5 */
    };
    if ( state [ SDL_SCANCODE_6 ] ) {
        RESET_MZKEYBIT ( 5, 2 ); /* 6 */
    };
    if ( state [ SDL_SCANCODE_7 ] ) {
        RESET_MZKEYBIT ( 5, 1 ); /* 7 */
    };
    if ( state [ SDL_SCANCODE_8 ] ) {
        RESET_MZKEYBIT ( 5, 0 ); /* 8 */
    };
}


static inline void iface_sdl_keyboard_scan_col6 ( const Uint8 *state ) {
    /* \ ~ - SPACE 0 9 , . */

    if ( state [ SDL_SCANCODE_F7 ] ) {
        RESET_MZKEYBIT ( 6, 7 ); /* \ */
    };
    if ( state [ SDL_SCANCODE_EQUALS ] ) {
        RESET_MZKEYBIT ( 6, 6 ); /* ~ */
    };
    if ( state [ SDL_SCANCODE_MINUS ] ) {
        RESET_MZKEYBIT ( 6, 5 ); /* - */
    };
    if ( state [ SDL_SCANCODE_SPACE ] ) {
        RESET_MZKEYBIT ( 6, 4 ); /* SPACE */
    };
    if ( state [ SDL_SCANCODE_0 ] ) {
        RESET_MZKEYBIT ( 6, 3 ); /* 0 */
    };
    if ( state [ SDL_SCANCODE_9 ] ) {
        RESET_MZKEYBIT ( 6, 2 ); /* 9 */
    };
    if ( state [ SDL_SCANCODE_COMMA ] ) {
        RESET_MZKEYBIT ( 6, 1 ); /* , */
    };
    if ( state [ SDL_SCANCODE_PERIOD ] ) {
        RESET_MZKEYBIT ( 6, 0 ); /* . */
    };
}


static inline void iface_sdl_keyboard_scan_col7 ( const Uint8 *state ) {
    /* INST DEL UP DOWN RIGHT LEFT ? / */

    if ( state [ SDL_SCANCODE_INSERT ] ) {
        RESET_MZKEYBIT ( 7, 7 ); /* INSERT */
    };
    if ( state [ SDL_SCANCODE_DELETE ] ) {
        RESET_MZKEYBIT ( 7, 6 ); /* DELETE */
    };
    if ( state [ SDL_SCANCODE_BACKSPACE ] ) {
        RESET_MZKEYBIT ( 7, 6 ); /* DELETE */
    };
    if ( state [ SDL_SCANCODE_UP ] ) {
        RESET_MZKEYBIT ( 7, 5 ); /* UP */
    };
    if ( state [ SDL_SCANCODE_DOWN ] ) {
        RESET_MZKEYBIT ( 7, 4 ); /* DOWN */
    };
    if ( state [ SDL_SCANCODE_RIGHT ] ) {
        RESET_MZKEYBIT ( 7, 3 ); /* RIGHT */
    };
    if ( state [ SDL_SCANCODE_LEFT ] ) {
        RESET_MZKEYBIT ( 7, 2 ); /* LEFT */
    };
    if ( state [ SDL_SCANCODE_F8 ] ) {
        RESET_MZKEYBIT ( 7, 1 ); /* ? */
    };
    if ( state [ SDL_SCANCODE_SLASH ] ) {
        RESET_MZKEYBIT ( 7, 0 ); /* / */
    };
}


static inline void iface_sdl_keyboard_scan_col8 ( const Uint8 *state ) {
    /* ESC CTRL SHIFT */

    if ( state [ SDL_SCANCODE_ESCAPE ] ) {
        RESET_MZKEYBIT ( 8, 7 ); /* ESC */
    };
    if ( state [ SDL_SCANCODE_END ] ) {
        RESET_MZKEYBIT ( 8, 7 ); /* END */
    };
    if ( state [ SDL_SCANCODE_LCTRL ] ) {
        RESET_MZKEYBIT ( 8, 6 ); /* CTRL */
    };
    if ( state [ SDL_SCANCODE_LSHIFT ] ) {
        RESET_MZKEYBIT ( 8, 0 ); /* SHIFT */
    };
    if ( state [ SDL_SCANCODE_RSHIFT ] ) {
        RESET_MZKEYBIT ( 8, 0 ); /* SHIFT */
    };
}


static inline void iface_sdl_keyboard_scan_col9 ( const Uint8 *state ) {
    /* F1 F2 F3 F4 F5 */

    if ( state [ SDL_SCANCODE_F1 ] ) {
        RESET_MZKEYBIT ( 9, 7 ); /* F1 */
    };
    if ( state [ SDL_SCANCODE_F2 ] ) {
        RESET_MZKEYBIT ( 9, 6 ); /* F2 */
    };
    if ( state [ SDL_SCANCODE_F3 ] ) {
        RESET_MZKEYBIT ( 9, 5 ); /* F3 */
    };
    if ( state [ SDL_SCANCODE_F4 ] ) {
        RESET_MZKEYBIT ( 9, 4 ); /* F4 */
    };
    if ( state [ SDL_SCANCODE_F5 ] ) {
        RESET_MZKEYBIT ( 9, 3 ); /* F5 */
    };
}


void iface_sdl_full_keyboard_scan ( void ) {
    const Uint8 *state = SDL_GetKeyboardState ( NULL );
    pio8255_keyboard_matrix_reset ( );
    iface_sdl_keyboard_scan_col0 ( state );
    iface_sdl_keyboard_scan_col1 ( state );
    iface_sdl_keyboard_scan_col2 ( state );
    iface_sdl_keyboard_scan_col3 ( state );
    iface_sdl_keyboard_scan_col4 ( state );
    iface_sdl_keyboard_scan_col5 ( state );
    iface_sdl_keyboard_scan_col6 ( state );
    iface_sdl_keyboard_scan_col7 ( state );
    iface_sdl_keyboard_scan_col8 ( state );
    iface_sdl_keyboard_scan_col9 ( state );
}


void iface_sdl_keydown_event ( SDL_Event *event ) {

    if ( event->key.keysym.scancode == SDL_SCANCODE_F12 ) {
        mz800_reset ( );
#if 0
    } else if ( event.key.keysym.scancode == SDL_SCANCODE_F11 ) {
        if ( g_mz800.debug_pc == 0 ) {
            printf ( "Turn ON debug PC\n" );
        } else {
            printf ( "Turn OFF debug PC\n" );
        };
        g_mz800.debug_pc = ~g_mz800.debug_pc & 1;
#endif
    } else if ( event->key.keysym.scancode == SDL_SCANCODE_F10 ) {
        printf ( "F10 - INTERRUPT\n" );
        unsigned interrupt_ticks = z80ex_int ( g_mz800.cpu );
        if ( interrupt_ticks ) {
            /* interrupt byl prijat */
            printf ( "Interrupt received!\n" );
        } else {
            printf ( "Interrupt NOT received!\n" );
        }

    } else if ( event->key.keysym.scancode == SDL_SCANCODE_LALT ) {
        g_iface_alt_key = 1;

        /*
         * 
         *  Obsluha klavesovych zkratek ALT+xx
         * 
         */
    } else if ( g_iface_alt_key ) {

        if ( event->key.keysym.scancode == SDL_SCANCODE_C ) {
            /*
             * Virtual CMT: Alt + C
             */
            ui_cmt_window_show_hide ( );

        } else if ( event->key.keysym.scancode == SDL_SCANCODE_M ) {
            /*
             * Max speed: Alt + M
             */
            mz800_switch_emulation_speed ( ( ~g_mz800.use_max_emulation_speed ) & 0x01 );

        } else if ( event->key.keysym.scancode == SDL_SCANCODE_P ) {
            /*
             * Pause emulation: Alt + P
             */
            mz800_pause_emulation ( ( ~g_mz800.emulation_paused ) & 0x01 );

#ifdef MZ800EMU_CFG_DEBUGGER_ENABLED                    
        } else if ( event->key.keysym.scancode == SDL_SCANCODE_D ) {
            /*
             * Debugger window: Alt + D
             */
            debugger_show_hide_main_window ( );

        } else if ( event->key.keysym.scancode == SDL_SCANCODE_B ) {
            /*
             * Breakpoints window: Alt + B
             */
            ui_breakpoints_show_hide_window ( );
#endif
        };

    };
}


void iface_sdl_keyup_event ( SDL_Event *event ) {
    if ( event->key.keysym.scancode == SDL_SCANCODE_LALT ) {
        g_iface_alt_key = 0;
    };
}


void iface_sdl_pool_keyboard_events ( void ) {

    SDL_PumpEvents ( );

    SDL_Event events [ 20 ];

    int keyboard_events = SDL_PeepEvents ( events, 20, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP );

    for ( int i = 0; i < keyboard_events; i++ ) {
        SDL_Event *event = &events [ i ];

        if ( event->type == SDL_KEYDOWN ) {
            iface_sdl_keydown_event ( event );
        } else {
            iface_sdl_keyup_event ( event );
        };
    };

    if ( keyboard_events != 0 ) {
        iface_sdl_full_keyboard_scan ( );
    }

}

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

#include "iface_sdl.h"

#include "pio8255/pio8255.h"

#define RESET_MZKEYBIT(bit) g_pio8255.keyboard_matrix [ g_pio8255.signal_PA_keybord_column ] &= ~ ( 1 << bit );

void iface_sdl_keybord_scan ( void ) {

    const Uint8 *state;

    iface_sdl_pool_all_events ( );

    
#if 0
    int i, numkeys;

    state = SDL_GetKeyboardState ( &numkeys );

    for ( i = 0; i < numkeys; i++ ) {
        if ( state [ i ] ) {
            printf ( "Key: 0x%03x, %s\n", i, SDL_GetScancodeName ( i ) );
        };
    };
    return;
#endif

    state = SDL_GetKeyboardState ( NULL );

    g_pio8255.keyboard_matrix [ g_pio8255.signal_PA_keybord_column ] = 0xff;
    

    switch ( g_pio8255.signal_PA_keybord_column ) {

        case 0:
            /* BLANK GRAPH LIBRA ALPHA TAB ; : CR */

            if ( state [ SDL_SCANCODE_GRAVE ] ) {
                RESET_MZKEYBIT ( 7 ); /* BLANK */
            };
            if ( state [ SDL_SCANCODE_CAPSLOCK ] ) {
                RESET_MZKEYBIT ( 6 ); /* GRAPH */
            };
            if ( state [ SDL_SCANCODE_F9 ] ) {
                RESET_MZKEYBIT ( 5 ); /* LIBRA */
            };
            if ( state [ SDL_SCANCODE_BACKSLASH ] ) {
                RESET_MZKEYBIT ( 4 ); /* ALPHA */
            };
            if ( state [ SDL_SCANCODE_TAB ] ) {
                RESET_MZKEYBIT ( 3 ); /* TAB */
            };
            if ( state [ SDL_SCANCODE_SEMICOLON ] ) {
                RESET_MZKEYBIT ( 2 ); /* ; */
            };
            if ( state [ SDL_SCANCODE_APOSTROPHE ] ) {
                RESET_MZKEYBIT ( 1 ); /* : */
            };
            /* samotny RETURN udajne zlobi na nejakem notebooku s winXP pri aktivnim NumLock */
            if ( ( state [ SDL_SCANCODE_RETURN ] ) ||
                    ( state [ SDL_SCANCODE_RETURN2 ] ) ||
                    ( state [ SDL_SCANCODE_KP_ENTER ] ) ) {
                RESET_MZKEYBIT ( 0 ); /* CR */
            };
            break;


        case 1:
            /* Y Z @ [ ] */

            if ( state [ SDL_SCANCODE_Y ] ) {
                RESET_MZKEYBIT ( 7 ); /* Y */
            };
            if ( state [ SDL_SCANCODE_Z ] ) {
                RESET_MZKEYBIT ( 6 ); /* Z */
            };
            if ( state [ SDL_SCANCODE_F6 ] ) {
                RESET_MZKEYBIT ( 5 ); /* @ */
            };
            if ( state [ SDL_SCANCODE_LEFTBRACKET ] ) {
                RESET_MZKEYBIT ( 4 ); /* [ */
            };
            if ( state [ SDL_SCANCODE_RIGHTBRACKET ] ) {
                RESET_MZKEYBIT ( 3 ); /* ] */
            };
            break;


        case 2:
            /* Q R S T U V W X */

            if ( state [ SDL_SCANCODE_Q ] ) {
                RESET_MZKEYBIT ( 7 ); /* Q */
            };
            if ( state [ SDL_SCANCODE_R ] ) {
                RESET_MZKEYBIT ( 6 ); /* R */
            };
            if ( state [ SDL_SCANCODE_S ] ) {
                RESET_MZKEYBIT ( 5 ); /* S */
            };
            if ( state [ SDL_SCANCODE_T ] ) {
                RESET_MZKEYBIT ( 4 ); /* T */
            };
            if ( state [ SDL_SCANCODE_U ] ) {
                RESET_MZKEYBIT ( 3 ); /* U */
            };
            if ( state [ SDL_SCANCODE_V ] ) {
                RESET_MZKEYBIT ( 2 ); /* V */
            };
            if ( state [ SDL_SCANCODE_W ] ) {
                RESET_MZKEYBIT ( 1 ); /* W */
            };
            if ( state [ SDL_SCANCODE_X ] ) {
                RESET_MZKEYBIT ( 0 ); /* X */
            };
            break;


        case 3:
            /* I J K L M N O P */

            if ( state [ SDL_SCANCODE_I ] ) {
                RESET_MZKEYBIT ( 7 ); /* I */
            };
            if ( state [ SDL_SCANCODE_J ] ) {
                RESET_MZKEYBIT ( 6 ); /* J */
            };
            if ( state [ SDL_SCANCODE_K ] ) {
                RESET_MZKEYBIT ( 5 ); /* K */
            };
            if ( state [ SDL_SCANCODE_L ] ) {
                RESET_MZKEYBIT ( 4 ); /* L */
            };
            if ( state [ SDL_SCANCODE_M ] ) {
                RESET_MZKEYBIT ( 3 ); /* M */
            };
            if ( state [ SDL_SCANCODE_N ] ) {
                RESET_MZKEYBIT ( 2 ); /* N */
            };
            if ( state [ SDL_SCANCODE_O ] ) {
                RESET_MZKEYBIT ( 1 ); /* O */
            };
            if ( state [ SDL_SCANCODE_P ] ) {
                RESET_MZKEYBIT ( 0 ); /* P */
            };
            break;


        case 4:
            /* A B C D E F G H  */

            if ( state [ SDL_SCANCODE_A ] ) {
                RESET_MZKEYBIT ( 7 ); /* A */
            };
            if ( state [ SDL_SCANCODE_B ] ) {
                RESET_MZKEYBIT ( 6 ); /* B */
            };
            if ( state [ SDL_SCANCODE_C ] ) {
                RESET_MZKEYBIT ( 5 ); /* C */
            };
            if ( state [ SDL_SCANCODE_D ] ) {
                RESET_MZKEYBIT ( 4 ); /* D */
            };
            if ( state [ SDL_SCANCODE_E ] ) {
                RESET_MZKEYBIT ( 3 ); /* E */
            };
            if ( state [ SDL_SCANCODE_F ] ) {
                RESET_MZKEYBIT ( 2 ); /* F */
            };
            if ( state [ SDL_SCANCODE_G ] ) {
                RESET_MZKEYBIT ( 1 ); /* G */
            };
            if ( state [ SDL_SCANCODE_H ] ) {
                RESET_MZKEYBIT ( 0 ); /* H */
            };
            break;



        case 5:
            /* 1 2 3 4 5 6 7 8 */

            if ( state [ SDL_SCANCODE_1 ] ) {
                RESET_MZKEYBIT ( 7 ); /* 1 */
            };
            if ( state [ SDL_SCANCODE_2 ] ) {
                RESET_MZKEYBIT ( 6 ); /* 2 */
            };
            if ( state [ SDL_SCANCODE_3 ] ) {
                RESET_MZKEYBIT ( 5 ); /* 3 */
            };
            if ( state [ SDL_SCANCODE_4 ] ) {
                RESET_MZKEYBIT ( 4 ); /* 4 */
            };
            if ( state [ SDL_SCANCODE_5 ] ) {
                RESET_MZKEYBIT ( 3 ); /* 5 */
            };
            if ( state [ SDL_SCANCODE_6 ] ) {
                RESET_MZKEYBIT ( 2 ); /* 6 */
            };
            if ( state [ SDL_SCANCODE_7 ] ) {
                RESET_MZKEYBIT ( 1 ); /* 7 */
            };
            if ( state [ SDL_SCANCODE_8 ] ) {
                RESET_MZKEYBIT ( 0 ); /* 8 */
            };
            break;


        case 6:
            /* \ ~ - SPACE 0 9 , . */

            if ( state [ SDL_SCANCODE_F7 ] ) {
                RESET_MZKEYBIT ( 7 ); /* \ */
            };
            if ( state [ SDL_SCANCODE_EQUALS ] ) {
                RESET_MZKEYBIT ( 6 ); /* ~ */
            };
            if ( state [ SDL_SCANCODE_MINUS ] ) {
                RESET_MZKEYBIT ( 5 ); /* - */
            };
            if ( state [ SDL_SCANCODE_SPACE ] ) {
                RESET_MZKEYBIT ( 4 ); /* SPACE */
            };
            if ( state [ SDL_SCANCODE_0 ] ) {
                RESET_MZKEYBIT ( 3 ); /* 0 */
            };
            if ( state [ SDL_SCANCODE_9 ] ) {
                RESET_MZKEYBIT ( 2 ); /* 9 */
            };
            if ( state [ SDL_SCANCODE_COMMA ] ) {
                RESET_MZKEYBIT ( 1 ); /* , */
            };
            if ( state [ SDL_SCANCODE_PERIOD ] ) {
                RESET_MZKEYBIT ( 0 ); /* . */
            };
            break;


        case 7:
            /* INST DEL UP DOWN RIGHT LEFT ? / */

            if ( state [ SDL_SCANCODE_INSERT ] ) {
                RESET_MZKEYBIT ( 7 ); /* INSERT */
            };
            if ( state [ SDL_SCANCODE_DELETE ] ) {
                RESET_MZKEYBIT ( 6 ); /* DELETE */
            };
            if ( state [ SDL_SCANCODE_BACKSPACE ] ) {
                RESET_MZKEYBIT ( 6 ); /* DELETE */
            };
            if ( state [ SDL_SCANCODE_UP ] ) {
                RESET_MZKEYBIT ( 5 ); /* UP */
            };
            if ( state [ SDL_SCANCODE_DOWN ] ) {
                RESET_MZKEYBIT ( 4 ); /* DOWN */
            };
            if ( state [ SDL_SCANCODE_RIGHT ] ) {
                RESET_MZKEYBIT ( 3 ); /* RIGHT */
            };
            if ( state [ SDL_SCANCODE_LEFT ] ) {
                RESET_MZKEYBIT ( 2 ); /* LEFT */
            };
            if ( state [ SDL_SCANCODE_F8 ] ) {
                RESET_MZKEYBIT ( 1 ); /* ? */
            };
            if ( state [ SDL_SCANCODE_SLASH ] ) {
                RESET_MZKEYBIT ( 0 ); /* / */
            };
            break;


        case 8:
            /* ESC CTRL SHIFT */

            if ( state [ SDL_SCANCODE_ESCAPE ] ) {
                RESET_MZKEYBIT ( 7 ); /* ESC */
            };
            if ( state [ SDL_SCANCODE_END ] ) {
                RESET_MZKEYBIT ( 7 ); /* END */
            };
            if ( state [ SDL_SCANCODE_LCTRL ] ) {
                RESET_MZKEYBIT ( 6 ); /* CTRL */
            };
            if ( state [ SDL_SCANCODE_LSHIFT ] ) {
                RESET_MZKEYBIT ( 0 ); /* SHIFT */
            };
            if ( state [ SDL_SCANCODE_RSHIFT ] ) {
                RESET_MZKEYBIT ( 0 ); /* SHIFT */
            };
            break;


        case 9:
            /* F1 F2 F3 F4 F5 */

            if ( state [ SDL_SCANCODE_F1 ] ) {
                RESET_MZKEYBIT ( 7 ); /* F1 */
            };
            if ( state [ SDL_SCANCODE_F2 ] ) {
                RESET_MZKEYBIT ( 6 ); /* F2 */
            };
            if ( state [ SDL_SCANCODE_F3 ] ) {
                RESET_MZKEYBIT ( 5 ); /* F3 */
            };
            if ( state [ SDL_SCANCODE_F4 ] ) {
                RESET_MZKEYBIT ( 4 ); /* F4 */
            };
            if ( state [ SDL_SCANCODE_F5 ] ) {
                RESET_MZKEYBIT ( 3 ); /* F5 */
            };
            break;

    };

}

/* 
 * File:   pio8255.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 21. června 2015, 22:56
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

#ifndef PIO8255_H
#define PIO8255_H

#ifdef __cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"


#define CTC_AUDIO_MASK  ( g_pio8255.signal_pc00 )


    typedef struct st_PIO8255 {
        Z80EX_BYTE keyboard_matrix [ 10 ];
        Z80EX_BYTE vkbd_matrix [ 10 ];
        unsigned signal_PA; /* Vystupni port A */
        unsigned signal_PA_keybord_column; /* vzorkovani klavesnice: 0. - 3. bit (0 - 9) */
        unsigned signal_PA_joy1_enabled; /* vzorkovani JOY1: 4. bit (L) */
        unsigned signal_PA_joy2_enabled; /* vzorkovani JOY2: 5. bit (L) */
        unsigned signal_PC; /* Port C: 0 - 3 je vystup, 4 - 7 je vstup */
        unsigned signal_pc00; /* OUT: blokovani zvuku z CTC0 */
        unsigned signal_pc01; /* OUT: data do CMT */
        unsigned signal_pc02; /* OUT: blokovani preruseni z CTC2, 0 = zakazano rusit */
        unsigned signal_pc03; /* OUT: rizeni motoru CMT - zmenu provede nabezna hrana */
        unsigned signal_pc04; /* IN: stav motoru  */
        unsigned signal_pc05; /* IN: data z CMT */
    } st_PIO8255;

    extern st_PIO8255 g_pio8255;


    extern void pio8255_init ( void );
    extern void pio8255_keyboard_matrix_reset ( void );
    extern Z80EX_BYTE pio8255_read ( int addr );
    extern void pio8255_write ( int addr, Z80EX_BYTE value );

    /* sluzby poskytovane interfacem */

    /* priprava klavesove matrix */
    extern void interface_keyboard_init ( void );

    /* vrati *keyboard_matrix */
    extern Z80EX_BYTE* interface_keyboard_scan ( void );

#define PIO8255_KEYBOARD_MATRIX_RESET()  pio8255_keyboard_matrix_reset()
#define PIO8255_VKBD_MATRIX_RESET()  pio8255_vkbd_matrix_reset()

#define PIO8255_MZKEYBIT_RESET( column, bit ) g_pio8255.keyboard_matrix [ column ] &= ~ ( 1 << bit );
#define PIO8255_VKBDBIT_RESET( column, bit ) g_pio8255.vkbd_matrix [ column ] &= ~ ( 1 << bit );
#define PIO8255_VKBDBIT_SET( column, bit ) g_pio8255.vkbd_matrix [ column ] |= ( 1 << bit );


#ifdef __cplusplus
}
#endif

#endif /* PIO8255_H */


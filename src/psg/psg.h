/* 
 * File:   psg.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. července 2015, 7:32
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

#ifndef PSG_H
#define	PSG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"

    typedef enum en_PSG_CHANNELS {
        PSG_CHANNEL_0 = 0,
        PSG_CHANNEL_1,
        PSG_CHANNEL_2,
        PSG_CHANNEL_3,
        PSG_CHANNELS_COUNT
    } en_PSG_CHANNELS;

    typedef enum en_PSG_CHTYPE {
        PSG_CHTYPE_TONE, /* square wave tone generator */
        PSG_CHTYPE_NOISE, /* periodic noise | white noise */
    } en_PSG_CHTYPE;

    typedef enum en_ATTENUATOR {
        PSG_OUT_MAX = 0, /* hodnota 0, utlum 0 db = max. hlasitost */
        PSG_OUT_LVL_14, /* utlum 2 db */
        PSG_OUT_LVL_13, /* utlum 4 db */
        PSG_OUT_LVL_12, /* utlum 6 db */
        PSG_OUT_LVL_11, /* utlum 8 db */
        PSG_OUT_LVL_10, /* utlum 10 db */
        PSG_OUT_LVL_9, /* utlum 12 db */
        PSG_OUT_LVL_8, /* utlum 14 db */
        PSG_OUT_LVL_7, /* utlum 16 db */
        PSG_OUT_LVL_6, /* utlum 18 db */
        PSG_OUT_LVL_5, /* utlum 20 db */
        PSG_OUT_LVL_4, /* utlum 22 db */
        PSG_OUT_LVL_3, /* utlum 24 db */
        PSG_OUT_LVL_2, /* utlum 26 db */
        PSG_OUT_LVL_1, /* utlum 28 db */
        PSG_OUT_OFF /* hodnota 15, zvukovy vystup kanalu je vypnuty */
    } en_ATTENUATOR;

    typedef enum en_NOISE_DIV_TYPE {
        NOISE_DIV_TYPE0 = 0, /* noise divider 0x10, 6.928 kHz pri CPUDIV = 5 */
        NOISE_DIV_TYPE1, /* noise divider 0x20, 3.464 kHz pri CPUDIV = 5 */
        NOISE_DIV_TYPE2, /* noise divider 0x40, 1.732 kHz pri CPUDIV = 5 */
        NOISE_DIV_TYPE3, /* noise divider nastavit podle channel 2 */
    } en_NOISE_DIV_TYPE;

    typedef enum en_NOISE_TYPE {
        NOISE_TYPE_PERIODIC = 0,
        NOISE_TYPE_WHITE
    } en_NOISE_TYPE;

    typedef struct st_PSG_NOISE {
        en_NOISE_DIV_TYPE div_type;
        en_NOISE_TYPE type; /* periodic noise | white noise */
        en_NOISE_TYPE last_noise_type;
        uint16_t shiftregister;
    } st_PSG_NOISE;

    typedef struct st_PSG_TONE {
        unsigned divider;
        unsigned latch_divider;
    } st_PSG_TONE;

    typedef struct st_PSG_CHANNEL {
        en_PSG_CHTYPE type;
        unsigned timer;
        en_ATTENUATOR attn;
        st_PSG_TONE tone;
        st_PSG_NOISE noise;
        unsigned output_signal;
    } st_PSG_CHANNEL;

    typedef struct st_PSG {
        unsigned latch_cs;
        unsigned latch_attn;
        st_PSG_CHANNEL channel [ PSG_CHANNELS_COUNT ];
    } st_PSG;


    extern st_PSG g_psg;

    extern void psg_init ( void );
    extern void psg_step ( void );
    extern void psg_write_byte ( Z80EX_BYTE value );


#ifdef	__cplusplus
}
#endif

#endif	/* PSG_H */


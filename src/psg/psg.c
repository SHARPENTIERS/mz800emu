/* 
 * File:   psg.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. července 2015, 7:33
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

/* Emulace PSG - SN76489AN */

#include "psg.h"
#include "gdg/gdg.h"
#include "audio.h"


#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"


st_PSG g_psg;

#ifdef AUDIO_FILLBUFF_v2
st_PSG_SAMPLES g_psg_audio;
#endif

void psg_init ( void ) {

#ifdef AUDIO_FILLBUFF_v2
    g_psg_audio.samples[0].timestamp = 0;
    g_psg_audio.samples[0].value = -1;
    g_psg_audio.count = 0;
#endif

    g_psg.channel [ PSG_CHANNEL_0 ].type = PSG_CHTYPE_TONE;
    g_psg.channel [ PSG_CHANNEL_1 ].type = PSG_CHTYPE_TONE;
    g_psg.channel [ PSG_CHANNEL_2 ].type = PSG_CHTYPE_TONE;

    g_psg.channel [ PSG_CHANNEL_3 ].type = PSG_CHTYPE_NOISE;
    g_psg.channel [ PSG_CHANNEL_3 ].noise.shiftregister = 1 << 15;

    unsigned i;
    for ( i = 0; i < PSG_CHANNELS_COUNT; i++ ) {
        g_psg.channel [ i ].attn = PSG_OUT_OFF;
    };
}


void psg_write_byte ( Z80EX_BYTE value ) {

    DBGPRINTF ( DBGINF, "value = 0x%02x\n", value );

#ifdef AUDIO_FILLBUFF_v1
    psg_real_write_byte ( value );
//    printf ( "REAL WRITE: %d - %d: 0x%02x\n", gdg_compute_total_ticks ( g_gdg.elapsed_screen_ticks ), gdg_compute_total_ticks ( g_gdg.elapsed_screen_ticks ), value );
//    printf ( "REAL WRITE: %d: 0x%02x\n", gdg_compute_total_ticks ( g_gdg.elapsed_screen_ticks ), value );
#endif

#ifdef AUDIO_FILLBUFF_v2
    unsigned total_ticks = gdg_compute_total_ticks ( g_gdg.elapsed_screen_ticks );
    if ( g_psg_audio.samples[0].value != -1 ) g_psg_audio.count++;
    g_psg_audio.samples[g_psg_audio.count].timestamp = total_ticks;
    g_psg_audio.samples[g_psg_audio.count].value = value;
#endif
}

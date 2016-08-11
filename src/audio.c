/* 
 * File:   audio.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 28. července 2015, 13:38
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

#include <math.h>

#include "audio.h"
#include "psg/psg.h"
#include "gdg/gdg.h"
#include "gdg/video.h"
#include "iface_sdl/iface_sdl_audio.h"


#define PSG_DIVIDER                     ( 16 * GDGCLK2CPU_DIVIDER )
#define PSG_RESAMPLE_PERIOD             ( (unsigned) (GDGCLK_BASE / 50 ) / IFACE_AUDIO_20MS_SAMPLES )

#define AUDIO_BUF_MAX_VALUE             0xffff
#define AUDIO_SRC_CANNELS_COUNT         ( 4 + 1 )   /* Celkovy pocet audio kanalu: 4 (psg) + 1 (ctc0) */
#define AUDIO_MAXVAL_PER_CHANNEL        ( AUDIO_BUF_MAX_VALUE / AUDIO_SRC_CANNELS_COUNT )


st_AUDIO g_audio;


void audio_init ( void ) {
    g_audio.last_update = 0;
    g_audio.resample_timer = PSG_RESAMPLE_PERIOD;
    g_audio.buffer_position = 0;
    g_audio.ctc0_output = 0;
}


AUDIO_BUF_t audio_scan ( void ) {

    AUDIO_BUF_t scan_value = 0;

    unsigned channel;
    for ( channel = 0; channel < PSG_CHANNELS_COUNT; channel++ ) {
        if ( g_psg.channel [ channel ].attn != PSG_OUT_OFF ) {
            if ( g_psg.channel [ channel ].output_signal ) {
                if ( g_psg.channel [ channel ].attn == PSG_OUT_MAX ) {
                    scan_value += AUDIO_MAXVAL_PER_CHANNEL;
                } else {
                    scan_value += AUDIO_MAXVAL_PER_CHANNEL * pow ( 10, -( (float) g_psg.channel [ channel ].attn / PSG_OUT_OFF ) );
                };
            };
        };
    };
    scan_value += g_audio.ctc0_output * AUDIO_MAXVAL_PER_CHANNEL;
    
    return scan_value;
}


void audio_fill_buffer ( unsigned event_ticks ) {
    static AUDIO_BUF_t last_value = 0;

    if ( event_ticks > ( BEAM_SCREEN_TICKS - 2 ) ) {
        event_ticks = ( BEAM_SCREEN_TICKS - 2 );
    };
    
    if ( ( event_ticks - g_audio.last_update ) < PSG_DIVIDER ) return;

    do {
        psg_step ( );

        /*
         * IIR filtr:
         * 
         *  OUT [ i + 1] = OUT [ i ] + ( IN [ i + 1] - OUT [ i ]) / x
         * 
         * x = vzorkovaci_frq / ( 2 * pi * delici_frq )
         * 
         */
        last_value = last_value + ( audio_scan ( ) - last_value ) / 16;
        
        
        if ( g_audio.resample_timer <= PSG_DIVIDER ) {
            if ( g_audio.buffer_position < IFACE_AUDIO_20MS_SAMPLES ) {
                g_audio.buffer [ g_audio.buffer_position++ ] = last_value;
            };
            g_audio.resample_timer += PSG_RESAMPLE_PERIOD;
        };

        g_audio.resample_timer -= PSG_DIVIDER;
        g_audio.last_update += PSG_DIVIDER;

    } while ( g_audio.last_update < event_ticks );
}


void audio_ctc0_changed ( unsigned value, unsigned event_ticks ) {
    if ( g_audio.ctc0_output == value ) return;
    audio_fill_buffer ( event_ticks );
    g_audio.ctc0_output = value;
}


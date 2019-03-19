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
#include <stdio.h>

#include "mz800emu_cfg.h"

#include "audio.h"
#include "psg/psg.h"
#include "gdg/gdg.h"

st_AUDIO g_audio;

AUDIO_BUF_t g_attenuator_volume_value [ PSG_OUT_OFF + 1 ];

#ifdef AUDIO_FILLBUFF_v2

st_AUDIO_CTC g_audio_ctc;

#endif


void audio_init ( void ) {

#ifdef AUDIO_FILLBUFF_v2
    g_audio_ctc.samples[0].timestamp = 0;
    g_audio_ctc.samples[0].state = 0;
    g_audio_ctc.count = 0;
#endif

    g_audio.last_update = 0;
    g_audio.resample_timer = AUDIO_RESAMPLE_PERIOD;
    g_audio.buffer_position = 0;
    g_audio.ctc0_output = 0;
#ifdef MACHINE_EMU_MZ800
    g_audio.last_value = 0;
#endif
#ifdef MACHINE_EMU_MZ1500
    g_audio.last_valueL = 0;
    g_audio.last_valueR = 0;
#endif
    int i;
    for ( i = 0; i <= PSG_OUT_OFF; i++ ) {
        g_attenuator_volume_value[i] = AUDIO_MAXVAL_PER_CHANNEL * pow ( 10, -( (float) i / PSG_OUT_OFF ) );
    };
}


void audio_ctc0_changed ( unsigned value, unsigned event_ticks ) {
    if ( g_audio.ctc0_output == value ) return;

#ifdef AUDIO_FILLBUFF_v1
#ifdef MACHINE_EMU_MZ800
    audio_mz800_fill_buffer_v1 ( event_ticks );
#endif
#ifdef MACHINE_EMU_MZ1500
    audio_mz1500_fill_buffer_v1 ( event_ticks );
#endif
#endif

    g_audio.ctc0_output = value;

#ifdef AUDIO_FILLBUFF_v2
    unsigned total_ticks = gdg_compute_total_ticks ( event_ticks );
    g_audio_ctc.count++;
    g_audio_ctc.samples[g_audio_ctc.count].timestamp = total_ticks;
    g_audio_ctc.samples[g_audio_ctc.count].state = ( value ) ? AUDIO_MAXVAL_PER_CHANNEL : 0;
#endif
}


#ifdef MACHINE_EMU_MZ800


static inline AUDIO_BUF_t psg_mz800_audio_scan ( void ) {

    AUDIO_BUF_t scan_value = 0;

    unsigned channel;
    for ( channel = 0; channel < PSG_CHANNELS_COUNT; channel++ ) {
        if ( g_psg.channel [ channel ].attn != PSG_OUT_OFF ) {
            if ( g_psg.channel [ channel ].output_signal ) {
                if ( g_psg.channel [ channel ].attn == PSG_OUT_MAX ) {
                    scan_value += AUDIO_MAXVAL_PER_CHANNEL;
                } else {
                    scan_value += g_attenuator_volume_value[g_psg.channel[channel].attn];
                };
            };
        };
    };

    return scan_value;
}
#endif

#ifdef MACHINE_EMU_MZ1500


static AUDIO_BUF_t g_scan_valueL = 0;
static AUDIO_BUF_t g_scan_valueR = 0;


static inline void psg_mz1500_audio_scan ( void ) {

    g_scan_valueL = 0;
    g_scan_valueR = 0;

    unsigned channel;

    for ( channel = 0; channel < PSG_CHANNELS_COUNT; channel++ ) {
        if ( g_psgL.channel [ channel ].attn != PSG_OUT_OFF ) {
            if ( g_psgL.channel [ channel ].output_signal ) {
                if ( g_psgL.channel [ channel ].attn == PSG_OUT_MAX ) {
                    g_scan_valueL += AUDIO_MAXVAL_PER_CHANNEL;
                } else {
                    g_scan_valueL += g_attenuator_volume_value[g_psgL.channel[channel].attn];
                };
            };
        };
    };

    for ( channel = 0; channel < PSG_CHANNELS_COUNT; channel++ ) {
        if ( g_psgR.channel [ channel ].attn != PSG_OUT_OFF ) {
            if ( g_psgR.channel [ channel ].output_signal ) {
                if ( g_psgR.channel [ channel ].attn == PSG_OUT_MAX ) {
                    g_scan_valueR += AUDIO_MAXVAL_PER_CHANNEL;
                } else {
                    g_scan_valueR += g_attenuator_volume_value[g_psgR.channel[channel].attn];
                };
            };
        };
    };
}
#endif

#ifdef AUDIO_FILLBUFF_v1

#ifdef MACHINE_EMU_MZ800


void audio_mz800_fill_buffer_v1 ( unsigned event_ticks ) {

    //printf ( "fill: %d, %d, %d\n", event_ticks, g_audio.last_update, g_audio.buffer_position );

    //static AUDIO_BUF_t last_value = 0;

    if ( event_ticks > ( VIDEO_SCREEN_TICKS ) ) {
        event_ticks = ( VIDEO_SCREEN_TICKS );
    };

    if ( ( event_ticks - g_audio.last_update ) < PSG_DIVIDER ) return;

    do {

        //printf ( "step: %d\n", gdg_compute_total_ticks ( g_audio.last_update ) );

        psg_step ( &g_psg );

        /*
         * IIR filtr:
         * 
         *  OUT [ i + 1] = OUT [ i ] + ( IN [ i + 1] - OUT [ i ]) / x
         * 
         * x = vzorkovaci_frq / ( 2 * pi * delici_frq )
         * 
         */
        AUDIO_BUF_t scan_value = psg_mz800_audio_scan ( ) + g_audio.ctc0_output * AUDIO_MAXVAL_PER_CHANNEL;
        g_audio.last_value = g_audio.last_value + ( scan_value - g_audio.last_value ) / 16;


        if ( g_audio.resample_timer <= PSG_DIVIDER ) {
            if ( g_audio.buffer_position < IFACE_AUDIO_WINDOW_SAMPLES ) {
                //printf ( "res: %d = %d\n", g_audio.buffer_position, last_value );
                if ( !TEST_EMULATION_PAUSED ) {
                    g_audio.buffer [ g_audio.buffer_position ] = g_audio.last_value;
                };
                g_audio.buffer_position++;
            };
            g_audio.resample_timer += AUDIO_RESAMPLE_PERIOD;
        };

        g_audio.resample_timer -= PSG_DIVIDER;
        g_audio.last_update += PSG_DIVIDER;

    } while ( g_audio.last_update < event_ticks );
}
#endif
#ifdef MACHINE_EMU_MZ1500


void audio_mz1500_fill_buffer_v1 ( unsigned event_ticks ) {

    //printf ( "fill: %d, %d, %d\n", event_ticks, g_audio.last_update, g_audio.buffer_position );

    //static AUDIO_BUF_t last_value = 0;

    if ( event_ticks > ( VIDEO_SCREEN_TICKS ) ) {
        event_ticks = ( VIDEO_SCREEN_TICKS );
    };

    if ( ( event_ticks - g_audio.last_update ) < PSG_DIVIDER ) return;

    do {

        //printf ( "step: %d\n", gdg_compute_total_ticks ( g_audio.last_update ) );

        psg_step ( &g_psgL );
        psg_step ( &g_psgR );

        /*
         * IIR filtr:
         * 
         *  OUT [ i + 1] = OUT [ i ] + ( IN [ i + 1] - OUT [ i ]) / x
         * 
         * x = vzorkovaci_frq / ( 2 * pi * delici_frq )
         * 
         */
        psg_mz1500_audio_scan ( );

        g_scan_valueL += g_audio.ctc0_output * AUDIO_MAXVAL_PER_CHANNEL;
        g_scan_valueR += g_audio.ctc0_output * AUDIO_MAXVAL_PER_CHANNEL;

        g_audio.last_valueL = g_audio.last_valueL + ( g_scan_valueL - g_audio.last_valueL ) / 16;
        g_audio.last_valueR = g_audio.last_valueR + ( g_scan_valueR - g_audio.last_valueR ) / 16;

        if ( g_audio.resample_timer <= PSG_DIVIDER ) {
            if ( g_audio.buffer_position < IFACE_AUDIO_WINDOW_SAMPLES ) {
                //printf ( "res: %d = %d\n", g_audio.buffer_position, last_value );
                if ( !TEST_EMULATION_PAUSED ) {
                    int real_position = g_audio.buffer_position * IFACE_AUDIO_CHANNELS;
                    g_audio.buffer [real_position++] = g_audio.last_valueL;
                    g_audio.buffer [real_position] = g_audio.last_valueR;
                };
                g_audio.buffer_position++;
            };
            g_audio.resample_timer += AUDIO_RESAMPLE_PERIOD;
        };

        g_audio.resample_timer -= PSG_DIVIDER;
        g_audio.last_update += PSG_DIVIDER;

    } while ( g_audio.last_update < event_ticks );
}
#endif

#endif




#ifdef AUDIO_FILLBUFF_v2

#define AUDIO_SCAN_PERIOD   PSG_DIVIDER
//#define AUDIO_SCAN_PERIOD   GDGCLK_1M1_DIVIDER


void audio_fill_buffer_v2 ( unsigned now_total_ticks ) {

    unsigned ctc0_buf_pos = 0;
    unsigned psg_buf_pos;
    unsigned dst_sample_pos = 0;

    static unsigned already_scaned = 0;

    static unsigned last_scan_end = 0;

    unsigned start_scan_time;

    if ( g_psg_audio.samples[0].value != -1 ) {
        if ( ( g_audio_ctc.samples[0].timestamp - last_scan_end ) < ( g_psg_audio.samples[0].timestamp - last_scan_end ) ) {
            start_scan_time = g_audio_ctc.samples[0].timestamp;
        } else {
            start_scan_time = g_psg_audio.samples[0].timestamp;
        };
        psg_buf_pos = 0;
    } else {
        start_scan_time = g_audio_ctc.samples[0].timestamp;
        psg_buf_pos = -1;
    };

    unsigned samples_width = now_total_ticks - start_scan_time;
    unsigned current_scan_time = start_scan_time;

    static AUDIO_BUF_t audio_last_value = 0;

    unsigned resample_width = samples_width / IFACE_AUDIO_WINDOW_SAMPLES;

    int ctc0_current_state_width = g_audio_ctc.samples[ctc0_buf_pos + 1].timestamp - current_scan_time;
    AUDIO_BUF_t ctc0_current_value = g_audio_ctc.samples[0].state;

    //    printf ( "FILL: %d - %d = %d, %d, %d, %d\n", now_total_ticks, start_scan_time, samples_width, g_audio_ctc.count, g_psg_audio.count, g_psg_audio.samples[0].value );

    while ( 1 ) {

        if ( psg_buf_pos <= g_psg_audio.count ) {

            unsigned tst1 = current_scan_time - start_scan_time;
            unsigned tst2 = g_psg_audio.samples[psg_buf_pos].timestamp - start_scan_time;

            while ( ( tst1 >= tst2 ) && ( psg_buf_pos <= g_psg_audio.count ) ) {
                //                printf ( "REAL WRITE: %d - %d (%d): 0x%02x\n", current_scan_time, g_psg_audio.samples[psg_buf_pos].timestamp, current_scan_time - g_psg_audio.samples[psg_buf_pos].timestamp,  g_psg_audio.samples[psg_buf_pos].value );
                //  printf ( "REAL WRITE: %d: 0x%02x\n", g_psg_audio.samples[psg_buf_pos].timestamp, g_psg_audio.samples[psg_buf_pos].value );
                psg_real_write_byte ( g_psg_audio.samples[psg_buf_pos++].value );
                tst2 = g_psg_audio.samples[psg_buf_pos].timestamp - start_scan_time;
            };
        };

        //printf ( "step: %d\n", current_scan_time );

        psg_step ( );

        audio_last_value = audio_last_value + ( ( psg_mz800_audio_scan ( ) + ctc0_current_value ) - audio_last_value ) / 16;

        if ( ctc0_buf_pos < g_audio_ctc.count ) {
            if ( ctc0_current_state_width <= AUDIO_SCAN_PERIOD ) {

                unsigned tst1 = current_scan_time - start_scan_time;
                unsigned tst2;

                do {
                    tst2 = g_audio_ctc.samples[++ctc0_buf_pos].timestamp - start_scan_time;
                } while ( ( ctc0_buf_pos < g_audio_ctc.count ) && ( tst1 > tst2 ) );

                ctc0_current_value = g_audio_ctc.samples[ctc0_buf_pos].state;
                ctc0_current_state_width = g_audio_ctc.samples[ctc0_buf_pos + 1].timestamp - current_scan_time;
            };
        };

        already_scaned += AUDIO_SCAN_PERIOD;

        if ( already_scaned >= resample_width ) {
            //          printf ( "res: %d = %d\n", dst_sample_pos, audio_last_value );
            g_audio.buffer [ dst_sample_pos ] = audio_last_value;
            if ( dst_sample_pos < IFACE_AUDIO_WINDOW_SAMPLES ) dst_sample_pos++;
            already_scaned -= resample_width;
            last_scan_end = current_scan_time;
        };

        current_scan_time += AUDIO_SCAN_PERIOD;

        if ( samples_width < AUDIO_SCAN_PERIOD ) break;

        samples_width -= AUDIO_SCAN_PERIOD;
        ctc0_current_state_width -= AUDIO_SCAN_PERIOD;
    };

    int tst1 = current_scan_time - g_audio_ctc.samples[0].timestamp;
    int tst2;

    if ( ctc0_buf_pos < g_audio_ctc.count ) {
        do {
            tst2 = g_audio_ctc.samples[++ctc0_buf_pos].timestamp - start_scan_time;
        } while ( ( ctc0_buf_pos < g_audio_ctc.count ) && ( tst1 > tst2 ) );
    };

    g_audio_ctc.samples[ctc0_buf_pos].timestamp = current_scan_time;

    if ( g_audio_ctc.count != 0 ) {

        unsigned i;
        for ( i = 0; ctc0_buf_pos <= g_audio_ctc.count; i++ ) {
            g_audio_ctc.samples[i].timestamp = g_audio_ctc.samples[ctc0_buf_pos].timestamp;
            g_audio_ctc.samples[i].state = g_audio_ctc.samples[ctc0_buf_pos].state;
            ctc0_buf_pos++;
        };
        g_audio_ctc.count = i - 1;
    };

    if ( ( psg_buf_pos != -1 ) && ( g_psg_audio.count - psg_buf_pos ) != -1 ) {
        while ( ( psg_buf_pos <= g_psg_audio.count ) ) {
            //printf ( "REAL WRITE: %d: 0x%02x\n", g_psg_audio.samples[psg_buf_pos].timestamp, g_psg_audio.samples[psg_buf_pos].value );
            psg_real_write_byte ( g_psg_audio.samples[psg_buf_pos++].value );
        };
    };
    g_psg_audio.samples[0].value = -1;
    g_psg_audio.count = 0;

}
#endif

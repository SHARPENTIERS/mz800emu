/* 
 * File:   audio.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 28. července 2015, 13:39
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

#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"
#include "gdg/video.h"
#include "gdg/gdg.h"


#include "iface_sdl/iface_sdl_audio.h"
#include <stdint.h>


    typedef uint16_t AUDIO_BUF_t;

#ifdef AUDIO_FILLBUFF_v2


    typedef struct st_AUDIO_SAMPLE {
        unsigned timestamp;
        AUDIO_BUF_t state;
    } st_AUDIO_SAMPLE;



#if ( GDGCLK_1M1_DIVIDER == 16 ) && ( MZ800EMU_CFG_MAX_SYNC_SPEED == 1000 )
#define AUDIO_MIN_CTC0_EVENT_WIDTH  ( GDGCLK_1M1_DIVIDER * 1.5 )  /* ( 24 = 16 * 1.5 ) */
#define AUDIO_MAX_SCAN_WIDTH  ( MZ800EMU_CFG_MAX_SYNC_SPEED * 0.01 * VIDEO_SCREEN_TICKS )  /* ( 3544320 = 1000 * 0.01 * 354432 ) */
#define AUDIO_MAX_CTC_SAMPLES ( 3544320 / 24 )
#else
#if ( GDGCLK_1M1_DIVIDER == 16 ) && ( MZ800EMU_CFG_MAX_SYNC_SPEED == 100 )
#define AUDIO_MIN_CTC0_EVENT_WIDTH  ( GDGCLK_1M1_DIVIDER * 1.5 )  /* ( 24 = 16 * 1.5 ) */
#define AUDIO_MAX_SCAN_WIDTH  VIDEO_SCREEN_TICKS
#define AUDIO_MAX_CTC_SAMPLES ( AUDIO_MAX_SCAN_WIDTH / 24 )
#else
#error Plese define AUDIO_MAX_CTC_SAMPLES
#endif
#endif


    typedef struct st_AUDIO_CTC {
        st_AUDIO_SAMPLE samples [ AUDIO_MAX_CTC_SAMPLES ];
        unsigned count;
    } st_AUDIO_CTC;

    extern st_AUDIO_CTC g_audio_ctc;
#endif


    typedef struct st_AUDIO {
        AUDIO_BUF_t buffer [ IFACE_AUDIO_20MS_SAMPLES ];
        unsigned buffer_position;
        unsigned last_update;
        unsigned resample_timer;
        unsigned ctc0_output;
    } st_AUDIO;

    extern st_AUDIO g_audio;

    extern void audio_init ( void );
    extern void audio_ctc0_changed ( unsigned value, unsigned event_ticks );

#ifdef AUDIO_FILLBUFF_v1
    extern void audio_fill_buffer ( unsigned event_ticks );
#endif

#ifdef AUDIO_FILLBUFF_v2
    extern void audio_fill_buffer_v2 ( unsigned total_ticks );
#endif


#ifdef __cplusplus
}
#endif

#endif /* AUDIO_H */


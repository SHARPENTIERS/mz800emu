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
#define	AUDIO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "iface_sdl/iface_sdl_audio.h"
#include <stdint.h>

    typedef uint16_t AUDIO_BUF_t;

    typedef struct st_AUDIO {
#ifdef USE_LINUX_AUDIO_DOUBLE_BUFFER
        AUDIO_BUF_t buffer [ 2 ] [ IFACE_AUDIO_20MS_SAMPLES ];
        unsigned active_buffer;
#else
        AUDIO_BUF_t buffer [ IFACE_AUDIO_20MS_SAMPLES ];
#endif
        unsigned buffer_position;
        unsigned last_update;
        unsigned resample_timer;
        unsigned ctc0_output;
    } st_AUDIO;

    extern st_AUDIO g_audio;

    extern void audio_init ( void );
    extern void audio_fill_buffer ( unsigned event_ticks );
    extern void audio_ctc0_changed ( unsigned value, unsigned event_ticks );


#ifdef	__cplusplus
}
#endif

#endif	/* AUDIO_H */


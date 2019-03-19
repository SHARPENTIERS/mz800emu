/* 
 * File:   iface_sdl_audio.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. července 2015, 18:30
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

#ifndef IFACE_SDL_AUDIO_H
#define IFACE_SDL_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"


#define IFACE_AUDIO_SAMPLE_RATE           48000
#ifdef MACHINE_EMU_MZ800
#define IFACE_AUDIO_CHANNELS              1   /* 1 - mono, 2 - stereo, 4 - quad, 6 - 5.1 */
#endif
#ifdef MACHINE_EMU_MZ1500
#define IFACE_AUDIO_CHANNELS              2   /* 1 - mono, 2 - stereo, 4 - quad, 6 - 5.1 */
#endif
    /* MZ800: 48000/50 = 960, 44100/50 = 882 */
#define IFACE_AUDIO_WINDOW_SAMPLES          ( IFACE_AUDIO_SAMPLE_RATE / VIDEO_SCREENS_PER_SEC )


    extern int iface_sdl_audio_init ( const char *preferedAudioDriverName, int preferedAudioDeviceId );
    extern void iface_sdl_audio_quit ( void );
    extern void audio_sdl_start_cycle ( void );
    extern void iface_sdl_audio_sync_20ms_cycle ( void );
    extern void iface_sdl_audio_update_buffer_state ( void );
    extern void iface_sdl_audio_pause_emulation ( unsigned value );


#ifdef __cplusplus
}
#endif

#endif /* IFACE_SDL_AUDIO_H */


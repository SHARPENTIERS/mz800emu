/* 
 * File:   iface_sdl.h
 * Author: chaky
 *
 * Created on 10. června 2015, 23:29
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

#ifndef IFACE_SDL_H
#define	IFACE_SDL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <SDL.h>
#include "iface_sdl/iface_sdl_log.h"

    typedef struct st_iface_sdl {
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Surface *active_surface;
        SDL_Surface *old_surface;
        int redraw_full_screen_request;
    } st_iface_sdl;

    extern struct st_iface_sdl g_iface_sdl;

    extern void iface_sdl_init ( void );
    extern void iface_sdl_quit ( void );
    //extern void iface_sdl_pool_window_events ( void );
    extern void iface_sdl_pool_all_events ( void );
    extern void iface_sdl_update_window ( void );
    extern void iface_sdl_render_status_line ( void );
    extern void iface_sdl_set_colors ( uint32_t *colormap );
    extern void iface_sdl_set_window_size ( float scale );
    
#ifdef	__cplusplus
}
#endif

#endif	/* IFACE_SDL_H */


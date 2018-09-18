/* 
 * File:   display.c
 * Author: chaky
 *
 * Created on 14. června 2015, 9:41
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

#include "display.h"
#include "iface_sdl/iface_sdl.h"
#include "cfgmain.h"



uint32_t display_predef_colors [ DISPLAY_MZCOLORS ] = {
                                                       0x000000, 0x4040ac, 0xd03400, 0xb40c8c,
                                                       0x406c00, 0x24ccff, 0xe8d430, 0xd0d0d0,
                                                       0x848484, 0x008ce8, 0xff0000, 0xf054cc,
                                                       0x54ff54, 0x80ffff, 0xffff28, 0xffffff
};


uint32_t display_predef_grays [ DISPLAY_MZCOLORS ] = {
                                                      0x000000, 0x545454, 0x606060, 0x6c6c6c,
                                                      0x909090, 0x9c9c9c, 0xc0c0c0, 0xcccccc,
                                                      0x787878, 0x848484, 0xa8a8a8, 0xb4b4b4,
                                                      0xd8d8d8, 0xe4e4e4, 0xf0f0f0, 0xffffff
};


uint32_t display_predef_greens [ DISPLAY_MZCOLORS ] = {
                                                       0x000000, 0x005400, 0x006000, 0x006c00,
                                                       0x009000, 0x009c00, 0x00c000, 0x00cc00,
                                                       0x007800, 0x008400, 0x00a800, 0x00b400,
                                                       0x00d800, 0x00e400, 0x00f000, 0x00ff00
};


st_DISPLAY g_display;


#define DEFAULT_COLOR_SCHEMA        DISPLAY_NORMAL


void display_init ( void ) {

    en_DISPLAY_COLOR_SCHEMA i;
    for ( i = 0; i < DISPLAY_COLORS_COUNT; i++ ) {
        switch ( i ) {
            case DISPLAY_NORMAL:
                g_display.color_predef[i] = display_predef_colors;
                break;
            case DISPLAY_GRAYSCALE:
                g_display.color_predef[i] = display_predef_grays;
                break;
            case DISPLAY_GREEN:
                g_display.color_predef[i] = display_predef_greens;
                break;
            case DISPLAY_COLORS_COUNT:
                break;
        };
    };


    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "DISPLAY" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "color_schema", CFGENTYPE_KEYWORD, DEFAULT_COLOR_SCHEMA,
                                           DISPLAY_NORMAL, "NORMAL",
                                           DISPLAY_GRAYSCALE, "GRAYSCALE",
                                           DISPLAY_GREEN, "GREEN",
                                           -1 );
    cfgelement_set_handlers ( elm, (void*) &g_display.color_schema, (void*) &g_display.color_schema );

    elm = cfgmodule_register_new_element ( cmod, "forced_full_screen_redrawing", CFGENTYPE_BOOL, 0 );
    cfgelement_set_handlers ( elm, (void*) &g_display.forced_full_screen_redrawing, (void*) &g_display.forced_full_screen_redrawing );

    elm = cfgmodule_register_new_element ( cmod, "locked_window_aspect_ratio", CFGENTYPE_BOOL, 0 );
    cfgelement_set_handlers ( elm, (void*) &g_display.locked_window_aspect_ratio, (void*) &g_display.locked_window_aspect_ratio );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );

    /* ui jeste neni inicializovan */
    //ui_display_update_menu ( );
}


uint32_t* display_get_default_color_schema ( void ) {
    return g_display.color_predef[g_display.color_schema];
}


void display_set_colors ( en_DISPLAY_COLOR_SCHEMA color_schema ) {
    g_display.color_schema = color_schema;
    iface_sdl_set_colors ( g_display.color_predef[g_display.color_schema] );
}


unsigned display_get_window_color_schema ( void ) {
    return g_display.color_schema;
}



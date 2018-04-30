/* 
 * File:   cmt.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. srpna 2015, 12:07
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


#include "mz800emu_cfg.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include "cmt.h"

#include "gdg/gdg.h"

#include "ui/ui_utils.h"
#include "ui/ui_main.h"
#include "ui/ui_cmt.h"

#include "cfgmain.h"

#include "cmt_hack.h"

#include "cmt_extension.h"
#include "cmt_wav.h"
#include "cmt_mzf.h"

st_CMT g_cmt;


void cmt_stop ( void ) {
    if ( !TEST_CMT_FILLED ) return;
    if ( TEST_CMT_STOP ) return;
    g_cmt.state = CMT_STATE_STOP;
    ui_cmt_window_update ( );
}


void cmt_eject ( void ) {
    if ( !TEST_CMT_FILLED ) return;
    if ( !TEST_CMT_STOP ) cmt_stop ( );
    g_cmt.ext->eject ( );
    g_cmt.ext = NULL;
    ui_cmt_window_update ( );
}


void cmt_play ( void ) {
    if ( !TEST_CMT_FILLED ) return;
    if ( !TEST_CMT_STOP ) return;
    g_cmt.state = CMT_STATE_PLAY;
    g_cmt.start_time = gdg_get_total_ticks ( );
    g_cmt.ui_player_update = 0;
    ui_cmt_window_update ( );
}


int cmt_change_speed ( en_CMT_SPEED speed ) {

    if ( !TEST_CMT_STOP ) return EXIT_FAILURE;
    if ( g_cmt.speed == speed ) return EXIT_SUCCESS;


    int ret = EXIT_SUCCESS;

    if ( TEST_CMT_FILLED ) {
        if ( g_cmt.ext->change_speed != NULL ) {
            if ( EXIT_SUCCESS != g_cmt.ext->change_speed ( speed ) ) {
                ret = EXIT_FAILURE;
            };
        };
    };

    if ( ret == EXIT_SUCCESS ) {
        g_cmt.speed = speed;
    }

    ui_cmt_window_update ( );

    return ret;
}


void cmt_exit ( void ) {

    cmt_eject ( );
    if ( g_cmt.last_filename ) ui_utils_mem_free ( g_cmt.last_filename );

    cmthack_exit ( );
    cmt_wav_exit ( );
    cmt_mzf_exit ( );
}


void cmt_propagatecfg_cmt_speed ( void *e, void *data ) {
    g_cmt.speed = cfgelement_get_keyword_value ( (CFGELM *) e );
    ui_cmt_window_update ( );
}


void cmt_rear_dip_switch_cmt_inverted_polarity ( unsigned value ) {
    value &= 1;
    if ( value == g_cmt.polarity ) return;
    g_cmt.polarity = value;
    ui_main_update_rear_dip_switch_cmt_inverted_polarity ( g_cmt.polarity );
}


void cmt_propagatecfg_inverted_polarity ( void *e, void *data ) {
    ui_main_update_rear_dip_switch_cmt_inverted_polarity ( cfgelement_get_bool_value ( (CFGELM *) e ) );
}


void cmt_init ( void ) {

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "CMT" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "cmt_speed", CFGENTYPE_KEYWORD, CMT_SPEED_1200,
                                           CMT_SPEED_1200, "NORMAL",
                                           CMT_SPEED_2400, "DOUBLE",
                                           CMT_SPEED_3600, "TRIPLE",
                                           -1 );
    cfgelement_set_propagate_cb ( elm, cmt_propagatecfg_cmt_speed, NULL );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_cmt.speed );

    elm = cfgmodule_register_new_element ( cmod, "cmt_polarity_inverted", CFGENTYPE_BOOL, CMT_POLARITY_NORMAL );
    cfgelement_set_propagate_cb ( elm, cmt_propagatecfg_inverted_polarity, NULL );
    cfgelement_set_handlers ( elm, (void*) &g_cmt.polarity, (void*) &g_cmt.polarity );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );

    g_cmt.output = 0;
    g_cmt.start_time = 0;
    g_cmt.state = CMT_STATE_STOP;
    g_cmt.ext = NULL;
    g_cmt.last_filename = ui_utils_mem_alloc0 ( 1 );
    g_cmt.ui_player_update = 0;

    cmt_wav_init ( );
    cmt_mzf_init ( );

    ui_cmt_init ( );
    ui_cmt_window_update ( );

    cmthack_init ( );
}


static int cmt_open_file_by_extension ( char *filename ) {

    int name_length = strlen ( filename );

    if ( name_length < 5 ) {
        ui_show_error ( "Bad filename '%s'\n", filename );
        return EXIT_FAILURE;
    };

    char *file_ext = &filename[( name_length - 3 )];

    st_CMT_EXTENSION *ext;

    if ( ( 0 == strncasecmp ( file_ext, "mzf", 3 ) ) || ( 0 == strncasecmp ( file_ext, "m12", 3 ) ) ) {
        ext = &g_cmt_mzf_extension;
    } else if ( 0 == strncasecmp ( file_ext, "wav", 3 ) ) {
        ext = &g_cmt_wav_extension;
    } else {
        ui_show_error ( "Unknown CMT file extension '%s'\n", filename );
        return EXIT_FAILURE;
    };

    cmt_eject ( );

    int ret = ext->open ( filename );
    if ( ret == EXIT_SUCCESS ) {
        g_cmt.ext = ext;
    };
    ui_cmt_window_update ( );
    return ret;
}


int cmt_open ( void ) {

    char window_title[] = "Select any CMT file to open";
    char *filename = NULL;


    if ( UIRET_OK != ui_open_file ( &filename, g_cmt.last_filename, 0, FILETYPE_ALLCMTFILES, window_title, OPENMODE_READ ) ) {
        return EXIT_FAILURE;
    };

    if ( filename == NULL ) {
        filename = ui_utils_mem_alloc0 ( 1 );
    };

    cmt_open_file_by_extension ( filename );

    ui_utils_mem_free ( filename );

    return EXIT_FAILURE;
}


void cmt_screen_done_period ( void ) {

    if ( !TEST_CMT_FILLED ) return;
    if ( TEST_CMT_STOP ) return;

    st_CMT_STREAM *cmt_stream = g_cmt.ext->get_stream ( );

    if ( cmt_stream == NULL ) return;

    double playtime = cmt_get_playtime ( );
    uint32_t sample_position = cmt_stream_get_position_by_time ( cmt_stream, playtime );

    if ( sample_position >= cmt_stream->scans ) {
        //printf ( "done: 100 %%\n" );
        cmt_stop ( );
        return;
    };

    // TODO: ui_update

    if ( g_cmt.ui_player_update++ < 49 ) return;

    g_cmt.ui_player_update = 0;
    //double total_time = ( cmt_stream->scan_time * cmt_stream->scans );
    //printf ( "playing: %1.2f s, remaining: %1.2f s, done: %1.2f %%\n", playtime, ( total_time - playtime ), ( playtime / total_time ) * 100 );
    ui_cmt_update_player ( );
}


void cmt_update_output ( void ) {

    if ( !TEST_CMT_FILLED ) return;
    if ( !TEST_CMT_PLAY ) return;

    st_CMT_STREAM *cmt_stream = g_cmt.ext->get_stream ( );

    if ( cmt_stream == NULL ) return;

    double playtime = cmt_get_playtime ( );

    uint32_t sample_position = cmt_stream_get_position_by_time ( cmt_stream, playtime );

    if ( sample_position >= cmt_stream->scans ) {
        //printf ( "done: 100 %%\n" );
        cmt_stop ( );
        return;
    };

    int sample = cmt_stream_get_value_on_position ( cmt_stream, sample_position );

    g_cmt.output = ( sample > 0 ) ? g_cmt.polarity : ( ~g_cmt.polarity & 1 );

    if ( sample_position == ( cmt_stream->scans - 1 ) ) {
        //printf ( "done: 100 %%\n" );
        cmt_stop ( );
    };
}


int cmt_read_data ( void ) {
    cmt_update_output ( );
    return g_cmt.output;
}

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
#include <assert.h>

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

#include "libs/mztape/mztape.h"


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
    cmtext_play ( g_cmt.ext );
}


int cmt_change_speed ( en_MZTAPE_SPEED speed ) {

    if ( !TEST_CMT_STOP ) return EXIT_FAILURE;
    if ( g_cmt.speed == speed ) return EXIT_SUCCESS;


    int ret = EXIT_SUCCESS;

    if ( TEST_CMT_FILLED ) {
        if ( cmtext_cmtfile_get_filetype ( g_cmt.ext ) == CMT_FILETYPE_MZF ) {
            if ( EXIT_SUCCESS != cmtext_cmtfile_change_speed ( g_cmt.ext, speed ) ) {
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
    cmtwav_exit ( );
    //    cmt_mzf_exit ( );
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
    if ( !TEST_CMT_FILLED ) return;
    if ( CMT_FILETYPE_WAV != cmtext_cmtfile_get_filetype ( g_cmt.ext ) ) return;
    if ( CMT_STREAM_TYPE_BITSTREAM != cmtext_cmtfile_get_stream_type ( g_cmt.ext ) ) {
        fprintf ( stderr, "%s():%d - Can't intvert stream polarity\n", __func__, __LINE__ );
        return;
    };
    cmt_bitstream_invert_data ( cmtext_cmtfile_get_bitstream ( g_cmt.ext ) );
}


void cmt_propagatecfg_inverted_polarity ( void *e, void *data ) {
    ui_main_update_rear_dip_switch_cmt_inverted_polarity ( cfgelement_get_bool_value ( (CFGELM *) e ) );
}


void cmt_init ( void ) {

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "CMT" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "cmt_speed", CFGENTYPE_KEYWORD, MZTAPE_SPEED_1_1,
                                           MZTAPE_SPEED_1_1, "SPEED_1/1",
                                           MZTAPE_SPEED_2_1, "SPEED_2/1",
                                           MZTAPE_SPEED_7_3, "SPEED_7/3",
                                           MZTAPE_SPEED_8_3, "SPEED_8/3",
                                           MZTAPE_SPEED_3_1, "SPEED_3/1",
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

    cmtwav_init ( );
    cmtmzf_init ( );

    ui_cmt_init ( );
    ui_cmt_window_update ( );

    cmthack_init ( );
}


int cmt_open_file_by_extension ( char *filename ) {

    int name_length = strlen ( filename );

    if ( name_length < 5 ) {
        ui_show_error ( "Bad filename '%s'\n", filename );
        return EXIT_FAILURE;
    };

    char *file_ext = &filename[( name_length - 3 )];

    st_CMTEXT *ext;

    if ( ( 0 == strncasecmp ( file_ext, "mzf", 3 ) ) || ( 0 == strncasecmp ( file_ext, "m12", 3 ) ) ) {
        ext = g_cmtmzf_extension;
    } else if ( 0 == strncasecmp ( file_ext, "wav", 3 ) ) {
        ext = g_cmtwav_extension;
    } else {
        ui_show_error ( "Unknown CMT file extension '%s'\n", filename );
        return EXIT_FAILURE;
    };

    assert ( ext != NULL );
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

    return EXIT_SUCCESS;
}


void cmt_update_output ( void ) {

    if ( !TEST_CMT_FILLED ) return;
    if ( !TEST_CMT_PLAY ) return;

    if ( CMT_STREAM_TYPE_BITSTREAM == cmtext_cmtfile_get_stream_type ( g_cmt.ext ) ) {
        st_CMT_BITSTREAM *cmt_bitstream = cmtext_cmtfile_get_bitstream ( g_cmt.ext );

        if ( cmt_bitstream == NULL ) return;

        double playtime = cmt_get_playtime ( );

        uint32_t sample_position = cmt_bitstream_get_position_by_time ( cmt_bitstream, playtime );

        if ( sample_position >= cmt_bitstream->scans ) {
            //printf ( "done2: 100 %%\n" );
            cmt_stop ( );
            return;
        };

        g_cmt.output = cmt_bitstream_get_value_on_position ( cmt_bitstream, sample_position );

        if ( sample_position == ( cmt_bitstream->scans - 1 ) ) {
            //printf ( "done3: 100 %%\n" );
            cmt_stop ( );
        };
    } else {
        uint64_t samples = gdg_get_total_ticks ( ) - g_cmt.start_time;
        st_CMT_VSTREAM *cmt_vstream = cmtext_cmtfile_get_vstream ( g_cmt.ext );
        assert ( cmt_vstream->rate == GDGCLK_BASE );
        if ( EXIT_FAILURE == cmt_vstream_get_value ( cmt_vstream, samples, &g_cmt.output ) ) {
            cmt_stop ( );
        };
    };
}


void cmt_screen_done_period ( void ) {

    if ( !TEST_CMT_FILLED ) return;
    if ( TEST_CMT_STOP ) return;

    cmt_update_output ( );

    if ( g_cmt.ui_player_update++ < 49 ) return;

    g_cmt.ui_player_update = 0;
    ui_cmt_update_player ( );
}


int cmt_read_data ( void ) {
    cmt_update_output ( );
    return g_cmt.output;
}

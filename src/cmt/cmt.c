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

#include "cmthack.h"
#include "libs/mztape/cmtspeed.h"
#include "cmtext.h"
#include "cmtext_block.h"

st_CMT g_cmt;


void cmt_stop ( void ) {
    if ( !TEST_CMT_FILLED ) return;
    if ( TEST_CMT_STOP ) return;
    g_cmt.state = CMT_STATE_STOP;
    g_cmt.playsts = CMTEXT_BLOCK_PLAYSTS_STOP;
    ui_cmt_window_update ( );
}


void cmt_eject ( void ) {
    if ( !TEST_CMT_FILLED ) return;
    if ( !TEST_CMT_STOP ) cmt_stop ( );
    g_cmt.ext->cb_eject ( );
    g_cmt.ext = NULL;
    ui_cmt_window_update ( );
}


void cmt_play ( void ) {
    if ( !TEST_CMT_FILLED ) return;
    if ( !TEST_CMT_STOP ) return;
    g_cmt.state = CMT_STATE_PLAY;
    g_cmt.playsts = CMTEXT_BLOCK_PLAYSTS_BODY;
    g_cmt.start_time = gdg_get_total_ticks ( );
    //printf ( "CMT start: %ul\n", gdg_get_total_ticks ( ) );
    g_cmt.ui_player_update = 0;
    ui_cmt_window_update ( );
    g_cmt.ext->block->cb_play ( g_cmt.ext );
}


int cmt_change_speed ( en_CMTSPEED cmtspeed ) {

    if ( !TEST_CMT_STOP ) return EXIT_FAILURE;
    if ( g_cmt.mz_cmtspeed == cmtspeed ) return EXIT_SUCCESS;


    int ret = EXIT_SUCCESS;

    if ( TEST_CMT_FILLED ) {
        if ( cmtext_block_get_block_speed ( g_cmt.ext->block ) == CMTEXT_BLOCK_SPEED_DEFAULT ) {
            assert ( g_cmt.ext->block->cb_set_speed != NULL );
            if ( g_cmt.ext->block->cb_set_speed != NULL ) {
                if ( EXIT_SUCCESS != g_cmt.ext->block->cb_set_speed ( g_cmt.ext, cmtspeed ) ) {
                    ret = EXIT_FAILURE;
                };
            };
        };
    };

    if ( ret == EXIT_SUCCESS ) {
        g_cmt.mz_cmtspeed = cmtspeed;
    }

    ui_cmt_window_update ( );

    return ret;
}


void cmt_exit ( void ) {

    cmt_eject ( );
    if ( g_cmt.last_filename ) ui_utils_mem_free ( g_cmt.last_filename );

    cmthack_exit ( );
    cmtext_exit ( );
}


void cmt_propagatecfg_cmt_speed ( void *e, void *data ) {
    g_cmt.mz_cmtspeed = cfgelement_get_keyword_value ( (CFGELM *) e );
    ui_cmt_window_update ( );
}


void cmt_rear_dip_switch_cmt_inverted_polarity ( unsigned value ) {
    value &= 1;
    if ( value == g_cmt.polarity ) return;
    g_cmt.polarity = value;
    ui_main_update_rear_dip_switch_cmt_inverted_polarity ( g_cmt.polarity );
    if ( !TEST_CMT_FILLED ) return;
    if ( g_cmt.ext->block->cb_set_polarity != NULL ) g_cmt.ext->block->cb_set_polarity ( g_cmt.ext, value );
}


void cmt_propagatecfg_inverted_polarity ( void *e, void *data ) {
    ui_main_update_rear_dip_switch_cmt_inverted_polarity ( cfgelement_get_bool_value ( (CFGELM *) e ) );
}


void cmt_init ( void ) {

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "CMT" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "mz_cmtspeed", CFGENTYPE_KEYWORD, CMTSPEED_1_1,
                                           CMTSPEED_1_1, "SPEED_1/1",
                                           CMTSPEED_2_1, "SPEED_2/1",
                                           CMTSPEED_7_3, "SPEED_7/3",
                                           CMTSPEED_8_3, "SPEED_8/3",
                                           CMTSPEED_3_1, "SPEED_3/1",
                                           -1 );
    cfgelement_set_propagate_cb ( elm, cmt_propagatecfg_cmt_speed, NULL );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_cmt.mz_cmtspeed );

    elm = cfgmodule_register_new_element ( cmod, "cmt_polarity_inverted", CFGENTYPE_BOOL, CMT_STREAM_POLARITY_NORMAL );
    cfgelement_set_propagate_cb ( elm, cmt_propagatecfg_inverted_polarity, NULL );
    cfgelement_set_handlers ( elm, (void*) &g_cmt.polarity, (void*) &g_cmt.polarity );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );

    g_cmt.output = 0;
    g_cmt.start_time = 0;
    g_cmt.state = CMT_STATE_STOP;
    g_cmt.playsts = CMTEXT_BLOCK_PLAYSTS_STOP;
    g_cmt.ext = NULL;
    g_cmt.last_filename = ui_utils_mem_alloc0 ( 1 );
    g_cmt.ui_player_update = 0;

    cmtext_init ( );

    ui_cmt_init ( );
    ui_cmt_window_update ( );

    cmthack_init ( );
}


int cmt_open_file_by_extension ( char *filename ) {

    st_CMTEXT *ext = cmtext_get_extension ( filename );

    if ( !ext ) {
        ui_show_error ( "Unknown CMT file extension '%s'\n", filename );
        return EXIT_FAILURE;
    }

    cmt_eject ( );

    int ret = ext->cb_open ( filename );
    if ( ret == EXIT_SUCCESS ) {
        g_cmt.ext = ext;
    } else {
        ui_show_error ( "%s can't open file '%s'\n", cmtext_get_description ( ext ), filename );
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

    uint64_t play_ticks = gdg_get_total_ticks ( ) - g_cmt.start_time;
    uint64_t transferred_ticks = 0;

    en_CMTEXT_BLOCK_PLAYSTS playsts = cmtext_block_get_output ( g_cmt.ext->block, play_ticks, &g_cmt.output, &transferred_ticks );

    if ( playsts != g_cmt.playsts ) {
        if ( CMTEXT_BLOCK_PLAYSTS_STOP == playsts ) {
            int total_blocks = cmtext_container_get_count_blocks ( g_cmt.ext->container );
            int play_block = cmtext_block_get_block_id ( g_cmt.ext->block ) + 1;

            if ( play_block < total_blocks ) {
                assert ( g_cmt.ext->container->cb_next_block );

                if ( ( !g_cmt.ext->container->cb_next_block ) || ( EXIT_FAILURE == g_cmt.ext->container->cb_next_block ( ) ) ) {
                    cmt_stop ( );
                } else {
                    g_cmt.playsts = CMTEXT_BLOCK_PLAYSTS_BODY;
                    g_cmt.start_time = gdg_get_total_ticks ( ) - transferred_ticks;
                    ui_cmt_window_update ( );
                };

            } else {
                cmt_stop ( );
            };

        } else if ( CMTEXT_BLOCK_PLAYSTS_PAUSE == playsts ) {
            printf ( "CMT: Playing a gap space of %d ms\n", cmtext_block_get_pause_after ( g_cmt.ext->block ) );
            g_cmt.playsts = playsts;
            ui_cmt_window_update ( );
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

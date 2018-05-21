/* 
 * File:   cmt.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. srpna 2015, 12:08
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

#ifndef CMT_H
#define CMT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "gdg/gdg.h"

#include "cmtext.h"

#include "libs/mztape/mztape.h"
#include "libs/cmt_stream/cmt_stream.h"


    typedef enum en_CMT_STATE {
        CMT_STATE_STOP = 0,
        CMT_STATE_PLAY
    } en_CMT_STATE;


    typedef struct st_CMT {
        st_CMTEXT_NEW *ext;
        char *last_filename;
        en_CMT_STREAM_POLARITY polarity;
        en_MZTAPE_SPEED speed;
        en_CMT_STATE state;
        en_CMTEXT_BLOCK_PLAYSTS playsts;
        int output;
        uint64_t start_time;
        int ui_player_update;
    } st_CMT;

    extern st_CMT g_cmt;

    extern void cmt_init ( void );
    extern void cmt_exit ( void );
    extern void cmt_rear_dip_switch_cmt_inverted_polarity ( unsigned value );

    extern int cmt_open_file_by_extension ( char *filename );
    extern int cmt_open ( void );
    extern void cmt_play ( void );
    extern void cmt_stop ( void );
    extern void cmt_eject ( void );
    extern int cmt_change_speed ( en_MZTAPE_SPEED speed );

    extern void cmt_screen_done_period ( void );
    extern int cmt_read_data ( void );
    extern void cmt_update_output ( void );

#define TEST_CMT_FILLED (g_cmt.ext != NULL)
#define TEST_CMT_STOP (g_cmt.state == CMT_STATE_STOP)
#define TEST_CMT_PLAY (g_cmt.state == CMT_STATE_PLAY)

#define cmt_on_screen_done_event( ) { if ( !TEST_CMT_STOP ) cmt_screen_done_period (); }


    static inline double cmt_get_playtime ( void ) {
        if ( !TEST_CMT_FILLED ) return 0;
        if ( TEST_CMT_STOP ) return 0;
        uint64_t now_ticks = gdg_get_total_ticks ( );
        uint64_t play_ticks = ( now_ticks - g_cmt.start_time );
        double play_time = play_ticks * ( 1 / (double) GDGCLK_BASE );
        return play_time;
    }


#ifdef __cplusplus
}
#endif

#endif /* CMT_H */


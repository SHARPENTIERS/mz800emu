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
#define	CMT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"

#define CMT_FILENAME_LENGTH 1024

#define DEF_CMT_BYTE_LENGTH		9
#define DEF_CMT_HEADER_SIZE		0x80
#define DEF_CMT_HEADER_LENGTH		( DEF_CMT_HEADER_SIZE * DEF_CMT_BYTE_LENGTH )

#if 1
#define DEF_CMT_HEADER_PILOT_LENGTH	4400	/* podle SM 22000 */
#define	DEF_CMT_BODY_PILOT_LENGTH	4400	/* podle SM 11000 */
#else
#define DEF_CMT_HEADER_PILOT_LENGTH	22000	/* podle SM 22000 */
#define	DEF_CMT_BODY_PILOT_LENGTH	11000	/* podle SM 11000 */
#endif

#define DEF_CMT_HEADER_TMARK_LENGTH	80
#define DEF_CMT_BODY_TMARK_LENGTH	40
#define DEF_CMT_BLOCK_PREFIX_LENGTH	2	/* podle SM 1 !!! */
#define DEF_CMT_BLOCK_SURFIX_LENGTH	2	/* podle SM 1 !!! */
#define DEF_CMT_CHKSUM_LENGTH		( 2 * DEF_CMT_BYTE_LENGTH )


#define RET_CMT_ERROR	0
#define RET_CMT_OK	1

    typedef enum {
        CMT_PLAY_NONE = 0,
        CMT_PLAY_START,
        CMT_PLAY_HEADER_PILOT,
        CMT_PLAY_HEADER_TMARK_L,
        CMT_PLAY_HEADER_TMARK_S,
        CMT_PLAY_HEADER_PREFIX,
        CMT_PLAY_HEADER,
        CMT_PLAY_HEADER_CHKSUM,
        CMT_PLAY_HEADER_SURFIX,
        CMT_PLAY_BODY_PILOT,
        CMT_PLAY_BODY_TMARK_L,
        CMT_PLAY_BODY_TMARK_S,
        CMT_PLAY_BODY_PREFIX,
        CMT_PLAY_BODY,
        CMT_PLAY_BODY_CHKSUM,
        CMT_PLAY_BODY_SURFIX,
        CMT_PLAY_DONE

    } mz800_cmt_play_state_t;

    typedef enum {
        CMT_SPEED_1200 = 0,
        CMT_SPEED_2400,
        CMT_SPEED_3600
    } mz800_cmt_speed_t;

    typedef struct {
        FILE *fh;
        char filename [ CMT_FILENAME_LENGTH ];
        int output_signal;
        mz800_cmt_play_state_t state;
        int bit_counter;
        int tick_counter;
        int tick_counter_low;
        int data_bit_position;
        Z80EX_BYTE data;
        Z80EX_WORD checksum;
        mz800_cmt_speed_t speed;

        int play_time;
        int refresh_counter;
        unsigned int play_iface_refresh_timer;

        unsigned int file_bits;
        unsigned int file_bits_elapsed;

        int mzf_filetype;
        char mzf_filename [ 17 ];
        Z80EX_WORD mzf_size;
        Z80EX_WORD mzf_exec;
        Z80EX_WORD mzf_start;

    } mz800_cmt_t;

    extern mz800_cmt_t g_cmt;

    extern void cmt_init ( void );
    extern void cmt_exit ( void );


    extern int cmt_open ( void );
    extern void cmt_eject ( void );
    extern void cmt_stop ( void );
    extern void cmt_play ( void );
    extern void cmt_stop ( void );
    extern void cmt_step ( void );


#ifdef	__cplusplus
}
#endif

#endif	/* CMT_H */

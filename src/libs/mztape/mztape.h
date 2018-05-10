/* 
 * File:   mztape.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 24. dubna 2018, 12:31
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


#ifndef MZTAPE_H
#define MZTAPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "libs/mzf/mzf.h"
#include "libs/cmt_stream/cmt_stream.h"
#include "libs/generic_driver/generic_driver.h"


    typedef enum en_MZTAPE_BLOCK {
        MZTAPE_BLOCK_LGAP = 0, // long GAP = 22 000 short (10 000 in MZ-80B!)
        MZTAPE_BLOCK_SGAP, // short GAP = 11 000 short
        MZTAPE_BLOCK_LTM, // long tapemark = 40 long + 40 short
        MZTAPE_BLOCK_STM, // short tapemark = 20 long + 20 short
        MZTAPE_BLOCK_HDR, // tape header = 128 Bytes (MZF header)
        MZTAPE_BLOCK_HDRC, // tape header - copy
        MZTAPE_BLOCK_FILE, // file body = MZF body
        MZTAPE_BLOCK_FILEC, // file body - copy
        MZTAPE_BLOCK_CHKH, // 2 Byte checksum of header = 16 bit sum total of all "1"
        MZTAPE_BLOCK_CHKF, // 2 Byte checksum of file body = 16 bit sum total of all "1"
        MZTAPE_BLOCK_2L, // 1 long pulse
        MZTAPE_BLOCK_256S, // 256 short pulses
        MZTAPE_BLOCK_STOP, // not real block
    } en_MZTAPE_BLOCK;


    typedef struct st_MZTAPE_PULSE_LENGTH {
        double high; // doba 1. casti pulsu (H)
        double low; // doba 2. casti pulsu (L)
        double total; // celkova doba pulsu (H + L)
    } st_MZTAPE_PULSE_LENGTH;


    typedef enum en_MZTAPE_PULSESET {
        MZTAPE_PULSESET_700 = 0, // MZ-700, MZ-80K, MZ-80A
        MZTAPE_PULSESET_800, // MZ-800, MZ-1500
        MZTAPE_PULSESET_80B, // MZ-80B
    } en_MZTAPE_PULSESET;


    typedef struct st_MZTAPE_PULSES_LENGTH {
        st_MZTAPE_PULSE_LENGTH long_pulse; // pulz reprezentujici "1"
        st_MZTAPE_PULSE_LENGTH short_pulse; // pulz reprezentujici "0"
    } st_MZTAPE_PULSES_LENGTH;


    typedef struct st_MZTAPE_PULSE_GDGTICS {
        uint16_t high;
        uint16_t low;
    } st_MZTAPE_PULSE_GDGTICS;


    typedef struct st_MZTAPE_PULSES_GDGTICS {
        st_MZTAPE_PULSE_GDGTICS long_pulse;
        st_MZTAPE_PULSE_GDGTICS short_pulse;
    } st_MZTAPE_PULSES_GDGTICS;


    typedef struct st_MZTAPE_FORMAT {
        uint32_t lgap;
        uint32_t sgap;
        en_MZTAPE_PULSESET pulseset;
        en_MZTAPE_BLOCK *blocks;
    } st_MZTAPE_FORMAT;


    typedef enum en_MZTAPE_FORMATSET {
        MZTAPE_FORMATSET_MZ800_SANE = 0, // header + body, lgap 4400, sgap 4400
        MZTAPE_FORMATSET_MZ800 = 1, // 2x header + 2x body, lgap 22000, sgap 11000
    } en_MZTAPE_FORMATSET;


    typedef enum en_MZTAPE_SPEED {
        MZTAPE_SPEED_1_1, // 1:1 1200 Bd
        MZTAPE_SPEED_2_1, // 2:1 2400 Bd
        MZTAPE_SPEED_7_3, // 7:3 2800 Bd
        MZTAPE_SPEED_8_3, // 8:3 3200 Bd
        MZTAPE_SPEED_3_1, // 3:1 3600 Bd
    } en_MZTAPE_SPEED;

#define MZTAPE_LGAP_LENGTH_DEFAULT 22000
#define MZTAPE_LGAP_LENGTH_SANE 4400
#define MZTAPE_LGAP_LENGTH_MZ80B 10000
#define MZTAPE_SGAP_LENGTH 11000
#define MZTAPE_SGAP_LENGTH_SANE 4400
#define MZTAPE_LTM_LLENGTH 40
#define MZTAPE_LTM_SLENGTH 40
#define MZTAPE_STM_LLENGTH 20
#define MZTAPE_STM_SLENGTH 20

#define MZTAPE_WAV_LEVEL_HIGH   -48
#define MZTAPE_WAV_LEVEL_LOW    48

#define MZTAPE_DEFAULT_BDSPEED 1200


    typedef struct st_MZTAPE_MZF {
        uint8_t header[sizeof ( st_MZF_HEADER )];
        uint8_t *body;
        uint16_t size;
        uint32_t chkh;
        uint32_t chkb;
    } st_MZTAPE_MZF;

    extern const double g_speed_divisor[];

    extern void mztape_mztmzf_destroy ( st_MZTAPE_MZF *mztmzf );
    extern st_MZTAPE_MZF* mztape_create_mztmzf ( st_HANDLER *mzf_handler );
    extern st_CMT_BITSTREAM* mztape_create_cmt_bitstream_from_mztmzf ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_FORMATSET mztape_format, en_MZTAPE_SPEED mztape_speed, uint32_t sample_rate );
    extern st_CMT_VSTREAM* mztape_create_17MHz_cmt_vstream_from_mztmzf ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_FORMATSET mztape_format, en_MZTAPE_SPEED mztape_speed );

#ifdef __cplusplus
}
#endif

#endif /* MZTAPE_H */


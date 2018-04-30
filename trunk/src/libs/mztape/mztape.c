/* 
 * File:   mztape.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 24. dubna 2018, 12:30
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


/*
 * 
 *	Popis standardniho MZ-800 CMT zaznamu 1200 baudu:
 *
 *              ___________
 *              |         |
 *      Long:   |         |________
 *
 *               470us     494us
 *
 *  Sharp GDG T:   8320      8640
 *
 *  Real GDG T:    8335      8760
 *
 *
 *              ______
 *              |    |
 *      Short:  |    |____
 *
 *              240us  278us
 *
 *  Sharp GDG T:  4220   4590
 *
 *  Real GDG T:   4356   4930
 *
 *
 *
 *  Read point:  379us od nastupne hrany = 6721 GDG T
 *
 * 
 * Format z MZ-800 ROM:
 * 
 * - Long GAP: 22 000 short
 * - Long Tape Mark: 40 long + 40 short
 * - 1 Long
 * - HDR - Header1
 * - CHK - Check Sum (2 bajty)
 * - 1 Long
 * - 256 Short
 * - HDR - Header2
 * - CHK - Check Sum (2 bajty)
 * - 1 Long
 * - Short GAP: 11 000 short
 * - Short Tape Mark: 20 long + 20 short
 * - 1 Long
 * - FILE - Data1
 * - CHK - Check Sum (2 bajty)
 * - 1 Long
 * - 256 Short
 * - FILE - Data2
 * - CHK - Check Sum (2 bajty)
 * - 1 Long
 * 
 * 
 * - datove bajty (HDR, FILE a CHK) jsou posilany od nejvyssiho bity k nejnizsimu
 * 
 * - za kazdym datovym bajtem nasleduje 1 stop bit (long)
 * 
 * - CHK je odesilan jako big endian !!! tedy nejprve horni bajt a pak dolni
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "ui/ui_utils.h"
#include "libs/generic_driver/generic_driver.h"
#include "libs/endianity/endianity.h"
#include "libs/cmt_stream/cmt_stream.h"

#include "mztape.h"

en_MZTAPE_BLOCK g_mztape_format_sharp_sane[] = {
                                                MZTAPE_BLOCK_LGAP,
                                                MZTAPE_BLOCK_LTM,
                                                MZTAPE_BLOCK_2L,
                                                MZTAPE_BLOCK_HDR,
                                                MZTAPE_BLOCK_CHKH,
                                                MZTAPE_BLOCK_2L,
                                                MZTAPE_BLOCK_SGAP,
                                                MZTAPE_BLOCK_STM,
                                                MZTAPE_BLOCK_2L,
                                                MZTAPE_BLOCK_FILE,
                                                MZTAPE_BLOCK_CHKF,
                                                MZTAPE_BLOCK_2L,
                                                MZTAPE_BLOCK_STOP
};

en_MZTAPE_BLOCK g_mztape_format_sharp[] = {
                                           MZTAPE_BLOCK_LGAP,
                                           MZTAPE_BLOCK_LTM,
                                           MZTAPE_BLOCK_2L,
                                           MZTAPE_BLOCK_HDR,
                                           MZTAPE_BLOCK_CHKH,
                                           MZTAPE_BLOCK_2L,
                                           MZTAPE_BLOCK_256S,
                                           MZTAPE_BLOCK_HDRC,
                                           MZTAPE_BLOCK_CHKH,
                                           MZTAPE_BLOCK_2L,
                                           MZTAPE_BLOCK_SGAP,
                                           MZTAPE_BLOCK_STM,
                                           MZTAPE_BLOCK_2L,
                                           MZTAPE_BLOCK_FILE,
                                           MZTAPE_BLOCK_CHKF,
                                           MZTAPE_BLOCK_2L,
                                           MZTAPE_BLOCK_256S,
                                           MZTAPE_BLOCK_FILEC,
                                           MZTAPE_BLOCK_CHKF,
                                           MZTAPE_BLOCK_2L,
                                           MZTAPE_BLOCK_STOP
};

const st_MZTAPE_PULSES g_mztape_pulses_700 = {
    { 0.000464, 0.000494, 0.000958 }, // LONG PULSE
    { 0.000240, 0.000264, 0.000504 }, // SHORT PULSE
};

const st_MZTAPE_PULSES g_mztape_pulses_800 = {
    { 0.000470, 0.000494, 0.000964 }, // LONG PULSE
    { 0.000240, 0.000278, 0.000518 }, // SHORT PULSE
};

const st_MZTAPE_PULSES g_mztape_pulses_80B = {
    { 0.000333, 0.000334, 0.000667 }, // LONG PULSE
    { 0.000166750, 0.000166, 0.000332750 }, // SHORT PULSE
};


const st_MZTAPE_PULSES *g_pulses[] = {
                                      &g_mztape_pulses_700,
                                      &g_mztape_pulses_800,
                                      &g_mztape_pulses_80B,
};

const st_MZTAPE_FORMAT g_format_mz800_sane = {
                                              MZTAPE_LGAP_LENGTH_SANE,
                                              MZTAPE_SGAP_LENGTH_SANE,
                                              MZTAPE_PULSESET_800,
                                              g_mztape_format_sharp_sane,
};

const st_MZTAPE_FORMAT g_format_mz800 = {
                                         MZTAPE_LGAP_LENGTH_DEFAULT,
                                         MZTAPE_SGAP_LENGTH,
                                         MZTAPE_PULSESET_800,
                                         g_mztape_format_sharp,
};

// poradi zachovat podle en_MZTAPE_FORMATSET !
const st_MZTAPE_FORMAT *g_formats[] = {
                                       &g_format_mz800_sane,
                                       &g_format_mz800,
};


// nasobitel rychlosti podle en_MZTAPE_SPEED
const double g_speed_multiplier[] = {
                                     1,
                                     ( (double) 1 / 2 ),
                                     ( (double) 1 / 3 ),
};


static uint32_t mztape_compute_block_checksum_32t ( uint8_t *block, uint16_t size ) {
    uint16_t checksum = 0;
    while ( size-- ) {
        uint8_t byte = *block;
        int bit;
        for ( bit = 0; bit < 8; bit++ ) {
            if ( byte & 1 ) checksum++;
            byte >>= 1;
        };
        block++;
    };
    return checksum;
}


static void mztape_compute_pulses ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_FORMATSET mztape_format, uint64_t *long_pulses, uint64_t *short_pulses ) {

    uint64_t total_long = 0;
    uint64_t total_short = 0;

    const en_MZTAPE_BLOCK *format = g_formats[mztape_format]->blocks;

    int i = 0;
    while ( format[i] != MZTAPE_BLOCK_STOP ) {

        switch ( format[i] ) {
            case MZTAPE_BLOCK_LGAP:
                total_short += g_formats[mztape_format]->lgap;
                break;

            case MZTAPE_BLOCK_SGAP:
                total_short += g_formats[mztape_format]->sgap;
                break;

            case MZTAPE_BLOCK_LTM:
                total_long += MZTAPE_LTM_LLENGTH;
                total_short += MZTAPE_LTM_SLENGTH;
                break;

            case MZTAPE_BLOCK_STM:
                total_long += MZTAPE_STM_LLENGTH;
                total_short += MZTAPE_STM_SLENGTH;
                break;

            case MZTAPE_BLOCK_2L:
                total_long += 2;
                break;

            case MZTAPE_BLOCK_256S:
                total_short += 256;
                break;

            case MZTAPE_BLOCK_HDR:
                total_long += mztmzf->chkh + sizeof ( st_MZF_HEADER ); // + stop bity
                total_short += ( sizeof ( st_MZF_HEADER ) * 8 ) - mztmzf->chkh;
                break;

            case MZTAPE_BLOCK_FILE:
                total_long += mztmzf->chkb + mztmzf->size; // + stop bity
                total_short += ( mztmzf->size * 8 ) - mztmzf->chkb;
                break;

            case MZTAPE_BLOCK_CHKH:
            {
                uint16_t chk = mztmzf->chkh;
                uint32_t chklong = mztape_compute_block_checksum_32t ( ( uint8_t* ) & chk, 2 );
                total_long += chklong + 2; // + stop bity
                total_short += 16 - chklong;
                break;
            }

            case MZTAPE_BLOCK_CHKF:
            {
                uint16_t chk = mztmzf->chkb;
                uint32_t chklong = mztape_compute_block_checksum_32t ( ( uint8_t* ) & chk, 2 );
                total_long += chklong + 2; // + stop bity
                total_short += 16 - chklong;
                break;
            }

            default:
                fprintf ( stderr, "%s() - Error: unknown block id=%d\n", __func__, format[i] );
        };
        i++;
    };

    *long_pulses = total_long;
    *short_pulses = total_short;
}


static void mztape_prepare_pulses_length ( st_MZTAPE_PULSES *pulses, en_MZTAPE_PULSESET pulseset, en_MZTAPE_SPEED mztape_speed ) {
    const st_MZTAPE_PULSES *src = g_pulses[pulseset];

    pulses->long_pulse.high = src->long_pulse.high * g_speed_multiplier[mztape_speed];
    pulses->long_pulse.low = src->long_pulse.low * g_speed_multiplier[mztape_speed];
    pulses->long_pulse.total = pulses->long_pulse.high + pulses->long_pulse.low;

    pulses->short_pulse.high = src->short_pulse.high * g_speed_multiplier[mztape_speed];
    pulses->short_pulse.low = src->short_pulse.low * g_speed_multiplier[mztape_speed];
    pulses->short_pulse.total = pulses->short_pulse.high + pulses->short_pulse.low;
}


static inline st_MZTAPE_PULSE* mztape_scan_onestate_block ( uint32_t *bit_counter, uint32_t block_size, st_MZTAPE_PULSE *block_pulse, int *flag_next_format_phase ) {

    st_MZTAPE_PULSE *pulse = NULL;

    if ( *bit_counter < block_size ) {
        pulse = block_pulse;
    } else {
        *flag_next_format_phase = 1;
    };

    *bit_counter += 1;
    return pulse;
}


static inline st_MZTAPE_PULSE* mztape_scan_twostate_block ( uint32_t *bit_counter, uint32_t block_lsize, uint32_t block_ssize, st_MZTAPE_PULSES *block_pulses, int *flag_next_format_phase ) {

    st_MZTAPE_PULSE *pulse = NULL;

    if ( *bit_counter < block_lsize ) {
        pulse = &block_pulses->long_pulse;
    } else if ( *bit_counter < ( block_lsize + block_ssize ) ) {
        pulse = &block_pulses->short_pulse;
    } else {
        *flag_next_format_phase = 1;
    };

    *bit_counter += 1;
    return pulse;
}


static inline st_MZTAPE_PULSE* mztape_scan_data_block ( uint32_t *bit_counter, uint32_t block_size, st_MZTAPE_PULSES *block_pulses, uint8_t *data, int *flag_next_format_phase ) {

    st_MZTAPE_PULSE *pulse = NULL;

    if ( *bit_counter < block_size ) {

        uint32_t byte_pos = *bit_counter / 9;
        int bit = *bit_counter % 9;

        if ( bit < 8 ) {
            uint8_t byte = data[byte_pos] << bit;

            if ( byte & 0x80 ) {
                pulse = &block_pulses->long_pulse;
            } else {
                pulse = &block_pulses->short_pulse;
            };
        } else {
            // stop bit
            pulse = &block_pulses->long_pulse;
        }

    } else {
        *flag_next_format_phase = 1;
    };

    *bit_counter += 1;
    return pulse;
}


void mztape_mztmzf_destroy ( st_MZTAPE_MZF *mztmzf ) {
    if ( !mztmzf ) return;
    ui_utils_mem_free ( mztmzf );
}


st_MZTAPE_MZF* mztape_create_mztmzf ( st_HANDLER *mzf_handler ) {

    st_HANDLER *h = mzf_handler;

    st_MZTAPE_MZF *mztmzf = ui_utils_mem_alloc0 ( sizeof ( st_MZTAPE_MZF ) );

    if ( !mztmzf ) {
        fprintf ( stderr, "%s():%d - Could create mztape mzf: %s\n", __func__, __LINE__, strerror ( errno ) );
        return NULL;
    };

    st_MZF_HEADER mzfhdr;

    if ( EXIT_SUCCESS != mzf_read_header ( h, &mzfhdr ) ) {
        fprintf ( stderr, "%s():%d - Could not read: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        mztape_mztmzf_destroy ( mztmzf );
        return NULL;
    };

    mztmzf->size = mzfhdr.fsize;

    if ( EXIT_SUCCESS != generic_driver_read ( h, 0, mztmzf->header, sizeof ( st_MZF_HEADER ) ) ) {
        fprintf ( stderr, "%s():%d - Could not read: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        mztape_mztmzf_destroy ( mztmzf );
        return NULL;
    };

    mztmzf->body = ui_utils_mem_alloc ( mztmzf->size );

    if ( EXIT_SUCCESS != generic_driver_read ( h, 0 + sizeof ( st_MZF_HEADER ), mztmzf->body, mzfhdr.fsize ) ) {
        fprintf ( stderr, "%s():%d - Could not read: %s, %s\n", __func__, __LINE__, strerror ( errno ), generic_driver_error_message ( h, h->driver ) );
        mztape_mztmzf_destroy ( mztmzf );
        ui_utils_mem_free ( mztmzf->body );
        return NULL;
    };

    mztmzf->chkh = mztape_compute_block_checksum_32t ( mztmzf->header, sizeof ( st_MZF_HEADER ) );
    mztmzf->chkb = mztape_compute_block_checksum_32t ( mztmzf->body, mztmzf->size );

    return mztmzf;
}


st_CMT_STREAM* mztape_create_cmt_stream_from_mztmzf ( st_MZTAPE_MZF *mztmzf, en_MZTAPE_FORMATSET mztape_format, en_MZTAPE_SPEED mztape_speed, uint32_t sample_rate ) {

    double sample_length = (double) 1 / sample_rate;

    /*
     * Secteni poctu pulzu a priprava jejich delky
     */

    uint64_t long_pulses;
    uint64_t short_pulses;

    mztape_compute_pulses ( mztmzf, mztape_format, &long_pulses, &short_pulses );

    st_MZTAPE_PULSES pulses;

    mztape_prepare_pulses_length ( &pulses, g_formats[mztape_format]->pulseset, mztape_speed );

    double stream_length = ( long_pulses * pulses.long_pulse.total ) + ( short_pulses * pulses.short_pulse.total );

    uint32_t data_size = stream_length / sample_length;

    /*
     * Vytvoreni CMT_STREAM
     */

    st_CMT_STREAM *cmt_stream = cmt_stream_new ( sample_rate, data_size );
    if ( !cmt_stream ) {
        fprintf ( stderr, "%s():%d - Could create cmt stream: %s\n", __func__, __LINE__, strerror ( errno ) );
        ui_utils_mem_free ( mztmzf->body );
        return NULL;
    };

    uint32_t sample_position = 0;
    uint32_t bit_counter = 0;
    double scan_time = 0;
    en_MZTAPE_BLOCK *format = g_formats[mztape_format]->blocks;
    int format_phase = 0;
    double pulse_time = 0;

    st_MZTAPE_PULSE *pulse = NULL;

    while ( scan_time <= stream_length ) {

        if ( ( pulse == NULL ) || ( pulse_time >= pulse->total ) ) {

            if ( pulse != NULL ) {
                pulse_time -= pulse->total;
            };

            int flag_skip_this_pulse = 0;

            do {

                int flag_next_format_phase;

                do {

                    flag_next_format_phase = 0;

                    switch ( format[format_phase] ) {

                        case MZTAPE_BLOCK_LGAP:
                            pulse = mztape_scan_onestate_block ( &bit_counter, g_formats[mztape_format]->lgap, &pulses.short_pulse, &flag_next_format_phase );
                            break;

                        case MZTAPE_BLOCK_SGAP:
                            pulse = mztape_scan_onestate_block ( &bit_counter, g_formats[mztape_format]->sgap, &pulses.short_pulse, &flag_next_format_phase );
                            break;

                        case MZTAPE_BLOCK_LTM:
                            pulse = mztape_scan_twostate_block ( &bit_counter, MZTAPE_LTM_LLENGTH, MZTAPE_LTM_SLENGTH, &pulses, &flag_next_format_phase );
                            break;

                        case MZTAPE_BLOCK_STM:
                            pulse = mztape_scan_twostate_block ( &bit_counter, MZTAPE_STM_LLENGTH, MZTAPE_STM_SLENGTH, &pulses, &flag_next_format_phase );
                            break;

                        case MZTAPE_BLOCK_2L:
                            pulse = mztape_scan_onestate_block ( &bit_counter, 2, &pulses.long_pulse, &flag_next_format_phase );
                            break;

                        case MZTAPE_BLOCK_256S:
                            pulse = mztape_scan_onestate_block ( &bit_counter, 256, &pulses.short_pulse, &flag_next_format_phase );
                            break;

                        case MZTAPE_BLOCK_HDR:
                        case MZTAPE_BLOCK_HDRC:
                            pulse = mztape_scan_data_block ( &bit_counter, ( sizeof ( st_MZF_HEADER ) * 8 ) + sizeof ( st_MZF_HEADER ), &pulses, mztmzf->header, &flag_next_format_phase );
                            break;

                        case MZTAPE_BLOCK_FILE:
                        case MZTAPE_BLOCK_FILEC:
                            pulse = mztape_scan_data_block ( &bit_counter, ( mztmzf->size * 8 ) + mztmzf->size, &pulses, mztmzf->body, &flag_next_format_phase );
                            break;

                        case MZTAPE_BLOCK_CHKH:
                        {
                            uint16_t chk = endianity_bswap16_BE ( mztmzf->chkh );
                            pulse = mztape_scan_data_block ( &bit_counter, 16 + 2, &pulses, ( uint8_t* ) & chk, &flag_next_format_phase );
                            break;
                        }

                        case MZTAPE_BLOCK_CHKF:
                        {
                            uint16_t chk = endianity_bswap16_BE ( mztmzf->chkb );
                            pulse = mztape_scan_data_block ( &bit_counter, 16 + 2, &pulses, ( uint8_t* ) & chk, &flag_next_format_phase );
                            break;
                        }

                        case MZTAPE_BLOCK_STOP:
                        default:
                            pulse = NULL;
                            break;
                    };

                    if ( flag_next_format_phase != 0 ) {
                        pulse = NULL;
                        bit_counter = 0;
                        format_phase++;
                    };

                } while ( flag_next_format_phase != 0 );


                if ( pulse != NULL ) {
                    if ( pulse_time >= pulse->total ) {
                        pulse_time -= pulse->total;
                        flag_skip_this_pulse = 1;
                    }
                } else {
                    flag_skip_this_pulse = 0;
                }

            } while ( flag_skip_this_pulse != 0 );

        };

        if ( pulse != NULL ) {
            int sample_value = ( pulse_time < pulse->high ) ? 0 : 1;
            cmt_stream_set_value_on_position ( cmt_stream, sample_position++, sample_value );
            pulse_time += sample_length;
        };

        scan_time += sample_length;
    }

    return cmt_stream;
}

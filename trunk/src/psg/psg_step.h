/* 
 * File:   psg_step.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 6. září 2016, 12:46
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


#ifndef PSG_STEP_H
#define PSG_STEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psg.h"


    static inline void psg_step ( void ) {

        unsigned cs;

        for ( cs = 0; cs < PSG_CHANNELS_COUNT; cs++ ) {
            st_PSG_CHANNEL *channel = &g_psg.channel [ cs ];


            if ( channel->attn != PSG_OUT_OFF ) {

                if ( channel->type == PSG_CHTYPE_TONE ) {

                    //                    printf ( "\t[%d] - a: %d, %d, %d\n", cs, channel->attn, channel->tone.divider, channel->timer );

                    /* tone */
                    st_PSG_TONE *tone = &channel->tone;
                    if ( tone->divider < 2 ) {
                        channel->output_signal = 1;

                    } else if ( 0 == channel->timer-- ) {
                        channel->timer = tone->divider - 1;
                        channel->output_signal = ( ~channel->output_signal ) & 0x01;
                    };
                } else {

                    //printf ( "\t[%d] - a: %d, %d, %d\n", cs, channel->attn, channel->noise.shiftregister, channel->timer );

                    /* noise */
                    unsigned bit0, bit3;
                    st_PSG_NOISE *noise = &channel->noise;
                    if ( ( noise->div_type == 0x03 ) && ( g_psg.channel [ PSG_CHANNEL_2 ].tone.divider < 2 ) ) {
                        channel->output_signal = 1;

                    } else if ( 0 == channel->timer-- ) {

                        if ( noise->div_type == NOISE_DIV_TYPE3 ) {
                            channel->timer = g_psg.channel [ PSG_CHANNEL_2 ].tone.divider - 1;
                        } else {
                            channel->timer = ( 0x10 << noise->div_type ) - 1;
                        };

                        bit0 = noise->shiftregister & 0x01;

                        if ( noise->last_noise_type != noise->type ) {
                            noise->shiftregister = 1 << 15;
                            noise->last_noise_type = noise->type;

                        } else if ( noise->type == NOISE_TYPE_WHITE ) {
                            bit3 = ( noise->shiftregister >> 3 ) & 0x01;
                            noise->shiftregister = noise->shiftregister >> 1;
                            noise->shiftregister |= ( bit0 ^ bit3 ) << 15;
                        } else {
                            noise->shiftregister = noise->shiftregister >> 1;
                            noise->shiftregister |= bit0 << 15;
                        };
                        channel->output_signal = noise->shiftregister & 0x01;
                    };
                };
            };
        };
    }


#ifdef __cplusplus
}
#endif

#endif /* PSG_STEP_H */


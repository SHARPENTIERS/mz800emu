/* 
 * File:   psg.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 23. července 2015, 7:33
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

/* Emulace PSG - SN76489AN */

#include "psg.h"
#include "gdg/gdg.h"
#include "audio.h"


#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"


st_PSG g_psg;


void psg_init ( void ) {
    g_psg.channel [ PSG_CHANNEL_0 ].type = PSG_CHTYPE_TONE;
    g_psg.channel [ PSG_CHANNEL_1 ].type = PSG_CHTYPE_TONE;
    g_psg.channel [ PSG_CHANNEL_2 ].type = PSG_CHTYPE_TONE;

    g_psg.channel [ PSG_CHANNEL_3 ].type = PSG_CHTYPE_NOISE;
    g_psg.channel [ PSG_CHANNEL_3 ].noise.shiftregister = 1 << 15;

    unsigned i;
    for ( i = 0; i < PSG_CHANNELS_COUNT; i++ ) {
        g_psg.channel [ i ].attn = PSG_OUT_OFF;
    };
}


void psg_step ( void ) {

    unsigned cs;

    for ( cs = 0; cs < PSG_CHANNELS_COUNT; cs++ ) {
        st_PSG_CHANNEL *channel = &g_psg.channel [ cs ];

        if ( channel->attn != PSG_OUT_OFF ) {

            if ( channel->type == PSG_CHTYPE_TONE ) {
                /* tone */
                st_PSG_TONE *tone = &channel->tone;
                if ( tone->divider < 2 ) {
                    channel->output_signal = 1;

                } else if ( 0 == channel->timer-- ) {
                    channel->timer = tone->divider - 1;
                    channel->output_signal = ( ~channel->output_signal ) & 0x01;
                };
            } else {
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



void psg_write_byte ( Z80EX_BYTE value ) {

    unsigned latch, attn, cs;

    DBGPRINTF ( DBGINF, "value = 0x%02x\n", value );

    latch = value & ( 1 << 7 );

    if ( latch ) {
        cs = ( value >> 5 ) & 0x03;
        attn = value & ( 1 << 4 );
        g_psg.latch_cs = cs;
        g_psg.latch_attn = attn;
    } else {
        cs = g_psg.latch_cs;
        attn = g_psg.latch_attn;
    };

    st_PSG_CHANNEL *channel = &g_psg.channel [ cs ];

    if ( attn ) {
        en_ATTENUATOR new_attn = value & 0x0f;
        if ( new_attn != channel->attn ) {
            audio_changed ( g_gdg.screen_ticks_elapsed );
            channel->attn = new_attn;
        };
    } else if ( ( latch ) && ( channel->type == PSG_CHTYPE_TONE ) ) {
        channel->tone.latch_divider = value & 0x0f;
    } else {
        if ( channel->type == PSG_CHTYPE_TONE ) {
            unsigned new_divider = ( value << 4 ) | channel->tone.latch_divider;
            if ( new_divider != channel->tone.divider ) {
                audio_changed ( g_gdg.screen_ticks_elapsed );
                channel->tone.divider = new_divider;
            };
        } else {
            en_NOISE_DIV_TYPE new_div_type = value & 0x03;
            en_NOISE_TYPE new_type = ( value >> 2 ) & 1;
            if ( ( new_div_type != channel->noise.div_type ) || ( new_type != channel->noise.type ) ) {
                audio_changed ( g_gdg.screen_ticks_elapsed );
                channel->noise.div_type = new_div_type;
                channel->noise.type = new_type;
            };
        };
    };
}


/* 
 * File:   pioz80.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 3. července 2015, 21:28
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

#ifndef PIOZ80_H
#define	PIOZ80_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"

    typedef enum en_PIOZ80_PORT {
        PIOZ80_PORT_A = 0,
        PIOZ80_PORT_B,
        PIOZ80_PORTS
    } en_PIOZ80_ADDR;

#define PIOZ80_PORT_MASK    ( 1 << 0 )
#define PIOZ80_CD_MASK      ( 1 << 1 )
#define PIOZ80_CONTROL      ( 0 << ( PIOZ80_CD_MASK / 2 ) )
#define PIOZ80_DATA         ( 1 << ( PIOZ80_CD_MASK / 2 ) )

    typedef enum en_PIOZ80_EXPECT {
        PIOZ80_CWSTATE_EXPECT_COMMAND = 0,
        PIOZ80_CWSTATE_EXPECT_MODEIO,
        PIOZ80_CWSTATE_EXPECT_INTMASK,
    } en_PIOZ80_EXPECT;
    

    typedef enum en_PIOZ80_INTFUNC {
        PIOZ80_INTFUNC_OR = 0,
        PIOZ80_INTFUNC_AND
    } en_PIOZ80_INTFUNC;

    
    typedef enum en_PIOZ80_ACTIVELVL {
        PIOZ80_ACTIVELVL_LOW = 0,
        PIOZ80_ACTIVELVL_HIGH
    } en_PIOZ80_ACTIVELVL;
    
typedef struct st_new_PIOZ80PORT {
    en_PIOZ80_EXPECT ctrl_expect;
    Z80EX_BYTE last_state;
    Z80EX_BYTE interrupt_vector;
    unsigned interrupt_enabled;
    en_PIOZ80_INTFUNC intfunc;
    en_PIOZ80_ACTIVELVL activelvl;
    Z80EX_BYTE interrupt_mask;
    Z80EX_BYTE io_mask;
    unsigned mode; /* Mode 0 - vse OUT, Mode 1 - vse IN, Mode 2 - jakysi bi direction rezim, Mode 3 - custom mode (JEN TENTO REZIM UMOZNUJE INTERRUPT - v MODE1 k interruptu nedojde!)*/
    Z80EX_BYTE value_latch;
    unsigned alarm;
} st_new_PIOZ80PORT;

typedef struct st_PIOZ80 {
    st_new_PIOZ80PORT port [ PIOZ80_PORTS ];
    unsigned signal_int;
    unsigned signal_ieo;
} st_PIOZ80;

st_PIOZ80 g_pioz80;
    
#define PIOZ80_INTERRUPT ( g_pioz80.signal_int )

    extern void pioz80_init ( void );
    extern void pioz80_reset ( void );
    
    extern Z80EX_BYTE pioz80_read_byte ( en_PIOZ80_ADDR addr );
    extern void pioz80_write_byte ( en_PIOZ80_ADDR, Z80EX_BYTE value );

    extern Z80EX_BYTE pioz80_intread_cb ( Z80EX_CONTEXT *cpu, void *user_data );
    extern void pioz80_reti_cb ( Z80EX_CONTEXT *cpu, void *user_data );

    extern void pioz80_port_event ( unsigned port, unsigned bit, unsigned value );
    
    extern void pioz80_int_ack ( void );

#ifdef	__cplusplus
}
#endif

#endif	/* PIOZ80_H */


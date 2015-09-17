/* 
 * File:   pioz80.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 3. července 2015, 21:27
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
 * Zjednodusena varianta PIO-Z80
 *
 *	Aplikujeme jen branu A, 4. - 5. bit
 *
 *	PA4: CTC0
 *	PA5: VBLN
 * 
 * Tato emulace obvodu nepocita s podporou vice portu - i kdyz se tak kod misty tvari !
 *
 */


/*
 * Odsledovane chovani:
 * 
 * - interrupt lze volat jen v MODE 3, nikoliv v mode 1
 * 
 * 1. nastavim PIO
 * 2. cpu: DI
 * 3. pomoci CTC0 vyvolam pulz, ktery aktivuje INT (CTC0 vratim do klidoveho stavu)
 * 4. PIO - disable int
 * 5. cpu: EI
 * 6. PIO - enable int
 * Vysledek: PIO opet aktivuje INT !!!
 * 
 * Pokud pred bodem 6. nastavim Mode 3 a libovolnou masku (i takovou, ktera vypne vsechny vstupy), tak se INT stejne zavola.
 * Stejne tak i pokud tam nastavim libovolny jiny MODE.
 * 
 * Pokud pred bodem 6. prijde RETI, tak to take volani nasledneho INT neovlivni.
 * 
 * Jakakoliv zmena INTMODE pred bodem 6. take neovlivni INT, POKUD nebyla nahrana i maska - potom je INT zapomenut.
 * 
 */


#include <string.h>

#include "z80ex/include/z80ex.h"

#include "pioz80/pioz80.h"
#include "mz800.h"
#include "ctc8253/ctc8253.h"
#include "gdg/gdg.h"

//#define DBGLEVEL (DBGNON /* | DBGERR | DBGWAR | DBGINF*/)
//#define DBGLEVEL (DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"

st_PIOZ80 g_pioz80;


void pioz80_init ( void ) {
    memset ( &g_pioz80, 0x00, sizeof ( g_pioz80 ) );
}


/* TODO: nevim zda se pri resetu skutecne neco takoveho deje, ale pokud to nepouzijem, tak napr. zatuhnem po RESET z cp/m */
void pioz80_reset ( void ) {
    memset ( &g_pioz80, 0x00, sizeof ( g_pioz80 ) );
    mz800_pioz80_interrupt_handle ( );
}

#define PIOZ80_PORT_SELECT  ( addr & PIOZ80_PORT_MASK )


Z80EX_BYTE pioz80_read_byte ( unsigned addr ) {

    if ( PIOZ80_DATA == ( addr & PIOZ80_CD_MASK ) ) {

        /* BUGFIX: Renegade od M.Kohutka cte portB na kterem se zpravidla nachazeji nahodna data */
        /* TODO: overit co presne ten renegade posune dal k zivotu */
        if ( PIOZ80_PORT_SELECT == PIOZ80_PORT_B ) {
            DBGPRINTF ( DBGINF, "addr = %d (DATA PORT_B), (bugfix Renegade) retval = 0x%02x, PC = 0x%04x\n", addr, g_pioz80.port [ PIOZ80_PORT_SELECT ].last_state, z80ex_get_reg ( g_mz800.cpu, regPC ) );
            return g_pioz80.port [ PIOZ80_PORT_SELECT ].last_state++;
        };
       
        Z80EX_BYTE retval = 0xc3; /* bit 2. a 3. = vzdy 0, 0. - 1. a 6. - 7. = printer ctrl ... 1 */
        
        retval |= ( CTC8253_OUT ( 0 ) == 1 ) ? 0 : 1 << 4;  /* invertovany CTC0 */
        retval |= SIGNAL_GDG_VBLNK ? 1 << 5 : 0x00;
       
        DBGPRINTF ( DBGINF, "addr = %d (DATA PORT_A), retval = 0x%02x, PC = 0x%04x\n", addr, retval, z80ex_get_reg ( g_mz800.cpu, regPC ) );
        
        return retval;
    };

    DBGPRINTF ( DBGINF, "addr = %d, read from ctrl is not supported!\n", addr );

    return 0xff;
}



#define PIOZ80_COMMAND_BITMASK      0x0f    /* Dolni 4 bity urcuji vyznam kontrolniho slova */
#define PIOZ80_COMMAND_MODEIO       0x0f
#define PIOZ80_COMMAND_INTCTRL      0x07
#define PIOZ80_COMMAND_INTDSBL      0x03


void pioz80_make_interrupt ( unsigned port ) {
    if ( g_pioz80.port [ port ].interrupt_enabled == 1 ) {
        /* interrupt je enabled */
        if ( g_pioz80.port [ port ].alarm == 1 ) {
            if ( ( g_pioz80.signal_ieo == 0 ) && ( g_pioz80.signal_int == 0 ) ) {
                g_pioz80.signal_ieo = 1;
                g_pioz80.signal_int = 1;
                mz800_pioz80_interrupt_handle ( );
                DBGPRINTF ( DBGINF, "Port: %d, INTERRUPT = 1\n", port );
            };
        } else {
            g_pioz80.signal_int = 0;
            mz800_pioz80_interrupt_handle ( );
        }

    } else if ( g_pioz80.signal_int == 1 ) {
        /* interrupt je disabled: meli jsme vystaveny int, ale zatim jej nikdo nepotvrdil */
        DBGPRINTF ( DBGINF, "INTERUPT = 0\n" );
        g_pioz80.signal_ieo = 0;
        g_pioz80.signal_int = 0;
        mz800_pioz80_interrupt_handle ( );
    };
}


void pioz80_write_byte ( unsigned addr, Z80EX_BYTE value ) {

    DBGPRINTF ( DBGINF, "RAW wr - addr = %d, value = 0x%02x, PC = 0x%04x\n", addr, value, z80ex_get_reg ( g_mz800.cpu, regPC ) );

    if ( PIOZ80_PORT_SELECT != PIOZ80_PORT_A ) {
        DBGPRINTF ( DBGWAR, "write into unsupported PORT B, addr = 0x%02x, value = 0x%02x\n", addr, value );
        return;
    };


    if ( PIOZ80_DATA == ( addr & PIOZ80_CD_MASK ) ) {
        DBGPRINTF ( DBGWAR, "write into data port is not supported, addr = 0x%02x, value = 0x%02x\n", addr, value );
        return;
    };


    if ( g_pioz80.port [ PIOZ80_PORT_SELECT ].ctrl_expect == PIOZ80_CWSTATE_EXPECT_COMMAND ) {

        unsigned modeio;

        switch ( value & PIOZ80_COMMAND_BITMASK ) {

            case PIOZ80_COMMAND_MODEIO:

                modeio = value >> 6;

                switch ( modeio ) {
                    case 0:
                        g_pioz80.port [ PIOZ80_PORT_SELECT ].mode = 0;
                        DBGPRINTF ( DBGWAR, "MODEIO - port = %d, modeio = 0 - OUTPUT (Unsupported!)\n", PIOZ80_PORT_SELECT );
                        break;

                    case 1:
                        g_pioz80.port [ PIOZ80_PORT_SELECT ].mode = 1;
                        DBGPRINTF ( DBGWAR, "MODEIO - port = %d, modeio = 1 - INPUT (Unsupported!)\n", PIOZ80_PORT_SELECT );
                        break;

                    case 2:
                        g_pioz80.port [ PIOZ80_PORT_SELECT ].mode = 2;
                        DBGPRINTF ( DBGWAR, "MODEIO - port = %d, modeio = 2 - BI-DIR (Unsupported!)\n", PIOZ80_PORT_SELECT );
                        break;

                    case 3:
                        g_pioz80.port [ PIOZ80_PORT_SELECT ].ctrl_expect = PIOZ80_CWSTATE_EXPECT_MODEIO;
                        DBGPRINTF ( DBGINF, "MODEIO - port = %d, modeio = 3 - CUSTOM - io mask is expected\n", PIOZ80_PORT_SELECT );
                        break;
                };
                break;

            case PIOZ80_COMMAND_INTCTRL:
                if ( 1 == ( ( value >> 4 ) & 1 ) ) {
                    /* zadost o vlozeni masky m.j. resetuje alarm */
#if ( DBGLEVEL & DBGINF )
                    if ( g_pioz80.port [ PIOZ80_PORT_SELECT ].alarm | g_pioz80.signal_ieo | g_pioz80.signal_int ) {
                        DBGPRINTF ( DBGINF, "Port: %d, INTCTRL reset alarm state - alarm: %d, int: %d, ieo: %d\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].alarm, g_pioz80.signal_int, g_pioz80.signal_ieo );
                    };
                    DBGPRINTF ( DBGINF, "INTCTRL - Port: %d, interrupt mask is expected\n", PIOZ80_PORT_SELECT );
#endif                    
                    g_pioz80.port [ PIOZ80_PORT_SELECT ].alarm = 0;
                    g_pioz80.signal_ieo = 0;
                    g_pioz80.signal_int = 0;
                    mz800_pioz80_interrupt_handle ( );

                    g_pioz80.port [ PIOZ80_PORT_SELECT ].ctrl_expect = PIOZ80_CWSTATE_EXPECT_INTMASK;
                    g_pioz80.port [ PIOZ80_PORT_SELECT ].value_latch = value;
                } else {
                    g_pioz80.port [ PIOZ80_PORT_SELECT ].activelvl = ( value >> 5 ) & 1;
                    g_pioz80.port [ PIOZ80_PORT_SELECT ].intfunc = ( value >> 6 ) & 1;

                    g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_enabled = ( value >> 7 ) & 1;
#if ( DBGLEVEL & DBGINF )
                    char strfunc [ 4 ];
                    if ( g_pioz80.port [ PIOZ80_PORT_SELECT ].intfunc == 0 ) {
                        strncpy ( strfunc, "OR", 3 );
                    } else {
                        strncpy ( strfunc, "AND", 4 );
                    };
                    if ( PIOZ80_PORT_SELECT == PIOZ80_PORT_A ) {
                        DBGPRINTF ( DBGINF, "INTCTRL - Port = %d, active_lvl = %d, intfunc = %s, interrupt_enabled = %d, old_mask (hint:active is 0) = 0x%02x: PA4 (inverted CTC0) = %d, PA5 (VBLN) = %d\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].activelvl, strfunc, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_enabled, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask, ( g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask >> 4 ) & 0x01, ( g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask >> 5 ) & 0x01 );
                    } else {
                        DBGPRINTF ( DBGINF, "INTCTRL - Port = %d, active_lvl = %d, intfunc = %s, interrupt_enabled = %d, old_mask (hint:active is 0) = 0x%02x\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].activelvl, strfunc, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_enabled, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask );
                    };
#endif
                    pioz80_make_interrupt ( PIOZ80_PORT_SELECT );

                };
                break;

            case PIOZ80_COMMAND_INTDSBL:
                g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_enabled = ( value >> 7 ) & 1;
                DBGPRINTF ( DBGINF, "port = %d, interrupt_enabled = %d\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_enabled );
                pioz80_make_interrupt ( PIOZ80_PORT_SELECT );

                break;

            default:
                if ( 0 == ( value & 0x01 ) ) {
                    g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_vector = value;
#if ( DBGLEVEL & DBGINF )
                    DBGPRINTF ( DBGINF, "port = %d, interrupt_vector = 0x%02x\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_vector );
                } else {
                    DBGPRINTF ( DBGINF, "port = %d - Unknown command: 0x%02x!\n", PIOZ80_PORT_SELECT, ( value & PIOZ80_COMMAND_BITMASK ) );
#endif
                };
                break;
        };

    } else if ( g_pioz80.port [ PIOZ80_PORT_SELECT ].ctrl_expect == PIOZ80_CWSTATE_EXPECT_INTMASK ) {
        g_pioz80.port [ PIOZ80_PORT_SELECT ].ctrl_expect = PIOZ80_CWSTATE_EXPECT_COMMAND;
        g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask = value;

        value = g_pioz80.port [ PIOZ80_PORT_SELECT ].value_latch;
        g_pioz80.port [ PIOZ80_PORT_SELECT ].activelvl = ( value >> 5 ) & 1;
        g_pioz80.port [ PIOZ80_PORT_SELECT ].intfunc = ( value >> 6 ) & 1;
        g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_enabled = ( value >> 7 ) & 1;

#if ( DBGLEVEL & DBGINF )
        char strfunc [ 4 ];
        if ( g_pioz80.port [ PIOZ80_PORT_SELECT ].intfunc == 0 ) {
            strncpy ( strfunc, "OR", 3 );
        } else {
            strncpy ( strfunc, "AND", 4 );
        };
        if ( PIOZ80_PORT_SELECT == PIOZ80_PORT_A ) {
            DBGPRINTF ( DBGINF, "INTCTRL MASK - Port = %d, active_lvl = %d, intfunc = %s, interrupt_enabled = %d, new_mask (hint:active is 0) = 0x%02x, PA4 (inverted CTC0) = %d, PA5 (VBLN) = %d\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].activelvl, strfunc, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_enabled, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask, ( g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask >> 4 ) & 0x01, ( g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask >> 5 ) & 0x01 );
        } else {
            DBGPRINTF ( DBGINF, "INTCTRL MASK - Port = %d, active_lvl = %d, intfunc = %s, interrupt_enabled = %d, new_mask (hint:active is 0) = 0x%02x\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].activelvl, strfunc, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_enabled, g_pioz80.port [ PIOZ80_PORT_SELECT ].interrupt_mask );
        };
#endif

    } else if ( g_pioz80.port [ PIOZ80_PORT_SELECT ].ctrl_expect == PIOZ80_CWSTATE_EXPECT_MODEIO ) {
        g_pioz80.port [ PIOZ80_PORT_SELECT ].mode = 3;
        g_pioz80.port [ PIOZ80_PORT_SELECT ].ctrl_expect = PIOZ80_CWSTATE_EXPECT_COMMAND;
        g_pioz80.port [ PIOZ80_PORT_SELECT ].io_mask = value;
#if ( DBGLEVEL & DBGINF )
        if ( PIOZ80_PORT_SELECT == PIOZ80_PORT_A ) {
            DBGPRINTF ( DBGINF, "MODEIO MASK - Port = %d, io_mask (hint:input is 1) = 0x%02x, PA4 (inverted CTC0) = %d, PA5 (VBLN) = %d\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].io_mask, ( g_pioz80.port [ PIOZ80_PORT_SELECT ].io_mask >> 4 ) & 0x01, ( g_pioz80.port [ PIOZ80_PORT_SELECT ].io_mask >> 5 ) & 0x01 );
        } else {
            DBGPRINTF ( DBGINF, "MODEIO MASK - Port = %d, io_mask (hint:input is 1) = 0x%02x\n", PIOZ80_PORT_SELECT, g_pioz80.port [ PIOZ80_PORT_SELECT ].io_mask );
        };
#endif
    };

}


/* Callback volany pri IM 2 interruptu: M1 + IORQ */
Z80EX_BYTE pioz80_intread_cb ( Z80EX_CONTEXT *cpu, void *user_data ) {

    //printf ( "im2_cb\n" );
    if ( g_pioz80.signal_int != 1 ) {
        g_pioz80.signal_int = 0;
        g_pioz80.signal_ieo = 0;
        mz800_pioz80_interrupt_handle ( );
        /* TODO: Zjistit co se deje, kdyz prijde IM 2 interrupt, ale PIOZ80 neni v alarm */
        DBGPRINTF ( DBGERR, "Interrupt IM 2 CB ERROR - PIOZ80 not in Alarm\n" );
        return 0x00;
    };

    unsigned port;
    for ( port = 0; port < PIOZ80_PORTS; port++ ) {
        if ( g_pioz80.port[port].alarm == 1 ) break;
    };

    if ( port == PIOZ80_PORTS ) {
        g_pioz80.signal_int = 0;
        g_pioz80.signal_ieo = 0;
        mz800_pioz80_interrupt_handle ( );
        /* TODO: Zjistit co se deje, kdyz prijde IM 2 interrupt, ale PIOZ80 neni v alarm */
        DBGPRINTF ( DBGERR, "Interrupt IM 2 CB ERROR - PIOZ80 not in Alarm\n" );
        return 0x00;
    };

    DBGPRINTF ( DBGINF, "Interrupt Ack - IM 2, Interrupt Vector: 0x%04x\n", ( ( z80ex_get_reg ( cpu, regI ) << 8 ) | g_pioz80.port[port].interrupt_vector ) );

    g_pioz80.signal_int = 0;
    g_pioz80.signal_ieo = 1;
    g_pioz80.port[port].alarm = 0;
    mz800_pioz80_interrupt_handle ( );

    return g_pioz80.port[port].interrupt_vector;
}


/* Interrupt byl prijat */


/* TODO: zjistit co se stane, kdyz PIO po IM 2 CB nedostane RETI, ale opet dostane INT ACK */
void pioz80_int_ack ( void ) {

    if ( g_pioz80.signal_int == 0 ) return;

    DBGPRINTF ( DBGINF, "Interrupt Ack in not IM 2 mode?\n" );

    g_pioz80.signal_ieo = 0;
    g_pioz80.signal_int = 0;

    unsigned port;
    for ( port = 0; port < PIOZ80_PORTS; port++ ) {
        if ( g_pioz80.port[port].alarm == 1 ) break;
    };

    if ( port != PIOZ80_PORTS ) {
        g_pioz80.port[port].alarm = 0;
    };

    mz800_pioz80_interrupt_handle ( );
}


void pioz80_reti_cb ( Z80EX_CONTEXT *cpu, void *user_data ) {

    DBGPRINTF ( DBGINF, "RETI: INT = %d, IEO = %d, alarm = %d\n", g_pioz80.signal_int, g_pioz80.signal_ieo, g_pioz80.port[PIOZ80_PORT_A].alarm );

    if ( ( g_pioz80.signal_int == 0 ) && ( g_pioz80.signal_ieo == 1 ) ) {
        g_pioz80.signal_ieo = 0;
        pioz80_make_interrupt ( PIOZ80_PORT_A );
    };
}


#define DBG_PRINT_EVENT 0


/*
 * PA4 - inverted CTC0
 * PA5 - VBLN
 * 
 */
void pioz80_port_event ( unsigned port, unsigned bit, unsigned value ) {

#if ( DBGLEVEL & DBGINF )
#if DBG_PRINT_EVENT
    if ( bit == 5 ) {
        DBGPRINTF ( DBGINF, "EVENT - VBLN, last = %d, value = %d\n", g_pioz80.port[port].last_state >> bit, value );
    };
#endif
#endif

    if ( g_pioz80.port[port].mode != 3 ) {
#if ( DBGLEVEL & DBGINF )
#if DBG_PRINT_EVENT

        if ( bit == 5 ) {
            DBGPRINTF ( DBGINF, "EVENT STOP! bad mode\n" );
        };
#endif
#endif
        return;
    };

    //    if ( g_pioz80.port[port].interrupt_enabled != 1 ) return;

    if ( g_pioz80.signal_int != 0 ) {
        /* Doplneno podle toho jak se chovalo interkarate ( demo rezim - po startu ok, ale jak se zacali mydlit, tak obcas nejaky interrupt chybel)*/
        if ( value ) {
            g_pioz80.port[port].last_state |= ( 1 << bit );
        } else {
            g_pioz80.port[port].last_state &= ~( 1 << bit );
        };
#if ( DBGLEVEL & DBGINF )
#if DBG_PRINT_EVENT
        if ( bit == 5 ) {
            DBGPRINTF ( DBGINF, "EVENT STOP! signal_int = 1\n" );
        };
#endif
#endif
        return;
    };

    /* neni pravda pri IEO = 1 - odhaleno diky Antiriad CS */
    //if ( g_pioz80.port[port].alarm != 0 ) return;
    if ( ( g_pioz80.port[port].alarm != 0 ) && ( g_pioz80.signal_ieo != 1 ) ) {
#if ( DBGLEVEL & DBGINF )
#if DBG_PRINT_EVENT

        if ( bit == 5 ) {
            DBGPRINTF ( DBGINF, "EVENT STOP! alarm = %d && signal_ieo = %d\n", g_pioz80.port[port].alarm, g_pioz80.signal_ieo );
        };
#endif
#endif
        return;
    };

    if ( g_pioz80.port [port].ctrl_expect == PIOZ80_CWSTATE_EXPECT_INTMASK ) {
#if ( DBGLEVEL & DBGINF )
#if DBG_PRINT_EVENT
        if ( bit == 5 ) {
            DBGPRINTF ( DBGINF, "EVENT STOP! mask expected\n" );
        };
#endif
#endif
        return;
    };

    unsigned sts_bits;
    unsigned port_bit_mask = 1 << bit;


    /* doslo k nejake zmene od posledniho znameho stavu? */
    if ( ( g_pioz80.port[port].last_state & port_bit_mask ) == ( value << bit ) ) {
#if ( DBGLEVEL & DBGINF )
        if ( bit == 5 ) {
            DBGPRINTF ( DBGINF, "EVENT STOP! input state not changed\n" );
        };
#endif
        return;
    };

#if ( DBGLEVEL & DBGINF )
    /*
    if ( port_bit == 4 ) {
        DBGPRINTF ( DBGINF, "CTC0 changed: %d\n", input_signal );
    };
     */
#endif

    /* ulozime si novy stav */
    if ( value ) {
        g_pioz80.port[port].last_state |= ( 1 << bit );
    } else {
        g_pioz80.port[port].last_state &= ~( 1 << bit );
    };

    /* zajima nas tento port_bit? je vstupni? */
    if ( 0 == ( ( ~g_pioz80.port[port].interrupt_mask ) & port_bit_mask & g_pioz80.port[port].io_mask ) ) {
        return;
    };


    /* je splnena podminka pro alarm? */

    /* nejprve stav portu, podle mask a hilow */
    sts_bits = g_pioz80.port[port].io_mask & ( ~g_pioz80.port[port].interrupt_mask );
    if ( g_pioz80.port[port].activelvl ) {
        /* zajimaji nas HI hodnoty */
        sts_bits &= g_pioz80.port[port].last_state;
    } else {
        /* zajimaji nas LO hodnoty */
        sts_bits &= ~g_pioz80.port[port].last_state;
    };


    /* je pri OR nastaven alespon 1 bit? */
    if ( ( g_pioz80.port[port].intfunc == PIOZ80_INTFUNC_OR ) && ( sts_bits != 0 ) ) {
        g_pioz80.port[port].alarm = 1;
        DBGPRINTF ( DBGINF, "port: %d, alarm = 1\n", port );

        /* je pri AND vyhovujici vse co je vybrano maskou? */
    } else if ( ( g_pioz80.port[port].intfunc == PIOZ80_INTFUNC_AND ) && ( sts_bits == ( ( ~g_pioz80.port[port].interrupt_mask ) & 0x30 ) ) ) {
        g_pioz80.port[port].alarm = 1;
        DBGPRINTF ( DBGINF, "port: %d, alarm = 1\n", port );
    } else {
        /* prave jsme v IEO = 1, INT = 0 a prestala platit podminka pro alarm */
        /* Odhaleno diky Antiriad CS */
        g_pioz80.port[port].alarm = 0;
        DBGPRINTF ( DBGINF, "port: %d, alarm = 0\n", port );
    }

    pioz80_make_interrupt ( port );
}

/* 
 * File:   breakpoints.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 30. září 2015, 10:15
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

#include <stdio.h>
#include <string.h>

#ifdef MZ800_DEBUGGER

#include "breakpoints.h"

st_BREAPOINTS g_breakpoints;


void breakpoints_init ( void ) {
    memset ( g_breakpoints.bpmap, -1, sizeof ( g_breakpoints.bpmap ) );
}


int breakpoints_event_add ( Z80EX_WORD addr, unsigned id ) {
    if ( g_breakpoints.bpmap [ addr ] != -1 ) return g_breakpoints.bpmap [ addr ];
    g_breakpoints.bpmap [ addr ] = id;
    return 0;
}


int breakpoints_event_clear ( Z80EX_WORD addr, int id ) {
    if ( g_breakpoints.bpmap [ addr ] != id ) return 0;
    g_breakpoints.bpmap [ addr ] = -1;
    return 1;
}


void breakpoints_event_clear_addr ( Z80EX_WORD addr ) {
    g_breakpoints.bpmap [ addr ] = -1;
}


void breakpoints_event_clear_id ( int id ) {
    unsigned addr;
    for ( addr = 0; addr <= 0xffff; addr++ ) {
        if ( g_breakpoints.bpmap [ addr ] == id ) {
            g_breakpoints.bpmap [ addr ] = -1;
        };
    };
}


int breakpoints_event_get_addr_by_id ( int id ) {
    unsigned addr;
    for ( addr = 0; addr <= 0xffff; addr++ ) {
        if ( g_breakpoints.bpmap [ addr ] == id ) {
            return addr;
        };
    };
    return -1;
}

int breakpoints_event_get_id_by_addr ( Z80EX_WORD addr ) {
    return g_breakpoints.bpmap [ addr ];
}

#endif

/* 
 * File:   ramdisk.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 10. srpna 2015, 11:10
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

#ifndef RAMDISK_H
#define	RAMDISK_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "z80ex/include/z80ex.h"

#define RAMDISK_DEFAULT_FILENAME	"rd.dat"


#define RAMDISK_IS_READONLY	( 1 << 1 )
#define RAMDISK_IS_IN_FILE	( 1 << 0 )

    typedef enum en_RAMDISK_TYPE {
        RAMDISK_TYPE_STD = 0,
        RAMDISK_TYPE_SRAM = RAMDISK_IS_IN_FILE,
        RAMDISK_TYPE_ROM = ( RAMDISK_IS_IN_FILE | RAMDISK_IS_READONLY )
    } en_RAMDISK_TYPE;

#define RAMDISK_PEZIK_E8	0x01
#define RAMDISK_PEZIK_68	0x00

    typedef enum en_RAMDISK_BANKMASK {
        RAMDISK_SIZE_64 = 0x00,
        RAMDISK_SIZE_256 = 0x03,
        RAMDISK_SIZE_512 = 0x07,
        RAMDISK_SIZE_1M = 0x0f,
        RAMDISK_SIZE_16M = 0xff
    } en_RAMDISK_BANKMASK;


#define RAMDISK_CONNECTED        1
#define RAMDISK_DISCONNECTED     0

#define RAMDISK_FILENAME_LENGTH 1024

    typedef struct st_RAMDISKPEZIK {
        unsigned connected;
        Z80EX_WORD latch;
        Z80EX_BYTE *memory;
    } st_RAMDISKPEZIK;

    typedef struct st_RAMDISKSTD {
        unsigned connected;
        en_RAMDISK_TYPE type;
        en_RAMDISK_BANKMASK size;
        char filepath [ RAMDISK_FILENAME_LENGTH ];
        Z80EX_WORD offset;
        Z80EX_BYTE bank;
        Z80EX_BYTE *memory;
    } st_RAMDISKSTD;

    typedef struct st_RAMDISK {
        st_RAMDISKSTD std;
        st_RAMDISKPEZIK pezik [ 2 ];
    } st_RAMDISK;

    extern st_RAMDISK g_ramdisk;

    extern void ramdisc_exit ( void );

    extern void ramdisk_init ( void );

    extern void ramdisk_std_init ( int connect, en_RAMDISK_TYPE type, en_RAMDISK_BANKMASK size, char *filepath );
    extern void ramdisk_std_disconnect ( void );
    extern void ramdisk_std_save ( void );
    extern void ramdisk_std_open_file ( void );

    extern void ramdisk_pezik_init ( int pezik_type, int connect );

    extern Z80EX_BYTE ramdisk_std_read_byte ( unsigned addr );
    extern void ramdisk_std_write_byte ( unsigned addr, Z80EX_BYTE value );

    extern Z80EX_BYTE ramdisk_pezik_read_byte ( unsigned addr );
    extern void ramdisk_pezik_write_byte ( unsigned addr, Z80EX_BYTE value );



#ifdef	__cplusplus
}
#endif

#endif	/* RAMDISK_H */


/* 
 * File:   qdisk.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. února 2016, 18:04
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

#ifndef QDISK_H
#define	QDISK_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef WIN32
#define COMPILE_FOR_EMULATOR
#undef COMPILE_FOR_UNICARD
#undef FS_LAYER_FATFS
#elif LINUX
#define COMPILE_FOR_EMULATOR
#undef COMPILE_FOR_UNICARD
#undef FS_LAYER_FATFS
#else
#undef COMPILE_FOR_EMULATOR
#define COMPILE_FOR_UNICARD
#define FS_LAYER_FATFS
#endif

#include "z80ex/include/z80ex.h"


#define QDISK_IMAGE_SIZE        61455

#define QDISK_DISCONNECTED     0
#define QDISK_CONNECTED        1

#define QDSIO_CHANNEL_A        0
#define QDSIO_CHANNEL_B        1


#define QDSTS_NO_DISC       0
#define QDSTS_IMG_READY     1
#define QDSTS_HEAD_HOME     2
#define QDSTS_IMG_SYNC      4
#define QDSTS_IMG_READONLY  8


#define QDISK_TYPE_IMAGE    0
#define QDISK_TYPE_VIRTUAL  1

#ifndef FS_LAYER_FATFS
#define QDISKK_FILENAME_LENGTH 1024
#endif

    typedef enum en_QDSIO_ADDR {
        QDSIO_ADDR_DATA_A = 0,
        QDSIO_ADDR_DATA_B,
        QDSIO_ADDR_CTRL_A,
        QDSIO_ADDR_CTRL_B
    } en_QDSIO_ADDR;

    typedef enum en_QDSIO_REGADRR {
        QDSIO_REGADDR_0 = 0, /* zakladni prikazy */
        QDSIO_REGADDR_1, /* nastaveni generovani interruptu a signalu Wait/Ready */
        QDSIO_REGADDR_2, /* vektor preruseni (zastoupen jen u kanalu B) */
        QDSIO_REGADDR_3, /* rizeni prijmu */
        QDSIO_REGADDR_4, /* nastaveni vlastnosti prenosu */
        QDSIO_REGADDR_5, /* nastaveni odesilani */
        QDSIO_REGADDR_6, /* sync1 */
        QDSIO_REGADDR_7 /* sync2 */
    } en_QDSIO_REGADRR;

    typedef enum en_QDSIO_WR0CMD {
        QDSIO_WR0CMD_NONE = 0,
        QDSIO_WR0CMD_SDLC_STOP, /* ukoncit vysilani (pouze v rezimu SDLC) */
        QDSIO_WR0CMD_RESET_INTF, /* reset priznaku preruseni */
        QDSIO_WR0CMD_RESET, /* reset kanalu */
        QDSIO_WR0CMD_ENABLE_INT, /* povol generovani interruptu pri dalsim prijatem znaku */
        QDSIO_WR0CMD_RESET_OUTBUF_INT, /* resetuj generovani interruptu pri prazdnem output bufferu */
        QDSIO_WR0CMD_RESET_ERRFL, /* reset  priznaku chyby */
        QDSIO_WR0CMD_RETI /* navrat z preruseni */
    } en_QDSIO_WR0CMD;

    typedef struct st_QDSIO_CHANNEL {
        char name;
        en_QDSIO_REGADRR REG_addr;
        Z80EX_BYTE Wreg [ 8 ];
        Z80EX_BYTE Rreg [ 3 ];
    } st_QDSIO_CHANNEL;

    typedef struct st_QDISK {
        unsigned connected;
        unsigned type;
        st_QDSIO_CHANNEL channel [ 2 ];
        unsigned status;
        FILE *fp;
        Z80EX_WORD out_crc16;
        unsigned image_position;
    } st_QDISK;

    extern st_QDISK g_qdisk;

    extern void qdisk_init ( void );
    extern void qdisk_exit ( void );
    extern Z80EX_BYTE qdisk_read_byte ( en_QDSIO_ADDR SIO_addr );
    extern void qdisk_write_byte ( en_QDSIO_ADDR SIO_addr, Z80EX_BYTE value );
    extern void qdisk_close ( void );
    extern void qdisk_open ( void );
    extern void qdisk_mount_image ( void );
    extern void qdisk_umount_image ( void );
    extern void qdisk_set_write_protected ( int value );


#ifdef	__cplusplus
}
#endif

#endif	/* QDISK_H */

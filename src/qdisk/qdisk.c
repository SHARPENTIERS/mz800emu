/* 
 * File:   qdisk.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. února 2016, 18:03
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


//#define DBGLEVEL        ( DBGNON /* | DBGERR | DBGWAR | DBGINF */ )
//#define DBGLEVEL        ( DBGNON | DBGERR | DBGWAR | DBGINF )
#include "debug.h"

#include "../fs_layer.h"

#include "z80ex/include/z80ex.h"

#include "main.h"

#include "mz800.h"
#include "qdisk.h"


#ifdef COMPILE_FOR_UNICARD
#include "hal.h"
#include "monitor.h"
#include "mzint.h"
#else
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "ui/ui_main.h"
#include "ui/ui_utils.h"
#include "ui/ui_qdisk.h"
#include "cfgmain.h"
#endif


st_QDISK g_qdisk;

CFGELM *g_elm_std_fp;
CFGELM *g_elm_virt_fp;
CFGELM *g_elm_wrprt;

//#define QDIMAGE_FILENAME    "notinitialized.mzq"
#define QDIMAGE_FILENAME    "empty.mzq"
//#define QDIMAGE_FILENAME    "disk_flappy.mzq"
//#define QDIMAGE_FILENAME    "3hry.mzq"


void qdisk_close ( void ) {
    if ( g_qdisk.connected == QDISK_CONNECTED ) {
        if ( g_qdisk.status & QDSTS_IMG_READY ) {
            g_qdisk.status &= ~QDSTS_IMG_READY;
            if ( FR_OK != FILE_FSYNC ( g_qdisk.fp ) ) {
                DBGPRINTF ( DBGERR, "fsync()\n" );
#ifdef COMPILE_FOR_EMULATOR
                ui_show_error ( "%s():%d - fsync error: %s", __func__, __LINE__, strerror ( errno ) );
#endif
            };
            FILE_FCLOSE ( g_qdisk.fp );
        };
    };
}

#define QDISK_CREATE_BLOCK_SIZE 100


void qdisk_create_image ( char *filename ) {

    FILE *fp;

    if ( !( fp = ui_utils_fopen ( filename, FILE_MODE_W ) ) ) {
        DBGPRINTF ( DBGERR, "fopen()\n" );
#ifdef COMPILE_FOR_EMULATOR
        ui_show_error ( "%s() - Can't create file '%s': %s", __func__, filename, strerror ( errno ) );
#endif
        return;
    };

    uint8_t block [ QDISK_CREATE_BLOCK_SIZE ];
    memset ( &block, 0x00, sizeof ( block ) );

    unsigned i;
    unsigned len;
    
    for ( i = QDISK_IMAGE_SIZE; i >= QDISK_CREATE_BLOCK_SIZE; i -= QDISK_CREATE_BLOCK_SIZE ) {
        if ( FR_OK != FILE_FWRITE ( fp, &block, sizeof ( block ), &len ) ) {
            DBGPRINTF ( DBGERR, "fwrite() error\n" );
        };
    };

    if ( i ) {
        if ( FR_OK != FILE_FWRITE ( fp, &block, i, &len ) ) {
            DBGPRINTF ( DBGERR, "fwrite() error\n" );
        };  
    };
    
    if ( FR_OK != FILE_FSYNC ( fp ) ) {
        DBGPRINTF ( DBGERR, "fsync()\n" );
#ifdef COMPILE_FOR_EMULATOR
        ui_show_error ( "%s() - Can't sync file '%s': %s", __func__, filename, strerror ( errno ) );
#endif        
    };

    FILE_FCLOSE ( fp );
}


void qdisk_open_image ( char *path ) {

    if ( g_qdisk.connected == QDISK_CONNECTED ) {

        g_qdisk.status = QDSTS_NO_DISC;
        g_qdisk.image_position = 0;
        qdisk_close ( );

        if ( strlen ( path ) != 0 ) {

            char *open_file_mode;
            unsigned read_only_flag;

            if ( ( 0 != cfgelement_get_bool_value ( g_elm_wrprt ) ) || ( ui_utils_access ( path, W_OK ) == -1 ) ) {
                open_file_mode = FILE_MODE_RO;
                read_only_flag = QDSTS_IMG_READONLY;
            } else {
                open_file_mode = FILE_MODE_RW;
                read_only_flag = 0;
            };

            if ( ( g_qdisk.fp = ui_utils_fopen ( path, open_file_mode ) ) ) {
                g_qdisk.status = QDSTS_IMG_READY | QDSTS_HEAD_HOME | read_only_flag;

#ifdef COMPILE_FOR_EMULATOR
#if 0
                printf ( "QDISK open: '%s' in ", path );
                if ( read_only_flag == 0 ) {
                    printf ( "R/W mode\n" );
                } else {
                    printf ( "R/O mode\n" );
                };
#endif
                cfgelement_set_text_value ( g_elm_std_fp, path );
#endif
            } else {
#ifdef COMPILE_FOR_EMULATOR
                cfgelement_set_text_value ( g_elm_std_fp, "" );
                ui_show_error ( "%s() - Can't open file '%s': %s", __func__, path, strerror ( errno ) );
#endif
            };
        } else {
#ifdef COMPILE_FOR_EMULATOR
            cfgelement_set_text_value ( g_elm_std_fp, "" );
#endif
        };

    } else {
        g_qdisk.status = QDSTS_NO_DISC;
    };
}


void qdisk_open ( void ) {
    if ( g_qdisk.type == QDISK_TYPE_IMAGE ) {
        char *filepath = cfgelement_get_text_value ( g_elm_std_fp );
        qdisk_open_image ( filepath );
#ifdef COMPILE_FOR_EMULATOR
        ui_qdisk_set_path ( filepath );
#endif
    } else {
        /* TODO: */
    };
}


void qdisk_mount_image ( void ) {
    char window_title[] = "Select MZQ file to open or type new image name";
    char filename [ QDISKK_FILENAME_LENGTH ];

    char *cfg_filename = cfgelement_get_text_value ( g_elm_std_fp );
    if ( cfg_filename [ 0 ] == 0x00 ) {
        cfg_filename = "new_image.mzq";
    };

    unsigned err;

    do {

        err = 0;

        filename[0] = 0x00;
        ui_open_file ( filename, cfg_filename, sizeof ( filename ), FILETYPE_MZQ, window_title, OPENMODE_READ_OR_NEW );

        unsigned len = strlen ( filename );

        if ( len == 0 ) {
            return;
        };

        if ( len < 5 ) {
            err = 1;
        } else {
            char *surfix = &filename [ len - 4 ];

            if ( 0 != strcasecmp ( surfix, ".mzq" ) ) {
                err = 1;
            };
        };

        if ( err ) {
            ui_show_error ( "%s() - Bad mzq image file name '%s'", __func__, filename );
        };

    } while ( err );

    if ( ui_utils_access ( filename, F_OK ) == -1 ) {
        /* soubor neexistuje - vyrobime novy */
        //printf ( "create new: '%s'\n", filename );
        qdisk_create_image ( filename );
    };

    //printf ( "open MZQ: '%s'\n", filename );
    qdisk_open_image ( filename );
    ui_qdisk_set_path ( cfgelement_get_text_value ( g_elm_std_fp ) );
}


void qdisk_umount_image ( void ) {
    qdisk_close ( );
    cfgelement_set_text_value ( g_elm_std_fp, "" );
    ui_qdisk_menu_update ( );
    ui_qdisk_set_path ( "" );
}


void qdisk_set_write_protected ( int value ) {
    cfgelement_set_bool_value ( g_elm_wrprt, value );
    if ( g_qdisk.status & QDSTS_IMG_READY ) {
        qdisk_close ( );
        qdisk_open ( );
    };
}


/* Pri ukonceni je potreba uzavrit vsechny otevrene soubory */
void qdisk_exit ( void ) {
    qdisk_close ( );
}


void qdisk_init ( void ) {

    memset ( &g_qdisk, 0x00, sizeof ( st_QDISK ) );
    g_qdisk.channel[0].name = 'A';
    g_qdisk.channel[1].name = 'B';

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "QDISK" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "mz1f11_connected", CFGENTYPE_BOOL, QDISK_CONNECTED );
    cfgelement_set_handlers ( elm, (void*) &g_qdisk.connected, (void*) &g_qdisk.connected );

    elm = cfgmodule_register_new_element ( cmod, "mz1f11_type", CFGENTYPE_KEYWORD, QDISK_TYPE_IMAGE,
            QDISK_TYPE_IMAGE, "STANDARD",
            QDISK_TYPE_VIRTUAL, "VIRTUAL",
            -1 );
    cfgelement_set_handlers ( elm, (void*) &g_qdisk.type, (void*) &g_qdisk.type );

    g_elm_std_fp = cfgmodule_register_new_element ( cmod, "mz1f11_std_filepath", CFGENTYPE_TEXT, "empty.mzq" );
    g_elm_virt_fp = cfgmodule_register_new_element ( cmod, "mz1f11_virtual_filepath", CFGENTYPE_TEXT, "" );
    g_elm_wrprt = cfgmodule_register_new_element ( cmod, "mz1f11_write_protected", CFGENTYPE_BOOL, 0 );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );

    qdisk_open ( );

    ui_qdisk_menu_update ( );
}


Z80EX_BYTE qdisk_read_byte_from_image ( void ) {
    unsigned int readlen;
    Z80EX_BYTE retval;

    /* pokud neni pripojen img - jdeme pryc */
    if ( 0 == ( g_qdisk.status & QDSTS_IMG_READY ) ) {
        return 0xff;
    };

    /* pokud nebezi motor - jdeme pryc */
    if ( ( g_qdisk.channel[ QDSIO_CHANNEL_B ].Wreg[ QDSIO_REGADDR_5 ] & 0x80 ) == 0x00 ) {
        return 0xff;
    };

    FILE_FREAD ( g_qdisk.fp, &retval, 1, &readlen );

    if ( 1 != readlen ) {
        DBGPRINTF ( DBGERR, "fread() error\n" );
    };

    if ( QDISK_IMAGE_SIZE == g_qdisk.image_position++ ) {
        g_qdisk.image_position = 0;
    };

    return retval;
}


void qdisk_write_byte_into_image ( Z80EX_BYTE value ) {
    unsigned len;

    /* pokud neni pripojen img - jdeme pryc */
    if ( 0 == ( g_qdisk.status & QDSTS_IMG_READY ) ) {
        return;
    };

    /* pokud je img write protected - jdeme pryc */
    if ( g_qdisk.status & QDSTS_IMG_READONLY ) {
        return;
    };

    /* pokud nebezi motor - jdeme pryc */
    if ( ( g_qdisk.channel[ QDSIO_CHANNEL_B ].Wreg[ QDSIO_REGADDR_5 ] & 0x80 ) == 0x00 ) {
        return;
    };

    /* pokud neni nastaven output mode - jdeme pryc */
    if ( ( g_qdisk.channel[ QDSIO_CHANNEL_A ].Wreg[ QDSIO_REGADDR_5 ] & 0x08 ) == 0x00 ) {
        return;
    };

    if ( FR_OK != FILE_FWRITE ( g_qdisk.fp, &value, 1, &len ) ) {
        DBGPRINTF ( DBGERR, "fwrite() error\n" );
    }

    if ( QDISK_IMAGE_SIZE == g_qdisk.image_position++ ) {
        g_qdisk.image_position = 0;
    };
}


Z80EX_BYTE qdisk_read_byte ( en_QDSIO_ADDR SIO_addr ) {


    Z80EX_BYTE retval = 0x00;

    st_QDSIO_CHANNEL *channel = &g_qdisk.channel[ SIO_addr & 0x01 ];

    switch ( SIO_addr ) {

        case QDSIO_ADDR_CTRL_A:

            /* hunt phase */
            if ( ( channel->Wreg [ QDSIO_REGADDR_3 ] & 0x11 ) == 0x11 ) {

                channel->Rreg [ QDSIO_REGADDR_0 ] |= 0x10;
                g_qdisk.status &= ~QDSTS_IMG_SYNC;

                Z80EX_BYTE sync1, sync2;

                sync1 = qdisk_read_byte_from_image ( );

                int i;
                for ( i = 0; i < 8; i++ ) {

                    sync2 = qdisk_read_byte_from_image ( );

                    if ( ( sync1 == channel->Wreg [ QDSIO_REGADDR_6 ] ) && ( sync2 == channel->Wreg [ QDSIO_REGADDR_7 ] ) ) {
                        channel->Rreg [ QDSIO_REGADDR_0 ] &= 0xef; // inverze huntphase bitu a konec
                        g_qdisk.status |= QDSTS_IMG_SYNC;
                        break;
                    }

                    sync1 = sync2;
                };
            };

            channel->Rreg [ QDSIO_REGADDR_0 ] |= 0x01; /* v prijimacim bufferu je alespon jeden bajt */
            channel->Rreg [ QDSIO_REGADDR_0 ] |= 0x04; /* output buffer je prazdny */

            if ( g_qdisk.status & QDSTS_IMG_READY ) {
                channel->Rreg [ QDSIO_REGADDR_0 ] |= 0x08; /* DCD 1: disk je pritomen */
            } else {
                channel->Rreg [ QDSIO_REGADDR_0 ] &= ~0x08;
            };

            if ( g_qdisk.status & QDSTS_IMG_READONLY ) {
                channel->Rreg [ QDSIO_REGADDR_0 ] &= ~0x20; /* CTS 0: disk chranen proti zapisu */
            } else {
                channel->Rreg [ QDSIO_REGADDR_0 ] |= 0x20;
            };

            retval = channel->Rreg [ channel->REG_addr & 0x03 ];

            //printf ( "%s(): channel: '%c', port: 0x%02x, retval: 0x%02x, (PC = 0x%04x)\n", __func__, channel->name, SIO_addr + 0xf4, retval, z80ex_get_reg ( g_mz800.cpu, regPC ) );

            break;

        case QDSIO_ADDR_CTRL_B:

            if ( g_qdisk.status & QDSTS_HEAD_HOME ) {
                channel->Rreg [ QDSIO_REGADDR_0 ] = 0x08;
            } else {
                channel->Rreg [ QDSIO_REGADDR_0 ] = 0x00;
            };

            if ( ( g_qdisk.channel[ QDSIO_CHANNEL_A ].Wreg[ QDSIO_REGADDR_5 ] & 0x1a ) == 0x0a ) {
                if ( g_qdisk.out_crc16 != 0 ) {
                    qdisk_write_byte_into_image ( 'C' );
                    qdisk_write_byte_into_image ( 'R' );
                    qdisk_write_byte_into_image ( 'C' );
                };
            };

            if ( QDSIO_REGADDR_0 == channel->REG_addr ) {
                retval = 0xff;
            } else {
                retval = channel->Rreg [ channel->REG_addr & 0x03 ];
            };

            break;

        case QDSIO_ADDR_DATA_A:

            g_qdisk.status &= ~QDSTS_HEAD_HOME;

            if ( g_qdisk.status & QDSTS_IMG_READY ) {
                retval = qdisk_read_byte_from_image ( );
            };
            break;

        case QDSIO_ADDR_DATA_B:
            retval = 0xff;
            break;
    };

    channel->REG_addr = QDSIO_REGADDR_0;

    return retval;
}


void qdisk_write_byte ( en_QDSIO_ADDR SIO_addr, Z80EX_BYTE value ) {

    st_QDSIO_CHANNEL *channel = &g_qdisk.channel[ SIO_addr & 0x01 ];


    /* zapis na CTRL [ A / B ] */
    if ( SIO_addr & 0x02 ) {

        channel->Wreg[ channel->REG_addr ] = value;

        if ( QDSIO_REGADDR_0 == channel->REG_addr ) {

            channel->REG_addr = value & 0x07;
            en_QDSIO_WR0CMD wr0cmd = ( value >> 3 ) & 0x07;

            /* reset vypoctu CRC odchozich dat */
            if ( ( value & 0xc0 ) == 0x80 ) {
                g_qdisk.out_crc16 = 0;
            };

            switch ( wr0cmd ) {

                case QDSIO_WR0CMD_RESET:
                    memset ( &channel->Wreg, 0x00, sizeof ( channel->Wreg ) );
                    break;

                case QDSIO_WR0CMD_NONE:
                case QDSIO_WR0CMD_RESET_INTF:
                case QDSIO_WR0CMD_SDLC_STOP:
                case QDSIO_WR0CMD_ENABLE_INT:
                case QDSIO_WR0CMD_RESET_OUTBUF_INT:
                case QDSIO_WR0CMD_RESET_ERRFL:
                case QDSIO_WR0CMD_RETI:
                    break;
            };

        } else {

            switch ( channel->REG_addr ) {


                case QDSIO_REGADDR_2:

                    /* nastaveni interrupt vectoru ( lze jen u kanalu B ) */
                    if ( channel->name == 'B' ) {
                        channel->Rreg [ channel->REG_addr ] = value;
                    }
                    break;

                    /* Rx CTRL*/
                case QDSIO_REGADDR_3:
                    if ( channel->Wreg [ QDSIO_REGADDR_3 ] & 0x10 ) {
                        /* vstup do rezimu Hunt */
                        channel->Rreg [ QDSIO_REGADDR_0 ] |= 0x10;
                    };
                    break;

                    /* Tx CTRL */
                case QDSIO_REGADDR_5:

                    if ( channel->name == 'B' ) {

                        if ( ( channel->Wreg[ QDSIO_REGADDR_5 ] & 0x80 ) == 0x00 ) {
                            /* QD motor nenaktivni */

                            if ( g_qdisk.status & QDSTS_IMG_READY ) {
                                if ( FR_OK != FILE_FSEEK ( g_qdisk.fp, 0 ) ) {
                                    DBGPRINTF ( DBGERR, "fseek() error\n" );
                                };
                            };

                            g_qdisk.image_position = 0;
                            g_qdisk.status |= QDSTS_HEAD_HOME;
                        };

                    } else {

                        if ( ( channel->Wreg[ QDSIO_REGADDR_5 ] & 0x18 ) == 0x18 ) {
                            /* signal preruseni vysilani + povoleno odesilani dat */
                            qdisk_write_byte_into_image ( 0x00 );

                        } else if ( ( channel->Wreg[ QDSIO_REGADDR_5 ] & 0x1a ) == 0x0a ) {
                            /* signal preruseni vysilani + povoleno odesilani dat + signal RTS */
                            /* zapis synchronizacni znacky */
                            qdisk_write_byte_into_image ( channel->Wreg[ QDSIO_REGADDR_6 ] );
                            qdisk_write_byte_into_image ( channel->Wreg[ QDSIO_REGADDR_7 ] );
                        };

                    };
                    break;

                case QDSIO_REGADDR_0:
                case QDSIO_REGADDR_1:
                case QDSIO_REGADDR_4:
                case QDSIO_REGADDR_6:
                case QDSIO_REGADDR_7:
                    break;

            };

            channel->REG_addr = QDSIO_REGADDR_0;
        };

    } else {
        if ( channel->name == 'A' ) {
            g_qdisk.out_crc16 ^= value;
            qdisk_write_byte_into_image ( value );
        };
    };

}

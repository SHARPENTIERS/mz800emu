/* 
 * File:   cmt.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. srpna 2015, 12:07
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
 *
 *	Popis standardniho CMT zaznamu 1200 baudu:
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
 *
 *  Format zaznamu:
 *
 *	22 000 short
 *
 *	40 long		TAPE MARK
 *	40 short
 *
 *	1 long
 *
 *	INFORMATION BLOCK1 ( 128 bajtu )
 *
 *	CHECK SUM ( 2 bajty )
 *
 *	1 long
 *
 *	SHORT ( 256 bajtu )
 *
 *	INFORMATION BLOCK2 ( 128 bajtu )
 *
 *	CHECK SUM ( 2 bajty )
 *
 *	1 long
 *
 *	11 000 short
 *
 *	20 long		TAPE MARK
 *	20 short
 *
 *	1 long
 *
 *	DATA BLOCK1
 *
 *	CHECK SUM ( 2 bajty )
 *
 *	1 long
 *
 *	SHORT ( 256 bajtu )
 *
 *	DATA BLOCK1
 *
 *	CHECK SUM ( 2 bajty )
 *
 *	1 long
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include "cmt.h"
#include "sharpmz_ascii.h"
#include "gdg/gdg.h"

#include "ui/ui_main.h"
#include "ui/ui_cmt.h"

#include "cfgmain.h"

#include "ui/ui_utils.h"


mz800_cmt_t g_cmt;


/* krokujeme podle 1M1, coz je gdg / 16 */
#define CMT_TIMEBASE_DIVIDER		GDGCLK_1M1_DIVIDER

/* Pouzivame hodnoty namerene z ROM. V poznamce jsou hodnoty, ktere odpovidaji informacim ze SM */
#define CMT_1200BPS_LOG1_HIGH	( 8320 / CMT_TIMEBASE_DIVIDER )	/* 8335 */
#define CMT_1200BPS_LOG1_LOW	( 8640 / CMT_TIMEBASE_DIVIDER )	/* 8760 */
#define CMT_1200BPS_LOG0_HIGH	( 4220 / CMT_TIMEBASE_DIVIDER )	/* 4356 */
#define CMT_1200BPS_LOG0_LOW	( 4590 / CMT_TIMEBASE_DIVIDER )	/* 4930 */


#define CMT_UI_PLAYER_REFRESH_PER_SEC			2
#define CMT_UI_PLAYER_REFRESH_TIMER_PRESET	( GDGCLK_BASE / CMT_TIMEBASE_DIVIDER / CMT_UI_PLAYER_REFRESH_PER_SEC )


void cmt_exit ( void ) {
    /* pokud jsme meli otevreny MZF soubor, tak ho zavrem */
    if ( g_cmt.fh ) {
        fclose ( g_cmt.fh );
        g_cmt.fh = NULL;
    };
}


void cmt_stop ( void ) {

    g_cmt.bit_counter = 0;
    g_cmt.tick_counter = 0;
    g_cmt.output_signal = 0;
    g_cmt.play_iface_refresh_timer = CMT_UI_PLAYER_REFRESH_TIMER_PRESET;
    g_cmt.file_bits_elapsed = 0;
    g_cmt.play_time = 0;
    g_cmt.refresh_counter = CMT_UI_PLAYER_REFRESH_PER_SEC;
    g_cmt.state = CMT_PLAY_NONE;
    if ( g_cmt.fh != NULL ) {
        fseek ( g_cmt.fh, 0, SEEK_SET );
    };
    ui_cmt_window_update ( );
}


void cmt_eject ( void ) {

    if ( g_cmt.fh != NULL ) {
        fclose ( g_cmt.fh );
        g_cmt.fh = NULL;
    };

    cmt_stop ( );

    g_cmt.file_bits = 0;
    g_cmt.mzf_filetype = 0x00;
    g_cmt.mzf_filename[0] = 0x00;
    g_cmt.mzf_size = 0x0000;
    g_cmt.mzf_exec = 0x0000;
    g_cmt.mzf_start = 0x0000;
    ui_cmt_window_update ( );
}


void cmt_propagatecfg_cmt_speed ( void *e, void *data ) {
    g_cmt.speed = cfgelement_get_keyword_value ( (CFGELM *) e );
    ui_cmt_window_update ( );
}


void cmt_init ( void ) {

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "CMT" );

    CFGELM *elm;
    elm = cfgmodule_register_new_element ( cmod, "cmt_speed", CFGENTYPE_KEYWORD, CMT_SPEED_1200,
            CMT_SPEED_1200, "NORMAL",
            CMT_SPEED_2400, "DOUBLE",
            CMT_SPEED_3600, "TRIPLE",
            -1 );
    cfgelement_set_propagate_cb ( elm, cmt_propagatecfg_cmt_speed, NULL );
    cfgelement_set_handlers ( elm, NULL, (void*) &g_cmt.speed );

    cfgmodule_parse ( cmod );
    cfgmodule_propagate ( cmod );

    g_cmt.fh = NULL;
    g_cmt.output_signal = 0;
    cmt_eject ( );
    ui_cmt_init ( );
    ui_cmt_window_update ( );
}


int cmt_open_mzf_file ( char *filename ) {

    unsigned char c[2];
    int i, found;

    /* TODO: zatim podporujeme jen cteni */
    if ( !( g_cmt.fh = ui_utils_fopen ( filename, "rb" ) ) ) {
        ui_show_error ( "Can't open MZF file '%s': %s\n", filename, strerror ( errno ) );
        return RET_CMT_ERROR;
    };

    fseek ( g_cmt.fh, 0, SEEK_SET );

    if ( 1 != fread ( &g_cmt.mzf_filetype, 1, 1, g_cmt.fh ) ) {
        ui_show_error ( "Can't read MZ filetype from MZF file '%s': %s\n", filename, strerror ( errno ) );
        return RET_CMT_ERROR;
    };

    if ( sizeof ( g_cmt.mzf_filename ) != fread ( &g_cmt.mzf_filename, 1, sizeof ( g_cmt.mzf_filename ), g_cmt.fh ) ) {
        ui_show_error ( "Can't read MZ filename from MZF file '%s': %s\n", filename, strerror ( errno ) );
        return RET_CMT_ERROR;
    };

    found = 0;
    for ( i = 0; i < sizeof ( g_cmt.mzf_filename ); i++ ) {
        if ( g_cmt.mzf_filename [ i ] == 0x0d ) {
            g_cmt.mzf_filename [ i ] = 0x00;
            found = 1;
            break;
        };
    };

    if ( !found ) {
        ui_show_error ( "Bad MZ filename in MZF file '%s': %s\nTerminator 0x0d not found!\n", filename );
        return RET_CMT_ERROR;
    };

    i = 0;
    while ( g_cmt.mzf_filename [ i ] != 0x00 ) {
        g_cmt.mzf_filename [ i ] = sharpmz_cnv_from ( g_cmt.mzf_filename [ i ] );
        i++;
    };

    if ( sizeof ( c ) != fread ( &c, 1, sizeof ( c ), g_cmt.fh ) ) {
        ui_show_error ( "Can't read MZ size from MZF file '%s': %s\n", filename, strerror ( errno ) );
        return RET_CMT_ERROR;
    };
    g_cmt.mzf_size = ( c [ 1 ] << 8 ) | c [ 0 ];

    if ( sizeof ( c ) != fread ( &c, 1, sizeof ( c ), g_cmt.fh ) ) {
        ui_show_error ( "Can't read MZ exec from MZF file '%s': %s\n", filename, strerror ( errno ) );
        return RET_CMT_ERROR;
    };
    g_cmt.mzf_exec = ( c [ 1 ] << 8 ) | c [ 0 ];


    if ( sizeof ( c ) != fread ( &c, 1, sizeof ( c ), g_cmt.fh ) ) {
        ui_show_error ( "Can't read MZ start addr from MZF file '%s': %s\n", filename, strerror ( errno ) );
        return RET_CMT_ERROR;
    };
    g_cmt.mzf_start = ( c [ 1 ] << 8 ) | c [ 0 ];

    if ( 0 != fseek ( g_cmt.fh, 0, SEEK_END ) ) {
        ui_show_error ( "SEEK_END error on MZF file '%s': %s\n", filename, strerror ( errno ) );
        return RET_CMT_ERROR;
    };

    if ( !( ftell ( g_cmt.fh ) >= ( g_cmt.mzf_size + DEF_CMT_HEADER_SIZE ) ) ) {
        ui_show_error ( "MZF file '%s' has bad length.\nMZ size is %d, but real size is %d\n", filename, g_cmt.mzf_size, ( ftell ( g_cmt.fh ) - DEF_CMT_HEADER_SIZE ) );
        return RET_CMT_ERROR;
    };

    if ( 0 != fseek ( g_cmt.fh, 0, SEEK_SET ) ) {
        ui_show_error ( "SEEK_SET 0 error on MZF file '%s': %s\n", filename, strerror ( errno ) );
        return RET_CMT_ERROR;
    };

    g_cmt.file_bits = DEF_CMT_HEADER_PILOT_LENGTH + DEF_CMT_BODY_PILOT_LENGTH + DEF_CMT_HEADER_LENGTH + DEF_CMT_HEADER_TMARK_LENGTH + DEF_CMT_BODY_TMARK_LENGTH + ( 2 * DEF_CMT_CHKSUM_LENGTH ) + ( 2 * DEF_CMT_BLOCK_PREFIX_LENGTH ) + ( 2 * DEF_CMT_BLOCK_SURFIX_LENGTH );
    g_cmt.file_bits += g_cmt.mzf_size * DEF_CMT_BYTE_LENGTH;

    cmt_stop ( );

    strncpy ( g_cmt.filename, filename, sizeof ( g_cmt.filename ) );
    g_cmt.filename [ sizeof ( g_cmt.filename ) - 1 ] = 0x00;
    return RET_CMT_OK;
}


int cmt_open ( void ) {

    char window_title[] = "Select MZF file to open";
    char filename [ CMT_FILENAME_LENGTH ];
    filename[0] = 0x00;

    /* TODO: zatim podporujeme jen mzf */
    if ( UIRET_OK == ui_open_file ( filename, g_cmt.filename, sizeof ( filename ), FILETYPE_MZF, window_title, OPENMODE_READ ) ) {

        cmt_eject ( );

        if ( RET_CMT_OK == cmt_open_mzf_file ( filename ) ) {
            ui_cmt_set_filename ( basename ( g_cmt.filename ) );
            ui_cmt_window_update ( );
            return RET_CMT_OK;
        };
    };
    return RET_CMT_ERROR;
}


void cmt_play ( void ) {
    cmt_stop ( );
    g_cmt.state = CMT_PLAY_START;
    ui_cmt_window_update ( );
}


void cmt_initialise_state ( void ) {


    switch ( g_cmt.state ) {

        case CMT_PLAY_HEADER_PILOT:
            g_cmt.bit_counter = DEF_CMT_HEADER_PILOT_LENGTH;
            break;

        case CMT_PLAY_BODY_PILOT:
            g_cmt.bit_counter = DEF_CMT_BODY_PILOT_LENGTH;
            break;

        case CMT_PLAY_HEADER_TMARK_L:
        case CMT_PLAY_HEADER_TMARK_S:
            g_cmt.bit_counter = DEF_CMT_HEADER_TMARK_LENGTH / 2;
            break;

        case CMT_PLAY_BODY_TMARK_L:
        case CMT_PLAY_BODY_TMARK_S:
            g_cmt.bit_counter = DEF_CMT_BODY_TMARK_LENGTH / 2;
            break;

        case CMT_PLAY_HEADER_PREFIX:
        case CMT_PLAY_HEADER_SURFIX:
        case CMT_PLAY_BODY_PREFIX:
        case CMT_PLAY_BODY_SURFIX:
            /* SM800 lze! ve skutecnosti jsou jako prefix pouzity 2 bity! */
            g_cmt.bit_counter = DEF_CMT_BLOCK_PREFIX_LENGTH;
            break;


        case CMT_PLAY_HEADER_CHKSUM:
        case CMT_PLAY_BODY_CHKSUM:
            g_cmt.bit_counter = DEF_CMT_CHKSUM_LENGTH;
#if DEF_CMT_DEBUG
            printf ( "Checksum DONE: 0x%04x\n", g_cmt.checksum );
#endif
            g_cmt.data_bit_position = 7;
            break;


        case CMT_PLAY_HEADER:
            g_cmt.bit_counter = DEF_CMT_HEADER_LENGTH;
            g_cmt.data_bit_position = 7;
            g_cmt.checksum = 0;
            break;

        case CMT_PLAY_BODY:
            g_cmt.bit_counter = g_cmt.mzf_size * DEF_CMT_BYTE_LENGTH;
            fseek ( g_cmt.fh, DEF_CMT_HEADER_SIZE, SEEK_SET );
            g_cmt.data_bit_position = 7;
            g_cmt.checksum = 0;
            break;

        default:
            break;
    };
}


void cmt_player_send_log0 ( void ) {
    g_cmt.output_signal = 1;
    g_cmt.tick_counter = CMT_1200BPS_LOG0_HIGH / ( g_cmt.speed + 1 );
    g_cmt.tick_counter_low = CMT_1200BPS_LOG0_LOW / ( g_cmt.speed + 1 );
}


void cmt_player_send_log1 ( void ) {
    g_cmt.output_signal = 1;
    g_cmt.tick_counter = CMT_1200BPS_LOG1_HIGH / ( g_cmt.speed + 1 );
    g_cmt.tick_counter_low = CMT_1200BPS_LOG1_LOW / ( g_cmt.speed + 1 );
}


void cmt_step ( void ) {

    /*
    if ( g_cmt.fh == NULL ) {
        return;
    };

    if ( CMT_PLAY_NONE == g_cmt.state ) {
        return;
    };
     */

    /* uz skoncil pultakt? */
    if ( 0 == g_cmt.tick_counter ) {

        if ( 0 == g_cmt.output_signal ) {
            /* pokud zrovna nic neodesilame, tak muzeme postoupit dal */

            /* tohle by se melo delat az po dokonceni log0 pultaktu */
            g_cmt.file_bits_elapsed++;


            if ( 0 == g_cmt.bit_counter ) {

                /* nemame zadne bity k odeslani, takze prejdeme do dalsi faze */
                g_cmt.state++;
#if DEF_CMT_DEBUG
                /*#if 1*/
                printf ( "Virtual CMT phase: %d, (%ds)\n", g_cmt.state, g_cmt.play_time );
#endif
                if ( CMT_PLAY_DONE == g_cmt.state ) {
                    cmt_stop ( );
                    return;
                };
                cmt_initialise_state ( );
            };



            switch ( g_cmt.state ) {

                case CMT_PLAY_HEADER_PILOT:
                case CMT_PLAY_BODY_PILOT:
                case CMT_PLAY_HEADER_TMARK_S:
                case CMT_PLAY_BODY_TMARK_S:
                    cmt_player_send_log0 ( );
                    break;

                case CMT_PLAY_HEADER_TMARK_L:
                case CMT_PLAY_BODY_TMARK_L:
                case CMT_PLAY_HEADER_PREFIX:
                case CMT_PLAY_HEADER_SURFIX:
                case CMT_PLAY_BODY_PREFIX:
                case CMT_PLAY_BODY_SURFIX:
                    cmt_player_send_log1 ( );
                    break;


                case CMT_PLAY_HEADER:
                case CMT_PLAY_BODY:
                    if ( -1 == g_cmt.data_bit_position ) {

                        /* odesilame stop bit */
                        cmt_player_send_log1 ( );
                        g_cmt.data_bit_position = 7;
#if DEF_CMT_DEBUG
                        printf ( "HB STOP bit\n" );
#endif

                    } else {
                        if ( g_cmt.data_bit_position == 7 ) {
                            if ( g_cmt.bit_counter > 1 ) {
                                /* nacist dalsi bajt */
                                if ( 1 != fread ( &g_cmt.data, 1, 1, g_cmt.fh ) ) {
                                    ui_show_error ( "Can't read MZF file '%s': %s\n", g_cmt.filename, strerror ( errno ) );
                                    cmt_stop ( );
                                    return;
                                };

#if DEF_CMT_DEBUG
                                printf ( "Virtual CMT data: 0x%02x, %d\n", g_cmt.data, g_cmt.bit_counter );
#endif
                            };
                        };


                        if ( ( g_cmt.data >> g_cmt.data_bit_position ) & 0x01 ) {
                            cmt_player_send_log1 ( );
                            g_cmt.checksum++;
#if DEF_CMT_DEBUG
                            printf ( "HB Bit (%d): 1\n", g_cmt.data_bit_position );
#endif
                        } else {
                            cmt_player_send_log0 ( );
#if DEF_CMT_DEBUG
                            printf ( "HB Bit (%d): 0\n", g_cmt.data_bit_position );
#endif
                        };
                        g_cmt.data_bit_position--;

                    };
                    break;


                case CMT_PLAY_HEADER_CHKSUM:
                case CMT_PLAY_BODY_CHKSUM:
                    if ( -1 == g_cmt.data_bit_position ) {

                        /* odesilame stop bit */
                        cmt_player_send_log1 ( );
                        g_cmt.data_bit_position = 7;
#if DEF_CMT_DEBUG
                        printf ( "HB STOP bit\n" );
#endif

                    } else {
                        if ( g_cmt.bit_counter == 2 * 9 ) {
                            g_cmt.data = ( g_cmt.checksum >> 8 ) & 0xff;
#if DEF_CMT_DEBUG
                            printf ( "Virtual CMT data: 0x%02x, %d\n", g_cmt.data, g_cmt.bit_counter );
#endif
                        } else if ( g_cmt.bit_counter == 9 ) {
                            g_cmt.data = g_cmt.checksum & 0xff;
#if DEF_CMT_DEBUG
                            printf ( "Virtual CMT data: 0x%02x, %d\n", g_cmt.data, g_cmt.bit_counter );
#endif
                        };

                        if ( ( g_cmt.data >> g_cmt.data_bit_position ) & 0x01 ) {
                            cmt_player_send_log1 ( );
#if DEF_CMT_DEBUG
                            printf ( "HB Bit (%d): 1\n", g_cmt.data_bit_position );
#endif
                        } else {
                            cmt_player_send_log0 ( );
#if DEF_CMT_DEBUG
                            printf ( "HB Bit (%d): 0\n", g_cmt.data_bit_position );
#endif
                        };
                        g_cmt.data_bit_position--;

                    };
                    break;

                default:
                    break;
            };

        } else {
            /* Mame odeslanu jen prvni polovinu pulzu, takze ted tam posleme i tu druhou */
            g_cmt.output_signal = 0;
            g_cmt.tick_counter = g_cmt.tick_counter_low;
            g_cmt.bit_counter--;
        };

    } else {
        /* pokracujeme v pultaktu */
        g_cmt.tick_counter--;
    };

    g_cmt.play_iface_refresh_timer--;
    if ( 0 == g_cmt.play_iface_refresh_timer ) {
        ui_cmt_update_player ( );
        g_cmt.play_iface_refresh_timer = CMT_UI_PLAYER_REFRESH_TIMER_PRESET;
        g_cmt.refresh_counter--;
        if ( 0 == g_cmt.refresh_counter ) {
            g_cmt.refresh_counter = CMT_UI_PLAYER_REFRESH_PER_SEC;
            g_cmt.play_time++;
        };
    };

}

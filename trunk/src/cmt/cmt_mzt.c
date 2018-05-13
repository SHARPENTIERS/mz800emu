/* 
 * File:   cmt_mzt.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 11. května 2018, 7:25
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
#include <stdint.h>
#include <assert.h>

#include "libs/generic_driver/generic_driver.h"
#include "ui/generic_driver/ui_memory_driver.h"
#include "libs/mzf/mzf.h"
#include "libs/mzf/mzf_tools.h"
#include "libs/mztape/mztape.h"

#include "ui/ui_main.h"
#include "ui/ui_utils.h"

#include "cmt_extension.h"
#include "cmt_mzt.h"

static st_DRIVER *g_driver = &g_ui_memory_driver_static;

st_CMTEXT *g_cmtmzt_extension = NULL;


static void cmtmzt_extension_eject ( void ) {
    assert ( g_cmtmzt_extension != NULL );
    cmtext_eject ( g_cmtmzt_extension );
}


static int cmtmzt_extension_open ( char * filename ) {

    assert ( g_cmtmzf_extension != NULL );

    cmtmzt_extension_eject ( );

    printf ( "CMTMZT Open: %s\n", filename );

    st_HANDLER *h = generic_driver_open_memory_from_file ( NULL, g_driver, filename );

    if ( !h ) {
        ui_show_error ( "CMTMZT: can't open MZT file '%s'\n", filename );
        return EXIT_FAILURE;
    };

    st_CMT_MZT_CONSPEC *mzt_spec = (st_CMT_MZT_CONSPEC*) ui_utils_mem_alloc0 ( sizeof ( st_CMT_MZT_CONSPEC ) );
    if ( !mzt_spec ) {
        fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, (int) sizeof ( st_CMT_MZT_CONSPEC ) );
        return EXIT_FAILURE;
    };

    uint32_t offset = 0;

    while ( offset < h->spec.memspec.size ) {

        st_MZF_HEADER *hdr = (st_MZF_HEADER*) ui_utils_mem_alloc0 ( sizeof ( st_MZF_HEADER ) );
        if ( !hdr ) {
            fprintf ( stderr, "%s() - %d: Can't alocate memory (%u)\n", __func__, __LINE__, (int) sizeof ( st_MZF_HEADER ) );
            return EXIT_FAILURE;
        };

        if ( EXIT_SUCCESS != mzf_read_header_on_offset ( h, offset, hdr ) ) {
            fprintf ( stderr, "%s() - %d: Can't read MZF header\n", __func__, __LINE__ );
            ui_utils_mem_free ( hdr );
            return EXIT_FAILURE;
        };

        char ascii_filename[MZF_FNAME_FULL_LENGTH];
        mzf_tools_get_fname ( hdr, ascii_filename );
        printf ( "file: %d\n", mzt_spec->count_files );
        printf ( "fname: %s\n", ascii_filename );
        printf ( "ftype: 0x%02x\n", hdr->ftype );
        printf ( "fstrt: 0x%04x\n", hdr->fstrt );
        printf ( "fsize: 0x%04x\n", hdr->fsize );
        printf ( "fexec: 0x%04x\n", hdr->fexec );

        st_MZTAPE_MZF *mztmzf = mztape_create_mztmzf ( h, offset );
        if ( !mztmzf ) {
            fprintf ( stderr, "%s() - %d: Can't create mztmzf\n", __func__, __LINE__ );
            ui_utils_mem_free ( hdr );
            return EXIT_FAILURE;
        };

        offset += hdr->fsize + sizeof ( st_MZF_HEADER );
        mzt_spec->count_files++;

        ui_utils_mem_free ( hdr );
        mztape_mztmzf_destroy ( mztmzf );
    };


    generic_driver_close ( h );

    cmtext_container_set_name ( g_cmtmzf_extension->container, ui_utils_basename ( filename ) );

    return EXIT_FAILURE;
}


void cmtmzt_exit ( void ) {
    cmtext_destroy ( g_cmtmzt_extension );
}


void cmtmzt_init ( void ) {
    g_cmtmzt_extension = cmtext_new ( cmtmzt_extension_open, cmtmzt_extension_eject, CMT_CONTAINER_TYPE_MZT );
}


/* 
 * File:   cfgmain.c
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 15. září 2015, 13:54
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "cfgmain.h"
#include "main.h"
#include "build_time.h"


struct st_CFGROOT *g_cfgmain;


#ifdef LINUX
#define CFGMAIN_PLATFORM    "Linux"
#endif

#ifdef WIN32
#define CFGMAIN_PLATFORM    "Windows"
#endif


#define CFGMAIN_TIMESTAMP_MAXLEN   20


char* cfgmain_create_timestamp ( void ) {
    static char timestamp [ CFGMAIN_TIMESTAMP_MAXLEN ];
    time_t t;
    struct tm *tmp;

    t = time ( NULL );
    tmp = localtime ( &t );

    if ( tmp == NULL ) {
        fprintf ( stderr, "%s():%d - localtime error: %s\n", __func__, __LINE__, strerror ( errno ) );
        main_app_quit ( EXIT_FAILURE );
    };

    if ( strftime ( timestamp, sizeof ( timestamp ), "%Y-%m-%d %H:%M:%S", tmp ) == 0 ) {
        fprintf ( stderr, "%s():%d - strftime error: %s\n", __func__, __LINE__, strerror ( errno ) );
        main_app_quit ( EXIT_FAILURE );
    };
    return timestamp;
}


void cfgmain_savecfg ( void *m, void *data ) {
    cfgelement_set_text_value ( cfgmodule_get_element_by_name ( (CFGMOD *) m, "This_is_Config_File_for" ), "MZ-800 Emulator" );
    cfgelement_set_text_value ( cfgmodule_get_element_by_name ( (CFGMOD *) m, "config_version" ), "1.0" );
    cfgelement_set_text_value ( cfgmodule_get_element_by_name ( (CFGMOD *) m, "emulator_version" ), CFGMAIN_EMULATOR_VERSION_TEXT );
    cfgelement_set_text_value ( cfgmodule_get_element_by_name ( (CFGMOD *) m, "emulator_build_time" ), build_time_get ( ) );
    cfgelement_set_text_value ( cfgmodule_get_element_by_name ( (CFGMOD *) m, "config_creation_timestamp" ), cfgmain_create_timestamp ( ) );
    cfgelement_set_text_value ( cfgmodule_get_element_by_name ( (CFGMOD *) m, "config_creation_platform" ), CFGMAIN_PLATFORM );
}


void cfgmain_init ( void ) {

    g_cfgmain = cfgroot_new ( CFGFILE_INI_FILENAME );

    CFGMOD *cmod = cfgroot_register_new_module ( g_cfgmain, "CFGMAIN" );

    cfgmodule_set_save_cb ( cmod, &cfgmain_savecfg, NULL );

    cfgmodule_register_new_element ( cmod, "This_is_Config_File_for", CFGENTYPE_TEXT, "" );
    cfgmodule_register_new_element ( cmod, "config_version", CFGENTYPE_TEXT, "" );
    cfgmodule_register_new_element ( cmod, "emulator_version", CFGENTYPE_TEXT, "" );
    cfgmodule_register_new_element ( cmod, "emulator_build_time", CFGENTYPE_TEXT, "" );
    cfgmodule_register_new_element ( cmod, "config_creation_timestamp", CFGENTYPE_TEXT, "" );
    cfgmodule_register_new_element ( cmod, "config_creation_platform", CFGENTYPE_TEXT, "" );

    cfgmodule_parse ( cmod );
}


void cfgmain_exit ( void ) {
    cfgroot_save ( g_cfgmain );
    cfgroot_destroy ( g_cfgmain );
}


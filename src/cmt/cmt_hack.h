/* 
 * File:   cmt_hack.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 2. července 2015, 20:50
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

#ifndef CMT_HACK_H
#define CMT_HACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "libs/generic_driver/generic_driver.h"
    
    typedef struct st_CMTHACK {
        st_HANDLER mzf_handler;
        char *last_filename;
        unsigned load_patch_installed;
    } st_CMTHACK;

    extern st_CMTHACK g_cmthack;

    extern void cmthack_reinstall_rom_patch ( void );
    extern void cmthack_load_rom_patch ( unsigned enabled );

    extern void cmthack_reset ( void );
    extern void cmthack_init ( void );
    extern void cmthack_exit ( void );

    extern void cmthack_load_file ( void );
    extern void cmthack_load_mzf_filename ( char *filename );
    extern void cmthack_read_mzf_body ( void );


#ifdef __cplusplus
}
#endif

#endif /* CMT_HACK_H */


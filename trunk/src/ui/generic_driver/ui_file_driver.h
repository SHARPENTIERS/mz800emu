/* 
 * File:   ui_file_driver.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 20. února 2017, 13:19
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


#ifndef UI_FILE_DRIVER_H
#define UI_FILE_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "libs/generic_driver/generic_driver.h"

    extern st_DRIVER g_ui_file_driver;


    typedef enum en_FILE_DRIVER_MODE {
        FILED_RIVER_MODE_RO = 0,
        FILED_RIVER_MODE_RW,
        FILED_RIVER_MODE_W,
    } en_FILE_DRIVER_MODE;

    extern void ui_file_driver_init ( void );
    extern int ui_file_driver_open ( void *handler, st_DRIVER *d, char *filename, en_FILE_DRIVER_MODE mode );
    extern int ui_file_driver_close ( void *handler, st_DRIVER *d );

#ifdef __cplusplus
}
#endif

#endif /* UI_FILE_DRIVER_H */


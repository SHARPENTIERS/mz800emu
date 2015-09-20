/* 
 * File:   ui_utils.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 20. září 2015, 22:06
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

#ifndef UI_UTILS_H
#define	UI_UTILS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
    
    extern FILE* ui_utils_fopen ( const char *filename_in_utf8, const char *mode );
    int ui_utils_access ( const char *filename_in_utf8, int type );

#ifdef	__cplusplus
}
#endif

#endif	/* UI_UTILS_H */


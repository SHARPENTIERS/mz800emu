/* 
 * File:   endianity.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. února 2017, 10:23
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


#ifndef ENDIANITY_H
#define ENDIANITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    extern uint16_t endianity_bswap16_BE ( uint16_t n );
    extern uint16_t endianity_bswap16_LE ( uint16_t n );
    extern uint32_t endianity_bswap32_BE ( uint32_t n );
    extern uint32_t endianity_bswap32_LE ( uint32_t n );
    extern uint64_t endianity_bswap64_BE ( uint64_t n );
    extern uint64_t endianity_bswap64_LE ( uint64_t n );

#ifdef __cplusplus
}
#endif

#endif /* ENDIANITY_H */


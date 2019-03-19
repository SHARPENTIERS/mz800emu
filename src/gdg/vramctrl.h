/* 
 * File:   vramctrl.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 18. června 2015, 19:12
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

#ifndef VRAMCTRL_H
#define VRAMCTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mz800emu_cfg.h"

#ifdef MACHINE_EMU_MZ800
#include "vramctrl_mz800.h"
#endif
#ifdef MACHINE_EMU_MZ1500
#include "vramctrl_mz1500.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* VRAMCTRL_H */


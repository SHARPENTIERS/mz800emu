/* 
 * File:   mz800emu_cfg.h
 * Author: Michal Hucik <hucik@ordoz.com>
 *
 * Created on 16. srpna 2016, 9:50
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

#ifndef MZ800EMU_CFG_H
#define MZ800EMU_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Konfiguracni vypnuti modulu MZ-800 debugger
     * ===========================================
     * 
     * Emulace se zjednodusi a mela by byt rychlejsi (nemeril jsem, zda je tomu skutecne tak).
     * 
     */
    
#define MZ800EMU_CFG_DEBUGGER_ENABLED
    
    
    /*
     * Konfigurace pro emulaci CLK 1.1 MHz (CTC8253-CTC0 a CMT )
     * =========================================================
     * 
     * !!! Pouzit jednu z voleb:
     * 
     * MZ800EMU_CFG_CLK1M1_SLOW - presna varianta pri ktere se vola CTC0_step a CMT_step 
     * pri kazdem 16 pixelclk
     * 
     * MZ800EMU_CFG_CLK1M1_FAST - prozatim nedokoncena varianta pri ktere se CLK 1M1 
     * step() zavola jen tehdy, pokud ma nastat konkretni event
     * 
     */

//#define MZ800EMU_CFG_CLK1M1_SLOW
#define MZ800EMU_CFG_CLK1M1_FAST


    
    /*
     * Experimentalni vypnuti audio vystupu a synchronizace snimku se zvukem
     * =====================================================================
     * 
     * Slouzi pouze k lepsimu ladeni maximalniho vykonu.
     * 
     */

//#define MZ800EMU_CFG_AUDIO_DISABLED

    
#ifdef __cplusplus
}
#endif

#endif /* MZ800EMU_CFG_H */


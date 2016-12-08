/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2016 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "global_config.h"
#include "trackpoint.h"
/**
 * Configuration data of keyboard:
 *  - Storage in eeprom
 *  - Modification via command mode
 *  - Hardware configuration of GPIOs
 */


/**
 * Setup initial config variables from defined values or EEPROM storage.
 *
 */
void init_config()
{
    print_config();

    // init default values before trying to load eeprom
    g_cfg = (kb_cfg_t) {
        .magic=EE_CFG_MAGIC_NUM, .fw_config=0,
        .tp_axis=0, .tp_config=0,
    };

    // check that eeprom version matches firmware
    // @TODO implement major/minor versioning to keep macros and other settings
    uint16_t magic = eeprom_read_word(EE_CFG_MAGIC);
    if(magic != EE_CFG_MAGIC_NUM) {
        printf("\nEE magic %04x != %04x", magic, EE_CFG_MAGIC_NUM);
        save_config(&g_cfg);
    } else {
        printf("\nEE magic OK");
        load_config(&g_cfg);
    }

    print_config();

    // led defaults
    g_led = (led_t) { .brightness=5, .on=0, .off=60 };
}

void print_config()
{
    printf("\nEE[%d] %04x: %02x", sizeof(g_cfg), g_cfg.magic, g_cfg.fw_config);
    printf("\nTP:Axies=%02x Sens: %3d/%3d SP=%3d TH=%3d",g_cfg.tp_axis, g_cfg.sensL, g_cfg.sens, g_cfg.speed, g_cfg.thres);
    printf("\nTP:PTS=%1x X=%1x Y=%1x S=%1x (%0x) ",g_cfg.pts, g_cfg.flipx, g_cfg.flipy, g_cfg.swapxy, g_cfg.tp_axis);
}

/**
 * Update complete config in one go
 */
void save_config(kb_cfg_t *cfg)
{
    eeprom_busy_wait();
    eeprom_update_block (( const void *) cfg, (void *) (EE_CFG_MAGIC), sizeof(kb_cfg_t));
}

/**
 * Read complete config from eeprom
 */
void load_config(kb_cfg_t *cfg)
{
    eeprom_busy_wait();
    eeprom_read_block (( void *) cfg, (const void *) (EE_CFG_MAGIC), sizeof(kb_cfg_t));
}


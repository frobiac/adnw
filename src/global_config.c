/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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
    ct_assert(sizeof(kb_cfg_t) < EE_ADDR_MACROS);
    print_config();

    // init default values before trying to load eeprom
    g_cfg = (kb_cfg_t) {
        .magic=EE_CFG_MAGIC_NUM, .fw.raw=0,
        .tp_axis.raw=0, .tp_config.raw=0,
        .led = (led_t) { .brightness=5, .on=0, .off=60 }
    };

#ifdef PS2MOUSE
    // load defined defaults
    g_cfg.tp_axis.raw = TP_AXES;

    // trackpoint defaults from firmware
    // Usable on RT: 28/128 207 13 (decimal)
    g_cfg.tp_config.sens  = TP_DEF_SENS;   // 0x80
    g_cfg.tp_config.sensL = 28;            // custom
    g_cfg.tp_config.speed = TP_DEF_SPEED;  // 0x61
    g_cfg.tp_config.thres = TP_DEF_THRESH; // 0x08
#endif

    print_config();

    // check that eeprom version matches firmware
    // @TODO implement major/minor versioning to keep macros and other settings
    uint16_t magic = eeprom_read_word(EE_CFG_MAGIC);
    if(magic != EE_CFG_MAGIC_NUM) {
        xprintf("\nEE magic %04X != %04X", magic, EE_CFG_MAGIC_NUM);
        save_config(&g_cfg);
    } else {
        xprintf("\nEE magic OK");
        load_config(&g_cfg);
    }

    print_config();
}

void print_config()
{
    xprintf("\nEE[%d] %04X: %02X ", sizeof(g_cfg), g_cfg.magic, g_cfg.fw.raw);
    xprintf("Mouse=%d-%d", g_cfg.fw.mouse_enabled, g_cfg.fw.swap_xy);
#ifdef HAS_LED
    xprintf(" LED:%02X %02X %02X ", g_cfg.led.brightness, g_cfg.led.on, g_cfg.led.off);
#endif
#ifdef PINKYDROP
    xprintf(" PD=%d", g_cfg.fw.pinkydrop);
#endif
#ifdef ALTERNATE_LAYER
    xprintf(" AL=%d", g_cfg.fw.alt_layer);
#endif
#ifdef PS2MOUSE
    xprintf("\nTP: Sens: %3d/%3d SP=%3d TH=%3d ", g_cfg.tp_config.sensL, g_cfg.tp_config.sens, g_cfg.tp_config.speed, g_cfg.tp_config.thres);
    xprintf("\nPTS=%1X X=%1X Y=%1X (%0X)",g_cfg.tp_axis.pts, g_cfg.tp_axis.flipx, g_cfg.tp_axis.flipy, g_cfg.tp_axis.raw);
#endif
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


void invalidate_config()
{
    eeprom_update_word( EE_CFG_MAGIC, 0xFFFF);
}

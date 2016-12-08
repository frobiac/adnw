/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2013-2016 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include "macro.h"
/**
 * @file global_config.h
 *
 * Global variables / configuration options for persistent storage in eeprom.
 */

/// define DEBUG_OUTPUT in makefile to enable printf() for hid_listen.
#ifdef DEBUG_OUTPUT
    #include "hhstdio.h"
    //#define TRACE printf
    #define TRACE(...)
#else
    #define TRACE(...)
    #define printf(...)
#endif


/**
 * Convenience macros for individual member access of EEPROM config.
 * @see https://projectgus.com/2010/07/eeprom-access-with-arduino/
 */
#define ee_cfg_read_to(dst_p, eeprom_field, dst_size) { \
        eeprom_busy_wait(); \
        eeprom_read_block(dst_p, (void *)offsetof(kb_cfg_t, eeprom_field), MIN(dst_size, sizeof((kb_cfg_t*)0)->eeprom_field)); }

#define ee_cfg_read(dst, eeprom_field) ee_cfg_read_to(&dst, eeprom_field, sizeof(dst))

#define ee_cfg_update_from(src_p, eeprom_field, src_size) { \
        eeprom_busy_wait(); \
        eeprom_update_block(src_p, (void *)offsetof(kb_cfg_t, eeprom_field), MIN(src_size, sizeof((kb_cfg_t*)0)->eeprom_field)); }

#define ee_cfg_update(src, eeprom_field) { typeof(src) x = src; eeprom_update_from(&x, eeprom_field, sizeof(x)); }


///@TODO major/minor increments to also purge macros or only update?
/**
 * increment to force eeprom re-initialisation upon init
 * this is only required if the address layout changes.
 * simple additions will be fine as long as they are saved somehow and random values are ok initially.
 */
#define EE_CFG_MAGIC_NUM    (uint16_t) 0x0002

#define EE_CFG_MAGIC        (uint16_t *) 0
#define EE_CFG_END          (uint8_t  *) (sizeof(kb_cfg_t)) // end of config

//@TODO Check for overlap with this hardcoded address, or use from end?
#define EE_ADDR_START       100
#define EE_ADDR_MACROS      (EE_ADDR_START+100)
#define EE_ADDR_MACRO(idx)  (EE_ADDR_MACROS + (idx*(MACRO_MAX_LEN+1)))


/**
 * MODIFIABLE PERSISTENT CONFIG OPTIONS BELOW
 */
typedef struct {
    uint8_t brightness;
    // in 100ms, roughly based on idle_count updates via ISR
    uint8_t on;
    uint8_t off;
} led_t;

typedef struct {
    /// eeprom magic to detect changes in memory layout
    uint16_t magic;

    /// persistent firmware specific flags.
    /// @TODO check whether compile time defines are needed anymore.
    union {
        struct {
            bool pinkydrop:1;
            bool alt_layer:1;
            bool mouse_enabled:1;
            uint8_t fw_config_unused:5;
        };
        uint8_t fw_config;
    };

    /// TrackPoint firmware axis configuration, not needed during runtime
    union {
        struct {
            bool pts:1;
            bool tp_axies_unused:1;
            bool btn2:1;
            bool flipx:1;
            bool flipy:1;
            bool flipz:1;
            bool swapxy:1; ///< This is used during normale operation as TrackPoint firmware flag does not work
            bool ftrans:1;
        };
        uint8_t tp_axis;
    };

    /// TrackPoint firmware usability configuration, not needed during runtime
    union {
        struct {
            uint8_t speed;
            uint8_t sens;
            uint8_t sensL;
            uint8_t thres;

        };
        uint32_t tp_config;
    };

    ///< led config, used regularly to update LED
    led_t led;

} kb_cfg_t;

// *THE* global config
// @TODO currently kept completely in RAM although only really needed during re-configuration
kb_cfg_t g_cfg;


void init_config(void);
void save_config(kb_cfg_t *cfg);
void load_config(kb_cfg_t *cfg);

void print_config(void);

#endif // GLOBAL_CONFIG_H


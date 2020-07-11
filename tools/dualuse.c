/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2018-2020 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/* DOCUMENTATION

Several different keys
----------------------

- [n]ormal key
- [N]ormal key with implicit modifier, like '%' or '@'
- [L]ayer key
- [M]odifier key
- [D]ualRole key


Single press result
-------------------
[n] : emit KC,      repeat as long as pressed
[N] : emit KC+Mod   repeat as long as pressed
[L] : select layer  active as long as pressed
[M] : emit Mod      active as long as pressed
[D] : wait dual delay - if released within limit, emit as [n/N] keycode
                        else emit [M] or select [L]

Multiple presses
----------------

Logic
-----
If there are no Dual-Use keys situation can be evaluated immediately: Add layers, modifiers + pressed keycodes.
With Dual-Use:
wait if release of Dual-Use occurs within timeout -> replace with its normal representation
otherwise keep permanently as modifier.
If another key is pressed -> Dual becomes modifier immediately
Repeat condition:
same key just released is pressed again and then held -> if it was Dual-Use before timeout now is zero -> immetiately intepret as regular representation:

TODO
====
-press and release timers
-cache last handled key for repeat

*/

const char *keytype_char = "-nNMLDdrm_";
enum keytype_t {
    invalid=0,
    normal,
    normal_modified,
    modifier,
    layer,
    dual,
    dualnormal,
    dualnormalrepeat,
    dualmod,
    last_type,
} ;

enum keystatus_t {
    unknown=0,
    pressed,
    released_in_duallimit,
    released,
    pressed_in_repeatlimit,

} ;

uint8_t char2type(char c)
{
    for(uint8_t i=0; i<sizeof(keytype_char); ++i) {
        if(c==keytype_char[i])
            return i;
    }
    return 0;
}

struct key_t {
    char    display; // char to display
    enum keytype_t type;
    uint8_t id;      // unique id, during load
    enum keystatus_t status;
};

// simulated press and release of a key
struct keypress_t {
    struct key_t key;
    uint8_t press;
    uint8_t release;
};

bool isNormal(struct keypress_t kp)
{
    return (kp.key.type == normal || kp.key.type == normal_modified);
}

#define MAX_KEYS 40
uint8_t g_keycnt=0;
// sequence to analyze
struct keypress_t seq[MAX_KEYS]; // 6 + 2 Mods is report max, but theoretically we could have more layers pressed.

uint8_t t; // time
uint8_t pressed_keys=0;


uint8_t typecnt[last_type] = {} ;
void updateStatus()
{
    for(uint8_t i=0; i<last_type; ++i) {
        typecnt[i]=0;
    }
    for(uint8_t i=0; i<MAX_KEYS; ++i) {
        if(seq[i].key.status == pressed || seq[i].key.status==pressed_in_repeatlimit) {
            typecnt[seq[i].key.type]++;
        }
    }
}
void printStatus()
{
    printf("%d|", pressed_keys);
    for(uint8_t i=1; i<last_type; ++i) {
        printf(" %d", typecnt[i]);
    }
    printf("| ");
}

struct keypress_t * changed(uint8_t time, uint8_t * state) //struct keypress_t *kp)
{
    *state = unknown;
    for(uint8_t i=0; i<g_keycnt; ++i) {
        if(seq[i].press == time) {
            *state = pressed;
            return &seq[i];
        } else if(seq[i].release == time) {
            *state = released;
            return &seq[i];
        }
    }
    return NULL;
}

void printChange(struct keypress_t *kp, char state)
{
    printf("\n%3d: %*s %c%c[%c] ", t, pressed_keys*4+1, " ", state, keytype_char[kp->key.type], kp->key.display);
    printf("%*s", (6-pressed_keys)*4, " ");
    updateStatus();
    printStatus();
    if(kp->key.status == released_in_duallimit)
        printf("dual normal release ");
    if(kp->key.status == pressed_in_repeatlimit)
        printf("dual repeat ");
}

void duals2mods(uint8_t timeout)
{
    for(uint8_t i=0; i<g_keycnt; ++i)
        if(seq[i].key.type == dual && seq[i].key.status == pressed) {
            if(t>= seq[i].press + timeout) {
                //printf("\n%3d: dual -> mod timeout",t);
                seq[i].key.type = dualmod;
                printChange(&seq[i], '.');
            }
        }
}


void main(int argc, char** argv)
{

    for(uint8_t i=0; i< argc-1; ++i) {
        //uint8_t
        int p,r;
        char t,c;
        sscanf(argv[i+1], "%d%c%c%d", &p, &t, &c, &r);
        printf("\nRead %d = %d=%c[%c] +%2d -%2d",  i, char2type(t), t,c,p,r);
        struct key_t k = { .display=c, .type=char2type(t), .id=i };
        struct keypress_t kp = { .key=k,  .press=p,  .release=r };
        seq[i] = kp;
        g_keycnt=i+1;
    }

    uint8_t id=0;
    /*
       printf("\n%d keys loaded: ", g_keycnt);
       for(uint8_t i=0; i< g_keycnt; ++i){
       printf("%c,", seq[i].key.display );
       }
       */
    char    state_indicator;
    const uint8_t RELEASE_TIMEOUT = 3;
    const uint8_t REPEAT_TIMEOUT = 3;
    char last_released_id;
    char last_released_dual_t;

    // header line
    printf("\nt():         %*s#|", 6*4, " ");
    for(uint8_t i=1; i<last_type; ++i)
        printf(" %c", keytype_char[i]);

    struct keypress_t * kp;
    pressed_keys=0;
    // loop over time
    for(t =1; t<80; ++t) {
        // convert previous duals to mods if timed out.
        duals2mods(RELEASE_TIMEOUT);
        uint8_t ret;
        kp = changed(t, &ret);
        if(ret != unknown) {
            if(ret==pressed) {
                if(kp->key.type != dual) {
                    // no dual key just pressed, so all previous duals are mods
                    duals2mods(0);
                }

                pressed_keys++;
                state_indicator = '+';
                kp->key.status = pressed;

                if(kp->key.type == dual && last_released_id==kp->key.display && t-last_released_dual_t<REPEAT_TIMEOUT) {
                    // repeat as normal
                    // all previous duals are already converted at this point
                    //
                    kp->key.status = pressed_in_repeatlimit;
                    kp->key.type = dualnormalrepeat;
                }
            } else if( ret==released) {
                last_released_id=kp->key.display;
                pressed_keys--;
                state_indicator = '-';
                kp->key.status = released;

                if( kp->key.type == dual ) {
                    // check dual release in time
                    if(t-kp->press<RELEASE_TIMEOUT) {
                        // convert to regular key
                        kp->key.status = released_in_duallimit;
                        kp->key.type = dualnormal;
                        // all other duals now mods
                        duals2mods(0);
                        last_released_dual_t=t;
                    }
                }
            } else {
                state_indicator = ' ';
                continue;
            }
            printChange(kp, state_indicator);

            // no dual pressed, so emit pressed normals or released dualnormal
            if(typecnt[dual]==0) {
                if(ret==pressed && ( isNormal(*kp) || kp->key.status == pressed_in_repeatlimit))
                    printf("emit ");
                if(ret==released && kp->key.type == dualnormal)
                    printf("emit %d", kp->key.status);
            } else { // dual condition


            }

        }
    }
    printf("\n");
}



// gcc -Werror --std=c11 dualuse.c && ./a.out 1Na3 4Lx12 5M^7 20Dr21 30Dr50 38na40 31Ds34 52Dr54 56Dr60


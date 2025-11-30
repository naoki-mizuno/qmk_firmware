/*  -*-  eval: (turn-on-orgtbl); -*-
 * default HHKB Layout
 *
 * Copyright 2021 Kan-Ru Chen <kanru@kanru.info>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include <stdio.h>
#ifdef BLUETOOTH_ENABLE
#include "bluefruit_le.h"
#include "bluefruit_le_uart.h"
// Forward declaration for battery reading function from bluefruit_le_uart.c
extern uint32_t bluefruit_le_read_battery_voltage(void);
#endif
#include "gpio.h"
#include "wait.h"

// LED pins
#define LED_0_PIN F4
#define LED_2_PIN F1
// Green LED under the right USB port
#define LED_4_PIN F0

enum custom_layers {
    BASE,
    QWERTY,
    HHKB,
    MOUSE,
};

#define BATTERY_FULL 556
#define BATTERY_EMPTY 326

extern uint8_t power_save_level;

enum my_keycodes {
    KC_VBAT = SAFE_RANGE,
    KC_LOCK,
    KC_UNLOCK,
    KC_BTRS,
};

const uint16_t PROGMEM unlock_combo[] = {KC_U, KC_H, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
    COMBO(unlock_combo, KC_UNLOCK),
};

#ifdef BLUETOOTH_ENABLE
// Helper function to read battery percentage from Bluetooth module
static uint8_t read_battery_percent_from_bluetooth(void) {
    // Read raw ADC value from VBAT via AT+HWADC=6 command to nRF51822
    uint32_t raw = bluefruit_le_read_battery_voltage();

    if (raw == 0) {
        return 0; // Failed to read or not connected
    }

    // Map raw ADC value to percentage
    if (raw >= BATTERY_FULL) {
        return 100;
    } else if (raw <= BATTERY_EMPTY) {
        return 0;
    }

    // Linear interpolation between empty and full
    return ((raw - BATTERY_EMPTY) * 100) / (BATTERY_FULL - BATTERY_EMPTY);
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
#ifdef BLUETOOTH_ENABLE
        case KC_VBAT:
            if (record->event.pressed) {
                char    vbat[8];
                uint8_t level = read_battery_percent_from_bluetooth();
                snprintf(vbat, sizeof(vbat), "%d", level);
                send_string(vbat);
            }
            return false;
        case KC_LOCK:
            if (record->event.pressed) {
                power_save_level = 4;
                suspend_power_down();
            }
            return false;
        case KC_UNLOCK:
            if (record->event.pressed) {
                power_save_level = 0;
                suspend_wakeup_init();
            }
            return false;
        case KC_BTRS:
            if (record->event.pressed) {
                // Green LED solid on during operation
                gpio_set_pin_output(LED_4_PIN);
                gpio_write_pin_high(LED_4_PIN);

#if 0
                // Factory reset (handles baud rate setting internally)
                bluefruit_le_factory_reset();

                wait_ms(1000);

                // Enable keyboard (sets device name, etc.)
                bluefruit_le_enable_keyboard();
#else
                // Disconnect current connection, then delete all bonds
                // This allows re-pairing without factory reset
                bluefruit_le_delbonds();
                wait_ms(200);
                bluefruit_le_disconnect();
                wait_ms(300);
#endif
                gpio_write_pin_low(LED_4_PIN);
            }
            return false;
#endif
        default:
            return true;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* BASE Level: Default Layer
     +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     | Esc |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  0  |  [  |  ]  |  \  |  `  |
     |-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-----+
     | Tab    |  '  |  ,  |  .  |  P  |  Y  |  F  |  G  |  C  |  R  |  L  |  /  |  =  | BackSp |
     |--------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+--------+
     | Control  |  A  |  O  |  E  |  U  |  I  |  D  |  H  |  T  |  N  |  S  |  -  |   Enter    |
     |----------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+------+-----+
     | L_Shift    |  ;  |  Q  |  J  |  K  |  X  |  B  |  M  |  W  |  V  |  Z  |  R_Shift | Fn1 |
     +--------+---+-+---+-----++----+-----+-----+-----+-----+----++-----+---+-+----+-----+-----+
              | Fn0 |   LAPO   |              Space              |   RAPC   | RGUI |
              +-----+----------+---------------------------------+----------+------+
    */

    [BASE] = LAYOUT_60_hhkb(
            KC_ESC , KC_1   , KC_2         , KC_3   , KC_4   , KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_LBRC, KC_RBRC, KC_BSLS, KC_GRV,
            KC_TAB , KC_QUOT, KC_COMM      , KC_DOT , KC_P   , KC_Y, KC_F, KC_G, KC_C, KC_R, KC_L, KC_SLSH, KC_EQL , KC_BSPC,
            KC_LCTL, KC_A   , KC_O         , KC_E   , KC_U   , KC_I, KC_D, KC_H, KC_T, KC_N, KC_S, KC_MINS, KC_ENT ,
            KC_LSFT, KC_SCLN, KC_Q         , KC_J   , KC_K   , KC_X, KC_B, KC_M, KC_W, KC_V, KC_Z, KC_RSFT, MO(3)  ,
            MO(2)  , SC_LAPO, SFT_T(KC_SPC), SC_RAPC, KC_RGUI
    ),

    /* QWERTY Level: QWERTY layout
     +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     | Esc |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  0  |  -  |  =  |  \  |  `  |
     |-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-----+
     | Tab    |  Q  |  W  |  E  |  R  |  T  |  Y  |  U  |  I  |  O  |  P  |  [  |  ]  | BackSp |
     |--------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+--------+
     | Control  |  A  |  S  |  D  |  F  |  G  |  H  |  J  |  K  |  L  |  ;  |  '  |      Enter |
     |----------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+------+-----+
     | L_Shift    |  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |  R_Shift | Ly0 |
     +--------+---+-+---+-----++----+-----+-----+-----+-----+----++-----+---+-+----+-----+-----+
              | Ly1 |   LAPO   |              Space              |   RAPC   | RGUI |
              +-----+----------+---------------------------------+----------+------+
    */
    [QWERTY] = LAYOUT_60_hhkb(
            KC_ESC , KC_1   , KC_2  , KC_3   , KC_4   , KC_5, KC_6, KC_7, KC_8   , KC_9  , KC_0   , KC_MINS, KC_EQL , KC_BSLS, KC_GRV,
            KC_TAB , KC_Q   , KC_W  , KC_E   , KC_R   , KC_T, KC_Y, KC_U, KC_I   , KC_O  , KC_P   , KC_LBRC, KC_RBRC, KC_BSPC,
            KC_LCTL, KC_A   , KC_S  , KC_D   , KC_F   , KC_G, KC_H, KC_J, KC_K   , KC_L  , KC_SCLN, KC_QUOT, KC_ENT ,
            KC_LSFT, KC_Z   , KC_X  , KC_C   , KC_V   , KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, DF(0)  ,
            MO(2)  , SC_LAPO, KC_SPC, SC_RAPC, KC_RGUI
    ),

    /* Layer HHKB: HHKB mode (HHKB Fn)
     +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     | Pwr | F1  | F2  | F3  | F4  | F5  | F6  | F7  | F8  | F9  | F10 | F11 | F12 | Ins | Del |
     |-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-----+
     |  Caps  |     |     |     |     |     |     |     | Psc | Slk | Pau | Up  |     | BackSp |
     |--------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+--------+
     |          | VoD | VoU | Mut |     |     |  *  |  /  | Hom | PgU | Lft | Rht |   VBattr   |
     |----------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+------+-----+
     |            |     |     |     |     |     |  +  |  -  | End | PgD | Dwn |  R_Shift | Ly3 |
     +--------+---+-+---+-----++----+-----+-----+-----+-----+----++-----+---+-+----+-----+-----+
              |     |          |              Lock               |          |      |
              +-----+----------+---------------------------------+----------+------+
     */

    [HHKB] = LAYOUT_60_hhkb(
            KC_PWR , KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 , KC_INS , KC_DEL,
            KC_CAPS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_PSCR, KC_SCRL, KC_PAUS, KC_UP  , KC_TRNS, KC_BSPC,
            KC_TRNS, KC_VOLD, KC_VOLU, KC_MUTE, KC_TRNS, KC_TRNS, KC_PAST, KC_PSLS, KC_HOME, KC_PGUP, KC_LEFT, KC_RGHT, KC_VBAT,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_PPLS, KC_PMNS, KC_END , KC_PGDN, KC_DOWN, KC_TRNS, DF(1)  ,
            KC_TRNS, KC_TRNS, KC_LOCK, KC_TRNS, KC_TRNS
    ),

    /* Layer Mouse: Mouse mode
     +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
     |     | F1  | F2  | F3  | F4  | F5  | F6  | F7  | F8  | F9  | F10 | F11 | F12 |     | Del |
     |-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-----+
     |        | Bt1 | Up  | Bt2 |     | Rc1 | Rc2 |     |     |     |     | Up  |     |        |
     |--------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+--------+
     |          | Lft | Dwn | Rht | WhU | Pl1 | Pl2 |     |     |     | Lft | Rht |            |
     |----------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+------+-----+
     |            | WhL | Bt3 | WhR | WhD |     | BTR |     |     |     | Dwn |          |     |
     +--------+---+-+---+-----++----+-----+-----+-----+-----+----++-----+---+-+----+-----+-----+
              |     |          |              Lock               |          |      |
              +-----+----------+---------------------------------+----------+------+

     */

    [MOUSE] = LAYOUT_60_hhkb(
            KC_TRNS, KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 , KC_TRNS, KC_DEL,
            KC_TRNS, MS_BTN1, MS_UP  , MS_BTN2, KC_TRNS, DM_REC1, DM_REC2, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_UP  , KC_TRNS, KC_TRNS,
            KC_TRNS, MS_LEFT, MS_DOWN, MS_RGHT, MS_WHLU, DM_PLY1, DM_PLY2, KC_TRNS, KC_TRNS, KC_TRNS, KC_LEFT, KC_RGHT, KC_TRNS,
            KC_TRNS, MS_WHLL, MS_BTN3, MS_WHLR, MS_WHLD, KC_TRNS, KC_BTRS, KC_TRNS, KC_TRNS, KC_TRNS, KC_DOWN, KC_TRNS, KC_TRNS,
            KC_TRNS, KC_TRNS, KC_LOCK, KC_TRNS, KC_TRNS
    ),
};

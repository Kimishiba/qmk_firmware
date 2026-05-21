// crkbd default keymap — with Mouse Jiggler toggle added
// Place this file at:
//   keyboards/crkbd/keymaps/default/keymap.c

#include QMK_KEYBOARD_H

// ─── Mouse Jiggler ────────────────────────────────────────────────────────────
// Toggle with MJ_TOGL (Layer 3, top-right key — hold Lower + Raise, tap P).
//
// When active: taps Right Shift once every 60 seconds.
// Right Shift alone does not type anything, but counts as keyboard activity —
// enough to prevent screen lock and reset away-status in Teams / Slack / etc.
//
// No MOUSEKEY_ENABLE required.
// ─────────────────────────────────────────────────────────────────────────────
#include "crkbd.h"
#include "quantum.h"
#include <stdlib.h>

static uint32_t last_activity_timer = 0;
static uint32_t keep_alive_timer = 0;
static uint32_t keep_alive_delay = 0;

#define MS_TEAMS_DEFAULT 240000  // 240 seconds (4 minutes)
#define MS_TEAMS_MIN_RATIO 0.5   // Minimum: 50% of default (120 seconds)
#define INACTIVITY_THRESHOLD 60000 // 60 seconds

// Initialize random interval
void set_keep_alive_delay(void) {
    float ratio = MS_TEAMS_MIN_RATIO + (float)rand() / RAND_MAX * (1.0f - MS_TEAMS_MIN_RATIO);
    keep_alive_delay = (uint32_t)(MS_TEAMS_DEFAULT * ratio);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Update activity timer on any (real) key press
    if (record->event.pressed) {
        last_activity_timer = timer_read32();
        // Reset keep-alive interval, so it doesn't overlap with your typing
        set_keep_alive_delay();
        keep_alive_timer = timer_read32();
    }
    return true;
}

void housekeeping_task_user(void) {
    uint32_t now = timer_read32();
    if (now - last_activity_timer > INACTIVITY_THRESHOLD) {
        // Inactive period detected
        if (now - keep_alive_timer > keep_alive_delay) {
            // Generate a "ghost" left shift press/release
            tap_code(KC_LSFT);
            // Set up next random delay
            set_keep_alive_delay();
            keep_alive_timer = timer_read32();
        }
    } else {
        // If you become active, reset the keep-alive timer
        keep_alive_timer = now;
    }
}
enum custom_keycodes {
    MJ_TOGL = SAFE_RANGE,
};

bool     mouse_jiggle_mode = false;
uint32_t jiggle_timer      = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MJ_TOGL:
            if (record->event.pressed) {
                mouse_jiggle_mode = !mouse_jiggle_mode;
                jiggle_timer = timer_read32();   // reset timer on every toggle
            }
            break;
    }
    return true;
}

void matrix_scan_user(void) {
    if (mouse_jiggle_mode) {
        if (timer_elapsed32(jiggle_timer) > 60000) {   // every 60 seconds
            tap_code(KC_RSFT);                          // tap Right Shift (no visible effect)
            jiggle_timer = timer_read32();
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  // Layer 0 – Base (QWERTY)
  [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_ESC,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI,   MO(1),  KC_SPC,     KC_ENT,   MO(2), KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  // Layer 1 – Lower (numbers + arrows)
  [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT,   MO(3), KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  // Layer 2 – Raise (symbols)
  [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI,   MO(3),  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  // Layer 3 – Adjust (Lower + Raise simultaneously)
  // MJ_TOGL = top-right key on right half (same position as P on layer 0)
  [3] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MJ_TOGL,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  )
};

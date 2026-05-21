// crkbd default keymap — with Teams keep-alive feature
// Place this file at:
//   keyboards/crkbd/keymaps/default/keymap.c

#include QMK_KEYBOARD_H
#include "crkbd.h"
#include "quantum.h"
#include <stdlib.h>

/*
 * ─── KEEP MICROSOFT TEAMS (OR SIMILAR) ACTIVE ─────────────────────────────
 *
 * This code simulates periodic keypresses to keep your Teams/Slack status
 * from going "away" due to inactivity. If you haven't typed in 60 seconds,
 * it will send a single Left Shift press/release at a pseudo-random
 * interval between 120 and 240 seconds. This is enough activity for Teams.
 *
 * - Activity is only sent if the user is otherwise inactive.
 * - Uses Left Shift (harmless in most situations).
 * - Intervals are randomized for realism (and to avoid detection).
 *
 * ──────────────────────────────────────────────────────────────────────────
 */
#define MS_TEAMS_DEFAULT      240000   // 240 seconds (4 minutes)
#define MS_TEAMS_MIN_RATIO    0.5      // 50% minimum interval (120 seconds)
#define INACTIVITY_THRESHOLD  60000    // Wait 60 seconds after last real keypress

// Timing state variables
static uint32_t last_activity_timer = 0; // Last time you pressed a real key
static uint32_t keep_alive_timer    = 0; // Last time fake key was sent
static uint32_t keep_alive_delay    = 0; // Next random interval

// Helper: pick a new random interval for the next keypress (between 120s–240s)
void set_keep_alive_delay(void) {
    float ratio = MS_TEAMS_MIN_RATIO + (float)rand() / RAND_MAX * (1.0f - MS_TEAMS_MIN_RATIO);
    keep_alive_delay = (uint32_t)(MS_TEAMS_DEFAULT * ratio);
}

// Track all real keypresses to reset inactivity
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        last_activity_timer = timer_read32();
        set_keep_alive_delay();
        keep_alive_timer = timer_read32();
    }
    return true;
}

// QMK background task — checks inactivity and sends fake key if needed
void housekeeping_task_user(void) {
    uint32_t now = timer_read32();
    if (now - last_activity_timer > INACTIVITY_THRESHOLD) {
        // If inactive for more than 60s, check if interval has passed
        if (now - keep_alive_timer > keep_alive_delay) {
            tap_code(KC_LSFT); // Send harmless Left Shift tap
            set_keep_alive_delay();
            keep_alive_timer = timer_read32();
        }
    } else {
        // If you typed, reset timer to avoid overlapping with your own activity
        keep_alive_timer = now;
    }
}
// ──────────────────────────────────────────────────────────────────────────

// ──────────────────────── KEYMAP DEFINITION BELOW ────────────────────────

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  // Layer 0 – Base (QWERTY)
  [0] = LAYOUT_split_3x6_3(
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
      KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_ESC,
                                      KC_LGUI,   MO(1),  KC_SPC,     KC_ENT,   MO(2), KC_RALT
  ),

  // Layer 1 – Lower (numbers + arrows)
  [1] = LAYOUT_split_3x6_3(
       KC_TAB,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_BSPC,
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, XXXXXXX, XXXXXXX,
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                      KC_LGUI, _______,  KC_SPC,     KC_ENT,   MO(3), KC_RALT
  ),

  // Layer 2 – Raise (symbols)
  [2] = LAYOUT_split_3x6_3(
       KC_TAB, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
                                      KC_LGUI,   MO(3),  KC_SPC,     KC_ENT, _______, KC_RALT
  ),

  // Layer 3 – Adjust (Lower + Raise simultaneously)
  [3] = LAYOUT_split_3x6_3(
      QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                      KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
  )
};

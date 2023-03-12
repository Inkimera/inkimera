#ifndef INKIMERA_H
#define INKIMERA_H
#include "flecs.h"
#include "raylib.h"
#include "raymath.h"

#include "components/pipeline.h"
#include "components/3d.h"
#include "components/gui.h"
#include "components/script.h"

/*
 * ENGINE CORE
 */

/* node_id_t */
typedef ecs_entity_t node_id_t;

/* engine_t */
typedef struct engine engine_t;

/* engine_key_t */
typedef enum {
  // Alphanumeric keys
  ENGINE_KEY_APOSTROPHE      = 39,         // Key: '
  ENGINE_KEY_COMMA           = 44,         // Key: ,
  ENGINE_KEY_MINUS           = 45,         // Key: -
  ENGINE_KEY_PERIOD          = 46,         // Key: .
  ENGINE_KEY_SLASH           = 47,         // Key: /
  ENGINE_KEY_ZERO            = 48,         // Key: 0
  ENGINE_KEY_ONE             = 49,         // Key: 1
  ENGINE_KEY_TWO             = 50,         // Key: 2
  ENGINE_KEY_THREE           = 51,         // Key: 3
  ENGINE_KEY_FOUR            = 52,         // Key: 4
  ENGINE_KEY_FIVE            = 53,         // Key: 5
  ENGINE_KEY_SIX             = 54,         // Key: 6
  ENGINE_KEY_SEVEN           = 55,         // Key: 7
  ENGINE_KEY_EIGHT           = 56,         // Key: 8
  ENGINE_KEY_NINE            = 57,         // Key: 9
  ENGINE_KEY_SEMICOLON       = 59,         // Key: ;
  ENGINE_KEY_EQUAL           = 61,         // Key: =
  ENGINE_KEY_A               = 65,         // Key: A | a
  ENGINE_KEY_B               = 66,         // Key: B | b
  ENGINE_KEY_C               = 67,         // Key: C | c
  ENGINE_KEY_D               = 68,         // Key: D | d
  ENGINE_KEY_E               = 69,         // Key: E | e
  ENGINE_KEY_F               = 70,         // Key: F | f
  ENGINE_KEY_G               = 71,         // Key: G | g
  ENGINE_KEY_H               = 72,         // Key: H | h
  ENGINE_KEY_I               = 73,         // Key: I | i
  ENGINE_KEY_J               = 74,         // Key: J | j
  ENGINE_KEY_K               = 75,         // Key: K | k
  ENGINE_KEY_L               = 76,         // Key: L | l
  ENGINE_KEY_M               = 77,         // Key: M | m
  ENGINE_KEY_N               = 78,         // Key: N | n
  ENGINE_KEY_O               = 79,         // Key: O | o
  ENGINE_KEY_P               = 80,         // Key: P | p
  ENGINE_KEY_Q               = 81,         // Key: Q | q
  ENGINE_KEY_R               = 82,         // Key: R | r
  ENGINE_KEY_S               = 83,         // Key: S | s
  ENGINE_KEY_T               = 84,         // Key: T | t
  ENGINE_KEY_U               = 85,         // Key: U | u
  ENGINE_KEY_V               = 86,         // Key: V | v
  ENGINE_KEY_W               = 87,         // Key: W | w
  ENGINE_KEY_X               = 88,         // Key: X | x
  ENGINE_KEY_Y               = 89,         // Key: Y | y
  ENGINE_KEY_Z               = 90,         // Key: Z | z
  ENGINE_KEY_LEFT_BRACKET    = 91,         // Key: [
  ENGINE_KEY_BACKSLASH       = 92,         // Key: '\'
  ENGINE_KEY_RIGHT_BRACKET   = 93,         // Key: ]
  ENGINE_KEY_GRAVE           = 96,         // Key: `
  // Function keys
  ENGINE_KEY_SPACE           = 32,         // Key: Space
  ENGINE_KEY_ESCAPE          = 256,        // Key: Esc
  ENGINE_KEY_ENTER           = 257,        // Key: Enter
  ENGINE_KEY_TAB             = 258,        // Key: Tab
  ENGINE_KEY_BACKSPACE       = 259,        // Key: Backspace
  ENGINE_KEY_INSERT          = 260,        // Key: Ins
  ENGINE_KEY_DELETE          = 261,        // Key: Del
  ENGINE_KEY_RIGHT           = 262,        // Key: Cursor right
  ENGINE_KEY_LEFT            = 263,        // Key: Cursor left
  ENGINE_KEY_DOWN            = 264,        // Key: Cursor down
  ENGINE_KEY_UP              = 265,        // Key: Cursor up
  ENGINE_KEY_PAGE_UP         = 266,        // Key: Page up
  ENGINE_KEY_PAGE_DOWN       = 267,        // Key: Page down
  ENGINE_KEY_HOME            = 268,        // Key: Home
  ENGINE_KEY_END             = 269,        // Key: End
  ENGINE_KEY_CAPS_LOCK       = 280,        // Key: Caps lock
  ENGINE_KEY_SCROLL_LOCK     = 281,        // Key: Scroll down
  ENGINE_KEY_NUM_LOCK        = 282,        // Key: Num lock
  ENGINE_KEY_PRINT_SCREEN    = 283,        // Key: Print screen
  ENGINE_KEY_PAUSE           = 284,        // Key: Pause
  ENGINE_KEY_F1              = 290,        // Key: F1
  ENGINE_KEY_F2              = 291,        // Key: F2
  ENGINE_KEY_F3              = 292,        // Key: F3
  ENGINE_KEY_F4              = 293,        // Key: F4
  ENGINE_KEY_F5              = 294,        // Key: F5
  ENGINE_KEY_F6              = 295,        // Key: F6
  ENGINE_KEY_F7              = 296,        // Key: F7
  ENGINE_KEY_F8              = 297,        // Key: F8
  ENGINE_KEY_F9              = 298,        // Key: F9
  ENGINE_KEY_F10             = 299,        // Key: F10
  ENGINE_KEY_F11             = 300,        // Key: F11
  ENGINE_KEY_F12             = 301,        // Key: F12
  ENGINE_KEY_LEFT_SHIFT      = 340,        // Key: Shift left
  ENGINE_KEY_LEFT_CONTROL    = 341,        // Key: Control left
  ENGINE_KEY_LEFT_ALT        = 342,        // Key: Alt left
  ENGINE_KEY_LEFT_SUPER      = 343,        // Key: Super left
  ENGINE_KEY_RIGHT_SHIFT     = 344,        // Key: Shift right
  ENGINE_KEY_RIGHT_CONTROL   = 345,        // Key: Control right
  ENGINE_KEY_RIGHT_ALT       = 346,        // Key: Alt right
  ENGINE_KEY_RIGHT_SUPER     = 347,        // Key: Super right
  ENGINE_KEY_KB_MENU         = 348,        // Key: KB menu
  // Keypad keys
  ENGINE_KEY_KP_0            = 320,        // Key: Keypad 0
  ENGINE_KEY_KP_1            = 321,        // Key: Keypad 1
  ENGINE_KEY_KP_2            = 322,        // Key: Keypad 2
  ENGINE_KEY_KP_3            = 323,        // Key: Keypad 3
  ENGINE_KEY_KP_4            = 324,        // Key: Keypad 4
  ENGINE_KEY_KP_5            = 325,        // Key: Keypad 5
  ENGINE_KEY_KP_6            = 326,        // Key: Keypad 6
  ENGINE_KEY_KP_7            = 327,        // Key: Keypad 7
  ENGINE_KEY_KP_8            = 328,        // Key: Keypad 8
  ENGINE_KEY_KP_9            = 329,        // Key: Keypad 9
  ENGINE_KEY_KP_DECIMAL      = 330,        // Key: Keypad .
  ENGINE_KEY_KP_DIVIDE       = 331,        // Key: Keypad /
  ENGINE_KEY_KP_MULTIPLY     = 332,        // Key: Keypad *
  ENGINE_KEY_KP_SUBTRACT     = 333,        // Key: Keypad -
  ENGINE_KEY_KP_ADD          = 334,        // Key: Keypad +
  ENGINE_KEY_KP_ENTER        = 335,        // Key: Keypad Enter
  ENGINE_KEY_KP_EQUAL        = 336,        // Key: Keypad =
  // Android key buttons
  ENGINE_KEY_BACK            = 4,          // Key: Android back button
  ENGINE_KEY_MENU            = 82,         // Key: Android menu button
  ENGINE_KEY_VOLUME_UP       = 24,         // Key: Android volume up button
  ENGINE_KEY_VOLUME_DOWN     = 25          // Key: Android volume down button
} engine_key_t;

/* engine_key_state_t */
typedef enum {
  ENGINE_KEY_STATE_DOWN = 1 << 0,
  ENGINE_KEY_STATE_PRESSED = 1 << 1,
  ENGINE_KEY_STATE_RELEASED = 1 << 2,
  ENGINE_KEY_STATE_UP = 1 << 3
} engine_key_state_t;

/* engine_init */
engine_t*
engine_init(
  void
);

/* engine_deinit */
int
engine_deinit(
  engine_t *eng
);

/*
 * INPUT
 */

/* engine_key_state_get */
engine_key_state_t
engine_key_state_get(
  engine_key_t key
);

/*
 * SIGNALS
 */

/*
 * ENGINE ECS
 */

ecs_world_t*
engine_ecs_context(
  engine_t *eng
);

//* engine_ecs_get_children_of */
int
engine_ecs_get_children_of(
  ecs_world_t *ecs_ctx,
  node_id_t node,
  node_id_t *children,
  int max_children
);

/*
 * ENGINE PLUGIN
 */

/* load_handle_t */
typedef int (*load_handle_t)(
  engine_t *eng,
  void *plug
);

/* unload_handle_t */
typedef int (*unload_handle_t)(
  engine_t *eng,
  void *plug
);

/* plugin_t */
typedef struct {
  engine_t *eng;
  void *plugin;
  load_handle_t load;
  unload_handle_t unload;
} plugin_t;
extern ECS_COMPONENT_DECLARE(plugin_t);

/*
 * ENGINE RUNTIME
 */

/* engine_run */
void
engine_run(
  engine_t *eng
);
#endif

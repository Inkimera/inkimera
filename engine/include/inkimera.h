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

/* ink_key_t */
typedef enum {
  // Alphanumeric keys
  INK_KEY_APOSTROPHE      = 39,         // Key: '
  INK_KEY_COMMA           = 44,         // Key: ,
  INK_KEY_MINUS           = 45,         // Key: -
  INK_KEY_PERIOD          = 46,         // Key: .
  INK_KEY_SLASH           = 47,         // Key: /
  INK_KEY_ZERO            = 48,         // Key: 0
  INK_KEY_ONE             = 49,         // Key: 1
  INK_KEY_TWO             = 50,         // Key: 2
  INK_KEY_THREE           = 51,         // Key: 3
  INK_KEY_FOUR            = 52,         // Key: 4
  INK_KEY_FIVE            = 53,         // Key: 5
  INK_KEY_SIX             = 54,         // Key: 6
  INK_KEY_SEVEN           = 55,         // Key: 7
  INK_KEY_EIGHT           = 56,         // Key: 8
  INK_KEY_NINE            = 57,         // Key: 9
  INK_KEY_SEMICOLON       = 59,         // Key: ;
  INK_KEY_EQUAL           = 61,         // Key: =
  INK_KEY_A               = 65,         // Key: A | a
  INK_KEY_B               = 66,         // Key: B | b
  INK_KEY_C               = 67,         // Key: C | c
  INK_KEY_D               = 68,         // Key: D | d
  INK_KEY_E               = 69,         // Key: E | e
  INK_KEY_F               = 70,         // Key: F | f
  INK_KEY_G               = 71,         // Key: G | g
  INK_KEY_H               = 72,         // Key: H | h
  INK_KEY_I               = 73,         // Key: I | i
  INK_KEY_J               = 74,         // Key: J | j
  INK_KEY_K               = 75,         // Key: K | k
  INK_KEY_L               = 76,         // Key: L | l
  INK_KEY_M               = 77,         // Key: M | m
  INK_KEY_N               = 78,         // Key: N | n
  INK_KEY_O               = 79,         // Key: O | o
  INK_KEY_P               = 80,         // Key: P | p
  INK_KEY_Q               = 81,         // Key: Q | q
  INK_KEY_R               = 82,         // Key: R | r
  INK_KEY_S               = 83,         // Key: S | s
  INK_KEY_T               = 84,         // Key: T | t
  INK_KEY_U               = 85,         // Key: U | u
  INK_KEY_V               = 86,         // Key: V | v
  INK_KEY_W               = 87,         // Key: W | w
  INK_KEY_X               = 88,         // Key: X | x
  INK_KEY_Y               = 89,         // Key: Y | y
  INK_KEY_Z               = 90,         // Key: Z | z
  INK_KEY_LEFT_BRACKET    = 91,         // Key: [
  INK_KEY_BACKSLASH       = 92,         // Key: '\'
  INK_KEY_RIGHT_BRACKET   = 93,         // Key: ]
  INK_KEY_GRAVE           = 96,         // Key: `
  // Function keys
  INK_KEY_SPACE           = 32,         // Key: Space
  INK_KEY_ESCAPE          = 256,        // Key: Esc
  INK_KEY_ENTER           = 257,        // Key: Enter
  INK_KEY_TAB             = 258,        // Key: Tab
  INK_KEY_BACKSPACE       = 259,        // Key: Backspace
  INK_KEY_INSERT          = 260,        // Key: Ins
  INK_KEY_DELETE          = 261,        // Key: Del
  INK_KEY_RIGHT           = 262,        // Key: Cursor right
  INK_KEY_LEFT            = 263,        // Key: Cursor left
  INK_KEY_DOWN            = 264,        // Key: Cursor down
  INK_KEY_UP              = 265,        // Key: Cursor up
  INK_KEY_PAGE_UP         = 266,        // Key: Page up
  INK_KEY_PAGE_DOWN       = 267,        // Key: Page down
  INK_KEY_HOME            = 268,        // Key: Home
  INK_KEY_END             = 269,        // Key: End
  INK_KEY_CAPS_LOCK       = 280,        // Key: Caps lock
  INK_KEY_SCROLL_LOCK     = 281,        // Key: Scroll down
  INK_KEY_NUM_LOCK        = 282,        // Key: Num lock
  INK_KEY_PRINT_SCREEN    = 283,        // Key: Print screen
  INK_KEY_PAUSE           = 284,        // Key: Pause
  INK_KEY_F1              = 290,        // Key: F1
  INK_KEY_F2              = 291,        // Key: F2
  INK_KEY_F3              = 292,        // Key: F3
  INK_KEY_F4              = 293,        // Key: F4
  INK_KEY_F5              = 294,        // Key: F5
  INK_KEY_F6              = 295,        // Key: F6
  INK_KEY_F7              = 296,        // Key: F7
  INK_KEY_F8              = 297,        // Key: F8
  INK_KEY_F9              = 298,        // Key: F9
  INK_KEY_F10             = 299,        // Key: F10
  INK_KEY_F11             = 300,        // Key: F11
  INK_KEY_F12             = 301,        // Key: F12
  INK_KEY_LEFT_SHIFT      = 340,        // Key: Shift left
  INK_KEY_LEFT_CONTROL    = 341,        // Key: Control left
  INK_KEY_LEFT_ALT        = 342,        // Key: Alt left
  INK_KEY_LEFT_SUPER      = 343,        // Key: Super left
  INK_KEY_RIGHT_SHIFT     = 344,        // Key: Shift right
  INK_KEY_RIGHT_CONTROL   = 345,        // Key: Control right
  INK_KEY_RIGHT_ALT       = 346,        // Key: Alt right
  INK_KEY_RIGHT_SUPER     = 347,        // Key: Super right
  INK_KEY_KB_MENU         = 348,        // Key: KB menu
  // Keypad keys
  INK_KEY_KP_0            = 320,        // Key: Keypad 0
  INK_KEY_KP_1            = 321,        // Key: Keypad 1
  INK_KEY_KP_2            = 322,        // Key: Keypad 2
  INK_KEY_KP_3            = 323,        // Key: Keypad 3
  INK_KEY_KP_4            = 324,        // Key: Keypad 4
  INK_KEY_KP_5            = 325,        // Key: Keypad 5
  INK_KEY_KP_6            = 326,        // Key: Keypad 6
  INK_KEY_KP_7            = 327,        // Key: Keypad 7
  INK_KEY_KP_8            = 328,        // Key: Keypad 8
  INK_KEY_KP_9            = 329,        // Key: Keypad 9
  INK_KEY_KP_DECIMAL      = 330,        // Key: Keypad .
  INK_KEY_KP_DIVIDE       = 331,        // Key: Keypad /
  INK_KEY_KP_MULTIPLY     = 332,        // Key: Keypad *
  INK_KEY_KP_SUBTRACT     = 333,        // Key: Keypad -
  INK_KEY_KP_ADD          = 334,        // Key: Keypad +
  INK_KEY_KP_ENTER        = 335,        // Key: Keypad Enter
  INK_KEY_KP_EQUAL        = 336,        // Key: Keypad =
  // Android key buttons
  INK_KEY_BACK            = 4,          // Key: Android back button
  INK_KEY_MENU            = 82,         // Key: Android menu button
  INK_KEY_VOLUME_UP       = 24,         // Key: Android volume up button
  INK_KEY_VOLUME_DOWN     = 25          // Key: Android volume down button
} ink_key_t;

typedef enum {
  INK_MOUSE_BUTTON_LEFT    = 0,         // Mouse button left
  INK_MOUSE_BUTTON_RIGHT   = 1,         // Mouse button right
  INK_MOUSE_BUTTON_MIDDLE  = 2,         // Mouse button middle (pressed wheel)
  INK_MOUSE_BUTTON_SIDE    = 3,         // Mouse button side (advanced mouse device)
  INK_MOUSE_BUTTON_EXTRA   = 4,         // Mouse button extra (advanced mouse device)
  INK_MOUSE_BUTTON_FORWARD = 5,         // Mouse button forward (advanced mouse device)
  INK_MOUSE_BUTTON_BACK    = 6,         // Mouse button back (advanced mouse device)
} ink_mouse_t;

/* ink_key_state_t */
typedef enum {
  INK_KEY_STATE_DOWN = 1 << 0,
  INK_KEY_STATE_PRESSED = 1 << 1,
  INK_KEY_STATE_RELEASED = 1 << 2,
  INK_KEY_STATE_UP = 1 << 3
} ink_key_state_t;

/* ink_init */
engine_t*
ink_init(
  void
);

/* ink_deinit */
int
ink_deinit(
  engine_t *eng
);

/* ink_shutdown */
void
ink_shutdown(
  engine_t *eng
);

/* engine_shutdown_t */
typedef bool engine_shutdown_t;
extern ECS_COMPONENT_DECLARE(engine_shutdown_t);

/*
 * INPUT
 */

/* ink_key_state_get */
ink_key_state_t
ink_key_state_get(
  ink_key_t key
);

/* ink_mouse_state_get */
ink_key_state_t
ink_mouse_state_get(
  ink_mouse_t key
);

/*
 * SIGNALS
 */

/*
 * ENGINE ECS
 */

ecs_world_t*
ink_ecs_context(
  engine_t *eng
);

//* ink_ecs_get_children_of */
int
ink_ecs_get_children_of(
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

/* ink_run */
void
ink_run(
  engine_t *eng
);
#endif

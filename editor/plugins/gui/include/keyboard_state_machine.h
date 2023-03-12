#ifndef KEYBOARD_STATE_MACHINE_H
#define KEYBOARD_STATE_MACHINE_H

#include "stdio.h"
#include "hashmap.h"

#define MAX_EVENTS 32
#define MAX_TRANSITIONS 32

/* state_machine_key_t */
typedef struct {
  int key;
  int action;
} state_machine_key_t;

#define state_machine_key state_machine_key_t
DECLARE_HASHMAP(state_machine_key)

typedef struct {
  hashmap_state_machine_key_t keys;
  int num_keys;
} state_machine_event_t;

typedef struct {
  char *name;
} state_machine_state_t;

#define state_machine_state state_machine_state_t
DECLARE_HASHMAP(state_machine_state)

typedef struct {
  state_machine_event_t *event;
  state_machine_state_t *from_state;
  state_machine_state_t *to_state;
  void (*on_enter);
  void (*on_exit);
} state_machine_transition_t;

typedef struct {
  hashmap_state_machine_state_t states;
  state_machine_event_t events[MAX_EVENTS];
  state_machine_transition_t transitions[MAX_TRANSITIONS];
  int num_states;
  int num_events;
  int num_transitions;
  state_machine_state_t *current_state;
} state_machine_t;

void
state_machine_init(
  state_machine_t *machine
);

state_machine_state_t*
state_machine_add_state(
  state_machine_t *machine,
  char *name
);

state_machine_event_t*
state_machine_add_event(
  state_machine_t *machine,
  int *keys,
  int *actions,
  int num_keys
);

state_machine_transition_t*
state_machine_add_transition(
  state_machine_t *machine,
  state_machine_event_t *event,
  state_machine_state_t *from_name,
  state_machine_state_t *to_state
);

int
state_machine_process(
  state_machine_t *machine,
  int *keys,
  int *actions,
  int num_keys
);

char*
state_machine_get_state(
  state_machine_t *machine
);

int
state_machine_set_state(
  state_machine_t *machine,
  char *name
);
#endif

#include "plugins/keyboard_state_machine.h"

#define MAX_KEY_STR 16

DEFINE_HASHMAP(state_machine_key)
DEFINE_HASHMAP(state_machine_state)

void
state_machine_init(
  state_machine_t *machine
) {
  hashmap_state_machine_state_init(&machine->states);
}

state_machine_state_t*
state_machine_add_state(
  state_machine_t *machine,
  char *name
) {
  state_machine_state_t state = { .name = name };
  hashmap_state_machine_state_insert(&machine->states, name, state);
  state_machine_state_t *state_ptr = hashmap_state_machine_state_lookup(&machine->states, name);
  if (!machine->current_state) {
    machine->current_state = state_ptr;
  }
  return state_ptr;
}

state_machine_event_t*
state_machine_add_event(
  state_machine_t *machine,
  int *keys,
  int *actions,
  int num_keys
) {
  state_machine_event_t event = {};
  hashmap_state_machine_key_init(&event.keys);
  for (int i = 0; i < num_keys; i++) {
    char key_str[MAX_KEY_STR];
    sprintf(key_str, "%d", keys[i]);
    hashmap_state_machine_key_insert(&event.keys, key_str, (state_machine_key_t) { .key = keys[i], .action = actions[i] });
  }
  event.num_keys = num_keys;
  machine->events[machine->num_events] = event;
  state_machine_event_t *event_ptr = &machine->events[machine->num_events];
  machine->num_events++;
  return event_ptr;
}

state_machine_transition_t*
state_machine_add_transition(
  state_machine_t *machine,
  state_machine_event_t *event,
  state_machine_state_t *from_state,
  state_machine_state_t *to_state
) {
  state_machine_transition_t transition = {
    .event = event,
    .from_state = from_state,
    .to_state = to_state
  };
  machine->transitions[machine->num_transitions] = transition;
  state_machine_transition_t *transition_ptr = &machine->transitions[machine->num_transitions];
  machine->num_transitions++;
  return transition_ptr;
}

int
state_machine_process(
  state_machine_t *machine,
  int *keys,
  int *actions,
  int num_keys
) {
  for (int i = 0; i < machine->num_transitions; i++) {
    state_machine_transition_t *transition = &machine->transitions[i];
    if (!transition || transition->from_state != machine->current_state) {
      continue;
    }
    int key_matches = 0;
    state_machine_event_t *event = transition->event;
    for (int j = 0; j < num_keys; j++) {
      char key_str[MAX_KEY_STR];
      sprintf(key_str, "%d", keys[j]);
      state_machine_key_t *event_key
        = hashmap_state_machine_key_lookup(&event->keys, key_str);
      if (event_key && event_key->action & actions[j]) {
        key_matches++;
      }
    }
    if (key_matches == event->num_keys) {
      machine->current_state = transition->to_state;
      return 1;
    }
  }
  return 0;
}

char*
state_machine_get_state(
  state_machine_t *machine
) {
  return machine->current_state->name;
}

int
state_machine_set_state(
  state_machine_t *machine,
  char *name
) {
  state_machine_state_t *state = hashmap_state_machine_state_lookup(&machine->states, name);
  if (state) {
    machine->current_state = state;
    return 1;
  } else {
    return 0;
  }
}

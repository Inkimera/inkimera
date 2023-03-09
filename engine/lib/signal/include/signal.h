#ifndef SIGNAL_H
#define SIGNAL_H

/* signal_t */
typedef struct signal_t {
  void *data;
  size_t data_size;
  void (*read)(void*);
  void (*write)(void*, void*);
  void *head;
} signal_t;

/* signal_handle_t */
typedef struct signal_handle_t {
  signal_t *signal;
  void (*callback)(void*);
  struct signal_handle_t *next;
} signal_handle_t;

/* INIT_SIGNAL */
signal_t*
INIT_SIGNAL(void *data, size_t data_size);

/* DEINIT_SIGNAL */
void
DEINIT_SIGNAL(signal_t *signal);

/* SIGNAL_REGISTER */
signal_handle_t*
SIGNAL_REGISTER(signal_t *signal, void (*callback)(void*));

/* SIGNAL_UNREGISTER */
void
SIGNAL_UNREGISTER(signal_handle_t *handle);

/* SIGNAL_READ */
#define SIGNAL_READ(handle) handle->signal->read(handle)

/* SIGNAL_WRITE */
#define SIGNAL_WRITE(handle, new_data) handle->signal->write(handle, &new_data)
#endif

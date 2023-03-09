#include "stdlib.h"
#include "signal.h"

static void*
_signal_mem_copy(
  void *dest,
  const void *src,
  size_t n
) {
  size_t i = 0;
  char *newsrc = (char*)src;
  char *newdest = (char*)dest;
  while (i < n) {
    newdest[i] = newsrc[i];
    i++;
  }
  return newdest;
}

static void
_signal_read(
  void *handle
) {
  if (handle) {
    signal_handle_t *signal_handle = (signal_handle_t*)handle;
    signal_handle->callback(signal_handle->signal->data);
  }
}

static void
_signal_write(
  void *handle,
  void *new_data
) {
  if (handle && new_data) {
    signal_handle_t *signal_handle = (signal_handle_t*)handle;
    _signal_mem_copy((char*) signal_handle->signal->data, (const char*) new_data, signal_handle->signal->data_size);
    while (signal_handle) {
      if (signal_handle->callback) {
        signal_handle->callback(signal_handle->signal->data);
      }
      signal_handle = signal_handle->next;
    }
  }
}

signal_handle_t*
create_signal_handle(
  signal_t *signal,
  void (*callback)(void*)
) {
  signal_handle_t *handle = malloc(sizeof(signal_handle_t));
  handle->signal = signal;
  handle->callback = callback;
  handle->next = NULL;
  return handle;
}

void
destroy_signal_handle(
  signal_handle_t *handle
) {
  if (handle) {
    free(handle);
  }
}

signal_t*
INIT_SIGNAL(
  void *data,
  size_t data_size
) {
  signal_t *signal = malloc(sizeof(signal_t));
  signal->data = malloc(data_size);
  signal->data_size = data_size;
  _signal_mem_copy((char*)signal->data, (const char*)data, data_size);
  signal->read = &_signal_read;
  signal->write = &_signal_write;
  signal->head = NULL;
  return signal;
}

void
DEINIT_SIGNAL(
  signal_t *signal
) {
  if (signal) {
    free(signal->data);
    free(signal);
  }
}

signal_handle_t*
SIGNAL_REGISTER(
  signal_t *signal,
  void (*callback)(void*)
) {
  signal_handle_t *handle = create_signal_handle(signal, callback);
  signal_handle_t *lastHandle = (signal_handle_t*)signal->head;
  if (!lastHandle) {
    signal->head = (void*)handle;
  } else {
    while (lastHandle->next) {
      lastHandle = lastHandle->next;
    }
    lastHandle->next = handle;
  }
  return handle;
}

void
SIGNAL_UNREGISTER(
  signal_handle_t *handle
) {
  signal_t *signal = handle->signal;
  signal_handle_t *prevHandle = NULL;
  signal_handle_t *currHandle = (signal_handle_t*)signal->head;
  while (currHandle) {
    if (currHandle == handle) {
      if (prevHandle) {
        prevHandle->next = currHandle->next;
      } else {
        signal->head = (void*)currHandle->next;
      }
      destroy_signal_handle(handle);
      return;
    }
    prevHandle = currHandle;
    currHandle = currHandle->next;
  }
}
